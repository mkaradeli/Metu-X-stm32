/*
 * LowPassFilter.hpp
 *
 *  Created on: May 23, 2026
 *      Author: karadeli
 */

#ifndef LOWPASSFILTER_HPP_
#define LOWPASSFILTER_HPP_

// LowPass.hpp - First-order IIR low-pass filter
#include <cmath>

class LowPass {
public:
    LowPass(float fc_hz, float fs_hz) { setCutoff(fc_hz, fs_hz); }

    void setCutoff(float fc_hz, float fs_hz) {
        const float rc = 1.0f / (2.0f * static_cast<float>(M_PI) * fc_hz);
        const float dt = 1.0f / fs_hz;
        alpha_ = dt / (rc + dt);
    }

    float update(float x) {
        if (!primed_) { y_ = x; primed_ = true; return x; }
        y_ += alpha_ * (x - y_);
        return y_;
    }

    void  reset()       { primed_ = false; y_ = 0.0f; }
    float value() const { return y_; }

private:
    float alpha_  = 0.0f;
    float y_      = 0.0f;
    bool  primed_ = false;
};

#endif /* LOWPASSFILTER_HPP_ */
