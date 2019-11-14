
#include<iostream>
#include"Server.h"
using namespace std;


int main()
{
    EasySocket::Server server;
    server.InitSocket();
    server.Bind(nullptr, 4567);
    server.Listen(10);
    
    while (server.IsRun())
    {
        server.OnRun();
    }
    server.Close();
    return 0;
}