/*
 * Created by kouki on 2021/12/30.
 */
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

#define PROXYPORT 8471//代理服务器的端口
#define WEBPORT 80//HTTP端口
#define BUFFSIZE 25600//缓存区大小

//全局变量
int nByte;//字节数
char localData[BUFFSIZE];//请求报文缓存区
char serverData[BUFFSIZE];//返回报文缓存区
char domin[256];//域名缓存
char ip[256];//IP缓存
char blacklist[256];//黑名单
char blockPage[200] = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 500\n\n<html><head><title>Naive!</title></head><body><h1>403</h1><h2>Forbidden</h2></body></html>";//403数据

void ShowClientInfo(char *dataSend) { //显示客户端信息
    //------------------------------------------------
    //--------------分析请求报文段的信息--------------
    //------------------------------------------------

    //********抓域名或者IP************
    int i, j;
    memset(domin, 0, sizeof(domin));  //清空域名缓存
    memset(domin, 0, sizeof(ip));  //清空IP缓存

    for (i = 0 ; i < strlen(dataSend); i++) {
        if ( !strncmp(dataSend + i, "Host:", 5) ) {
            for (j = 0, i = i + 6 ; dataSend[i] != ':' ; i++, j++)
                domin[j] = dataSend[i];
            printf("----------------------------\n");
            printf("正在访问:服务器的域名是%s\n", domin);
            break;
        }
    }

    if (i == strlen(dataSend))
        printf("报文中没有服务器域名或者IP信息");

    //********把域名转换成IP********

    if (i != strlen(dataSend)) {
        char hostIP[256] = {0};
        if (domin[0] >= 0 && domin[0] <= 9) {
            strcpy(hostIP, domin);
            printf("IP addr: %s\n", hostIP );
        } else {
            struct hostent *host = gethostbyname(domin);
            if (!host)
                puts("无法解析IP地址!");
            else
                for (i = 0; host->h_addr_list[i]; i++)
                    strcpy(hostIP, inet_ntoa( *(struct in_addr*)host->h_addr_list[i] )); //只取最后一个IP

            printf("IP 地址 %d: %s\n", i + 1, hostIP);
        }
        ///
        strcpy(ip, hostIP);
        ///
    }

    //********抓http版本号 *********
    char HttpVersion[10];
    for(i=0; i<=strlen(dataSend); i++)
    {
        if(!strncmp(dataSend+i,"HTTP/",5))
            strncpy(HttpVersion,dataSend+i,9);//HTTP/1.0或者HTTP/1.1是8个字符，但是后面还有一个'\0'
    }

    printf(" HTTP的版本号是：%s \n",HttpVersion);

    //********抓显示语言************
    char Language[100];
    for(i=0; i<strlen(dataSend); i++)
    {
        if(!strncmp(dataSend+i,"Accept-Language:",15))
        {
            for(j=0,i=i+16; dataSend[i+1] != '\n'; i++,j++)
                Language[j] =dataSend[i];
            Language[j] ='\0';
            printf("显示语言是：%s \n",Language);
            break;
        }
    }

    if(i==strlen(dataSend))
        printf("报文中没有显示语言信息\n");

    //********抓操作系统版本号	*********
    char version[1000];
    for(i = 0 ; i<strlen(dataSend) ; i++)
    {
        if(!strncmp(dataSend+i,"UA-OS:",6))
        {
            for(j=0,i=i+6; dataSend[i+1] != '\n'; i++,j++)
                version[j]=dataSend[i];
            printf("操作系统版本是：%s\n",version);
            break;
        }
    }
    if(i==strlen(dataSend))
        printf("报文中没有操作系统版本信息\n\n\n");
}

void ShowServerInfo(char *dataRecv) { //显示服务器端信息
    //-------------------------------------------------
    //--------------分析收取报文段的信息---------------
    //-------------------------------------------------

    int i, j;

    //********抓显示语言************
    char Language[100];
    for(i=0; i<strlen(dataRecv); i++)
    {
        if(!strncmp(dataRecv+i,"Accept-Language:",15))
        {
            for(j=0,i=i+16; dataRecv[i+1] != '\n'; i++,j++)
                Language[j] =dataRecv[i];
            Language[j] ='\0';
            printf("显示语言是：%s \n",Language);
            break;
        }
    }

    if(i==strlen(dataRecv))
        printf("服务器响应报文中没有显示语言信息\n");
    //********抓服务器软件信息版本号	*********

    char versionServer[1000];
    for(i=0; i<strlen(dataRecv); i++)
    {
        if(!strncmp(dataRecv+i,"Server:",7))
        {
            for(j=0,i=i+7; dataRecv[i+1] != '\n'; i++,j++)
                versionServer[j]=dataRecv[i];
            printf("服务器软件信息版本号是：%s\n",versionServer);
            break;
        }
    }

    if(i==strlen(dataRecv))
        printf("服务器响应报文中没有服务器软件信息版本号信息\n");

    printf("**************************************************\n\n");
}

void RecieveAndProcess(char *domin, SOCKET sSock) { //接受数据，处理数据，转发数据
    int err;//错误码（无错误时为0）

    SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));

    clientAddr.sin_family = AF_INET;//IPv4
    clientAddr.sin_addr.S_un.S_addr = inet_addr(ip);//浏览器请求的域名
    clientAddr.sin_port = htons(WEBPORT);//HTTP端口

    //循环连接直到成功为止
    do {
        err = connect(clientSock, (struct sockaddr*)&clientAddr, sizeof(clientAddr));  //和服务器建立连接
        if (err == SOCKET_ERROR)
            printf("连接失败，错误码: %d\n", WSAGetLastError());
    } while (err != 0);
    //nByte = 0;
    do {
        //将请求报文直接转发给服务器端(clientSock)
        nByte = send(clientSock, localData, BUFFSIZE, 0);
        if (nByte == 0) {
            printf("此次未能将信息发送给服务端。\n");
        }
    } while(nByte > 0);
    //nByte = 0;
    //从服务器端接受返回的报文
    do {
        nByte = recv(clientSock, serverData, BUFFSIZE, 0);

        //将收到的数据转发给浏览器端(serverSock)
        if (nByte > 0)
            send(sSock, serverData, nByte, 0);
        else if (nByte == 0)
            printf("连接已丢失。\n");
        else
            printf("无法接收服务器信息，错误码: %d\n", WSAGetLastError() );

        memset(serverData, 0, sizeof(serverData));  //清空缓存区
    } while (nByte > 0);

    closesocket(clientSock);//关闭socket
}

int JudgeBlocklist() {
    FILE *fp;
    fp = fopen("blacklist.txt", "r");
    char temp[256];
    int i, j;
    int r = 0;

    //将文件内容写入全局变量中
    for (i = 0 ; blacklist[i] != EOF ; i++)
        blacklist[i] = fgetc(fp);

    i = 0;
    while (blacklist[i] != '\0' && i < 256) {
        for (j = 0 ; blacklist[i] != '\n' && blacklist[i] != ' '; i++, j++) {
            temp[j] = blacklist[i];
            if (i > 256)
                break;
        }
        if ( !(strcmp(temp, domin)) ) {
            r = 1;
            break;
        }
        memset(temp, 0, sizeof(temp));
        i++;
    }

    fclose(fp);
    return r;
}

int main(int argc, char *argv[]) {

    //system("color f4");

    int err;

    //初始化 DLL
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);

    //创建浏览器端socket
    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//IPv4,TCP

    //绑定socket到本地端口
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;//IPv4
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//本地ip
    serverAddr.sin_port = htons(PROXYPORT);//本地端口

    int result = bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));  //绑定
    if (result == SOCKET_ERROR) {
        printf("Binding failure\n");
    }


    //开始监听本地请求
    result = listen(serverSock, 5);
    if (result == SOCKET_ERROR) {
        printf("Listening failure\n");
    }
    //循环等待
    while (1) {
        //初始化缓存区
        memset(localData, 0, sizeof(localData));
        memset(serverData, 0, sizeof(serverData));

        //创建本地端socket
        SOCKADDR localAddr;
        int addrLength = sizeof(SOCKADDR);
        SOCKET localSock = accept(serverSock, (struct sockaddr*)&localAddr, &addrLength);

        nByte = recv(localSock, localData, BUFFSIZE, 0);   //接受请求数据

        ShowClientInfo(localData);//显示浏览器的信息

        //如果请求页面在黑名单内则返回403页面
        if (JudgeBlocklist()) {
            printf("Request illegal !\n");
            send(localSock, blockPage, 162, 0);   //发送403页面
        } else if (nByte >= 0) { //无错误时执行处理操作
            RecieveAndProcess(domin, localSock); //处理并转发数据
            ShowServerInfo(serverData);//显示服务器的信息
        } else //出错时打印错误信息
            printf("Recieving local request error.\nError code: %d \n", WSAGetLastError() );

        closesocket(localSock);//关闭本地端socket
    }

    //终止使用DLL
    WSACleanup();

    return 0;
}