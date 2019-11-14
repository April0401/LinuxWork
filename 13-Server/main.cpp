
#include<iostream>
#include<thread>
#include"Server.h"
using namespace std;


bool isRun = true;
void CmdThread()
{
    while (true)
    {
        //3.输入请求命令
        char buf[128] = {};
        cin >> buf;
        if (0 == strcmp(buf, "exit"))
        {
            isRun = false;
            cout << "exit!" << endl;
            break;
        }
        else
        {
            cout << "Cmd invalid, input again." << endl;
        }
    }
}


int main()
{
    EasySocket::Server server;
    server.InitSocket();
    server.Bind(nullptr, 4567);
    server.Listen(10);
    
    //启动线程
    thread t1(CmdThread);
    t1.detach();

    while (isRun)
    {
        server.OnRun();
    }
    server.Close();
    return 0;
}