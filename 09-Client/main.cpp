#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<thread>
using namespace std;

//��Ӿ�̬���ӿ�
//����1���ڴ����м��� #pragma comment(lib, "ws2_32.lib")
//����2������ - ������ - ���� - "ws2_32.lib"
//������ƽ̨��c++����ʱ��Ӧ��ʹ�õڶ��ַ���

//�ͻ��������̣߳������û�����

//��Ϣ����
enum DataType
{
    Cmd_Login,
    Cmd_LoginResult,
    Cmd_Logout,
    Cmd_LogoutResult,
    Cmd_NewClient,
    Cmd_Error
};
//��Ϣͷ
struct DataHeader
{
    int length;
    DataType type;
};

//��Ϣ��
struct Login :public DataHeader
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

struct NewClient :public DataHeader
{
    NewClient()
    {
        length = sizeof(NewClient);
        type = Cmd_NewClient;
        sock = 0;
    }
    int sock;
};

int Processor(SOCKET sock)
{
    //5.���տͻ�������ͷ
    DataHeader header = {};
    int recvLen = recv(sock, (char*)&header, sizeof(header), 0);
    if (recvLen <= 0)
    {
        return -1;
    }
    //6.��������, ��ͻ��˷���һ������   
    if (Cmd_LoginResult == header.type)
    {
        LoginResult result = {};
        recv(sock, (char *)&result + sizeof(DataHeader), sizeof(LoginResult) - sizeof(DataHeader), 0);
        if (result.result == 0)
        {
            cout << "Server: login success." << endl;
        }
        else
        {
            cout << "Server: login fail. " << endl;
        }
    }
    else if (Cmd_LogoutResult == header.type)
    {
        LogoutResult result = {};
        recv(sock, (char *)&result + sizeof(DataHeader), sizeof(LogoutResult) - +sizeof(DataHeader), 0);
        if (result.result == 0)
        {
            cout << "Server: logout success." << endl;
        }
        else
        {
            cout << "Server: logout fail. " << endl;
        }
    }
    else if (Cmd_NewClient == header.type)
    {
        NewClient result = {};
        recv(sock, (char *)&result + sizeof(DataHeader), sizeof(NewClient) - sizeof(DataHeader), 0);
        cout << "Server: New client join " << result.sock << endl;
    }

    return 0;
}

bool isRun = true;
void CmdThread(SOCKET sock)
{
    while (true) 
    {
        //3.������������
        char buf[128] = {};
        cin >> buf;
        if (0 == strcmp(buf, "exit"))
        {
            isRun = false;
            cout << "exit!" << endl;
            break;
        }
        else if (0 == strcmp(buf, "login"))
        {
            //4.������������
            Login login;
            strcpy_s(login.name, "April");
            strcpy_s(login.password, "0000");
            send(sock, (char*)&login, sizeof(Login), 0);
        }
        else if (0 == strcmp(buf, "logout"))
        {
            Logout logout;
            strcpy_s(logout.name, "April");
            send(sock, (char*)&logout, sizeof(Logout), 0);
        }
        else
        {
            cout << "Cmd invalid, input again." << endl;
        }
    }
    
}

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

    //�����߳�
    thread t1(CmdThread, sock);
    t1.detach();
    while (isRun)
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(sock, &fdReads);
        timeval t = { 1.0, 0.0 };
        int ret = select(sock, &fdReads, 0, 0, &t);
        if (ret < 0)
        {
            break;
        }
        if (FD_ISSET(sock, &fdReads))
        {
            FD_CLR(sock, &fdReads);
            if (-1 == Processor(sock))
            {
                break;
            }
        }
        //cout << "�ͻ��˿���ʱ�䣬��������ҵ��" << endl;
    }

    //4.�ر��׽���
    closesocket(sock);
    //�ر�
    WSACleanup();
    getchar();
    return 0;
}