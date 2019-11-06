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

enum DataType
{
    Student,
    Other
};
//消息头
struct DataHeader
{
    DataType type;
};

//消息体
struct DataStudent
{
    char name[32];
    int age;
};

struct DataMessage
{
    char log[128];
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
        cout << "Create sock  failed!" << endl;
        return -1;
    }
    //2.连接服务器
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;     //ipv4
    addr.sin_port = htons(4567);   // host to net unsigned short
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  //ip addr
    if (SOCKET_ERROR == connect(sock, (sockaddr*)&addr, sizeof(addr)))
    {
        cout << "Connect  failed!" << endl;
    }

    cout << "输入请求类型：0：学生信息，1：附加信息" << endl;
    while (true)
    {
        //3.输入请求命令
        DataHeader sendHeader = {};    
        int index;
        cin >> index;
        if (index != 0 && index != 1)
        {
            index = 2;
        }
        sendHeader.type = (DataType)index;
        //4.发送请求命令
        send(sock, (char*)&sendHeader, sizeof(DataHeader), 0);
        //5.接收服务器的信息
        DataHeader header = {};
        if (recv(sock, (char *)&header, sizeof(DataHeader), 0) > 0)
        {
            if (DataType::Student == header.type)
            {
                DataStudent info = {};
                recv(sock, (char *)&info, sizeof(DataStudent), 0);
                cout << "Server: name = " << info.name << ", age= " << info.age << endl;
            }
            else
            {
                DataMessage info = {};
                recv(sock, (char *)&info, sizeof(DataMessage), 0);
                cout << "Server: " << info.log << endl;
            }
        }
    }

    //4.关闭套接字
    closesocket(sock);
    //关闭
    WSACleanup();
    getchar();
    return 0;
}