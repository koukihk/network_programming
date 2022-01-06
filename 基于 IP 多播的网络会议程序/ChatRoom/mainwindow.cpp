#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#define MCASTADDR "233.0.0.1" //����ʹ�õĶಥ���ַ��
#define MCASTPORT 5150 //���ض˿ںš�
#define BUFSIZE 1024 //�������ݻ����С��

//#pragma comment(lib,"ws2_32")
using namespace std;
SOCKET sock,sockM;
struct sockaddr_in remote,local;


void MainWindow::receiver(char* recvbuf,char* ipadd){
    cout<<ipadd << "+"<<recvbuf <<endl;
    QString recvip = QString(ipadd);
    QString recvtext = QString(recvbuf);
    ui->textEdit->textCursor().insertText("<"+recvip+"> : \n  "+recvtext+"\n");
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    WSADATA wsd;
    if( WSAStartup( MAKEWORD(2,2),&wsd) != 0 ){
        cout << "WSAStartup() failed\n" << endl;
     }
    if((sock=WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,WSA_FLAG_MULTIPOINT_C_LEAF|WSA_FLAG_MULTIPOINT_D_LEAF|WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        cout<< "socket failed with:" << WSAGetLastError() <<endl;
        WSACleanup();
    }
    //�� sock �󶨵�����ĳ�˿��ϡ�
    local.sin_family = AF_INET;
    local.sin_port = htons(MCASTPORT);
    local.sin_addr.s_addr = INADDR_ANY;
    if( bind(sock,(struct sockaddr*)&local,sizeof(local)) == SOCKET_ERROR )
    {
        cout<< "bind failed with:" << WSAGetLastError() <<endl;
        closesocket(sock);
        WSACleanup();
    }
     remote.sin_family = AF_INET;
     remote.sin_port = htons(MCASTPORT);
     remote.sin_addr.s_addr = inet_addr( MCASTADDR );
     if(( sockM = WSAJoinLeaf(sock,(SOCKADDR*)&remote,sizeof(remote),NULL,NULL,NULL,NULL,JL_BOTH)) == INVALID_SOCKET)
     {
            cout<< "WSAJoinLeaf() failed: " << WSAGetLastError() <<endl;
            closesocket(sock);
            WSACleanup();
     }
     Thread = new TestThread();//�½��߳�
     connect(Thread, SIGNAL(TestSignal(char*,char*)), this, SLOT(receiver(char*,char*)));
     Thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString sendtext = ui->lineEdit->text();
    char sendbuf[BUFSIZE];
    strcpy(sendbuf,sendtext.toStdString().c_str());
    cout <<"sendbuf: "<< sendbuf <<endl;
    if( sendto(sockM,(char*)sendbuf,strlen(sendbuf),0,(struct sockaddr*)&remote,sizeof(remote))==SOCKET_ERROR)
     {
            printf("sendto failed with: %d\n",WSAGetLastError());
            closesocket(sockM);
            closesocket(sock);
            WSACleanup();
     }
    cout <<"send sucessfully."<<endl;
    ui->lineEdit->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    exit(0);
    closesocket(sockM);
    closesocket(sock);
    WSACleanup();
}
