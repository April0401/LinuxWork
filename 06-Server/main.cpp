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
    //启动windows socket 网络环境
    WSAStartup(ver, &dat);
    //1.建立socket套接字
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock)
    {
        cout << "Create socket  failed!" << endl;
        return -1;
    }
    cout << "Create socket success!" << endl;

    //2.绑定网络端口
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

    //3.监听网络端口
    if (SOCKET_ERROR == listen(sock, 10))  //最多接收10个客户端的连接
    {
        cout << "Listen port failed!" << endl;
        return -1;
    }
    cout << "Listen port success!" << endl;

    //4.等待客户端的连接
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
        //5.接收客户端数据头
        DataHeader header = {};
        int recvLen = recv(clientSock, (char*)&header, sizeof(header), 0);
        if (recvLen <= 0)
        {
            break;
        }
        //6.处理请求, 向客户端发送一条数据   
        if (Cmd_Login == header.type)
        {
            Login login = {};
            recv(clientSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0); //由于数据头已读取,偏移和长度需要修正
            cout << "Client: " << "Type = " << login.type <<
                ", name = " << login.name <<
                ", password = " << login.password << endl;
            //业务逻辑，验证用户名和密码
            LoginResult result;
            result.result = 0;
            send(clientSock, (const char*)&result, sizeof(LoginResult), 0);                  
        }
        else if (Cmd_Logout == header.type)
        {
            Logout logout = {};
            recv(clientSock, (char*)&logout + sizeof(DataHeader), sizeof(Logout)- sizeof(DataHeader), 0); //由于数据头已读取,偏移和长度需要修正 
            cout << "Client: " << "Type = " << logout.type <<
                ", name = " << logout.name << endl;
            //业务逻辑，退出登录
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

    //6.关闭套接字
    closesocket(sock);
    //关闭
    WSACleanup();
    return 0;
}