//
// Created by deril on 2/17/26.
//

#pragma once


namespace Vox::Render {
    class FpsCounter {
        constexpr static int SAMPLE_COUNT = 100;
        double mSamples[SAMPLE_COUNT] = {};
        int mSampleIndex = 0;
        double mFps = 0;

    public:
        void update(double deltaTime);
        double getFps();
    };
}