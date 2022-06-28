//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
using namespace std;

SOCKET Connections[100];
int Counter = 0;
enum Packet { Pack, Test };
DWORD WINAPI ServerThread(LPVOID number) {
    Packet packettype;
    int index = ((int*)number)[0];
    cout << "socket number=" << index << endl;
    SOCKET Con = Connections[index];
    while (true) {
        recv(Con, (char*)&packettype, sizeof(Packet), NULL);
        if (packettype == Pack) {
            cout << "recieve packet PACK" << endl;
            int msg_size, toWhom;
            recv(Con, (char*)&toWhom, sizeof(int), NULL);
            recv(Con, (char*)&msg_size, sizeof(int), NULL);
            char* msg = new char[msg_size + 1];
            msg[msg_size] = 0;
            recv(Con, msg, msg_size, NULL);  cout << "Client" << index << ": " << msg << endl;
            if (toWhom == -1) {
                for (int i = 0; i < Counter; i++)
                {
                    if (i == index)continue;
                    Packet msgtype = Pack;
                    send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
                    send(Connections[i], (char*)&index, sizeof(int), NULL);
                    send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
                    send(Connections[i], msg, msg_size, NULL);
                }
                delete[] msg;
            }
            else {
                Packet msgtype = Pack;
                send(Connections[toWhom], (char*)&msgtype, sizeof(Packet), NULL);
                send(Connections[toWhom], (char*)&index, sizeof(int), NULL);
                send(Connections[toWhom], (char*)&msg_size, sizeof(int), NULL);
                send(Connections[toWhom], msg, msg_size, NULL);
                delete[] msg;
            }
        }
        else {
            cout << "Unrecognized packet: " << packettype << endl; break;
        }
    }
    closesocket(Con);
    return 0;
}
int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    WSAData wsaData;        //WSAStartup
    WORD DllVersion = MAKEWORD(2, 1);
    if (WSAStartup(DllVersion, &wsaData) != 0)
    {
        cout << "Error" << endl; exit(1);
    }
    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(123);
    addr.sin_family = AF_INET;
    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, 10);
    SOCKET newConnection;
    for (int i = 0; i < 10; i++) {
        newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
        if (newConnection == 0) { cout << "Error #2\n"; }
        else {
            cout << "Client Connected!\n";
            string msg = "Welcome to chat!";
            int msg_size = msg.size(); Packet msgtype = Pack;
            send(newConnection, (char*)&msgtype, sizeof(Packet), NULL);
            send(newConnection, (char*)&i, sizeof(int), NULL);
            send(newConnection, (char*)&msg_size, sizeof(int), NULL);
            send(newConnection, (char*)&msg[0], msg_size, NULL);
            Connections[i] = newConnection;     Counter++;
            cout << "count=" << Counter << endl;
            CreateThread(NULL, NULL, ServerThread, &i, NULL, NULL);
            Packet testpacket = Test;
            cout << "type packet=TEST" << endl;
            send(newConnection, (char*)&testpacket, sizeof(Packet), NULL);
        }
    }
    system("pause");
    return 0;
}



