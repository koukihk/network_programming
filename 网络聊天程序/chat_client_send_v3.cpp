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

	// ������Ϣ
	string input;

	// �û���
	string user;

	// ��������
	string room;

	// �ָ���
	string separate = ",";

	char str[MAXLINE] = "";

	WSADATA wsadata;

	// Server IP��ַ
	const char server_ip[16] = "127.0.0.1";

	// Server �˿ں�
	u_short server_port = 5678;

	// ����״̬
	int conn_status;

	cout << "��ӭʹ�������� client �˷��������" << endl;

	cout << "������ʹ�������ƣ�Ψһ�ģ�: ";
	getline(cin, user);
	cout << endl;

	cout << "���������������ƣ�Ψһ�ģ�: ";
	getline(cin, room);
	cout << endl;

	// ���� WSAStartup() ��ע�� "WinSock DLL"
	WSAStartup(0x101, (LPWSADATA) &wsadata);

	// ��ʼ׼��������
	// ���ṹ�� sockaddr_ing struct (serv)
	serv.sin_family = AF_INET;

	// Server��IP��ַ
	serv.sin_addr.s_addr = inet_addr(server_ip);

	// Server �˿ں�
	// htons: host to network
	serv.sin_port = htons(server_port);

	while (true) {
		// ����һ���׽���
		sd = socket(AF_INET, SOCK_STREAM, 0);

		cout << "[����] ����������: (���� bye �뿪������)" << endl;

		getline(cin, input);

		// ���ӵ�Server
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
		cout << "[" << user << "��" << room << "����] : " << input << endl;

		cout << "--------------------------------" << endl;

		if (input == "bye") {
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
