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

enum DataType
{
    Student,
    Other
};
//��Ϣͷ
struct DataHeader
{
    DataType type;
};

//��Ϣ��
struct DataStudent
{
    char name[32];
    int age;
};

struct DataMessage
{
    char log[128];
};

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

    cout << "�����������ͣ�0��ѧ����Ϣ��1��������Ϣ" << endl;
    while (true)
    {
        //3.������������
        DataHeader sendHeader = {};    
        int index;
        cin >> index;
        if (index != 0 && index != 1)
        {
            index = 2;
        }
        sendHeader.type = (DataType)index;
        //4.������������
        send(sock, (char*)&sendHeader, sizeof(DataHeader), 0);
        //5.���շ���������Ϣ
        DataHeader header = {};
        if (recv(sock, (char *)&header, sizeof(DataHeader), 0) > 0)
        {
            if (DataType::Student == header.type)
            {
                DataStudent info = {};
                recv(sock, (char *)&info, sizeof(DataStudent), 0);
                cout << "Server: name = " << info.name << ", age= " << info.age << endl;
            }
            else
            {
                DataMessage info = {};
                recv(sock, (char *)&info, sizeof(DataMessage), 0);
                cout << "Server: " << info.log << endl;
            }
        }
    }

    //4.�ر��׽���
    closesocket(sock);
    //�ر�
    WSACleanup();
    getchar();
    return 0;
}