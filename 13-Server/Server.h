#pragma once

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

#include<vector>
#include"Message.h"

namespace EasySocket
{

#define _BufSize  10240
    class ClientSocket
    {
    public:
        ClientSocket(SOCKET socket = INVALID_SOCKET)
        {
            mSocket = socket;
            memset(mBuffer, 0, sizeof(mBuffer));
            mPos = 0;
        }
        ~ClientSocket() {}

    public:
        SOCKET mSocket;
        char mBuffer[_BufSize * 10];
        int mPos;
    };

    class Server
    {
    public:
        Server();
        ~Server();

        void InitSocket();
        int Bind(const char * ip, unsigned short port);
        int Listen(int num);
        int Accept();
        void Close();
        bool OnRun();
        bool IsRun();
        int ReceiveData(ClientSocket* clientSock);
        int SendData(SOCKET clientSock, DataHeader *header);
        void SendDataToAll(DataHeader *header);
        int Processor(SOCKET clientSock, DataHeader *header);
    private:
        SOCKET mSock;
        std::vector<ClientSocket*> mClients;
    };

}



