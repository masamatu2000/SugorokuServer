#include<Winsock2.h>
#include <vector>
#include <ws2tcpip.h>
#include<iostream>
#include<string>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
const int COUSTOMER_NUM = 4;
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
	int index = 0;
	//それぞれの手を出した人を覚えておく変数
	vector<SOCKET>Stone;//グーを出した人
	vector<SOCKET>Paper;//パーを出した人
	vector<SOCKET>Scissor;//チョキを出した人
	//勝ったときに進める数
	const int WIN_STONE = 3;
	const int WIN_SCISSOR = 6;
	const int WIN_PAPER = 6;
	const int LOSE = 0;
	//じゃんけんが行われたか
	bool DoneJanken = false;

	// メインループ
	while (1)
	{
		//各手の移動量
		int MoveStone = 0;
		int MoveScissor = 0;
		int MovePaper = 0;
		// 接続受け入れ
		if (socks.size() < COUSTOMER_NUM) {
		int tmpSock = accept(listenSock, (SOCKADDR*)&clientAddr, &addressLength);
		if (tmpSock > 0)
		{
			ioctlsocket(tmpSock, FIONBIO, &cmdarg);
			socks.push_back(tmpSock);

		}
	}
	// 受信処理
		for (int i = 0; i < socks.size(); i++)
		{
			string recvData;
			int ret = recv(socks[i], (char*)&recvData, sizeof(recvData), 0);

			//ここでストリングでグー・チョキ・パーかをそれぞれのプレイヤーから受け取る
			// そして誰が何を出したかを覚えておく
			if (ret > 0)
			{
				if (recvData == "グー")
				{
					Stone.push_back(socks[i]);
				}
				else if (recvData == "チョキ")
				{
					Scissor.push_back(socks[i]);
				}
				else {
					Paper.push_back(socks[i]);
				}
			}

			else {
				if (WSAGetLastError() == WSAECONNRESET)
				{
					cout << "shutdown" << endl;
					shutdown(socks[i], SD_BOTH);
					closesocket(socks[i]);
					socks[i] = -1;
				}
			}

	}
	//全員が手を出したかチェック
	if (Stone.size() + Scissor.size() + Paper.size() == socks.size()) {
		DoneJanken = true;
		//じゃんけんの勝敗判定
		if (Stone.size() > 0 && Scissor.size() > 0 && Paper.size() > 0)
		{
			//あいこ
		}
		else if (Stone.size() > 0 && Scissor.size() > 0 && Paper.size() <= 0)
		{
			//グー（Stone）が勝ち
			MoveStone = WIN_STONE;
		}
		else if (Stone.size() > 0 && Scissor.size() <= 0 && Paper.size() > 0)
		{
			//パーが勝ち
			MovePaper = WIN_PAPER;
		}
		else if (Stone.size() <= 0 && Scissor.size() > 0 && Paper.size() > 0)
		{
			//チョキが勝ち
			MoveScissor = WIN_SCISSOR;
		}
		for (const auto& s : Stone)
		{
			int ret = send(s, (char*)&MoveStone, sizeof(MoveStone), 0);
			if (ret < 0)
				cout << "Error:Send" << endl;
		}
		for (const auto& p : Paper)
		{
			int ret = send(p, (char*)&MovePaper, sizeof(MovePaper), 0);
			if (ret < 0)
				cout << "Error:Send" << endl;
		}
		for (const auto& sc : Scissor)
		{
			int ret = send(sc, (char*)&MoveScissor, sizeof(MoveScissor), 0);
			if (ret < 0)
				cout << "Error:Send" << endl;
		}
	}
	//じゃんけんが行われた時だけ出された手をリセット
	if (DoneJanken) {
		Stone.clear();
		Scissor.clear();
		Paper.clear();
		DoneJanken = false;
	}
		// 切断されたソケットの削除
		socks.erase(
		remove_if(socks.begin(), socks.end(), [](SOCKET sock)
			{ return sock == -1; }),
			socks.end());
	}
}
