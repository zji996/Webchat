#include "util.h"

namespace platinum {

pid_t GetThreadId() { return syscall(SYS_gettid);};

static int __lstat(const char* file, struct stat* st = nullptr) {
    struct stat lst;
    int ret = lstat(file, &lst);
    if(st) {
        *st = lst;
    }   
    return ret;
}

static int __mkdir(const char* dirname) {
    if(access(dirname, F_OK) == 0) {
        return 0;
    }   
    return mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

bool FSUtil::Mkdir(const std::string& dirname) {
    if(__lstat(dirname.c_str()) == 0) {
        return true;
    }
    char* path = strdup(dirname.c_str());
    char* ptr = strchr(path + 1, '/');
    do {
        for(; ptr; *ptr = '/', ptr = strchr(ptr + 1, '/')) {
            *ptr = '\0';
            if(__mkdir(path) != 0) {
                break;
            }
        }
        if(ptr != nullptr) {
            break;
        } else if(__mkdir(path) != 0) {
            break;
        }
        free(path);
        return true;
    } while(0);
    free(path);
    return false;
}

std::string FSUtil::Dirname(const std::string& filename) {
    if(filename.empty()) {
        return ".";
    }   
    auto pos = filename.rfind('/');
    if(pos == 0) {
        return "/";
    } else if(pos == std::string::npos) {
        return ".";
    } else {
        return filename.substr(0, pos);
    }   
}

bool FSUtil::OpenForWrite(std::ofstream& ofs, const std::string& filename
                                ,std::ios_base::openmode mode) {
    ofs.open(filename.c_str(), mode);
    if(!ofs.is_open()) {
        std::string dir = Dirname(filename);
        Mkdir(dir);
        ofs.open(filename.c_str(), mode);
    }
    return ofs.is_open();
}

size_t readn(int fd, char * buf, size_t count)
{
	size_t nleft = count;
	char * pbuf = buf;
	while(nleft > 0)
	{
		int nread = ::read(fd, pbuf, nleft);
		if(-1 == nread)
		{
			if(errno == EINTR)//
				continue;
			return EXIT_FAILURE;
		}
		else if(0 == nread) //EOF
		{
			break;
		}
		pbuf += nread;
		nleft -= nread;
	}
	return (count - nleft);
}


size_t writen(int fd, const char * buf, size_t count)
{
	size_t nleft = count;
	const char * pbuf = buf;
	while(nleft > 0)
	{
		int nwrite = ::write(fd, pbuf, nleft);
		if(nwrite == -1)
		{
			if(errno == EINTR)
				continue;
			return EXIT_FAILURE;
		}
		nleft -= nwrite;
		pbuf += nwrite;
	}
	return (count - nleft);
}

std::string Time2Str(time_t ts, const std::string& format) {
    struct tm tm; 
    localtime_r(&ts, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), format.c_str(), &tm);
    return buf;
}

int Time2String(std::string &strDateStr,const time_t &timeData)
{
    char chTmp[100];
    bzero(chTmp,sizeof(chTmp));
    struct tm *p;
    p = localtime(&timeData);
    p->tm_year = p->tm_year + 1900;
    p->tm_mon = p->tm_mon + 1;

    snprintf(chTmp,sizeof(chTmp),"%04d-%02d-%02d %02d:%02d:%02d",
            p->tm_year, p->tm_mon, p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
    strDateStr = chTmp;
    return 0;
}

}; // end of platinum
