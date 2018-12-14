#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

typedef struct trump
{
	char shape[3];
	int num;
}trump;

void make_card(trump card[]);
void print_card(trump card[]);
void shuffle_card(trump card[]);

int main()
{
	trump card[53];
	make_card(card);
	shuffle_card(card);
	print_card(card);
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
}