#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
using namespace std;

//添加静态链接库
//方法1：在代码中加入 #pragma comment(lib, "ws2_32.lib")
//方法2：属性 - 连接器 - 输入 - "ws2_32.lib"
//开发跨平台的c++代码时，应该使用第二种方法

//一个客户端，一个服务端，收发多条消息
//消息头和消息体一起发送


//消息类型
enum DataType
{
    Cmd_Login,
    Cmd_LoginResult,
    Cmd_Logout,
    Cmd_LogoutResult,
    Cmd_Error
};
//消息头
struct DataHeader
{
    int length;
    DataType type;
};

//消息体
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
    //启动windows socket 网络环境
    WSAStartup(ver, &dat);
    //1.建立socket套接字
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock)
    {
        cout << "Create sock  failed!" << endl;
        return -1;
    }
    //2.连接服务器
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
        //3.输入请求命令
        char buf[128] = {};
        cin >> buf;
        if (0 == strcmp(buf, "exit"))
        {
            cout << "exit!" << endl;
            break;
        }
        else if (0 == strcmp(buf, "login"))
        {
            //4.发送请求命令
            Login login;
            strcpy_s(login.name, "April");
            strcpy_s(login.password, "0000");
            send(sock, (char*)&login, sizeof(Login), 0);
           
            //5.接收服务器的信息
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

    //4.关闭套接字
    closesocket(sock);
    //关闭
    WSACleanup();
    getchar();
    return 0;
}