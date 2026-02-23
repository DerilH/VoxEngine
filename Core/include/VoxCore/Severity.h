#pragma once
#include <iostream>
#include <vulkan/vulkan_core.h>

namespace Vox::Log {
    enum class Severity {
        WARNING,
        INFO,
        ERROR,
        VERBOSE
    };

    inline Severity convertSeverity(const VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
        switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return Severity::VERBOSE;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: return Severity::INFO;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return Severity::WARNING;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: return Severity::ERROR;
            default: return Severity::VERBOSE;
        }
    }

    inline std::tuple<std::ostream *, std::string> getSeverityInfo(const Severity severity) {
#ifdef _WIN32
        switch (severity) {
            case Severity::VERBOSE: return {&std::cout, "[VERBOSE] "};
            case Severity::INFO: return {&std::cout, "[INFO] "};
            case Severity::WARNING: return {&std::cout, "[WARNING] "};
            case Severity::ERROR: return {&std::cerr, "[ERROR] "};
        }
#else
        switch (severity) {
            case Severity::VERBOSE: return {&std::cout, "\033[36m[VERBOSE]\033[0m "};
            case Severity::INFO: return {&std::cout, "\033[32m[INFO]\033[0m "};
            case Severity::WARNING: return {&std::cout, "\033[33m[WARNING]\033[0m "};
            case Severity::ERROR: return {&std::cerr, "\033[31m[ERROR]\033[0m "};
        }
#endif
        return {&std::cout, "[UNKNOWN] "};
    }
}
