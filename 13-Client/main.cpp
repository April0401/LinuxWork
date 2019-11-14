#include"Client.h"
#include<iostream>
#include<thread>
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
    //启动线程
    thread t1(CmdThread);
    t1.detach();

    EasySocket::Client client;
    client.InitSocket();
    client.Connect("1.1.1.100", 4567);
    Login login;
    strcpy_s(login.name, "April");
    strcpy_s(login.password, "0000");
    while (isRun)
    {
        client.OnRun();     
        client.SendData(&login);
    }
    client.Close();
    return 0;
}