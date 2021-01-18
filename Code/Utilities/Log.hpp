#ifndef _LOG_HPP
#define _LOG_HPP 1

#include <string>
#include <ostream>
#include "glm/gtx/string_cast.hpp"

#if 0

#include "spdlog/spdlog.h"

class Logger
{
public:
    static Logger& smt_Get(void);

    template<typename FormatString, typename... Args>
    void mt_Debug(const FormatString &fmt, const Args &... args)
    {
        log(level::err, fmt, args...);
    }

private:
    Logger();
    std::shared_ptr<spdlog::logger> m_Console_Logger;
};

#define LOG_trc(...) Logger::smt_Get().mt_Get_Console_Logger().trace(__VA_ARGS__)
#define LOG_inf(...) Logger::smt_Get().mt_Get_Console_Logger().info(__VA_ARGS__)
#define LOG_wrn(...) Logger::smt_Get().mt_Get_Console_Logger().warn(__VA_ARGS__)
#define LOG_err(...) Logger::smt_Get().mt_Get_Console_Logger().error(__VA_ARGS__)
#define LOG_ctr(...) Logger::smt_Get().mt_Get_Console_Logger().critical(__VA_ARGS__)

#endif // 0

extern "C"
{
#include "log.h"
}

#define LOG_dbg(...) log_debug(__VA_ARGS__)
#define LOG_inf(...) log_info(__VA_ARGS__)
#define LOG_wrn(...) log_warn(__VA_ARGS__)
#define LOG_err(...) log_error(__VA_ARGS__)
#define LOG_ctr(...) log_fatal(__VA_ARGS__)

#endif // _LOG_HPP
