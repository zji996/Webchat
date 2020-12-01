#include "log.h"

namespace platinum {
Log::Log() {
    m_lastTime = time(0);
    reopen();
}

void Log::log(const char * file, const int line, uint64_t timeval, const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(file, line, timeval, fmt, al);
    va_end(al);
}

void Log::format(const char * file, const int line, uint64_t timeval, const char* fmt, va_list al) {
    char * buf = nullptr;
    if(timeval >= (m_lastTime + 3)) {
        reopen();
        m_lastTime = timeval;
    }
    int len = vasprintf(&buf, fmt, al);
    if(len != -1) {
        m_filestream << timeval << "[" << file << ":" << line << "][ThreadID="<< GetThreadId() << "]" << std::string(buf, len) << std::endl;;
        free(buf);
    }
}

bool Log::reopen() {
    MutexType::Lock lock(m_mutex);
    if(m_filestream) {
        m_filestream.close();
    }
    return FSUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
}

}; // end of platinum
