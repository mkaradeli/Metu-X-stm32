/*
 * LowPassFilter.hpp
 *
 *  Created on: May 23, 2026
 *      Author: karadeli
 */

#ifndef LOWPASSFILTER_HPP_
#define LOWPASSFILTER_HPP_

// LowPass.hpp - First-order IIR low-pass filter with settling detection
#include <cmath>
#include <cstdint>

class LowPass {
public:
    LowPass(float fc_hz, float fs_hz,
            float settle_threshold = 0.0f,
            float settle_time_s = 5.0f)
        : settle_threshold_(settle_threshold)
    {
        setCutoff(fc_hz, fs_hz);
        settle_samples_ = static_cast<uint32_t>(fs_hz * settle_time_s + 0.5f);
    }

    void setCutoff(float fc_hz, float fs_hz) {
        const float rc = 1.0f / (2.0f * static_cast<float>(M_PI) * fc_hz);
        const float dt = 1.0f / fs_hz;
        alpha_ = dt / (rc + dt);
    }

    void setSettleThreshold(float threshold) { settle_threshold_ = threshold; }

    float update(float x) {
        if (!primed_) {
            y_ = x;
            primed_ = true;
            settle_ref_ = y_;
            return x;
        }

        y_ += alpha_ * (x - y_);

        if (fabsf(y_ - settle_ref_) > settle_threshold_) {
            // deviated: restart settling from here
            settle_ref_   = y_;
            settle_count_ = 0;
            stable_       = false;
        } else if (settle_count_ < settle_samples_) {
            if (++settle_count_ >= settle_samples_) {
                stable_ = true;
            }
        }

        return y_;
    }

    void reset() {
        primed_       = false;
        y_            = 0.0f;
        settle_ref_   = 0.0f;
        settle_count_ = 0;
        stable_       = false;
    }

    float value()    const { return y_; }
    bool  isStable() const { return stable_; }

private:
    // Filter
    float alpha_  = 0.0f;
    float y_      = 0.0f;
    bool  primed_ = false;

    // Settling detection
    float    settle_threshold_ = 0.0f;
    uint32_t settle_samples_   = 0;
    float    settle_ref_       = 0.0f;
    uint32_t settle_count_     = 0;
    bool     stable_           = false;
};

#endif /* LOWPASSFILTER_HPP_ */
