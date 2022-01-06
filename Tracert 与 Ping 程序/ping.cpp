#include <stdio.h>
#include <time.h>
#include <string.h>
#include <Winsock2.h>
#include <Windows.h>

#pragma comment (lib, "ws2_32.lib")

// 2字节 对齐 sizeof(icmp_header) == 8
typedef struct icmp_header
{
	unsigned char icmp_type;    // 消息类型 1
	unsigned char icmp_code;    // 代码 1
	unsigned short icmp_checksum;    // 校验和 2
	unsigned short icmp_id;     // 用来惟一标识此请求的ID号，通常设置为进程ID 2
	unsigned short icmp_sequence;   // 序列号 2
} icmp_header;

#define ICMP_HEADER_SIZE sizeof(icmp_header)
#define ICMP_ECHO_REQUEST 0x08
#define ICMP_ECHO_REPLY 0x00

// 计算校验和
unsigned short chsum(struct icmp_header *picmp, int len)
{
	long sum = 0;
	unsigned short *pusicmp = (unsigned short *)picmp;

	while ( len > 1 )
	{
		sum += *(pusicmp++);
		if ( sum & 0x80000000 )
		{
			sum = (sum & 0xffff) + (sum >> 16);
		}
		len -= 2;
	}

	if ( len )
	{
		sum += (unsigned short)*(unsigned char *)pusicmp;
	}

	while ( sum >> 16 )
	{
		sum = (sum & 0xffff) + (sum >> 16);
	}

	return (unsigned short)~sum;
}

static int respNum = 0;
static int minTime = 0,maxTime = 0,sumTime = 0;
int ping(char *szDestIp)
{
	printf("正在测试: %s\n",szDestIp);
	int bRet = 1;

	WSADATA wsaData;
	int nTimeOut = 1000;//1s
	char szBuff[ICMP_HEADER_SIZE + 32] = { 0 };
	icmp_header *pIcmp = (icmp_header *)szBuff;
	char icmp_data[32] = { 0 };

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	// 创建原始套接字
	SOCKET s = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);

	// 设置接收超时
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char const*)&nTimeOut, sizeof(nTimeOut));

	// 设置目的地址
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.S_un.S_addr = inet_addr(szDestIp);
	dest_addr.sin_port = htons(0);

	// 构造ICMP封包
	pIcmp->icmp_type = ICMP_ECHO_REQUEST;
	pIcmp->icmp_code = 0;
	pIcmp->icmp_id = (USHORT)::GetCurrentProcessId();
	pIcmp->icmp_sequence = 0;
	pIcmp->icmp_checksum = 0;

	// 填充数据
	memcpy((szBuff + ICMP_HEADER_SIZE), "abcdefghijklmnopqrstuvwabcdefghi", 32);

	// 计算校验和
	pIcmp->icmp_checksum = chsum((struct icmp_header *)szBuff, sizeof(szBuff));

	sockaddr_in from_addr;
	char szRecvBuff[1024];
	int nLen = sizeof(from_addr);
	int ret,flag = 0;

	DWORD  start = GetTickCount();
	ret = sendto(s, szBuff, sizeof(szBuff), 0, (SOCKADDR *)&dest_addr, sizeof(SOCKADDR));
	//printf("ret = %d ,errorCode:%d\n",ret ,WSAGetLastError() );

	int i = 0;

	while(1){
		if(i++ > 2){// icmp报文 如果到不了目标主机，是不会返回报文，多尝试几次接受数据，如果都没收到 即请求失败
			flag = 1;
			break;
		}
		memset(szRecvBuff,0,1024);
		//printf("errorCode1:%d\n",WSAGetLastError() );
		int ret = recvfrom(s, szRecvBuff, MAXBYTE, 0, (SOCKADDR *)&from_addr, &nLen);
		//printf("errorCode2:%d\n",WSAGetLastError() );
		//printf("ret=%d,%s\n",ret,inet_ntoa(from_addr.sin_addr)) ;
		//接受到 目标ip的 报文
		if( strcmp(inet_ntoa(from_addr.sin_addr),szDestIp) == 0)  {
			respNum++;
			break;
		}
	}



	DWORD  end = GetTickCount();
	DWORD time = end - start;

	if(flag){
		printf("请求超时，该主机可能不在线。响应时长>1s\n");
		printf("-----------------------------------------------\n");
		return bRet;
	}
	sumTime += time;
	if( minTime > time){
		minTime = time;
	}
	if( maxTime < time){
		maxTime = time;
	}



	// Windows的原始套接字 开发，系统没有去掉IP协议头，需要程序自己处理。
	// ip头部的第一个字节（只有1个字节不涉及大小端问题），前4位 表示 ip协议版本号，后4位 表示IP 头部长度(单位为4字节)
	char ipInfo = szRecvBuff[0];
	// ipv4头部的第9个字节为TTL的值
	char ttl = szRecvBuff[8];
	//printf("ipInfo = %x\n",ipInfo);


	int ipVer = ipInfo >> 4;
	int ipHeadLen = ((char)( ipInfo << 4) >> 4) * 4;
	if( ipVer  == 4) {
		// 跨过ip协议头，得到ICMP协议头的位置，不过是网络字节序。
		// 网络字节序 是大端模式 低地址 高位字节 高地址 低位字节。-> 转换为 本地字节序 小端模式 高地址高字节 低地址低字节
		icmp_header* icmp_rep = (icmp_header*)(szRecvBuff + ipHeadLen);
		//由于校验和是 2个字节 涉及大小端问题，需要转换字节序
		unsigned short checksum_host = ntohs(icmp_rep->icmp_checksum);// 转主机字节序 


		if(icmp_rep->icmp_type == 0){ //回显应答报文
			printf("主机 %s 在线。TTL = %d ,响应时长 %2dms <1ms \n", szDestIp, ttl,time);
			printf("-----------------------------------------------\n");
		} else{
			bRet = 0;
			printf("请求超时，该主机可能不在线。响应时长>1s type = %d\n",icmp_rep->icmp_type);
			printf("-----------------------------------------------\n");
		}
	}else{
		//printf("ipv6 len = %d\n",ipLen);
	}

	return bRet;
}

int main()
{
	system("color f4");

	int i = 1,j,k;
	char ip[20],tmp[5];

	memset(ip,'\0',sizeof(ip));
	memset(tmp,'\0',sizeof(tmp));

	printf("请输入局域网最小地址:\n");
	gets(ip);

	for(k = strlen(ip)-1;k >= 0;k--){
		if(ip[k] == '.') break;
	}
	strcpy(tmp,ip+k+1);
	j = atoi(tmp);

	while(j <= 255 ){

		int result = ping(ip);

		j++;
		itoa(j,tmp,10);
		strcpy(ip+k+1,tmp);
	}

	printf("已全部测试完毕。");

	return 0;
}
