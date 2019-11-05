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
    //2.连接服务器
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;     //ipv4
    addr.sin_port = htons(4567);   // host to net unsigned short
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  //ip addr
    if (SOCKET_ERROR == connect(sock, (sockaddr*)&addr, sizeof(addr)))
    {
        cout << "Connect  failed!" << endl;
    }
 
    while (true)
    {
        //3.输入请求命令
        char cmdBuf[128] = {};
        cin >> cmdBuf;
       // scanf_s("%s",cmdBuf);
        if (0 == strcmp(cmdBuf, "exit"))
        {
            break;
        }
        //4.发送请求命令
        send(sock, cmdBuf, strlen(cmdBuf)+1, 0);
        //5.接收服务器的信息
        char msg[128] = {};
        if (recv(sock, msg, sizeof(msg), 0) > 0)
        {
            cout << "Server: " << msg << endl;
        }
    }
    
    //4.关闭套接字
    closesocket(sock);
    //关闭
    WSACleanup();
    getchar();
    return 0;
}