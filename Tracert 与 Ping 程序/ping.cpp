#include <stdio.h>
#include <time.h>
#include <string.h>
#include <Winsock2.h>
#include <Windows.h>

#pragma comment (lib, "ws2_32.lib")

// 2�ֽ� ���� sizeof(icmp_header) == 8
typedef struct icmp_header
{
	unsigned char icmp_type;    // ��Ϣ���� 1
	unsigned char icmp_code;    // ���� 1
	unsigned short icmp_checksum;    // У��� 2
	unsigned short icmp_id;     // ����Ωһ��ʶ�������ID�ţ�ͨ������Ϊ����ID 2
	unsigned short icmp_sequence;   // ���к� 2
} icmp_header;

#define ICMP_HEADER_SIZE sizeof(icmp_header)
#define ICMP_ECHO_REQUEST 0x08
#define ICMP_ECHO_REPLY 0x00

// ����У���
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
	printf("���ڲ���: %s\n",szDestIp);
	int bRet = 1;

	WSADATA wsaData;
	int nTimeOut = 1000;//1s
	char szBuff[ICMP_HEADER_SIZE + 32] = { 0 };
	icmp_header *pIcmp = (icmp_header *)szBuff;
	char icmp_data[32] = { 0 };

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	// ����ԭʼ�׽���
	SOCKET s = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);

	// ���ý��ճ�ʱ
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char const*)&nTimeOut, sizeof(nTimeOut));

	// ����Ŀ�ĵ�ַ
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.S_un.S_addr = inet_addr(szDestIp);
	dest_addr.sin_port = htons(0);

	// ����ICMP���
	pIcmp->icmp_type = ICMP_ECHO_REQUEST;
	pIcmp->icmp_code = 0;
	pIcmp->icmp_id = (USHORT)::GetCurrentProcessId();
	pIcmp->icmp_sequence = 0;
	pIcmp->icmp_checksum = 0;

	// �������
	memcpy((szBuff + ICMP_HEADER_SIZE), "abcdefghijklmnopqrstuvwabcdefghi", 32);

	// ����У���
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
		if(i++ > 2){// icmp���� ���������Ŀ���������ǲ��᷵�ر��ģ��ೢ�Լ��ν������ݣ������û�յ� ������ʧ��
			flag = 1;
			break;
		}
		memset(szRecvBuff,0,1024);
		//printf("errorCode1:%d\n",WSAGetLastError() );
		int ret = recvfrom(s, szRecvBuff, MAXBYTE, 0, (SOCKADDR *)&from_addr, &nLen);
		//printf("errorCode2:%d\n",WSAGetLastError() );
		//printf("ret=%d,%s\n",ret,inet_ntoa(from_addr.sin_addr)) ;
		//���ܵ� Ŀ��ip�� ����
		if( strcmp(inet_ntoa(from_addr.sin_addr),szDestIp) == 0)  {
			respNum++;
			break;
		}
	}



	DWORD  end = GetTickCount();
	DWORD time = end - start;

	if(flag){
		printf("����ʱ�����������ܲ����ߡ���Ӧʱ��>1s\n");
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



	// Windows��ԭʼ�׽��� ������ϵͳû��ȥ��IPЭ��ͷ����Ҫ�����Լ�����
	// ipͷ���ĵ�һ���ֽڣ�ֻ��1���ֽڲ��漰��С�����⣩��ǰ4λ ��ʾ ipЭ��汾�ţ���4λ ��ʾIP ͷ������(��λΪ4�ֽ�)
	char ipInfo = szRecvBuff[0];
	// ipv4ͷ���ĵ�9���ֽ�ΪTTL��ֵ
	char ttl = szRecvBuff[8];
	//printf("ipInfo = %x\n",ipInfo);


	int ipVer = ipInfo >> 4;
	int ipHeadLen = ((char)( ipInfo << 4) >> 4) * 4;
	if( ipVer  == 4) {
		// ���ipЭ��ͷ���õ�ICMPЭ��ͷ��λ�ã������������ֽ���
		// �����ֽ��� �Ǵ��ģʽ �͵�ַ ��λ�ֽ� �ߵ�ַ ��λ�ֽڡ�-> ת��Ϊ �����ֽ��� С��ģʽ �ߵ�ַ���ֽ� �͵�ַ���ֽ�
		icmp_header* icmp_rep = (icmp_header*)(szRecvBuff + ipHeadLen);
		//����У����� 2���ֽ� �漰��С�����⣬��Ҫת���ֽ���
		unsigned short checksum_host = ntohs(icmp_rep->icmp_checksum);// ת�����ֽ��� 


		if(icmp_rep->icmp_type == 0){ //����Ӧ����
			printf("���� %s ���ߡ�TTL = %d ,��Ӧʱ�� %2dms <1ms \n", szDestIp, ttl,time);
			printf("-----------------------------------------------\n");
		} else{
			bRet = 0;
			printf("����ʱ�����������ܲ����ߡ���Ӧʱ��>1s type = %d\n",icmp_rep->icmp_type);
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

	printf("�������������С��ַ:\n");
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

	printf("��ȫ��������ϡ�");

	return 0;
}
