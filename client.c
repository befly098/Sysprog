// original_chat_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>

#define CHATDATA 1024

void *do_send_chat(void *);
void *do_receive_chat(void *);

pthread_t thread_1, thread_2;

char escape[] = "exit";
char nickname[20];

int main (int argc, char *argv[]) {
	int c_socket;
	struct sockaddr_in c_addr;
	int len;
	char chatData[CHATDATA];
	char buf[CHATDATA];
	int nfds;
	fd_set read_fds;
	int n;

	if (argc < 3) {
		printf("usage : %s ip_address port_number\n", argv[0]);
		exit(-1);
	}
    
	c_socket = socket(PF_INET, SOCK_STREAM, 0);
    
	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = inet_addr(argv[1]); // 서버 주소를 설정한다.
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(atoi(argv[2]));
    
	printf("Input Nickname : ");
	scanf("%s", nickname);

	// 닉네임을 입력받고 설정한 주소를 통해 연결을 시도한다.
	if (connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr)) == -1) {
		printf("Can not connect\n");
		return -1;
	}

	// 연결한 소켓을 이용하여 채팅 송수신 함수를 스레드로 생성 후 실행한다.
	pthread_create(&thread_1, NULL, do_send_chat, (void *) c_socket);
	pthread_create(&thread_2, NULL, do_receive_chat, (void *) c_socket);
    
	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);
    
	close(c_socket);
	return 0;
}

void *do_send_chat(void *arg) {
	char chatData[CHATDATA];
	char buf[CHATDATA];
	int n;
	int c_socket = (int) arg;
    
	while(1) {
		memset(buf, 0, sizeof(buf));
		// buf에 값이 들어오면 이 값을 닉네임과 합쳐서 서버와 연결된 소켓으로 송신한다.
		if ((n = read(0, buf, sizeof(buf))) > 0) {
			sprintf(chatData, "[%s] %s", nickname, buf);
			write(c_socket, chatData, strlen(chatData));
			
			// 종료 문자열이 들어오면 스레드를 삭제한다.
			if (!strncmp(buf, escape, strlen(escape))) {
				pthread_kill(thread_2, SIGINT);
				break;
			}
		}
	}
}

void *do_receive_chat(void *arg) {
	char chatData[CHATDATA];
	int n;
	int c_socket = (int) arg;
    
	while(1) {
		memset(chatData, 0, sizeof(chatData));
		// 읽어온 값이 있으면 값을 출력한다.
		if ((n = read(c_socket, chatData, sizeof(chatData))) > 0) {
			write(1, chatData, n);// 1은 표준 출력을 의미한다.
		}
	}
}

