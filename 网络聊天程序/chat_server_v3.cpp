/*
 * Created by kouki on 2021/12/27.
 */
#include <iostream>
#include <cstring>
#include <winsock.h>
#define MAXLINE 1024

using namespace std;

struct translate_word {
	string english;
	string chinese;
};

struct bad_word {
	string origin;
	string cover;
};

const int translate_word_num = 5;
const int bad_word_num = 7;
//提供简单的翻译功能
translate_word translate_word_list[translate_word_num] = {
		{"hello",       "你好"},
		{"hi",          "嗨"},
		{"how are you", "你好吗"},
		{"I'm find",    "我很好"},
		{"What's up",    "怎么了"}
};
//不能说的脏话！
bad_word bad_word_list[bad_word_num] = {
		{"bullshit", "bullsxxt"},
		{"bitch",    "bixxh"},
		{"fuck",     "fxxk"},
		{"cnm",     "真可爱"},
		{"tmd",     "甜蜜的"},
		{"sb",     "xx"},
		{"傻逼",     "傻宝"}
};


string cover_bad(const string &input);

string translate(const string &input);

string get_user(const string &input);

string get_room(const string &input);

string get_msg(const string &input);

int main() {
	system("color f5");
	// 使用人数
	int cli_num = 3;

	// 一堆属性
	string now_room;
	string now_user;
	string now_msg;

	string tmp;

	cout << "欢迎使用聊天室Server端！" << endl;
	cout << "请输入使用人数: ";
	cin >> cli_num;

	SOCKET serv_sd;
	SOCKET clis_sd;
	SOCKET cli_sd[cli_num];

	int clis_len;
	int cli_len[cli_num];

	char str_recv[MAXLINE];
	char str_send[MAXLINE];

	sockaddr_in serv{};
	sockaddr_in clis{};
	sockaddr_in cli[cli_num];

	WSADATA wsadata;

	// Server IP地址
	const char server_ip[16] = "127.0.0.1";

	// Server的端口号
	u_short server_port = 5678;

	// 用来存接受的数据长度
	int rec_len;

	// 用来存发送的数据长度
	int send_len;

	// 绑定状态
	int bind_status;

	// 调用 WSAStartup() 来注册 "WinSock DLL"
	WSAStartup(0x101, (LPWSADATA) &wsadata);

	// 开启一个套接字
	serv_sd = socket(AF_INET, SOCK_STREAM, 0);

	// 开始准备连接了
	// 填充结构体 sockaddr_in struct (serv)
	serv.sin_family = AF_INET;

	// Server IP地址
	serv.sin_addr.s_addr = inet_addr(server_ip);//将一个点分十进制的IP转换成一个长整型数

	// Server的端口号
	// htons: host to network
	serv.sin_port = htons(server_port); //整型变量从主机字节顺序转变成网络字节顺序

	// 绑定
	bind_status = bind(serv_sd, (LPSOCKADDR) &serv, sizeof(serv));

	if (bind_status == SOCKET_ERROR) {
		cout << "bind function failed with error: " << WSAGetLastError() << endl;
		closesocket(serv_sd);
		WSACleanup();
		return 1;
	}

	// 调用 listen() 函数来让套接字进入监听模式
	listen(serv_sd, 5);

	for (int i = 0; i < cli_num; i++) {
		cli_len[i] = sizeof(cli[i]);

		// 接受连接！
		cout << "[信息] 等待 client " << i + 1 << " 上线" << endl;
		cli_sd[i] = accept(serv_sd, (LPSOCKADDR) &cli[i], &cli_len[i]);
		cout << "[信息] client " << i + 1 << " 已上线" << endl;
	}

	clis_len = sizeof(clis);
	while (true) {
		clis_sd = accept(serv_sd, (LPSOCKADDR) &clis, &clis_len);

		if (clis_sd == SOCKET_ERROR) {
			cout << "[错误] 无法接受信息" << endl;
			cout << "get hp error, code:" << WSAGetLastError() << endl;
			break;
		}

		//cout << "[信息] client 已上线" << endl;

		rec_len = recv(clis_sd, str_recv, MAXLINE, 0);

		if (rec_len <= 0) {
			cout << "[错误] 无法接受信息" << endl;
			break;
		}

		closesocket(clis_sd);

		//cout << "[接收] " << str_recv << endl;

		now_room = get_room(str_recv);
		now_user = get_user(str_recv);
		now_msg = get_msg(str_recv);

		cout << "[room]: " << now_room << " [user]: " << now_user << " [msg] : " << now_msg << endl;

		// 覆盖脏话
		tmp = cover_bad(string(now_msg));

		if (!tmp.empty()) {
			now_msg = tmp;
		}

		// 翻译
		tmp = translate(string(now_msg));

		if (!tmp.empty()) {
			now_msg = tmp;
		}

		tmp = now_user;
		tmp.append(",");
		tmp.append(now_room);
		tmp.append(",");
		tmp.append(now_msg);

		strcpy(str_send, tmp.c_str());

		for (int i = 0; i < cli_num; i++) {
			// 发送信息给client 1
			send_len = send(cli_sd[i], str_send, int(strlen(str_send) + 1), 0);

			if (send_len == SOCKET_ERROR) {
				cout << "[错误] 发送信息到" << i << "失败" << endl;
			}
		}

		if (now_msg == "bye") {
			break;
		}
	}

	// 关闭套接字
	closesocket(serv_sd);
	closesocket(clis_sd);
	for (int i = 0; i < cli_num; i++) {
		closesocket(cli_sd[i]);
	}

	// 结束 "WinSock DLL"
	WSACleanup();

	//system("pause");

	return 0;
}

string cover_bad(const string &input) {
	for (auto &i : bad_word_list) {
		if (input == i.origin) {
			return i.cover;
		}
	}
	return "";
}

string translate(const string &input) {
	for (auto &i : translate_word_list) {
		if (input == i.english) {
			return "(英文 -> 中文) 原文: " + input + " 翻译: " + i.chinese;
		} else if (input == i.chinese) {
			return "(中文 -> 英文) 原文: " + input + " 翻译: " + i.english;
		}
	}
	return "";
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
