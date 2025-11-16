#include "singleton.h"

Singleton& get_singleton(){
    static Singleton single;
    return single;
}