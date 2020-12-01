#ifndef _PLATINUM_WEBSERVER_H_
#define _PLATINUM_WEBSERVER_H_

#include "log.h"
#include "threadpool.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>

#include <map>

namespace platinum {

class WebServer {
public:
    typedef std::function<void(int,char*)> Func;
    enum Method {
        GET = 0,
        POST = 1
    };
    WebServer() = delete;
    WebServer(int port, int threadnum1, int threadnum2): m_port(port),
            m_threadnum1(threadnum1),
            m_threadnum2(threadnum2),
            m_accept_threadpool(threadnum1),
            m_work_threadpool(threadnum2) {
    }
    void run();
    void stop();
    void bindMethodPath(const std::string& str, const WebServer::Method& method, std::function<void(int,char*)> cb) {
            std::string methodstr;
        if(method == WebServer::GET)
            methodstr = "GET";
        else if(method == WebServer::POST)
            methodstr = "POST";
        else {
            return;
        }
        methodstr += "home";
        methodstr += str;
        std::cout << "init---------------" <<methodstr << std::endl;
        if(!m_started)
            m_register[methodstr] = cb;
    }

    static void Headers(int, size_t);
private:
    void init();
    void acceptloop();
    void handlerequest(int client);
    bool execute_servlet(int, const char *, const char *, const char *);

private:
    int m_port = 80;
    int m_fd;
    int m_threadnum1;
    int m_threadnum2;
    bool m_started =false;
    struct sockaddr_in m_sockaddr;
    ThreadPool m_accept_threadpool;
    ThreadPool m_work_threadpool;
    std::map<std::string, Func> m_register;
};
};// end of platinum

#endif
