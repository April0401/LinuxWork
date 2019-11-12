#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<vector>
using namespace std;

//��Ӿ�̬���ӿ�
//����1���ڴ����м��� #pragma comment(lib, "ws2_32.lib")
//����2������ - ������ - ���� - "ws2_32.lib"
//������ƽ̨��c++����ʱ��Ӧ��ʹ�õڶ��ַ���

//�������Ϳͻ��˶�ʹ�� select ģ��
//���Ӷ���ͻ���

//��Ϣ����
enum DataType
{
    Cmd_Login,
    Cmd_LoginResult,
    Cmd_Logout,
    Cmd_LogoutResult,
    Cmd_NewClient,
    Cmd_Error
};
//��Ϣͷ
struct DataHeader
{
    int length;
    DataType type;
};

//��Ϣ��
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
    //5.���տͻ�������ͷ
    DataHeader header = {};
    int recvLen = recv(clientSock, (char*)&header, sizeof(header), 0);
    if (recvLen <= 0)
    {
        return -1;
    }
    //6.��������, ��ͻ��˷���һ������   
    if (Cmd_Login == header.type)
    {
        Login login = {};
        recv(clientSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0); //��������ͷ�Ѷ�ȡ,ƫ�ƺͳ�����Ҫ����
        cout << "Client: " << "Type = " << login.type <<
            ", name = " << login.name <<
            ", password = " << login.password << endl;
        //ҵ���߼�����֤�û���������
        LoginResult result;
        result.result = 0;
        send(clientSock, (const char*)&result, sizeof(LoginResult), 0);
    }
    else if (Cmd_Logout == header.type)
    {
        Logout logout = {};
        recv(clientSock, (char*)&logout + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0); //��������ͷ�Ѷ�ȡ,ƫ�ƺͳ�����Ҫ���� 
        cout << "Client: " << "Type = " << logout.type <<
            ", name = " << logout.name << endl;
        //ҵ���߼����˳���¼
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
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    //����windows socket ���绷��
    WSAStartup(ver, &dat);
    //1.����socket�׽���
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock)
    {
        cout << "Create socket  failed!" << endl;
        return -1;
    }
    cout << "Create socket success!" << endl;

    //2.������˿�
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

    //3.��������˿�
    if (SOCKET_ERROR == listen(sock, 10))  //������10���ͻ��˵�����
    {
        cout << "Listen port failed!" << endl;
        return -1;
    }
    cout << "Listen port success!" << endl;

    while (true)
    {
        //socket����
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExcept;
        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExcept);

        FD_SET(sock, &fdRead);
        FD_SET(sock, &fdWrite);
        FD_SET(sock, &fdExcept);

        for (int i = 0; i < g_clients.size(); i++)
        {
            FD_SET(g_clients[i], &fdRead);
        }

        timeval t = {1,0};
        // ���һ��������ʾ��ʱʱ��
        //  t = {0,0}��ʾû������ʱ,����ִ�У���������
         // t = {1,0}��ʾû������ʱ,�ȴ�1s��,����ִ�У���������
        int ret = select(sock + 1, &fdRead, &fdWrite, &fdExcept, &t);
        if (ret < 0)
        {
            break;
        }
        if (FD_ISSET(sock, &fdRead))
        {
            FD_CLR(sock, &fdRead);
            //4.�ȴ��ͻ��˵�����
            sockaddr_in clientAddr = {};
            int len = sizeof(clientAddr);
            SOCKET clientSock = INVALID_SOCKET;
            clientSock = accept(sock, (sockaddr*)&clientAddr, &len);
            if (INVALID_SOCKET == clientSock)
            {
                cout << "Invalid client socket!" << endl;
            }

            //�¿ͻ��˼���ʱ��Ⱥ����Ϣ��֪ͨ�����ͻ���
            for (int i = 0; i < g_clients.size(); i++)
            {
                NewClient newClient;
                newClient.sock = clientSock;
                send(g_clients[i], (const char*)&newClient, sizeof(NewClient), 0);
            }
            g_clients.push_back(clientSock);
            cout << "Client: " << inet_ntoa(clientAddr.sin_addr) << endl;
        }

        for (int i = 0; i < fdRead.fd_count; i++)
        {
            if (-1 == Processor(fdRead.fd_array[i]))
            {
                auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[i]);
                if (iter != g_clients.end())
                {
                    g_clients.erase(iter);
                }
            }
        }
        cout<<"����������ʱ�䣬��������ҵ��"<<endl;
    }

    for (int i = 0; i < g_clients.size(); i++)
    {
        closesocket(g_clients[i]);;
    }

    //6.�ر��׽���
    closesocket(sock);
    //�ر�
    WSACleanup();
    return 0;
}