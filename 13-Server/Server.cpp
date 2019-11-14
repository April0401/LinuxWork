
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
        //关闭旧连接
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

        mClients.push_back(new ClientSocket(clientSock));
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
                closesocket(mClients[i]->mSocket);
                delete mClients[i];
            }
            closesocket(mSock);
            WSACleanup();
#else
            for (int i = 0; i < mClients.size(); i++)
            {
                close(mClients[i]->mSocket);
                delete mClients[i];
            }
            close(sock);
#endif
            mSock = INVALID_SOCKET;
        }
        mClients.clear();
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
            FD_SET(mClients[i]->mSocket, &fdRead);
            if (maxSock < mClients[i]->mSocket)
            {
                maxSock = mClients[i]->mSocket;
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
            if (FD_ISSET(mClients[i]->mSocket, &fdRead))
            {
                if (-1 == ReceiveData(mClients[i]))
                {
                    vector<ClientSocket*>::iterator iter = mClients.begin() + i;
                    if (iter != mClients.end())
                    {
                        //delete mClients[i];
                        mClients.erase(iter);
                        i--;
                    }
                }
            }
        }
        //cout << "服务器空闲时间，处理其他业务" << endl;
        return true;
    }

    bool Server::IsRun()
    {
        return mSock != INVALID_SOCKET;
    }

    char _buf[_BufSize] = {};
    int Server::ReceiveData(ClientSocket* clientSock)
    {
        int recvLen = (int)recv(clientSock->mSocket, _buf, _BufSize, 0);
        if (recvLen <= 0)
        {
            cout << "Client: " << clientSock->mSocket << " exit" << endl;
            return -1;
        }
        memcpy(clientSock->mBuffer + clientSock->mPos, _buf, recvLen);
        clientSock->mPos += recvLen;

        while (clientSock->mPos >= sizeof(DataHeader))
        {
            DataHeader *header = (DataHeader*)clientSock->mBuffer;
            if (clientSock->mPos >= header->length)
            {
                //处理网络消息
                Processor(clientSock->mSocket,header);
                int len = clientSock->mPos - header->length;
                memcpy(clientSock->mBuffer, clientSock->mBuffer + header->length, len);
                clientSock->mPos = len;
            }
            else
            {
                //缓冲区内的数据不够一条完整的消息
                break;
            }
        }
     
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
            SendData(mClients[i]->mSocket, header);
        }     
    }

    int Server::Processor(SOCKET clientSock, DataHeader * header)
    {
        if (Cmd_Login == header->type)
        {
            Login *login = (Login*)header;          
           /* cout << "Client: " << "Type = " << login->type <<
                ", name = " << login->name <<
                ", password = " << login->password << endl;*/

            LoginResult result;
            result.result = 0;
            SendData(clientSock, &result);
        }
        else if (Cmd_Logout == header->type)
        {
            Logout *logout = (Logout*)header;            
           /* cout << "Client: " << "Type = " << logout->type <<
                ", name = " << logout->name << endl;*/

            LogoutResult result;
            result.result = 0;
            SendData(clientSock, &result);
        }
        else
        {
            cout << "Error message." << endl;
            /*header->length = sizeof(DataHeader);
            header->type = Cmd_Error;
            SendData(clientSock, header);*/
        }
        return 0;
    }
}
