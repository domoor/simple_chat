#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <iostream>
using namespace std;

#define STR_LEN 1024

void recv_msg(void* sock) {
	while(1){
//		Sleep(100);
		char msg[STR_LEN];
//		msg[0] = '\0';
//		int res = recv(*(SOCKET*)sock, msg, sizeof(msg), 0);
		int res = recv(*(SOCKET*)sock, msg, STR_LEN, 0);
		if(res == SOCKET_ERROR)	break;
		res < STR_LEN ? res : res = STR_LEN-1;
		msg[res] = '\0';
		cout << msg << endl;
	}
	closesocket(*(SOCKET*)sock);
	WSACleanup();
}

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) {
		cout << "socket error!!\n";
		return 1;
	}

	SOCKADDR_IN srv;
	srv.sin_family = AF_INET;
	srv.sin_port = htons(1234);
	srv.sin_addr.S_un.S_addr = inet_addr("192.168.40.3");

	int error = connect(tcp_sock, (SOCKADDR *)&srv, sizeof(srv));
	if (error == SOCKET_ERROR) {
		cout << "connect error!!\n";
		return 1;
	}

//	char recv_str[STR_LEN];
	int res;
//	int res = recv(tcp_sock, recv_str, sizeof(recv_str), 0);
//	if(res == SOCKET_ERROR) {
//		cout << "Message recv error!!\n";
//		return 1;
//	}
//	recv_str[res] = '\0';
//	size = send(tcp_sock, str, sizeof(str), 0);
//	cout << recv_str << endl;
//	printf("%s\n", str);

	_beginthread(recv_msg, 0, &tcp_sock);
	char send_str[STR_LEN];
	do {
		cin >> send_str;
		res = send(tcp_sock, send_str, strlen(send_str), 0);
	}while(res != SOCKET_ERROR && strncmp(send_str, "exit", res));

	closesocket(tcp_sock);
	WSACleanup();
	return 0;
}