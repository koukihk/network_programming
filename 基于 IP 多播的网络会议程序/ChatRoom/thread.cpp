#include "thread.h"
#include "mainwindow.h"

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#define MCASTADDR "233.0.0.1" //����ʹ�õĶಥ���ַ��
#define MCASTPORT 5150 //���ض˿ںš�
#define BUFSIZE 1024 //�������ݻ����С��
using namespace std;

struct sockaddr_in from;

TestThread::TestThread(QObject *parent) :
    QThread(parent)
{
}

void TestThread::run()
{
    //�����ź�
    char recvbuf[BUFSIZE];
    /*struct ip_mreq mcast; // Winsock1.0 */
    int len = sizeof( struct sockaddr_in);
    int ret;
    while(1){
        if(( ret = recvfrom(sock,recvbuf,BUFSIZE,0,(struct sockaddr*)&from,&len)) == SOCKET_ERROR){
            printf("recvfrom failed with:%d\n",WSAGetLastError());
            closesocket(sockM);
            closesocket(sock);
            WSACleanup();
        }
        recvbuf[ret] = '\0';
        emit TestSignal(recvbuf,inet_ntoa(from.sin_addr));
    }
}
