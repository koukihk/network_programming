/*
 * Created by kouki on 2021/12/30.
 */
#include <iostream>
#include <string>
#include <WinSock2.h> //适用平台 Windows
#pragma comment(lib, "ws2_32.lib") /*链接ws2_32.lib动态链接库*/

using namespace std;

// base64 加密函数,用这个也行：https://www.qqxiuzi.cn/bianma/base64.htm
string base64(string str)   //base64加密算法
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
	char buff[500]; //recv函数返回的结果缓冲
	string message;
	string info;
	string subject;
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 1); //版本2.1
	int err = WSAStartup(wVersionRequested, &wsaData);
	SOCKET sockClient;
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	HOSTENT* pHostent;//记录主机的信息，包括主机名、别名、地址类型、地址长度和地址列表
	while(1)
	{
		int flag;
		cout<<"\t欢迎使用第三方邮箱客户端！========\n"<<endl;
		cout<<"\t请先确认打开了邮箱的POP3/IMAP/SMTP功能，然后选择您的邮箱类型！\n\t1.QQ邮箱  2.网易163/126邮箱"<<endl;
		cout<<"\t邮箱类型:";
		cin>>flag;
		if(flag == 1)
		{
			cout<<"\t您选择的是:QQ邮箱"<<endl;
			pHostent = gethostbyname("smtp.qq.com"); //链接到qq邮箱服务器
		}
		else
		{
			cout<<"\t您选择的是:网易163/126邮箱"<<endl;
			pHostent = gethostbyname("smtp.163.com"); //链接到163邮箱服务器
		}
		SOCKADDR_IN addrServer; //服务端地址
		addrServer.sin_addr.S_un.S_addr = *((DWORD *)pHostent->h_addr_list[0]); //得到smtp服务器的网络字节序的ip地址
		addrServer.sin_family = AF_INET; //使用的是 IP 地址族
		addrServer.sin_port = htons(25); //连接端口25
		err = connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)); //向服务器发送请求
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		message = "ehlo hello\r\n";  // 向服务器标识用户身份
		send(sockClient, message.c_str(), message.length(), 0); //发送ehlo命令
		buff[recv(sockClient, buff, 500, 0)] = '\0';   //接收返回值
		// cout << "helo:" << buff << endl;     //输出返回值
		message = "auth login\r\n";   //进行用户身份认证
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		//cout << "auth login:" << buff << endl;
		/*
		发送base64加密的用户名、密码
		 */
		string Smail;
		cout<< "\t输入您的邮箱：";
		cin>>Smail;
		string name = Smail.substr(0,Smail.find("@"));
		message = base64(name) + "\r\n"; //base64 编码的用户名
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		string Swd;
		cout << "\t请输入您的授权码（如果没有请前往邮箱服务提供商处获取！）：";
		cin>>Swd;
		message = base64(Swd) + "\r\n";//base64 编码的密码
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		cout << "\tpassword:" << buff << endl;

		string Rmail;
		cout << "\t输入收件人邮箱：";
		cin >> Rmail;
		message = "mail from:<"+Smail+">\r\n";
		// message = "MAIL FROM:<XXX@qq.com> \r\nRCPT TO:<XXX@163.com> \r\n";指定邮件的发送方和接收方

		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';

		message = "rcpt to:<";

		message.append(Rmail);
		message.append(">\r\n");
		//cout << "message=" << message;

		send(sockClient, message.c_str(), message.length(), 0);

		buff[recv(sockClient, buff, 500, 0)] = '\0';

		message = "DATA\r\n";  //数据传输初始化
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		message = "From: "+Smail+"\r\nTo: "+Rmail+"\r\nsubject:";
		cout<<"\t+主题：";
		cin>>subject;
		message.append(subject);
		message.append("\r\n\r\n");
		cout<<"\t+内容：";
		cin>>info;
		message.append(info);
		message.append("\r\n.\r\n");

		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		cout<<" \tCongratulation 发送成功!"<<endl;
		cout<<"\t";
		cout<<buff;
		message = "QUIT\r\n";
		send(sockClient, message.c_str(), message.length(), 0);
		buff[recv(sockClient, buff, 500, 0)] = '\0';
		char mes;
		cout<<"\t输入Q退出,任意键继续发送邮件"<<endl;
		cout<<"\t";
		cin>>mes;
		if(mes == 'Q')
		{
			break;
		}
	}

	cout << "退出成功！"<<endl;
	system("pause");
}

