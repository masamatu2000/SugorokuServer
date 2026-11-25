#include<Winsock2.h>
#include <vector>
#include <ws2tcpip.h>
#include<iostream>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
int main()
{
    // WinSock2.2 初期化
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Error : WSAStartup" << endl;
        return 1;
    }
    cout << "Success : WSAStartup" << endl;

    // TCPリスンソケットの作成
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET)
    {
        cout << "Error : socket" << endl;
        return 1;
    }
    cout << "Success : socket" << endl;

    // ノンブロッキング設定
    unsigned long cmdarg = 0x01;
    ioctlsocket(listenSock, FIONBIO, &cmdarg);

    // 固定アドレスの割り当て
    SOCKADDR_IN bindSocketAddress;
    memset(&bindSocketAddress, 0, sizeof(bindSocketAddress));
    bindSocketAddress.sin_family = AF_INET;
    bindSocketAddress.sin_port = htons(58888);
    bindSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenSock, (SOCKADDR*)&bindSocketAddress, sizeof(bindSocketAddress)) == SOCKET_ERROR)
    {
        cout << "Error : bind" << endl;
        return 1;
    }
    cout << "Success : bind" << endl;

    // リスン状態に設定
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Error : listen" << endl;
        return 1;
    }
    cout << "Success : listen" << endl;

    // クライアントソケット管理
    vector<SOCKET> socks;
    SOCKADDR_IN clientAddr;
    int addressLength = sizeof(SOCKADDR_IN);

    // メインループ
    while (1)
    {
        // 接続受け入れ
        int tmpSock = accept(listenSock, (SOCKADDR*)&clientAddr, &addressLength);
        if (tmpSock > 0)
        {
            ioctlsocket(tmpSock, FIONBIO, &cmdarg);
            socks.push_back(tmpSock);
        }

        // 受信処理
        for (int i = 0; i < socks.size(); i++)
        {
           
            //int ret = recv(socks[i], (char*)&recvData, sizeof(recvData), 0);
          /*  if (ret > 0)
            {
               
            }
            else
            {
                if (WSAGetLastError() == WSAECONNRESET)
                {
                    cout << "shutdown" << endl;
                    shutdown(socks[i], SD_BOTH);
                    closesocket(socks[i]);
                    socks[i] = -1;
                }
            }*/
        }

        // 切断されたソケットの削除
        socks.erase(
            remove_if(socks.begin(), socks.end(), [](SOCKET sock)
                { return sock == -1; }),
            socks.end());
    }
}