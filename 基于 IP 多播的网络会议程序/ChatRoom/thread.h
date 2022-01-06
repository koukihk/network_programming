#ifndef TESTTHREAD_H
#define TESTTHREAD_H

#include <QThread>

#include "mainwindow.h"

class TestThread : public QThread
{
    Q_OBJECT
public:
    explicit TestThread(QObject *parent = 0);

protected:
    void run();

signals:
    void TestSignal(char*,char*);

private:
    int tnc=4;
};

#endif // TESTTHREAD_H
