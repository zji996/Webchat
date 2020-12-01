#ifndef _PLATINUM_UTIL_H_
#define _PLATINUM_UTIL_H_

#include <fstream>
#include <iostream>

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <string.h>
#include <time.h>

namespace platinum {

pid_t GetThreadId();

class FSUtil {
public:
    static bool Mkdir(const std::string& dirname);
    static std::string Dirname(const std::string& filename);
    static bool OpenForWrite(std::ofstream& ofs, const std::string& filename, std::ios_base::openmode mode);
};

std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");
int Time2String(std::string &strDateStr,const time_t &timeData);

size_t readn(int fd, char * buf, size_t count);

size_t writen(int fd, const char * buf, size_t count);

}; //end of platinum
#endif
