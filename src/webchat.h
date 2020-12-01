#ifndef _PLATINUM_WBECHAT_H_
#define _PLATINUM_WBECHAT_H_

#include "webserver.h"
#include <list>
#include <fstream>
#include <sstream>

namespace platinum {

class WebChat{
public:
    typedef Mutex MutexType;
    WebChat(const std::string& val) :m_filename(val) {
    }

    void reopen() {
        Spinlock::Lock lock(m_spin);
        if(m_filestream)
            m_filestream.close();
        FSUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
    }

    void run() {
        load();
    }

    void load() {
        Spinlock::Lock lock(m_spin);
        MutexType::Lock lock1(m_mutex);
        std::string words;
        char line[MAX_CH_SIZE];
        uint64_t n_time = 0;

        std::fstream ifstream(m_filename, std::ios::in);
        if(!ifstream.is_open()) {
            exit(1);
        }

        int i = 0;
        while(!ifstream.eof()) {
            memset(line, 0, MAX_CH_SIZE);
            i = i + 1;
            i = i % 2;
            if(1 == i) {
                ifstream.getline(line, MAX_CH_SIZE, '\n');
                int count = ifstream.gcount();
                if(count!=0) {
                    n_time = atoi(line);
                } else {
                    n_time = 0;
                }
            }
            else {
                ifstream.getline(line, MAX_CH_SIZE, '\n');
                int count = ifstream.gcount();
                if(count!=0) {
                    words = line;
                } else {
                    LOG_ERROR("WebChat load get line' size = %d", 0);
                    exit(1);
                }
                if(n_time != 0) {
                    m_historylist.push_back(std::make_pair(n_time, words));
                } else {
                    LOG_ERROR("WebChat load get n_time = %d", 0);
                    exit(1);
                }
            }
        }
        ifstream.close();
    }

    void saveWords(uint64_t timeval, const std::string& words) {
        MutexType::Lock lock(m_mutex);
        if(timeval >= (m_time + 3)) {
            reopen();
            m_time = timeval;
        }
        uint64_t utime = timeval;
        std::pair<uint64_t, std::string> lpair(utime, words);
        m_historylist.push_back(lpair);
        Spinlock::Lock lock1(m_spin);
        m_filestream << utime << std::endl;
        m_filestream << words.c_str() << std::endl;
    }
    void getWords(std::string& strHistory) {
        MutexType::Lock lock(m_mutex);
        for(auto it : m_historylist) {
            time_t ct = it.first;
            std::string tmp;
            Time2String(tmp, ct);
            strHistory+=tmp;
            strHistory+="    ";
            strHistory+=it.second;
            strHistory+='\n';
        }
    }


private:
    const int MAX_CH_SIZE = 4097;
    MutexType m_mutex;
    Spinlock m_spin;
    std::list<std::pair<uint64_t, std::string> > m_historylist;
    uint64_t m_time = 0;

    std::string m_filename;
    std::ofstream m_filestream;
};
}; // end of platinum

#endif

