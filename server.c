#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct trump
{
	char shape[3];
	int num;
}trump;

void make_card(trump card[]);
void print_card(trump card[]);
void shuffle_card(trump card[]);
void distribute_card(trump card[], trump **player, int one_more);
int main()
{
	int one_more = -1;
	trump card[53];
	trump **player = NULL;

	make_card(card);
	shuffle_card(card);
	print_card(card);
	distribute_card(card, player, one_more);
}

//make card set
void make_card(trump card[])
{
	char shape[4][3] = { "¢¼", "¡ß", "¢¾", "¢À" };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 1; j < 14; j++)
		{
			strcpy(card[i * 13 + j].shape, shape[i]);
			card[i * 13 + j].num = j;

			if (j == 0)
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
	strcpy(card[0].shape, "¡Ù");
	card[0].num = 0;
}

//print cardset for test
void print_card(trump card[])
{
	for (int i = 0; i < 53; i++)
	{
		if(card[i].num < 11)
			printf("%s-%d ", card[i].shape, card[i].num);
		else
			printf("%s-%c ", card[i].shape, card[i].num);
		if (i % 13 == 0)
			printf("\n");
	}
}

//shuffle cardset
void shuffle_card(trump card[])
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
void distribute_card(trump card[], trump **player, int one_more)
{
	//dynamic allocation of player set
	player = (trump*)malloc(sizeof(trump*) * 4);
	for (int i = 0; i < 4; i++)
		player[i] = malloc(sizeof(trump) * 14);

	//player who will get the one more card
	one_more = rand() % 4;
	printf("player %d will get one more card\n", one_more+1);

	//distribute card
	for (int i = 0; i < 52; i++)
		player[i % 4][i / 4] = card[i];

	//give one more card to selected user
	player[one_more][13] = card[52];

	//print card to test
	for (int i = 0; i < 4; i++) 
	{
		printf("player %d : ", i+1);
		for (int j = 0; j < 12; j++)
		{
			printf("%s-%d ", player[i][j].shape, player[i][j].num);
		}
		if(i == one_more)
			printf("%s-%d ", player[i][13].shape, player[i][13].num);
		printf("\n");
	}
}