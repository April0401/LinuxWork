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

#include"Message.h"
namespace EasySocket
{
    class Client
    {
    public:
        Client();
        ~Client();
        void InitSocket();
        int Connect(const char *ip, unsigned short port);
        void Close();
        bool OnRun();
        bool IsRun();
        int ReceiveData();
        int SendData(DataHeader *header);
        int Processor(DataHeader *header);
    private:
        SOCKET mSock;
    };
}



