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
    //2.������˿�
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;     //ipv4
    addr.sin_port = htons(4567);   // host to net unsigned short
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  //ip addr
    if (SOCKET_ERROR == bind(sock, (sockaddr*)&addr, sizeof(addr)))
    {
        cout << "Bind port failed!" << endl;
        return -1;
    }
    //3.��������˿�
    if (SOCKET_ERROR == listen(sock, 10))  //������10���ͻ��˵�����
    {
        cout << "Listen port failed!" << endl;
        return -1;
    }
    //4.�ȴ��ͻ��˵�����
    sockaddr_in clientAddr = {};
    int len = sizeof(clientAddr);
    SOCKET clientSock = INVALID_SOCKET;
    char msg[] = "Hello, I'm server.";
    while (true)
    {
        clientSock = accept(sock, (sockaddr*)&clientAddr, &len);
        if (INVALID_SOCKET == clientSock)
        {
            cout << "Invalid client socket!" << endl;
        }
        cout << "Client: " << inet_ntoa(clientAddr.sin_addr) << endl;
        //5.��ͻ��˷���һ������      
        send(clientSock, msg, sizeof(msg) + 1, 0);
    }
    
    //6.�ر��׽���
    closesocket(sock);
    //�ر�
    WSACleanup();
    return 0;
}