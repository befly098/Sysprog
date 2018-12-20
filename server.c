// original_chat_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h> // gcc 옵션에 -lpthread 사용.
#include <time.h>

#define MAX_CLIENT	4
#define CHATDATA	1024
#define INVALID_SOCK -1 // 클라이언트 소켓 배열의 초기 값.
#define TRUE 1
#define FALSE 0
typedef struct trump
{
	char shape[10];
	int num;
}trump;

void make_card();
void print_card(int index, int one_more);
void shuffle_card();
void distribute_card(int * one_more);
int popClient(int s);
int pushClient(int c_socket);
void* do_game(void *);

pthread_t thread;
pthread_mutex_t mutex;
int count_num=0, start_flag=FALSE;
trump card[53];
trump player[4][13];
int one_more=-1;
FILE *fp;
int list_c[MAX_CLIENT]; // 클라이언트들의 소켓 번호를 저장하기 위한 배열

char escape[] = "exit";
char greeting[] = "You have entered in room #";
char CODE200[] = "Sorry No More Connection\n";
char port[]= "111111";
int main(int argc, char *argv[]) {

	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int nfds = 0;
	int i, j, n;
	fd_set read_fds;
	int res;
	make_card();
	shuffle_card();
	distribute_card(&one_more);
	if (argc < 2) {
		printf("usage: %s port_number\n", argv[0]);
		exit(-1);
	}
    
	if (pthread_mutex_init(&mutex, NULL) != 0) {
		printf("Can not create mutex\n");
		return -1;
	}
    
	s_socket = socket(PF_INET, SOCK_STREAM, 0); // TCP 소켓 생성
	
	// 연결 요청을 수신할 주소와 포트 설정.
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY: IP주소
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(atoi(argv[1]));//입력 받은 port 번호를 바탕으로 계산한다.
	strcpy(port,argv[1]);
	strcat(greeting,port);
	strcat(greeting,"\n");
	// 위에서 만든 주소를 소켓에 연결.
	if (bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
		printf("Can not Bind\n");
		return -1;
	}
    
	// 운영체제에 개통 요청. 이 시점부터 연결을 받아들인다.
	if (listen(s_socket, MAX_CLIENT) == -1) {
		printf("listen Fail\n");
		return -1;
	}
    
	// 클라이언트 소켓 배열의 초기 값 세팅.
	for (i = 0; i < MAX_CLIENT; i++)
		list_c[i] = INVALID_SOCK;
    
	while(1) {
		// 클라이언트로부터의 연결 요청 수신. 듣기 소켓과 연결 소켓.
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len);
        
		// 연결 소켓의 번호를 클라이언트 배열에 추가.
		res = pushClient(c_socket);
		if (res < 0) {
			write(c_socket, CODE200, strlen(CODE200));
			close(c_socket);
		} else {
			// 클라이언트 배열에 무사히 소켓 번호가 추가됐으면 환영 인사를 보내주고 채팅 스레드를 생성한다.
			write(c_socket, greeting, strlen(greeting));
			pthread_create(&thread, NULL, do_game, (void *) c_socket);
		}
	}
	return 0;
}

void *do_game(void *arg) {
	int c_socket = (int) arg;
	char chatData[CHATDATA];
	int i,j, n;
	while(1) {
		memset(chatData, 0, sizeof(chatData));
		// 소켓으로부터 읽어온 데이터가 있으면 전체 클라이언트에 메시지를 보낸다.
		   if(count_num==MAX_CLIENT && !start_flag){
                                for(i=0;i<MAX_CLIENT;i++){
                                        strcpy(chatData,"The game is started");
					fp=fdopen(list_c[i],"w");
					fprintf(fp,"The game is started\n");
					print_card(i,one_more);
					fflush(fp);
                                }
				start_flag=TRUE;
                        }
			
		if((n = read(c_socket, chatData, sizeof(chatData))) > 0) {
			for (i = 0; i < MAX_CLIENT; i++){
				if (list_c[i] != INVALID_SOCK) 
					write(list_c[i], chatData, n);
			}
			// 종료 문자열이 포함되어 있으면 해당 클라이언트를 배열에서 지운다.
			if (strstr(chatData, escape) != NULL) 
				popClient(c_socket);
			
		}
	}
}

int pushClient(int c_socket) {
	int i,n=0;
	for (i = 0; i < MAX_CLIENT; i++) {
		pthread_mutex_lock(&mutex);
		if (list_c[i] == INVALID_SOCK) {
			count_num++;
			list_c[i] = c_socket;
			pthread_mutex_unlock(&mutex);
			return i;
		}
		pthread_mutex_unlock(&mutex);
			

	}
    
	if (i == MAX_CLIENT)
		return -1;
}

int popClient(int s) {
	int i;

	close(s);

	// push할 때와 마찬가지로 공유자원 접근을 제한 후 클라이언트를 지웁니다.
	for (i = 0; i < MAX_CLIENT; i++) {
		pthread_mutex_lock(&mutex);
		if (s == list_c[i]) {
			list_c[i] = INVALID_SOCK;	
			count_num--;
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}
void make_card()
{
	int i,j;
        char shape[4][10] = { " ♠ ", " ◆ ", " ♥ ", " ♣ " };
        for (i = 0; i < 4; i++)
        {
                for (j = 1; j < 14; j++)
                {
                        strcpy(card[i * 13 + j].shape, shape[i]);
                        card[i * 13 + j].num = j;

                        if (j == 1)
                                card[i * 13 + j].num = 'A';
                        if (j == 11)
                                card[i * 13 + j].num = 'J';
                        if (j == 12)
                                card[i * 13 + j].num = 'Q';
                        if (j == 13)
                                card[i * 13 + j].num = 'K';
                }
        }

        //joker card
        strcpy(card[0].shape, "☆ ");
        card[0].num = 0;
}

//print cardset for test
void print_card(int index, int one_more)
{
	//print card to player[index]
       	fprintf(fp,"player %d : ", index+1);
        for (int j = 0; j < 13; j++)
                fprintf(fp,"%s -%d ", player[index][j].shape, player[index][j].num);
       	if(index == one_more)
                fprintf(fp,"%s -%d ", player[index][13].shape, player[index][13].num);
       	fprintf(fp,"\n");
        
	fflush(fp);

}

//shuffle cardset
void shuffle_card()
{
        srand(time(NULL));
        int time;
        int src, dst;
        trump tmp;

        time = rand() % 20;
        time += 40;

        for (int i = 0; i < time; i++)
        {
                src = rand() % 53;
                dst = rand() % 53;

                tmp = card[src];
                card[src] = card[dst];
                card[dst] = tmp;
        }

        //to shuffle jocker to random place
        dst = rand() % 53;
        tmp = card[dst];
        card[dst] = card[0];
        card[0] = tmp;
}


//distribute card
void distribute_card(int *one_more)
{	
	int i=0;
        //player who will get the one more card
        *(one_more) = rand() % 4;
        printf("player %d will get one more card\n", (*one_more)+1);

        //distribute card
        for (i = 0; i < 52; i++)
                player[i % 4][i / 4] = card[i];

        //give one more card to selected user
        player[(*one_more)][13] = card[52];
}

