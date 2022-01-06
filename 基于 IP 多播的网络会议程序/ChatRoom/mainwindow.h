#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "thread.h"

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
class TestThread;
//全局变量声明
extern struct sockaddr_in remote,local,from;
extern SOCKET sock,sockM;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void receiver(char*,char*);
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    TestThread *Thread;
};

#endif // MAINWINDOW_H
