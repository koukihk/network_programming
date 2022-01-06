/*
 * Created by kouki on 2021/12/30.
 */
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll

#define PROXYPORT 8471//����������Ķ˿�
#define WEBPORT 80//HTTP�˿�
#define BUFFSIZE 25600//��������С

//ȫ�ֱ���
int nByte;//�ֽ���
char localData[BUFFSIZE];//�����Ļ�����
char serverData[BUFFSIZE];//���ر��Ļ�����
char domin[256];//��������
char ip[256];//IP����
char blacklist[256];//������
char blockPage[200] = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 500\n\n<html><head><title>Naive!</title></head><body><h1>403</h1><h2>Forbidden</h2></body></html>";//403����

void ShowClientInfo(char *dataSend) { //��ʾ�ͻ�����Ϣ
    //------------------------------------------------
    //--------------���������Ķε���Ϣ--------------
    //------------------------------------------------

    //********ץ��������IP************
    int i, j;
    memset(domin, 0, sizeof(domin));  //�����������
    memset(domin, 0, sizeof(ip));  //���IP����

    for (i = 0 ; i < strlen(dataSend); i++) {
        if ( !strncmp(dataSend + i, "Host:", 5) ) {
            for (j = 0, i = i + 6 ; dataSend[i] != ':' ; i++, j++)
                domin[j] = dataSend[i];
            printf("----------------------------\n");
            printf("���ڷ���:��������������%s\n", domin);
            break;
        }
    }

    if (i == strlen(dataSend))
        printf("������û�з�������������IP��Ϣ");

    //********������ת����IP********

    if (i != strlen(dataSend)) {
        char hostIP[256] = {0};
        if (domin[0] >= 0 && domin[0] <= 9) {
            strcpy(hostIP, domin);
            printf("IP addr: %s\n", hostIP );
        } else {
            struct hostent *host = gethostbyname(domin);
            if (!host)
                puts("�޷�����IP��ַ!");
            else
                for (i = 0; host->h_addr_list[i]; i++)
                    strcpy(hostIP, inet_ntoa( *(struct in_addr*)host->h_addr_list[i] )); //ֻȡ���һ��IP

            printf("IP ��ַ %d: %s\n", i + 1, hostIP);
        }
        ///
        strcpy(ip, hostIP);
        ///
    }

    //********ץhttp�汾�� *********
    char HttpVersion[10];
    for(i=0; i<=strlen(dataSend); i++)
    {
        if(!strncmp(dataSend+i,"HTTP/",5))
            strncpy(HttpVersion,dataSend+i,9);//HTTP/1.0����HTTP/1.1��8���ַ������Ǻ��滹��һ��'\0'
    }

    printf(" HTTP�İ汾���ǣ�%s \n",HttpVersion);

    //********ץ��ʾ����************
    char Language[100];
    for(i=0; i<strlen(dataSend); i++)
    {
        if(!strncmp(dataSend+i,"Accept-Language:",15))
        {
            for(j=0,i=i+16; dataSend[i+1] != '\n'; i++,j++)
                Language[j] =dataSend[i];
            Language[j] ='\0';
            printf("��ʾ�����ǣ�%s \n",Language);
            break;
        }
    }

    if(i==strlen(dataSend))
        printf("������û����ʾ������Ϣ\n");

    //********ץ����ϵͳ�汾��	*********
    char version[1000];
    for(i = 0 ; i<strlen(dataSend) ; i++)
    {
        if(!strncmp(dataSend+i,"UA-OS:",6))
        {
            for(j=0,i=i+6; dataSend[i+1] != '\n'; i++,j++)
                version[j]=dataSend[i];
            printf("����ϵͳ�汾�ǣ�%s\n",version);
            break;
        }
    }
    if(i==strlen(dataSend))
        printf("������û�в���ϵͳ�汾��Ϣ\n\n\n");
}

void ShowServerInfo(char *dataRecv) { //��ʾ����������Ϣ
    //-------------------------------------------------
    //--------------������ȡ���Ķε���Ϣ---------------
    //-------------------------------------------------

    int i, j;

    //********ץ��ʾ����************
    char Language[100];
    for(i=0; i<strlen(dataRecv); i++)
    {
        if(!strncmp(dataRecv+i,"Accept-Language:",15))
        {
            for(j=0,i=i+16; dataRecv[i+1] != '\n'; i++,j++)
                Language[j] =dataRecv[i];
            Language[j] ='\0';
            printf("��ʾ�����ǣ�%s \n",Language);
            break;
        }
    }

    if(i==strlen(dataRecv))
        printf("��������Ӧ������û����ʾ������Ϣ\n");
    //********ץ�����������Ϣ�汾��	*********

    char versionServer[1000];
    for(i=0; i<strlen(dataRecv); i++)
    {
        if(!strncmp(dataRecv+i,"Server:",7))
        {
            for(j=0,i=i+7; dataRecv[i+1] != '\n'; i++,j++)
                versionServer[j]=dataRecv[i];
            printf("�����������Ϣ�汾���ǣ�%s\n",versionServer);
            break;
        }
    }

    if(i==strlen(dataRecv))
        printf("��������Ӧ������û�з����������Ϣ�汾����Ϣ\n");

    printf("**************************************************\n\n");
}

void RecieveAndProcess(char *domin, SOCKET sSock) { //�������ݣ��������ݣ�ת������
    int err;//�����루�޴���ʱΪ0��

    SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));

    clientAddr.sin_family = AF_INET;//IPv4
    clientAddr.sin_addr.S_un.S_addr = inet_addr(ip);//��������������
    clientAddr.sin_port = htons(WEBPORT);//HTTP�˿�

    //ѭ������ֱ���ɹ�Ϊֹ
    do {
        err = connect(clientSock, (struct sockaddr*)&clientAddr, sizeof(clientAddr));  //�ͷ�������������
        if (err == SOCKET_ERROR)
            printf("����ʧ�ܣ�������: %d\n", WSAGetLastError());
    } while (err != 0);
    //nByte = 0;
    do {
        //��������ֱ��ת������������(clientSock)
        nByte = send(clientSock, localData, BUFFSIZE, 0);
        if (nByte == 0) {
            printf("�˴�δ�ܽ���Ϣ���͸�����ˡ�\n");
        }
    } while(nByte > 0);
    //nByte = 0;
    //�ӷ������˽��ܷ��صı���
    do {
        nByte = recv(clientSock, serverData, BUFFSIZE, 0);

        //���յ�������ת�����������(serverSock)
        if (nByte > 0)
            send(sSock, serverData, nByte, 0);
        else if (nByte == 0)
            printf("�����Ѷ�ʧ��\n");
        else
            printf("�޷����շ�������Ϣ��������: %d\n", WSAGetLastError() );

        memset(serverData, 0, sizeof(serverData));  //��ջ�����
    } while (nByte > 0);

    closesocket(clientSock);//�ر�socket
}

int JudgeBlocklist() {
    FILE *fp;
    fp = fopen("blacklist.txt", "r");
    char temp[256];
    int i, j;
    int r = 0;

    //���ļ�����д��ȫ�ֱ�����
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

    //��ʼ�� DLL
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);

    //�����������socket
    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//IPv4,TCP

    //��socket�����ض˿�
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;//IPv4
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//����ip
    serverAddr.sin_port = htons(PROXYPORT);//���ض˿�

    int result = bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));  //��
    if (result == SOCKET_ERROR) {
        printf("Binding failure\n");
    }


    //��ʼ������������
    result = listen(serverSock, 5);
    if (result == SOCKET_ERROR) {
        printf("Listening failure\n");
    }
    //ѭ���ȴ�
    while (1) {
        //��ʼ��������
        memset(localData, 0, sizeof(localData));
        memset(serverData, 0, sizeof(serverData));

        //�������ض�socket
        SOCKADDR localAddr;
        int addrLength = sizeof(SOCKADDR);
        SOCKET localSock = accept(serverSock, (struct sockaddr*)&localAddr, &addrLength);

        nByte = recv(localSock, localData, BUFFSIZE, 0);   //������������

        ShowClientInfo(localData);//��ʾ���������Ϣ

        //�������ҳ���ں��������򷵻�403ҳ��
        if (JudgeBlocklist()) {
            printf("Request illegal !\n");
            send(localSock, blockPage, 162, 0);   //����403ҳ��
        } else if (nByte >= 0) { //�޴���ʱִ�д������
            RecieveAndProcess(domin, localSock); //����ת������
            ShowServerInfo(serverData);//��ʾ����������Ϣ
        } else //����ʱ��ӡ������Ϣ
            printf("Recieving local request error.\nError code: %d \n", WSAGetLastError() );

        closesocket(localSock);//�رձ��ض�socket
    }

    //��ֹʹ��DLL
    WSACleanup();

    return 0;
}