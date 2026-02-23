//
// Created by deril on 2/17/26.
//

#pragma once
#include <chrono>

#include "SingletonBase.h"

namespace Vox {
    class Time {
        static std::chrono::time_point<std::chrono::steady_clock> sStart;
        static std::chrono::time_point<std::chrono::steady_clock> sLast;
        static double sDelta;

    public:
        static std::chrono::time_point<std::chrono::steady_clock> Now() {
            return std::chrono::steady_clock::now();
        }

        //From engine startup
        static uint64_t Millis() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - sStart).count();
        }

        //From engine startup
        static uint64_t Seconds() {
            return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - sStart).count();
        }

        //Call once per frame
        static void Update()
        {
            const auto now = std::chrono::steady_clock::now();
            sDelta = std::chrono::duration<double>(now - sLast).count();
            sLast = now;
        }

        static double Delta() {
            return sDelta;
        }
    };
}
