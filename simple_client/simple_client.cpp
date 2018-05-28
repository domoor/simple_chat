#include <iostream>
#include <mutex>
#include <process.h>
//#include <stdio.h>
//#include <windows.h>
#include <winsock2.h>

using namespace std;

#define STR_LEN 1024

mutex mtx;

void usage() {
	cout << "syntax: simple_client.exe <IP> <PORT>\n";
	cout << "sample: simple_client.exe 10.10.10.10 1234\n";
}

void recv_msg(void* sock) {
	while(1){
		char msg[STR_LEN];
		int res = recv(*(SOCKET*)sock, msg, STR_LEN, 0);
		if(res == SOCKET_ERROR)	break;
		res < STR_LEN ? res : res = STR_LEN-1;
		msg[res] = '\0';

		mtx.lock();
		cout << msg << endl;
		mtx.unlock();
	}
	closesocket(*(SOCKET*)sock);
	WSACleanup();
}

int main(int argc, char* argv[])
{
	if(argc != 3) {
		usage();
		return 1;
	}

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_sock == INVALID_SOCKET) {
		cout << "socket error!!\n";
		return 1;
	}

	SOCKADDR_IN srv;
	srv.sin_family = AF_INET;
	srv.sin_addr.S_un.S_addr = inet_addr(argv[1]);
	srv.sin_port = htons(atoi(argv[2]));

	int error = connect(tcp_sock, (SOCKADDR *)&srv, sizeof(srv));
	if (error == SOCKET_ERROR) {
		cout << "connect error!!\n";
		return 1;
	}

//	char recv_str[STR_LEN];
	int res = 1;
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
		puts("123");
		cin.getline(send_str, STR_LEN);
		if(!strlen(send_str)) continue;
		res = send(tcp_sock, send_str, strlen(send_str), 0);
	}while(res != SOCKET_ERROR && strncmp(send_str, "exit", res));

	closesocket(tcp_sock);
	WSACleanup();
	cout << "프로그램을 종료합니다.\n";
	return 0;
}