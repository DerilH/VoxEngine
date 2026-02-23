#pragma once

#include <string>

#define QUILL_DISABLE_NON_PREFIXED_MACROS
#define QUILL_COMPILE_ACTIVE_LOG_LEVEL -1

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include <string_view>

#include "Backward.h"
#include "Severity.h"
#include "Time.h"

#define LOG_VERBOSE(msg, ...) QUILL_LOG_TRACE_L3(Vox::Log::Logger::GetInternal(), msg, ##__VA_ARGS__);
#define LOG_INFO(msg, ...) QUILL_LOG_INFO(Vox::Log::Logger::GetInternal(), msg, ##__VA_ARGS__);
#define LOG_DEBUG(msg, ...) QUILL_LOG_DEBUG(Vox::Log::Logger::GetInternal(), msg, ##__VA_ARGS__);
#define LOG_WARN(msg, ...) QUILL_LOG_WARNING(Vox::Log::Logger::GetInternal(), msg, ##__VA_ARGS__);
#define LOG_ERROR(msg, ...) QUILL_LOG_ERROR(Vox::Log::Logger::GetInternal(), msg, ##__VA_ARGS__);
#define LOG_CRITICAL(msg, ...) QUILL_LOG_CRITICAL(Vox::Log::Logger::GetInternal(), msg, ##__VA_ARGS__);

#define LOG_TRACE() {\
            backward::StackTrace st;\
            st.load_here(32);\
            backward::Printer pt;\
            pt.print(st);\
        }\

namespace Vox::Log {

    class Logger : public SingletonBase<Logger> {
        quill::Logger *mLogger;
    public:
        Logger() {
            quill::BackendOptions backendOptions;
            quill::Backend::start(backendOptions);

            auto opt = quill::PatternFormatterOptions();
            opt.format_pattern = "%(time) [%(log_level)] %(file_name):%(line_number) %(message)";

            auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1");
            mLogger = quill::Frontend::create_or_get_logger("root", std::move(console_sink), opt);
            mLogger->set_log_level(quill::LogLevel::TraceL3);
            backward::SignalHandling::addHandler(flushCrash);
        }

        static void flushCrash(int, siginfo_t *, void *) {
            GetInternal()->flush_log();
        }

        inline static quill::Logger *GetInternal() {
            return Get().mLogger;
        }
    };
//Must stay like this for preserving order of log and error
    inline const Logger& logger = Logger::Get();
    inline backward::SignalHandling sh;

    inline void log(const std::string &msg, const Severity severity = Severity::INFO) {
        switch (severity) {
            case Severity::VERBOSE:
                LOG_VERBOSE("{}", msg);
                break;
            case Severity::INFO:
                LOG_INFO("{}", msg);
                break;
            case Severity::WARNING:
                LOG_WARN("{}", msg);
                break;
            case Severity::ERROR:
                LOG_ERROR("{}", msg);
                break;
        }
    }
}
