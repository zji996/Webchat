#include "webserver.h"
#include <fstream>

namespace platinum {
#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: platinumhttpd/0.1.0\r\n"
#define STDIN   0
#define STDOUT  1
#define STDERR  2

void accept_request(void *);
void bad_request(int);
void cat(int, std::ifstream&);
void cannot_execute(int);
void error_die(const char *);
int get_line(int, char *, int);
void not_found(int);
void serve_file(int, const char *);
void unimplemented(int);
void headers(int, const char *, size_t);

void bad_request(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(client, buf, sizeof(buf), 0);
}

void cat(int client, std::ifstream & resource)
{
    char buf[1024];
    size_t flength = 0;

    while (!resource.eof())
    {
        resource.read(buf, 1024);
        flength += resource.gcount();
        send(client, buf, resource.gcount(), 0);
    }
}

void cannot_execute(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, strlen(buf), 0);
}

void error_die(const char *sc)
{
    perror(sc);
    exit(1);
}

bool WebServer::execute_servlet(int client, const char *path,
        const char *method, const char *query_string) {
    std::cout << "servlet method=" << method << " path=" << path << std::endl;
    char buf[1024];
    int numchars = 1;
    int content_length = -1;

    buf[0] = 'A'; buf[1] = '\0';
    if (strcasecmp(method, "GET") == 0)
        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
            numchars = get_line(client, buf, sizeof(buf));
    else if (strcasecmp(method, "POST") == 0) /*POST*/
    {
        numchars = get_line(client, buf, sizeof(buf));
        while ((numchars > 0) && strcmp("\n", buf))
        {
            buf[15] = '\0';
            if (strcasecmp(buf, "Content-Length:") == 0)
                content_length = atoi(&(buf[16]));
            numchars = get_line(client, buf, sizeof(buf));
        }
        if (content_length == -1) {
            bad_request(client);
            return true;
        }
    }
    else/*HEAD or other*/
    {
    }
	
	char* strbody = new char[content_length + 1];
	memset(strbody, 0, content_length + 1);
	readn(client, strbody, content_length);
    
    std::string formatpath = path;
    std::string::size_type ppos = formatpath.find_first_of('\?');
    if(ppos == std::string::npos)
        formatpath = formatpath.substr(0, ppos);
    //handle the request
    std::string methodstr;
    if (strcasecmp(method, "GET") == 0) {
        methodstr="GET";
        methodstr+=formatpath;
        auto pre = m_register.find(methodstr);
        if(pre != m_register.end()) {
            m_work_threadpool.addTask(std::bind(m_register[methodstr], client, strbody));
            return false;
        }else {
            bad_request(client);
        }
    }
    else if (strcasecmp(method, "POST") == 0) /*POST*/ {
        methodstr="POST";
        methodstr+=formatpath;
        std::cout << "servlet POST mm="<< methodstr << std::endl;
        auto pre = m_register.find(methodstr);
        if(pre != m_register.end()) {
            m_work_threadpool.addTask(std::bind(m_register[methodstr], client, strbody));
            std::cout << "servlet regist" << std::endl;
            LOG_INFO("servlet POST path = %s", methodstr.c_str());
            return false;
        }else {
            bad_request(client);
        }
    }
    return true;
}

int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                {
                    recv(sock, &c, 1, 0);
                }
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';

    return(i);
}

bool isImg(std::string ftype) {
    if(ftype == "webp" || ftype == "png" ||
            ftype == "gif" || ftype == "jpg" ||
            ftype == "jpeg" || ftype == "bmp" ) {
        return true;
    }
    return false;
}

void WebServer::Headers(int client, size_t fsize) {
    char buf[1024];
    std::string ftype = "text/html";

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: %ld\r\n", fsize);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: %s\r\n", ftype.c_str());
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "Content-Transfer-Encoding: binary\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

void headers(int client, const char *filename, size_t fsize)
{
    char buf[1024];
    (void)filename;  /* could use filename to determine file type */
    std::string fname = filename;
    std::string ftype = "html";

    std::string::size_type pos = fname.find_last_of('.');
    if(pos != std::string::npos) {
       ftype = fname.substr(pos + 1); 
    }
    if(isImg(ftype)) {
        ftype = "image/" + ftype;
    } else {
        ftype = "text/html";
    }

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: %ld\r\n", fsize);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: %s\r\n", ftype.c_str());
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "Content-Transfer-Encoding: binary\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

void not_found(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "your request because the resource specified\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "is unavailable or nonexistent.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

void serve_file(int client, const char *filename, size_t fsize)
{
    std::ifstream resource(filename, std::ios::in|std::ios::binary);
    int numchars = 1;
    char buf[1024];

    buf[0] = 'A'; buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
        numchars = get_line(client, buf, sizeof(buf));

    if (!resource.is_open()) {
        not_found(client);
        return;
    }
    else
    {
        headers(client, filename, fsize);
        cat(client, resource);
    }
    LOG_INFO("GET %s", filename);

    if(resource.is_open())
        resource.close();
}

void unimplemented(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

void WebServer::run() {
    m_accept_threadpool.run();
    m_work_threadpool.run();
    init();
    m_started = true;
    acceptloop();
}

void WebServer::init() {
    m_fd = 0;
    int on = 1;

    m_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (m_fd == -1) {
        perror("socket");
        exit(1);
    }

    memset(&m_sockaddr, 0, sizeof(m_sockaddr));
    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_port = htons(m_port);
    m_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {  
        perror("setsockopt");
        exit(1);
    }
    if (bind(m_fd, (struct sockaddr *)&m_sockaddr, sizeof(m_sockaddr)) < 0) {
        perror("bind");
        exit(1);
    }
    if (m_port == 0)  {
        socklen_t namelen = sizeof(m_sockaddr);
        if (getsockname(m_fd, (struct sockaddr *)&m_sockaddr, &namelen) == -1) {
            perror("getsockname");
            exit(1);
        }
        m_port = ntohs(m_sockaddr.sin_port);
    }
    if (listen(m_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }
}

void WebServer::stop() {
    close(m_fd);
    m_started = false;
}

void WebServer::acceptloop() {
	int cfd = -1;
	struct sockaddr_in csockaddr;
    socklen_t  csockaddr_len = sizeof(csockaddr);
	while (1)
    {
        cfd = accept(m_fd, (struct sockaddr *)&csockaddr, &csockaddr_len);
        if (cfd == -1) {
            perror("accept");
            exit(1);
		};

        m_accept_threadpool.addTask(std::bind(&WebServer::handlerequest, this, cfd));
    }
}

void WebServer::handlerequest(int client) {
    char buf[1024];
    size_t numchars;
    char method[255];
    char url[255];
    char path[512];
    size_t i, j;
    struct stat st;
    int cgi = 0;
    bool f_close = true;
    char *query_string = NULL;

    numchars = get_line(client, buf, sizeof(buf));
    i = 0; j = 0;
    while (!ISspace(buf[i]) && (i < sizeof(method) - 1))
    {
        method[i] = buf[i];
        i++;
    }
    j=i;
    method[i] = '\0';

    if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
    {
        unimplemented(client);
        return;
    }

    if (strcasecmp(method, "POST") == 0) {
        cgi = 1;
        std::cout << "POST" << std::endl;
    }

    i = 0;
    while (ISspace(buf[j]) && (j < numchars))
        j++;
    while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < numchars))
    {
        url[i] = buf[j];
        i++; j++;
    }
    url[i] = '\0';

    if (strcasecmp(method, "GET") == 0)
    {
        query_string = url;
        while ((*query_string != '?') && (*query_string != '\0'))
            query_string++;
        if (*query_string == '?')
        {
            cgi = 1;
            *query_string = '\0';
            query_string++;
        }
    }

    std::cout << "POST1" << std::endl;
    sprintf(path, "home%s", url);
    if (path[strlen(path) - 1] == '/')
        strcat(path, "index.html");
    if (stat(path, &st) == -1) {
        if(0 == cgi) {
            while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
                numchars = get_line(client, buf, sizeof(buf));
            not_found(client);
            close(client);
            return;
        }
    }
    else
    {
        if ((st.st_mode & S_IFMT) == S_IFDIR)
            strcat(path, "/index.html");
        if (!cgi)
            serve_file(client, path, st.st_size);
           f_close = execute_servlet(client, path, method, query_string);
    }
    if(cgi) {
        f_close = execute_servlet(client, path, method, query_string);
    }

    if(f_close) {
        close(client);
    }
}
}; // end of platinum
