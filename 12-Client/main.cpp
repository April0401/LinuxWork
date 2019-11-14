#include"Client.h"
#include<iostream>
#include<thread>
using namespace std;


void CmdThread(EasySocket::Client *client)
{
    while (true)
    {
        //3.输入请求命令
        char buf[128] = {};
        cin >> buf;
        if (0 == strcmp(buf, "exit"))
        {
            client->Close();
            cout << "exit!" << endl;
            break;
        }
        else if (0 == strcmp(buf, "login"))
        {
            //4.发送请求命令
            Login login;
            strcpy_s(login.name, "April");
            strcpy_s(login.password, "0000");
            client->SendData(&login);
        }
        else if (0 == strcmp(buf, "logout"))
        {
            Logout logout;
            strcpy_s(logout.name, "April");
            client->SendData(&logout);
        }
        else
        {
            cout << "Cmd invalid, input again." << endl;
        }
    }
}

int main()
{
    EasySocket::Client client;
    client.InitSocket();
    client.Connect("1.1.1.100", 4567);
    //启动线程
    thread t1(CmdThread, &client);
    t1.detach();
    while (client.IsRun())
    {
        client.OnRun();
    }
    client.Close();
    return 0;
}