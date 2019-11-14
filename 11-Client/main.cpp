
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define _WINSOCK_DEPRECATED_NO_WARNINGS 
    #include<Windows.h>
    #include<WinSock2.h>
#else
    #include<unistd.h>
    #include<arpa/inet.h>
    #include<string.h>
    #define SOCKET int
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR            (-1)
#endif
#include<thread>
#include<iostream>
using namespace std;

enum DataType
{
    Cmd_Login,
    Cmd_LoginResult,
    Cmd_Logout,
    Cmd_LogoutResult,
    Cmd_NewClient,
    Cmd_Error
};

struct DataHeader
{
    int length;
    DataType type;
};

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
    DataHeader header;
    int recvLen = recv(sock, (char*)&header, sizeof(header), 0);
    if (recvLen <= 0)
    {
        return -1;
    }

    if (Cmd_LoginResult == header.type)
    {
        LoginResult result;
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
        LogoutResult result;
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
        NewClient result;
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
        //3.输入请求命令
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
            //4.发送请求命令
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
#ifdef _WIN32  
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);
#endif

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock)
    {
        cout << "Create sock  failed!" << endl;
        return -1;
    }
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4567);
    cout << addr.sin_port << endl;
#ifdef _WIN32
    addr.sin_addr.S_un.S_addr = inet_addr("1.1.1.100");
#else
    addr.sin_addr.s_addr = inet_addr("1.1.1.105");
#endif   
    if (SOCKET_ERROR == connect(sock, (sockaddr*)&addr, sizeof(addr)))
    {
        cout << "Connect  failed!" << endl;
    }
    //启动线程
    thread t1(CmdThread, sock);
    t1.detach();
    while (isRun)
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(sock, &fdReads);
        timeval t = { 1.0, 0.0 };
        int ret = select(sock + 1, &fdReads, 0, 0, &t);
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
        //cout << "客户端空闲时间，处理其他业务" << endl;
    }

#ifdef _WIN32 
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif   
    return 0;
}