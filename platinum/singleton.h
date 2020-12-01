#ifndef _PLATINUM_SINGLETON_H_
#define _PLATINUM_SINGLETON_H_

#include <memory>
#include <iostream>

namespace platinum {

template<typename T>
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

}; //end of platinum

#endif
