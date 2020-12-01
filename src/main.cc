#include <iostream>
#include <string>
#include <vector>
#include "platinum/log.cc"
#include "platinum/thread.h"
#include "platinum/threadpool.h"
#include "platinum/webserver.h"
#include "src/webchat.h"

platinum::WebChat webchat("./chathistory");

void login(int fd, char* bodystr) {
    char str[10] = "haha";
    platinum::WebServer::Headers(fd, 5);
    platinum::writen(fd, str, 5);
    delete [] bodystr;
    close(fd);
}

void pullmessage(int fd, char* bodystr) {
    std::string str = bodystr;
    std::string anstr;
    
    std::cout << str << std::endl;

    webchat.getWords(anstr);
    std::cout << "anstr = " << anstr << std::endl;
    platinum::WebServer::Headers(fd, anstr.size());
    platinum::writen(fd, anstr.c_str(), anstr.size());
    delete [] bodystr;
    close(fd);
}

void pushmessage(int fd, char* bodystr) {
    std::string str = bodystr;

    std::cout << str << std::endl;

    uint64_t now = time(0);
    webchat.saveWords(now, str);
    char str1[10] = "good";
    platinum::WebServer::Headers(fd, 6);
    platinum::writen(fd, str1, 6);
    delete [] bodystr;
    close(fd);
}

int main() {
    platinum::WebServer ws(80, 5, 5);



    ws.bindMethodPath("/login", platinum::WebServer::Method::POST, login);
    ws.bindMethodPath("/pullmessage", platinum::WebServer::Method::POST, pullmessage);
    ws.bindMethodPath("/pushmessage", platinum::WebServer::Method::POST, pushmessage);

    webchat.run();
    ws.run();
    return 0;
}
