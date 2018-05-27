#include <winsock2.h>
#include <WS2tcpip.h> // inet_ntop
#include <stdio.h>
#include <stdint.h>
//#include <string>
#include <process.h>
#include <list>
#include <mutex>
#include <iostream>

using std::list;
using std::mutex;
using std::cout;

#define FULL		100
#define STR_LEN		1024

list<SOCKET> user;
//list<SOCKET>::iterator user_num;
//	list<SOCKET>::iterator user_num = user.begin();
mutex mtx;

void recv_clt(void* clt) {
	mtx.lock();
	printf("식별 번호 : %d\n\n", *(SOCKET*)clt);
	SOCKET sock = *(SOCKET*)clt;
	mtx.unlock();

	char msg[STR_LEN]="Simple chating Server in!\nHi!!!";
	int res = send(sock, msg, strlen(msg), 0);
	while(res != SOCKET_ERROR) {
		msg[0] = '\0';
		res = recv(sock, msg, sizeof(msg), 0);

		mtx.lock();
		char edit_msg[STR_LEN] = {0};
		sprintf(edit_msg, "[ %d ] ", sock);
		int size = strlen(edit_msg);
		res < STR_LEN-size ? res : res = STR_LEN-size;
		strncat(edit_msg, msg, res);

		for(list<SOCKET>::iterator it=user.begin(); it != user.end(); it++) {
			if(*it != sock) send(*it, edit_msg, strlen(edit_msg), 0);
		}
		mtx.unlock();
		if(!strncmp(msg, "exit", res)) res == SOCKET_ERROR;
	}
	list<SOCKET>::iterator del_it;
	for (del_it = user.begin(); *del_it != sock; del_it++);
	user.erase(del_it);
	closesocket(sock);
	mtx.lock();
	cout << "익명의 이용자 (식별 번호 : " << sock << " ) 퇴장\n\n";
	mtx.unlock();

}

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) {
		printf("소켓 에러!!\n");
		return 0;
	}

	SOCKADDR_IN srv;
	srv.sin_family = AF_INET;
	srv.sin_port = htons(1234);
//	srv.sin_addr.s_addr = htonl(INADDR_ANY);
	srv.sin_addr.s_addr = inet_addr("192.168.40.3"); // 아이피가 존재하지 않으면 bind error
														// 서버 아이피

	int error = bind(tcp_sock, (SOCKADDR *)&srv, sizeof(srv));
	if (error == SOCKET_ERROR) {
		cout << "socket error!!\n";
		return 1;
	}

	error = listen(tcp_sock, SOMAXCONN);	// 서비스 과부하시 에러
	if (error == SOCKET_ERROR) {
		printf("listen error!!\n");
		return 0;
	}

	SOCKADDR_IN clt_addr;	// client address
	int clt_len = sizeof(clt_addr);
	while(1) {
//		SOCKET clt = accept(tcp_sock, (SOCKADDR *)&clt_addr, &clt_len);
//				└클라이언트를 구분짓기 위한 변수로 이해하기
		user.push_back(accept(tcp_sock, (SOCKADDR *)&clt_addr, &clt_len));
	/*
		if (clt == INVALID_SOCKET) {
			printf("accept error!!\n");
			return 0;
		}
	*/
//		printf("clt : %d\t%u\n", *user_num, *user_num);
//		printf("clt : %d\t%u", clt, clt);
//		if(*(++user_num) != INVALID_SOCKET) {
		if(user.back() != INVALID_SOCKET) {
//		if(clt != INVALID_SOCKET) {
			_beginthread(recv_clt, 0, &user.back());
			char ip_buf[16];
//			printf("%d번째 이용자 < %s > 입장\n", user.size(), inet_ntop(AF_INET, &clt_addr.sin_addr, ip_buf, 16));
			mtx.lock();
			cout << user.size() << "번째 익명의 이용자 < " << inet_ntop(AF_INET, &clt_addr.sin_addr, ip_buf, 16) << " > 입장\n";
			mtx.unlock();
		}
		else {
//			user_num--;
			user.pop_back();
		}
	}
/*
	char str[1024];
	recv(clt, str, sizeof(str), 0); // 사용자 구분(clt 변수)
	printf("%s\n", str);

	recv(clt, str, sizeof(str), 0); // 사용자 구분(clt 변수)
	printf("%s\n", str);
*/
	closesocket(tcp_sock);
	WSACleanup();
	return 0;
}