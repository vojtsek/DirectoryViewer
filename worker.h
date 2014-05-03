#include <QWidget>
#include <QThread>
#include "osinterface.h"
#ifndef WORKER_H
#define WORKER_H

class Worker : public QThread
{
    Q_OBJECT
public:
    Worker( std::string &s1, std::string &s2):  _s1(s1), _s2(s2) {}
    ~Worker() {}
private:
    std::string _s1, _s2;
    //void (OSInterface::*_fnc)(std::string &, std::string &);
    void run();
signals:
    void done();
};

#endif // WORKER_H
