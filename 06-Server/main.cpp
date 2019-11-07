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

//һ���ͻ��ˣ�һ������ˣ��շ�������Ϣ
//��Ϣͷ����Ϣ��һ����

//��Ϣ����
enum DataType
{
    Cmd_Login,
    Cmd_LoginResult,
    Cmd_Logout,
    Cmd_LogoutResult,
    Cmd_Error
};
//��Ϣͷ
struct DataHeader
{
    int length;
    DataType type;
};

//��Ϣ��
struct Login:public DataHeader
{
    Login()
    {
        length = sizeof(Login);
        type = Cmd_Login;
    }
    char name[32];
    char password[32];
};

struct LoginResult :public DataHeader
{
    LoginResult()
    {
        length = sizeof(LoginResult);
        type = Cmd_LoginResult;
    }
    int result;
};

struct Logout :public DataHeader
{
    Logout()
    {
        length = sizeof(Logout);
        type = Cmd_Logout;
    }
    char name[32];
};

struct LogoutResult :public DataHeader
{
    LogoutResult()
    {
        length = sizeof(LogoutResult);
        type = Cmd_LogoutResult;
    }
    int result;
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

    while (true)
    {
        //5.���տͻ�������ͷ
        DataHeader header = {};
        int recvLen = recv(clientSock, (char*)&header, sizeof(header), 0);
        if (recvLen <= 0)
        {
            break;
        }
        //6.��������, ��ͻ��˷���һ������   
        if (Cmd_Login == header.type)
        {
            Login login = {};
            recv(clientSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0); //��������ͷ�Ѷ�ȡ,ƫ�ƺͳ�����Ҫ����
            cout << "Client: " << "Type = " << login.type <<
                ", name = " << login.name <<
                ", password = " << login.password << endl;
            //ҵ���߼�����֤�û���������
            LoginResult result;
            result.result = 0;
            send(clientSock, (const char*)&result, sizeof(LoginResult), 0);                  
        }
        else if (Cmd_Logout == header.type)
        {
            Logout logout = {};
            recv(clientSock, (char*)&logout + sizeof(DataHeader), sizeof(Logout)- sizeof(DataHeader), 0); //��������ͷ�Ѷ�ȡ,ƫ�ƺͳ�����Ҫ���� 
            cout << "Client: " << "Type = " << logout.type <<
                ", name = " << logout.name << endl;
            //ҵ���߼����˳���¼
            LogoutResult result;
            result.result = 0;
            send(clientSock, (const char*)&result, sizeof(LogoutResult), 0);
        }
        else
        {
            header.length = 0;
            header.type = Cmd_Error;
            send(clientSock, (const char*)&header, sizeof(DataHeader), 0);
        }
    }

    //6.�ر��׽���
    closesocket(sock);
    //�ر�
    WSACleanup();
    return 0;
}