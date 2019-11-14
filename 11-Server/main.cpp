
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

#include<iostream>
#include<vector>
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


vector<SOCKET> g_clients;

int Processor(SOCKET clientSock)
{
    DataHeader header;
    int recvLen = (int)recv(clientSock, (char*)&header, sizeof(header), 0);
    if (recvLen <= 0)
    {
        cout << "Client " << clientSock << " exit" << endl;
        return -1;
    }

    if (Cmd_Login == header.type)
    {
        Login login;
        recv(clientSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
        cout << "Client: " << "Type = " << login.type <<
            ", name = " << login.name <<
            ", password = " << login.password << endl;

        LoginResult result;
        result.result = 0;
        send(clientSock, (const char*)&result, sizeof(LoginResult), 0);
    }
    else if (Cmd_Logout == header.type)
    {
        Logout logout;
        recv(clientSock, (char*)&logout + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
        cout << "Client: " << "Type = " << logout.type <<
            ", name = " << logout.name << endl;

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
    return 0;
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
        cout << "Create socket  failed!" << endl;
        return -1;
    }
    cout << "Create socket success!" << endl;

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4567);
#ifdef _WIN32 
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    addr.sin_addr.s_addr = INADDR_ANY;
#endif

    if (SOCKET_ERROR == bind(sock, (sockaddr*)&addr, sizeof(addr)))
    {
        cout << "Bind port failed!" << endl;
        return -1;
    }
    cout << "Bind port success!" << endl;

    if (SOCKET_ERROR == listen(sock, 10))
    {
        cout << "Listen port failed!" << endl;
        return -1;
    }
    cout << "Listen port success!" << endl;

    while (true)
    {
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExcept;
        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExcept);

        FD_SET(sock, &fdRead);
        FD_SET(sock, &fdWrite);
        FD_SET(sock, &fdExcept);

        SOCKET maxSock = sock;
        for (int i = 0; i < (int)g_clients.size(); i++)
        {
            FD_SET(g_clients[i], &fdRead);
            if (maxSock < g_clients[i])
            {
                maxSock = g_clients[i];
            }
        }

        timeval t = { 1,0 };
        int ret = (int)select(maxSock + 1, &fdRead, &fdWrite, &fdExcept, &t);
        if (ret < 0)
        {
            break;
        }
        if (FD_ISSET(sock, &fdRead))
        {
            FD_CLR(sock, &fdRead);
            sockaddr_in clientAddr;
            int len = sizeof(sockaddr_in);
            SOCKET clientSock = INVALID_SOCKET;
#ifdef _WIN32 
            clientSock = accept(sock, (sockaddr*)&clientAddr, &len);
#else
            clientSock = accept(sock, (sockaddr*)&clientAddr, (socklen_t *)&len);
#endif
            if (INVALID_SOCKET == clientSock)
            {
                cout << "Invalid client socket!" << endl;
            }

            for (int i = 0; i < g_clients.size(); i++)
            {
                NewClient newClient;
                newClient.sock = clientSock;
                send(g_clients[i], (const char*)&newClient, sizeof(NewClient), 0);
            }
            g_clients.push_back(clientSock);
            cout << "Client: " << inet_ntoa(clientAddr.sin_addr) << endl;
        }

        for (int i = 0; i < g_clients.size(); i++)
        {
            if (FD_ISSET(g_clients[i], &fdRead))
            {
                if (-1 == Processor(g_clients[i]))
                {
                    vector<SOCKET>::iterator iter = g_clients.begin() + i;
                    if (iter != g_clients.end())
                    {
                        g_clients.erase(iter);
                        i--;
                    }
                }
            }
        }
        cout << "服务器空闲时间，处理其他业务" << endl;
    }

#ifdef _WIN32 
    for (int i = 0; i < g_clients.size(); i++)
    {
        closesocket(g_clients[i]);;
    }
    closesocket(sock);
    WSACleanup();
#else
    for (int i = 0; i < g_clients.size(); i++)
    {
        close(g_clients[i]);;
    }
    close(sock);
#endif
    return 0;
}