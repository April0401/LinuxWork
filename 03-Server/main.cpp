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

//����һ���ͻ��ˣ��շ�������Ϣ
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
        cout << "Create socket  failed!" << endl;
        return -1;
    }
    cout << "Create socket success!" << endl;

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
    cout << "Bind port success!" << endl;

    //3.��������˿�
    if (SOCKET_ERROR == listen(sock, 10))  //������10���ͻ��˵�����
    {
        cout << "Listen port failed!" << endl;
        return -1;
    }
    cout << "Listen port success!" << endl;

    //4.�ȴ��ͻ��˵�����
    sockaddr_in clientAddr = {};
    int len = sizeof(clientAddr);
    SOCKET clientSock = INVALID_SOCKET;
    clientSock = accept(sock, (sockaddr*)&clientAddr, &len);
    if (INVALID_SOCKET == clientSock)
    {
        cout << "Invalid client socket!" << endl;
    }
    cout << "Client: " << inet_ntoa(clientAddr.sin_addr) << endl;

    char recvBuf[128] = {};
    char sendBuf[128] = {};
    while (true)
    {
        //5.���տͻ�������
        int recvLen = recv(clientSock, recvBuf, sizeof(recvBuf), 0);
        if (recvLen <= 0)
        {
            break;
        }
        cout << "Client: " << recvBuf << endl;
        //6.��������
        if (0 == strcmp(recvBuf, "getName"))
        {
            strcpy_s(sendBuf, "April");
        }
        else if (0 == strcmp(recvBuf, "getAge"))
        {
            strcpy_s(sendBuf, "20");
        }
        else
        {
            strcpy_s(sendBuf, "???");
        }
        //7.��ͻ��˷���һ������      
        send(clientSock, sendBuf, strlen(sendBuf) + 1, 0);
    }

    //8.�ر��׽���
    closesocket(sock);
    //�ر�
    WSACleanup();
    return 0;
}