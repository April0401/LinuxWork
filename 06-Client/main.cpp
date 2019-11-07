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
        char buf[128] = {};
        cin >> buf;
        if (0 == strcmp(buf, "exit"))
        {
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
           
            //5.���շ���������Ϣ
            LoginResult result = {};
            recv(sock, (char *)&result, sizeof(LoginResult), 0);
            if (result.result == 0)
            {
                cout << "Server: login success."  << endl;
            }
            else
            {
                cout << "Server: login fail. " << endl;
            }         
        }
        else if (0 == strcmp(buf, "logout"))
        {
            Logout logout;
            strcpy_s(logout.name, "April");
            send(sock, (char*)&logout, sizeof(Logout), 0);

            LogoutResult result = {};
            recv(sock, (char *)&result, sizeof(LogoutResult), 0);
            if (result.result == 0)
            {
                cout << "Server: logout success." << endl;
            }
            else
            {
                cout << "Server: logout fail. " << endl;
            }
        }
        else
        {
            cout << "Cmd invalid, input again." << endl;
        }      
    }

    //4.�ر��׽���
    closesocket(sock);
    //�ر�
    WSACleanup();
    getchar();
    return 0;
}