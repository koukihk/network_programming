/*
 * Created by kouki on 2022/1/2.
 */
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include<windows.h>
#pragma comment(lib, "ws2_32.lib")

bool CreateServerSocket()
{
	printf("���ڵȴ�����......\n");
	BOOL bRet=FALSE;
	do
	{
		//�����׽���
		SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET== server)
		{
			SetLastError(WSAGetLastError());
			break;
		}

		//��Socket������
		sockaddr_in server_addr = {0};
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(23); 
		server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//���ػ���

		if (SOCKET_ERROR == bind(server, (sockaddr*)&server_addr, sizeof(server_addr)))
		{
			SetLastError(WSAGetLastError());
			break;
		}

		//��������
		if (SOCKET_ERROR == listen(server, 10))
		{
			SetLastError(WSAGetLastError());
			break;
		}
		//������������
		sockaddr_in client_addr = {0};
		int len = sizeof(client_addr);
		SOCKET client=accept(server, (sockaddr*)&client_addr, &len);
		if (INVALID_SOCKET == client)
		{
			SetLastError(WSAGetLastError());
			break;
		}
		//��������
		printf("���ӳɹ���\n");
		char szInfo[1024] = { 0 };
		int iRet=-1;
		while (1)
		{
			//printf("----------------\n");
			//printf("������ȡ���\n");
			memset(szInfo,'\0',sizeof(szInfo));
			iRet = recv(client, szInfo, 1013, 0);

			if (0 >= iRet)
			{
				continue;
			}
			else
			{
				//printf("----------------\n");
				printf("%s\n",szInfo);
				system(szInfo);   //��ʼ�������ݴ���
				iRet = -1;
			}
		}

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

int main()
{
	system("color f4");
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {

		printf("�����ˣ�������Ϊ: %d\n", err);
		return -1;
	}


	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
	//ȡ��16��������ͻ��
		printf("û�к��ʵİ汾��\n");
		WSACleanup();
		return -1;
	}

	if (FALSE==CreateServerSocket())
	{
		printf("�����ˣ�������Ϊ: %d\n",GetLastError());
		return -1;
	}
	system("pause");
	WSACleanup();
	return 0;
}
