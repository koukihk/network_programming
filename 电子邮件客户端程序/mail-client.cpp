/*
 * Created by kouki on 2021/12/30.
 */
#include <iostream>
#include <string>
#include <WinSock2.h> //����ƽ̨ Windows
#pragma comment(lib, "ws2_32.lib") /*����ws2_32.lib��̬���ӿ�*/

using namespace std;

// base64 ���ܺ���,�����Ҳ�У�https://www.qqxiuzi.cn/bianma/base64.htm
string base64(string str)   //base64�����㷨
{
	string base64_table="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int str_len = str.length();
	string res="";
	for (int strp=0; strp<str_len/3*3; strp+=3)
	{
		res+=base64_table[str[strp]>>2];
		res+=base64_table[(str[strp]&0x3)<<4 | (str[strp+1])>>4];
		res+=base64_table[(str[strp+1]&0xf)<<2 | (str[strp+2])>>6];
		res+=base64_table[(str[strp+2])&0x3f];
	}
	if (str_len%3==1)
	{
		int pos=str_len/3 * 3;
		res += base64_table[str[pos]>>2];
		res += base64_table[(str[pos]&0x3)<<4];
		res += "=";
		res += "=";
	}
	else if (str_len%3==2)
	{
		int pos=str_len/3 * 3;
		res += base64_table[str[pos]>>2];
		res += base64_table[(str[pos]&0x3)<<4 | (str[pos+1])>>4];
		res += base64_table[(str[pos+1]&0xf)<<2];
		res += "=";
	}
	return res;
}

int main()
{
	system("color f4");
	char buff[500]; //recv�������صĽ������
	string message;
	string info;
	string subject;
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 1); //�汾2.1
	int err = WSAStartup(wVersionRequested, &wsaData);
	SOCKET sockClient;
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	HOSTENT* pHostent;//��¼��������Ϣ����������������������ַ���͡���ַ���Ⱥ͵�ַ�б�
	while(1)
	{
		int flag;
		cout<<"\t��ӭʹ�õ���������ͻ��ˣ�========\n"<<endl;
		cout<<"\t����ȷ�ϴ��������POP3/IMAP/SMTP���ܣ�Ȼ��ѡ�������������ͣ�\n\t1.QQ����  2.����163/126����"<<endl;
		cout<<"\t��������:";
		cin>>flag;
		if(flag == 1)
		{
			cout<<"\t��ѡ�����:QQ����"<<endl;
			pHostent = gethostbyname("smtp.qq.com"); //���ӵ�qq���������
		}
		else
		{
			cout<<"\t��ѡ�����:����163/126����"<<endl;
			pHostent = gethostbyname("smtp.163.com"); //���ӵ�163���������
		}
		SOCKADDR_IN addrServer; //����˵�ַ
		addrServer.sin_addr.S_un.S_addr = *((DWORD *)pHostent->h_addr_list[0]); //�õ�smtp�������������ֽ����ip��ַ
		addrServer.sin_family = AF_INET; //ʹ�õ��� IP ��ַ��
		addrServer.sin_port = htons(25); //���Ӷ˿�25
		err = connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)); //���������������
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		message = "ehlo hello\r\n";  // ���������ʶ�û����
		send(sockClient, message.c_str(), message.length(), 0); //����ehlo����
		buff[recv(sockClient, buff, 500, 0)] = '\0';   //���շ���ֵ
		// cout << "helo:" << buff << endl;     //�������ֵ
		message = "auth login\r\n";   //�����û������֤
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		//cout << "auth login:" << buff << endl;
		/*
		����base64���ܵ��û���������
		 */
		string Smail;
		cout<< "\t�����������䣺";
		cin>>Smail;
		string name = Smail.substr(0,Smail.find("@"));
		message = base64(name) + "\r\n"; //base64 ������û���
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		string Swd;
		cout << "\t������������Ȩ�루���û����ǰ����������ṩ�̴���ȡ������";
		cin>>Swd;
		message = base64(Swd) + "\r\n";//base64 ���������
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		cout << "\tpassword:" << buff << endl;

		string Rmail;
		cout << "\t�����ռ������䣺";
		cin >> Rmail;
		message = "mail from:<"+Smail+">\r\n";
		// message = "MAIL FROM:<XXX@qq.com> \r\nRCPT TO:<XXX@163.com> \r\n";ָ���ʼ��ķ��ͷ��ͽ��շ�

		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';

		message = "rcpt to:<";

		message.append(Rmail);
		message.append(">\r\n");
		//cout << "message=" << message;

		send(sockClient, message.c_str(), message.length(), 0);

		buff[recv(sockClient, buff, 500, 0)] = '\0';

		message = "DATA\r\n";  //���ݴ����ʼ��
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		message = "From: "+Smail+"\r\nTo: "+Rmail+"\r\nsubject:";
		cout<<"\t+���⣺";
		cin>>subject;
		message.append(subject);
		message.append("\r\n\r\n");
		cout<<"\t+���ݣ�";
		cin>>info;
		message.append(info);
		message.append("\r\n.\r\n");

		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		cout<<" \tCongratulation ���ͳɹ�!"<<endl;
		cout<<"\t";
		cout<<buff;
		message = "QUIT\r\n";
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		char mes;
		cout<<"\t����Q�˳�,��������������ʼ�"<<endl;
		cout<<"\t";
		cin>>mes;
		if(mes == 'Q')
		{
			break;
		}
	}

	cout << "�˳��ɹ���"<<endl;
	system("pause");
}

