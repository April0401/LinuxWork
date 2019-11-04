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
    //2.绑定网络端口
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;     //ipv4
    addr.sin_port = htons(4567);   // host to net unsigned short
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  //ip addr
    if (SOCKET_ERROR == bind(sock, (sockaddr*)&addr, sizeof(addr)))
    {
        cout << "Bind port failed!" << endl;
        return -1;
    }
    //3.监听网络端口
    if (SOCKET_ERROR == listen(sock, 10))  //最多接收10个客户端的连接
    {
        cout << "Listen port failed!" << endl;
        return -1;
    }
    //4.等待客户端的连接
    sockaddr_in clientAddr = {};
    int len = sizeof(clientAddr);
    SOCKET clientSock = INVALID_SOCKET;
    char msg[] = "Hello, I'm server.";
    while (true)
    {
        clientSock = accept(sock, (sockaddr*)&clientAddr, &len);
        if (INVALID_SOCKET == clientSock)
        {
            cout << "Invalid client socket!" << endl;
        }
        cout << "Client: " << inet_ntoa(clientAddr.sin_addr) << endl;
        //5.向客户端发送一条数据      
        send(clientSock, msg, sizeof(msg) + 1, 0);
    }
    
    //6.关闭套接字
    closesocket(sock);
    //关闭
    WSACleanup();
    return 0;
}