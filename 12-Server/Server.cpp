
#include"Server.h"
#include<iostream>
using namespace std;

namespace EasySocket
{
    Server::Server()
    {
        mSock = INVALID_SOCKET;
    }

    Server::~Server()
    {
        Close();
    }

    void Server::InitSocket()
    {
#ifdef _WIN32 
        WORD ver = MAKEWORD(2, 2);
        WSADATA dat;
        WSAStartup(ver, &dat);
#endif
        //�رվ�����
        if (INVALID_SOCKET != mSock)
        {
            Close();
        }
        mSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == mSock)
        {
            cout << "Create socket  failed!" << endl;
            return;
        }
        cout << "Create socket success!" << endl;

    }

    int Server::Bind(const char * ip, unsigned short port)
    {
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        auto ipAddr = (ip == nullptr? INADDR_ANY:inet_addr(ip));
#ifdef _WIN32 
        addr.sin_addr.S_un.S_addr = ipAddr;
#else
        addr.sin_addr.s_addr = ipAddr;
#endif

        if (SOCKET_ERROR == bind(mSock, (sockaddr*)&addr, sizeof(addr)))
        {
            cout << "Bind port failed!" << endl;
            return -1;
        }
        cout << "Bind port success!" << endl;
        return 0;
    }

    int Server::Listen(int num)
    {
        if (SOCKET_ERROR == listen(mSock, num))
        {
            cout << "Listen port failed!" << endl;
            return -1;
        }
        cout << "Listen port success!" << endl;
        return 0;
    }

    int Server::Accept()
    {
        sockaddr_in clientAddr;
        int len = sizeof(sockaddr_in);
        SOCKET clientSock = INVALID_SOCKET;
#ifdef _WIN32 
        clientSock = accept(mSock, (sockaddr*)&clientAddr, &len);
#else
        clientSock = accept(mSock, (sockaddr*)&clientAddr, (socklen_t *)&len);
#endif
        if (INVALID_SOCKET == clientSock)
        {
            cout << "Invalid client socket!" << endl;
            return -1;
        }

        NewClient newClient;
        newClient.sock = clientSock;
        SendDataToAll(&newClient);

        mClients.push_back(clientSock);
        cout << "Client: " << inet_ntoa(clientAddr.sin_addr) << " " << clientSock << endl;
        return 0;
    }

    void Server::Close()
    {
        if (mSock != INVALID_SOCKET)
        {
#ifdef _WIN32 
            for (int i = 0; i < mClients.size(); i++)
            {
                closesocket(mClients[i]);;
            }
            closesocket(mSock);
            WSACleanup();
#else
            for (int i = 0; i < mClients.size(); i++)
            {
                close(mClients[i]);;
            }
            close(sock);
#endif
            mSock = INVALID_SOCKET;
        }

    }

    bool Server::OnRun()
    {
        if (!IsRun())
        {
            return false;
        }
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExcept;
        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExcept);

        FD_SET(mSock, &fdRead);
        FD_SET(mSock, &fdWrite);
        FD_SET(mSock, &fdExcept);

        SOCKET maxSock = mSock;
        for (int i = 0; i < (int)mClients.size(); i++)
        {
            FD_SET(mClients[i], &fdRead);
            if (maxSock < mClients[i])
            {
                maxSock = mClients[i];
            }
        }

        timeval t = { 1,0 };
        int ret = (int)select(maxSock + 1, &fdRead, &fdWrite, &fdExcept, &t);
        if (ret < 0)
        {
            //Close();
            return false;
        }
        if (FD_ISSET(mSock, &fdRead))
        {
            FD_CLR(mSock, &fdRead);
            Accept();
        }

        for (int i = 0; i < mClients.size(); i++)
        {
            if (FD_ISSET(mClients[i], &fdRead))
            {
                if (-1 == ReceiveData(mClients[i]))
                {
                    vector<SOCKET>::iterator iter = mClients.begin() + i;
                    if (iter != mClients.end())
                    {
                        mClients.erase(iter);
                        i--;
                    }
                }
            }
        }
        cout << "����������ʱ�䣬��������ҵ��" << endl;
        return true;
    }

    bool Server::IsRun()
    {
        return mSock != INVALID_SOCKET;
    }

    int Server::ReceiveData(SOCKET clientSock)
    {
        char buf[4096] = {};      
        int recvLen = (int)recv(clientSock, buf, sizeof(DataHeader), 0);
        DataHeader *header = (DataHeader*)buf;
        if (recvLen <= 0)
        {
            cout << "Client: " << clientSock << " exit" << endl;
            return -1;
        }
        recv(clientSock, buf + sizeof(DataHeader), header->length - sizeof(DataHeader), 0);
        Processor(clientSock, header);
        return 0;
    }

    int Server::SendData(SOCKET clientSock, DataHeader * header)
    {
        if (IsRun() && header)
        {
            return send(clientSock, (const char *)header, header->length, 0);
        }
        return -1;
    }

    void Server::SendDataToAll(DataHeader * header)
    {      
        for (int i = 0; i < mClients.size(); i++)
        {
            SendData(mClients[i], header);
        }     
    }

    int Server::Processor(SOCKET clientSock, DataHeader * header)
    {
        if (Cmd_Login == header->type)
        {
            Login *login = (Login*)header;          
            cout << "Client: " << "Type = " << login->type <<
                ", name = " << login->name <<
                ", password = " << login->password << endl;

            LoginResult result;
            result.result = 0;
            SendData(clientSock, &result);
        }
        else if (Cmd_Logout == header->type)
        {
            Logout *logout = (Logout*)header;            
            cout << "Client: " << "Type = " << logout->type <<
                ", name = " << logout->name << endl;

            LogoutResult result;
            result.result = 0;
            SendData(clientSock, &result);
        }
        else
        {
            header->length = 0;
            header->type = Cmd_Error;
            SendData(clientSock, header);
        }
        return 0;
    }
}
