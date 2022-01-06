/*
 * Created by kouki on 2021/12/27.
 */
#include <iostream>
#include <winsock.h>
#define MAXLINE 1024

using namespace std;

int main() {
	system("color f4");
	SOCKET sd;
	struct sockaddr_in serv{};

	// 键入信息
	string input;

	// 用户名
	string user;

	// 聊天室名
	string room;

	// 分隔符
	string separate = ",";

	char str[MAXLINE] = "";

	WSADATA wsadata;

	// Server IP地址
	const char server_ip[16] = "127.0.0.1";

	// Server 端口号
	u_short server_port = 5678;

	// 连接状态
	int conn_status;

	cout << "欢迎使用聊天室 client 端发送软件！" << endl;

	cout << "请输入使用者名称（唯一的）: ";
	getline(cin, user);
	cout << endl;

	cout << "请输入聊天室名称（唯一的）: ";
	getline(cin, room);
	cout << endl;

	// 调用 WSAStartup() 来注册 "WinSock DLL"
	WSAStartup(0x101, (LPWSADATA) &wsadata);

	// 开始准备连接了
	// 填充结构体 sockaddr_ing struct (serv)
	serv.sin_family = AF_INET;

	// Server的IP地址
	serv.sin_addr.s_addr = inet_addr(server_ip);

	// Server 端口号
	// htons: host to network
	serv.sin_port = htons(server_port);

	while (true) {
		// 开启一个套接字
		sd = socket(AF_INET, SOCK_STREAM, 0);

		cout << "[输入] 请输入文字: (输入 bye 离开聊天室)" << endl;

		getline(cin, input);

		// 连接到Server
		conn_status = connect(sd, (LPSOCKADDR) &serv, sizeof(serv));

		if (conn_status == SOCKET_ERROR) {
			cout << "connect function failed with error: " << WSAGetLastError() << endl;
			closesocket(sd);
			WSACleanup();
			return 1;
		}
		strcpy(str, user.c_str());
		strcat(str, separate.c_str());
		strcat(str, room.c_str());
		strcat(str, separate.c_str());
		strcat(str, input.c_str());

		// send to server
		send(sd, str, int(strlen(str) + 1), 0);
		cout << "[" << user << "于" << room << "发送] : " << input << endl;

		cout << "--------------------------------" << endl;

		if (input == "bye") {
			break;
		}
	}

	// 关闭套接字
	closesocket(sd);

	// 结束 "WinSock DLL"
	WSACleanup();

	//system("pause");

	return 0;
}
