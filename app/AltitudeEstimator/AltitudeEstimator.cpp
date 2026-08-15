#include "AltitudeEstimator.hpp"

namespace gnc {

namespace {
constexpr float kGravity = 9.80665f;
/// NIS low-pass coefficient. tau ~ 1 s at 200 Hz update rate.
constexpr float kNisAlpha = 0.005f;
}  // namespace

// ---------------------------------------------------------------------------
// configuration
// ---------------------------------------------------------------------------

void AltitudeEstimator::begin() noexcept
{
    AltitudeKf::Config& cfg = kf_.config();

    // ---- geometry: REPLACE WITH YOUR MECHANICAL NUMBERS ----
    // Body origin placed at the LANDING CONTACT PLANE, so that h == 0 exactly
    // at touchdown and the control law needs no offset. The aperture sits
    // 100 mm above that plane when the lander is down.
    // If your control laws reference the CG instead, measure lidarPos from the
    // CG and expect h(touchdown) = 0.100 - lidarPos.z.
    cfg.lidarPos  = { 0.0f, 0.0f, 0.100f };   // aperture w.r.t. body origin [m]
    cfg.lidarBeam = { 0.0f, 0.0f, -1.0f };    // beam direction, body frame

    // ---- noise: qAccel MUST come from a static-fire vibration log ----
    cfg.qAccel          = 0.30f * 0.30f;
    cfg.qBias           = 0.005f * 0.005f;
    cfg.sigmaRangeFloor = 0.025f;
    cfg.kStrength       = 50.0f;              // fit on a static rig
    cfg.sigmaTilt       = 2.0f * 0.017453293f;

    // ---- gating ----
    cfg.nisGate     = 9.0f;
    cfg.minTiltCos  = 0.866f;                 // 30 deg
    // MUST sit below the 0.100 m pad reading or calibration can never complete.
    // Verify the TF02-Pro's real blind zone on a bench before trusting this.
    cfg.minRange    = 0.08f;
    cfg.maxRange    = 25.0f;
    cfg.minStrength = 100u;
    cfg.maxDt       = 0.02f;                  // 8x the 2.5 ms nominal step

    restart();
}

void AltitudeEstimator::restart() noexcept
{
    kf_.reset();
    med_.reset();

    qHead_.store(0, std::memory_order_relaxed);
    qTail_.store(0, std::memory_order_relaxed);
    decim_ = 0;

    calN_       = 0;
    calRangeN_  = 0;
    calUpAccel_ = 0.0f;
    calRange_   = 0.0f;
    calTiltCos_ = 1.0f;
    calLambda_  = 0.0f;

    meanNis_      = 0.0f;
    lastAcceptUs_ = 0;
    padCheck_     = PadCheck::Pending;
    padCheckErr_  = 0.0f;
    phase_        = Phase::Calibrating;
    clamped_ = true;
}

// ---------------------------------------------------------------------------
// producer side: UART6 idle ISR
// ---------------------------------------------------------------------------

void AltitudeEstimator::onLidarFrame(uint16_t distanceMm,
                                     uint16_t strength) noexcept
{
    if (phase_ == Phase::Idle) { return; }

    // Median-of-5 over the raw 1 kHz stream. This is the correct use of the
    // TF02-Pro's high frame rate: spike rejection, not update rate. Samples at
    // 1 kHz are not independent, and feeding them to the filter as if they
    // were makes it overconfident. See README 5.3.
    float m;
    if (!med_.push(static_cast<float>(distanceMm) * kRawToM, m)) { return; }

    // Decimate 1 kHz -> 200 Hz.
    if (++decim_ < 5u) { return; }
    decim_ = 0;

    const uint32_t head = qHead_.load(std::memory_order_relaxed);
    const uint32_t next = (head + 1u) & kQueueMask;
    if (next == qTail_.load(std::memory_order_acquire)) {
        ++qOverflow_;                // consumer stalled; drop, never block
        return;
    }
    queue_[head].rangeM   = m;
    queue_[head].strength = strength;
    qHead_.store(next, std::memory_order_release);
}

bool AltitudeEstimator::popLidar(LidarSample& out) noexcept
{
    const uint32_t tail = qTail_.load(std::memory_order_relaxed);
    if (tail == qHead_.load(std::memory_order_acquire)) { return false; }
    out = queue_[tail];
    qTail_.store((tail + 1u) & kQueueMask, std::memory_order_release);
    return true;
}

// ---------------------------------------------------------------------------
// pad calibration
// ---------------------------------------------------------------------------

void AltitudeEstimator::runCalibration(const Quat& q, const Vec3& accel) noexcept
{
    const Vec3  r3 = q.upRow();
    const AltitudeKf::Config& cfg = kf_.config();

    float c = -r3.dot(cfg.lidarBeam);
    if (c < 1.0e-3f) { c = 1.0e-3f; }

    // Running means: mean += (x - mean) / n
    ++calN_;
    const float invN = 1.0f / static_cast<float>(calN_);
    calUpAccel_ += (r3.dot(accel) - calUpAccel_) * invN;
    calTiltCos_ += (c            - calTiltCos_) * invN;
    calLambda_  += (r3.dot(cfg.lidarPos) - calLambda_) * invN;

    LidarSample s;
    while (popLidar(s)) {
        if (s.rangeM < cfg.minRange || s.rangeM > cfg.maxRange) { continue; }
        if (s.strength < cfg.minStrength || s.strength == 65535u) { continue; }
        ++calRangeN_;
        calRange_ += (s.rangeM - calRange_) / static_cast<float>(calRangeN_);
    }

    if (calN_ < kCalSamples) { return; }

    // --- altitude datum: mechanical, not measured -------------------------
    // The standing height comes from the drawing. It is a rigid dimension
    // known to a millimetre, whereas the lidar would give it to us with
    // centimetres of noise at the worst point in its range. Deriving h0 from
    // the sensor would be strictly worse AND would make arming depend on a
    // marginal near-field return.
    const float h0 = padHeightM_;

    // --- accelerometer bias: this is what the pad average is actually for --
    const float b0 = calUpAccel_ - kGravity;   // bias on body z, near-vertical

    // --- lidar cross-check: agreement, not derivation ---------------------
    if (calRangeN_ < 50u) {
        padCheck_    = PadCheck::NoReturns;
        padCheckErr_ = 0.0f;
    } else {
        const float hLidar = calTiltCos_ * calRange_ - calLambda_;
        padCheckErr_ = hLidar - padHeightM_;
        padCheck_ = (padCheckErr_ < padCheckTolM_ && padCheckErr_ > -padCheckTolM_)
                  ? PadCheck::Ok : PadCheck::Mismatch;
    }

    // The filter itself is valid either way -- the bias is what it needed.
    // Arming is a mission-control decision: gate it on readyToArm().
    kf_.init(h0, b0, 0.05f, 0.10f, 0.50f);
    phase_ = Phase::Running;
}

// ---------------------------------------------------------------------------
// consumer side: tim7, 1 kHz
// ---------------------------------------------------------------------------

void AltitudeEstimator::service(bool newImu, const Quat& q, const Vec3& accel,
                                uint64_t nowUs) noexcept
{
    if (phase_ == Phase::Idle) { return; }

    if (phase_ == Phase::Calibrating) {
        // Only ever calibrate while held. Averaging range on a moving vehicle
        // produces a meaningless datum; far better to fail visibly on the pad.
        // If phase() never reaches Running while clamped, the lidar cannot see
        // the ground from the standing height -- do not launch.
        if (!clamped_) { return; }
        if (newImu) { runCalibration(q, accel); }
        return;
    }

    // --- time update, driven by IMU data arrival (~400 Hz) ---
    // dt comes from the timestamp, not from an assumed loop rate, so tim7
    // jitter or a missed IMU report cannot corrupt the integration.
    if (newImu) {
        kf_.predictAtMicros(nowUs, accel, q);
    }

    // --- measurement updates: drain whatever the UART ISR enqueued ---
    LidarSample s;
    while (popLidar(s)) {
        const AltitudeKf::UpdateResult res = kf_.updateLidar(s.rangeM, s.strength);
        if (res == AltitudeKf::UpdateResult::Accepted) {
            meanNis_ += (kf_.lastNis() - meanNis_) * kNisAlpha;
            lastAcceptUs_ = nowUs;
        }
    }

    // --- zero-velocity while held ---
    if (clamped_) {
        kf_.updateZeroVelocity(0.02f);
    }
}

}  // namespace gnc
