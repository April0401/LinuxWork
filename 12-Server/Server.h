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
        int ReceiveData(SOCKET clientSock);
        int SendData(SOCKET clientSock, DataHeader *header);
        void SendDataToAll(DataHeader *header);
        int Processor(SOCKET clientSock, DataHeader *header);
    private:
        SOCKET mSock;
        std::vector<SOCKET> mClients;
    };

}



