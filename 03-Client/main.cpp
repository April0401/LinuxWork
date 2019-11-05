#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
using namespace std;

//��Ӿ�̬���ӿ�
//����1���ڴ����м��� #pragma comment(lib, "ws2_32.lib")
//����2������ - ������ - ���� - "ws2_32.lib"
//������ƽ̨��c++����ʱ��Ӧ��ʹ�õڶ��ַ���

int main()
{
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    //����windows socket ���绷��
    WSAStartup(ver, &dat);
    //1.����socket�׽���
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock)
    {
        cout << "Create sock  failed!" << endl;
        return -1;
    }
    //2.���ӷ�����
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;     //ipv4
    addr.sin_port = htons(4567);   // host to net unsigned short
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  //ip addr
    if (SOCKET_ERROR == connect(sock, (sockaddr*)&addr, sizeof(addr)))
    {
        cout << "Connect  failed!" << endl;
    }
 
    while (true)
    {
        //3.������������
        char cmdBuf[128] = {};
        cin >> cmdBuf;
       // scanf_s("%s",cmdBuf);
        if (0 == strcmp(cmdBuf, "exit"))
        {
            break;
        }
        //4.������������
        send(sock, cmdBuf, strlen(cmdBuf)+1, 0);
        //5.���շ���������Ϣ
        char msg[128] = {};
        if (recv(sock, msg, sizeof(msg), 0) > 0)
        {
            cout << "Server: " << msg << endl;
        }
    }
    
    //4.�ر��׽���
    closesocket(sock);
    //�ر�
    WSACleanup();
    getchar();
    return 0;
}