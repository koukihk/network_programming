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
//�ṩ�򵥵ķ��빦��
translate_word translate_word_list[translate_word_num] = {
		{"hello",       "���"},
		{"hi",          "��"},
		{"how are you", "�����"},
		{"I'm find",    "�Һܺ�"},
		{"What's up",    "��ô��"}
};
//����˵���໰��
bad_word bad_word_list[bad_word_num] = {
		{"bullshit", "bullsxxt"},
		{"bitch",    "bixxh"},
		{"fuck",     "fxxk"},
		{"cnm",     "��ɰ�"},
		{"tmd",     "���۵�"},
		{"sb",     "xx"},
		{"ɵ��",     "ɵ��"}
};


string cover_bad(const string &input);

string translate(const string &input);

string get_user(const string &input);

string get_room(const string &input);

string get_msg(const string &input);

int main() {
	system("color f5");
	// ʹ������
	int cli_num = 3;

	// һ������
	string now_room;
	string now_user;
	string now_msg;

	string tmp;

	cout << "��ӭʹ��������Server�ˣ�" << endl;
	cout << "������ʹ������: ";
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

	// Server IP��ַ
	const char server_ip[16] = "127.0.0.1";

	// Server�Ķ˿ں�
	u_short server_port = 5678;

	// ��������ܵ����ݳ���
	int rec_len;

	// �����淢�͵����ݳ���
	int send_len;

	// ��״̬
	int bind_status;

	// ���� WSAStartup() ��ע�� "WinSock DLL"
	WSAStartup(0x101, (LPWSADATA) &wsadata);

	// ����һ���׽���
	serv_sd = socket(AF_INET, SOCK_STREAM, 0);

	// ��ʼ׼��������
	// ���ṹ�� sockaddr_in struct (serv)
	serv.sin_family = AF_INET;

	// Server IP��ַ
	serv.sin_addr.s_addr = inet_addr(server_ip);//��һ�����ʮ���Ƶ�IPת����һ����������

	// Server�Ķ˿ں�
	// htons: host to network
	serv.sin_port = htons(server_port); //���ͱ����������ֽ�˳��ת��������ֽ�˳��

	// ��
	bind_status = bind(serv_sd, (LPSOCKADDR) &serv, sizeof(serv));

	if (bind_status == SOCKET_ERROR) {
		cout << "bind function failed with error: " << WSAGetLastError() << endl;
		closesocket(serv_sd);
		WSACleanup();
		return 1;
	}

	// ���� listen() ���������׽��ֽ������ģʽ
	listen(serv_sd, 5);

	for (int i = 0; i < cli_num; i++) {
		cli_len[i] = sizeof(cli[i]);

		// �������ӣ�
		cout << "[��Ϣ] �ȴ� client " << i + 1 << " ����" << endl;
		cli_sd[i] = accept(serv_sd, (LPSOCKADDR) &cli[i], &cli_len[i]);
		cout << "[��Ϣ] client " << i + 1 << " ������" << endl;
	}

	clis_len = sizeof(clis);
	while (true) {
		clis_sd = accept(serv_sd, (LPSOCKADDR) &clis, &clis_len);

		if (clis_sd == SOCKET_ERROR) {
			cout << "[����] �޷�������Ϣ" << endl;
			cout << "get hp error, code:" << WSAGetLastError() << endl;
			break;
		}

		//cout << "[��Ϣ] client ������" << endl;

		rec_len = recv(clis_sd, str_recv, MAXLINE, 0);

		if (rec_len <= 0) {
			cout << "[����] �޷�������Ϣ" << endl;
			break;
		}

		closesocket(clis_sd);

		//cout << "[����] " << str_recv << endl;

		now_room = get_room(str_recv);
		now_user = get_user(str_recv);
		now_msg = get_msg(str_recv);

		cout << "[room]: " << now_room << " [user]: " << now_user << " [msg] : " << now_msg << endl;

		// �����໰
		tmp = cover_bad(string(now_msg));

		if (!tmp.empty()) {
			now_msg = tmp;
		}

		// ����
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
			// ������Ϣ��client 1
			send_len = send(cli_sd[i], str_send, int(strlen(str_send) + 1), 0);

			if (send_len == SOCKET_ERROR) {
				cout << "[����] ������Ϣ��" << i << "ʧ��" << endl;
			}
		}

		if (now_msg == "bye") {
			break;
		}
	}

	// �ر��׽���
	closesocket(serv_sd);
	closesocket(clis_sd);
	for (int i = 0; i < cli_num; i++) {
		closesocket(cli_sd[i]);
	}

	// ���� "WinSock DLL"
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
			return "(Ӣ�� -> ����) ԭ��: " + input + " ����: " + i.chinese;
		} else if (input == i.chinese) {
			return "(���� -> Ӣ��) ԭ��: " + input + " ����: " + i.english;
		}
	}
	return "";
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
