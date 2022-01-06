/*
 * Created by kouki on 2021/12/27.
 */
#include <iostream>
#include <winsock.h>
#define MAXLINE 1024

using namespace std;

string get_user(const string &input);

string get_room(const string &input);

string get_msg(const string &input);

int main() {
	system("color f4");
	SOCKET sd;
	struct sockaddr_in serv{};

	// 用户名
	string user;
	string now_room;
	string now_user;
	string now_msg;

	// 聊天室名
	string room;

	char str[MAXLINE] = "";

	WSADATA wsadata;

	// Server IP地址
	const char server_ip[16] = "127.0.0.1";

	// Server的端口号
	u_short server_port = 5678;

	// 用来存接受的数据长度
	int rec_len;

	// 连接状态
	int conn_status;

	cout << "欢迎使用聊天室 client 端显示软件！" << endl;

	cout << "请输入使用者名称（唯一的）: ";
	getline(cin, user);
	cout << endl;

	cout << "请输入聊天室名称（唯一的）: ";
	getline(cin, room);
	cout << endl;

	// 调用 WSAStartup() 来注册 "WinSock DLL"
	WSAStartup(0x101, (LPWSADATA) &wsadata);

	// 开启一个套接字
	sd = socket(AF_INET, SOCK_STREAM, 0);

	// 开始准备连接了
	// 填充结构体 sockaddr_in struct (serv)
	serv.sin_family = AF_INET;

	// Server IP地址
	serv.sin_addr.s_addr = inet_addr(server_ip);

	// Server的端口号
	// htons: host to network
	serv.sin_port = htons(server_port);

	// 连接到Server
	conn_status = connect(sd, (LPSOCKADDR) &serv, sizeof(serv));

	if (conn_status == SOCKET_ERROR) {
		cout << "connect function failed with error: " << WSAGetLastError() << endl;
		closesocket(sd);
		WSACleanup();
		return 1;
	}

	while (true) {
		// 从Server接收信息
		rec_len = recv(sd, str, MAXLINE, 0);

		if (rec_len <= 0) {
			cout << "[错误] 接收信息错误" << endl;
			break;
		}

		// 接收并打印信息

		// 聊天室判断

		now_user = get_user(str);
		now_room = get_room(str);
		now_msg = get_msg(str);
		//cout << "[user]: " << now_user << "[room]: " << now_room << "[msg] : " << now_msg << endl;

		if (now_room == room) {
			if (now_user == user) {
				// my self
				cout << "[我自己]:\t" << get_msg(str) << endl;
			} else {
				// other people
				cout << "[" << get_user(str) << "]:\t" << get_msg(str) << endl;
			}
		}
		//cout << "[信息 ]: " << str << endl;


		if (now_msg == "bye") {
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
//一堆getter
string get_user(const string &input) {
	return input.substr(0, input.find(','));
}

string get_room(const string &input) {
	// no room
	string no = input.substr(input.find(',') + 1);
	return no.substr(0, no.find(','));
}

string get_msg(const string &input) {
	string no;
	// no room
	no = input.substr(input.find(',') + 1);
	// no user
	no = no.substr(no.find(',') + 1);
	return no.substr(0, no.find(','));
}
