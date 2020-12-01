#ifndef _PLATINUM_NOCOPYABLE_H_
#define _PLATINUM_NOCOPYABLE_H_

namespace platinum {

class Nocopyable {
public:
    Nocopyable() = default;

    ~Nocopyable() = default;

    Nocopyable(const Nocopyable&) = delete;
    
    Nocopyable& operator=(const Nocopyable&) = delete;
};

}; // end of platinum

#endif
