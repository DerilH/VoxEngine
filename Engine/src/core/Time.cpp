//
// Created by deril on 2/17/26.
//

#include <VoxCore/Time.h>
namespace Vox {
    std::chrono::time_point<std::chrono::steady_clock> Time::sStart = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> Time::sLast = std::chrono::steady_clock::now();
    double Time::sDelta = 1;
}