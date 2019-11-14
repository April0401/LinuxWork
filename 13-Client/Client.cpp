
#include<thread>
#include<iostream>
#include"Client.h"
using namespace std;


namespace EasySocket
{
    Client::Client()
    {
        mSock = INVALID_SOCKET;
    }

    Client::~Client()
    {
        Close();
    }

    void Client::InitSocket()
    {
#ifdef _WIN32  
        WORD ver = MAKEWORD(2, 2);
        WSADATA dat;
        WSAStartup(ver, &dat);
#endif
        if (INVALID_SOCKET != mSock)
        {
            Close();
        }
        mSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == mSock)
        {
            cout << "Create socket  failed!" << endl;          
        }
        else
        {
            cout << "Create socket  success!" << endl;
        }       
    }

    int Client::Connect(const char * ip, unsigned short port)
    {
        if (INVALID_SOCKET == mSock)
        {
            InitSocket();
        }

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
#ifdef _WIN32
        addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
        addr.sin_addr.s_addr = inet_addr(ip);
#endif   
        if (SOCKET_ERROR == connect(mSock, (sockaddr*)&addr, sizeof(addr)))
        {
            cout << "Connect failed!" << endl;
            return -1;
        }
        else
        {          
            cout << "Connect success!" << endl;
            return 0;
        }      
    }

    void Client::Close()
    {
        if (mSock != INVALID_SOCKET)
        {
#ifdef _WIN32 
            closesocket(mSock);
            WSACleanup();
#else
            close(mSock);
#endif 
            mSock = INVALID_SOCKET;
        }
    }

    bool Client::OnRun()
    {
        if (!IsRun())
            return false;

        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(mSock, &fdReads);
        timeval t = { 1.0, 0.0 };
        int ret = select(mSock + 1, &fdReads, 0, 0, &t);
        if (ret < 0)
        {
            Close();
            return false;
        }
        if (FD_ISSET(mSock, &fdReads))
        {
            FD_CLR(mSock, &fdReads);
            if (-1 == ReceiveData())
            {
                Close();
                return false;
            }
        }
        return true;
        //cout << "客户端空闲时间，处理其他业务" << endl;
    }

    bool Client::IsRun()
    {
        return mSock != INVALID_SOCKET;
    }

    #define _BufSize  10240
    char _buf[_BufSize] = {};
    char _recvBuf[_BufSize * 10] = {};
    int _pos = 0;
    int Client::ReceiveData()
    {       
        int recvLen = recv(mSock, _buf, _BufSize, 0);
        if (recvLen <= 0)
        {
            cout << "Client: exit!" << endl;
            return -1;
        }
        memcpy(_recvBuf + _pos, _buf, recvLen);
        _pos += recvLen;
        
        while (_pos >= sizeof(DataHeader))
        {
            DataHeader *header = (DataHeader*)_recvBuf;
            if (_pos >= header->length)
            {
                //处理网络消息
                Processor(header);
                int len = _pos - header->length;
                memcpy(_recvBuf, _recvBuf + header->length, len);
                _pos = len;
            }
            else
            {
                //缓冲区内的数据不够一条完整的消息
                break;
            }
        }  
    }


    int Client::SendData(DataHeader * header)
    {
        if (IsRun() && header)
        {
            return send(mSock, (const char *)header, header->length, 0);
        }   
        return -1;
    }

    int Client::Processor(DataHeader *header)
    {
        if (Cmd_LoginResult == header->type)
        {
            LoginResult *result = (LoginResult*)header;
            /*if (result->result == 0)
            {
                cout << "Server: login success." << endl;
            }
            else
            {
                cout << "Server: login fail. " << endl;
            }*/
        }
        else if (Cmd_LogoutResult == header->type)
        {
            LogoutResult *result = (LogoutResult*)header;
           /* if (result->result == 0)
            {
                cout << "Server: logout success." << endl;
            }
            else
            {
                cout << "Server: logout fail." << endl;
            }*/
        }
        else if (Cmd_NewClient == header->type)
        {
            NewClient *result = (NewClient*)header;
           // cout << "Server: New client join." << result->sock << endl;
        }
        else if (Cmd_Error == header->type)
        {
           // cout << "Server: Error message." << endl;
        }
        else
        {
            cout << "Error message." << endl;
        }
        return 0;
    }
}
