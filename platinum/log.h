#ifndef _PLATINUM_H_
#define _PLATINUM_H_

#include "singleton.h"
#include "mutex.h"
#include "util.h"

#include <iostream>
#include <stdarg.h>
#include <time.h>
#include <string>
#include <sstream>
#include <fstream>

#define LOG_LEVEL(level, fmt, ...) \
    if(platinum::LogMgr::GetInstance()->getLevel() < level) \
        platinum::LogMgr::GetInstance()->log(__FILE__, __LINE__, time(0), fmt, __VA_ARGS__);

#define LOG_DEBUG(fmt, ...) LOG_LEVEL(platinum::Log::Level::DEBUG, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...)  LOG_LEVEL(platinum::Log::Level::INFO, fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG_LEVEL(platinum::Log::Level::WARN, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_LEVEL(platinum::Log::Level::ERROR, fmt, __VA_ARGS__)
#define LOG_FATAL(fmt, ...) LOG_LEVEL(platinum::Log::Level::FATAL, fmt, __VA_ARGS__)
        

namespace platinum {
class Log {
public:
    typedef Spinlock MutexType;
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL =5
    };
    Log();

    Level getLevel() { return m_level; }
    void setLevel(Level val) { m_level = val; }

    void log(const char * file, const int line, uint64_t timeval, const char* fmt, ...);
    void format(const char * file, const int line, uint64_t timeval, const char* fmt, va_list al);
private:
    bool reopen();

private:
    Level m_level = Level::DEBUG;
    MutexType m_mutex;
    std::string m_filename = "./server.log";
    std::ofstream m_filestream;
    
    uint64_t m_lastTime = 0;
};

typedef platinum::Singleton<Log> LogMgr;

} // end of paltinum


#endif
