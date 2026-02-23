//
// Created by deril on 2/17/26.
//

#include "../../include/VoxEngine/render/FpsCounter.h"

namespace Vox::Render {
    void FpsCounter::update(const double deltaTime) {
        mSamples[mSampleIndex] = deltaTime;
        mSampleIndex = (mSampleIndex + 1) % SAMPLE_COUNT;

        double sum = 0.0;
        for (const double sample: mSamples)
            sum += sample;

        mFps = 1.0 / (sum / SAMPLE_COUNT);
    }

    double FpsCounter::getFps() {
        return mFps;
    }
}
