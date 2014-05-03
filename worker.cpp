#include "worker.h"


void Worker::run(){
    OSInterface::doCopy(_s1, _s2);
    exec();
}
