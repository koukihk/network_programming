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

	// �û���
	string user;
	string now_room;
	string now_user;
	string now_msg;

	// ��������
	string room;

	char str[MAXLINE] = "";

	WSADATA wsadata;

	// Server IP��ַ
	const char server_ip[16] = "127.0.0.1";

	// Server�Ķ˿ں�
	u_short server_port = 5678;

	// ��������ܵ����ݳ���
	int rec_len;

	// ����״̬
	int conn_status;

	cout << "��ӭʹ�������� client ����ʾ�����" << endl;

	cout << "������ʹ�������ƣ�Ψһ�ģ�: ";
	getline(cin, user);
	cout << endl;

	cout << "���������������ƣ�Ψһ�ģ�: ";
	getline(cin, room);
	cout << endl;

	// ���� WSAStartup() ��ע�� "WinSock DLL"
	WSAStartup(0x101, (LPWSADATA) &wsadata);

	// ����һ���׽���
	sd = socket(AF_INET, SOCK_STREAM, 0);

	// ��ʼ׼��������
	// ���ṹ�� sockaddr_in struct (serv)
	serv.sin_family = AF_INET;

	// Server IP��ַ
	serv.sin_addr.s_addr = inet_addr(server_ip);

	// Server�Ķ˿ں�
	// htons: host to network
	serv.sin_port = htons(server_port);

	// ���ӵ�Server
	conn_status = connect(sd, (LPSOCKADDR) &serv, sizeof(serv));

	if (conn_status == SOCKET_ERROR) {
		cout << "connect function failed with error: " << WSAGetLastError() << endl;
		closesocket(sd);
		WSACleanup();
		return 1;
	}

	while (true) {
		// ��Server������Ϣ
		rec_len = recv(sd, str, MAXLINE, 0);

		if (rec_len <= 0) {
			cout << "[����] ������Ϣ����" << endl;
			break;
		}

		// ���ղ���ӡ��Ϣ

		// �������ж�

		now_user = get_user(str);
		now_room = get_room(str);
		now_msg = get_msg(str);
		//cout << "[user]: " << now_user << "[room]: " << now_room << "[msg] : " << now_msg << endl;

		if (now_room == room) {
			if (now_user == user) {
				// my self
				cout << "[���Լ�]:\t" << get_msg(str) << endl;
			} else {
				// other people
				cout << "[" << get_user(str) << "]:\t" << get_msg(str) << endl;
			}
		}
		//cout << "[��Ϣ ]: " << str << endl;


		if (now_msg == "bye") {
			break;
		}
	}

	// �ر��׽���
	closesocket(sd);

	// ���� "WinSock DLL"
	WSACleanup();

	//system("pause");

	return 0;
}
//һ��getter
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
