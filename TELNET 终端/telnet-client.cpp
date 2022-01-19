/*
 * Created by kouki on 2022/1/2.
 */
#include <winsock2.h>
#include <iostream>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

char ip[20] = {0};

bool CreateClientSocket()
{
	BOOL bRet = FALSE;
	do
	{
		//创建套接字
		SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == client)
		{
			SetLastError(WSAGetLastError());
			break;
		}
		//连接服务器socke地址
		sockaddr_in server_addr = { 0 };
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(23); 
		server_addr.sin_addr.S_un.S_addr = inet_addr(ip);

		if (SOCKET_ERROR == connect(client, (sockaddr*)&server_addr, sizeof(server_addr)))
		{
			SetLastError(WSAGetLastError());
			break;
		}


		//发送数据

		char sz_send_info[1024] = { 0 };
		int iRet = 0;
		while (1)
		{
			printf("----------------\n");
			printf("输入命令： \n");
			std::cin.getline(sz_send_info,sizeof(sz_send_info));
			iRet=send(client, (char*)sz_send_info, sizeof(sz_send_info), 0);
			if (iRet!=SOCKET_ERROR)
			{
				printf("已发送 %s\n",sz_send_info);
			}
			else
			{
				printf("发送失败了\n");
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
	memset(ip,'\0',sizeof(ip));
	printf("请输入主机地址（使用端口号为23）： \n");
	scanf("%s",ip);
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {

		printf("出错了，错误码为: %d\n", err);
		return -1;
	}


	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		//没有合适的版本号
		printf("没有合适的版本号\n");
		WSACleanup();
		return -1;
	}

	if (FALSE == CreateClientSocket())
	{
		printf("出错了，错误码为: %d\n", GetLastError());
		return -1;
	}
	system("pause");
	WSACleanup();
	return 0;
}
