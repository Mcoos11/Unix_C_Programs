#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

	int N_GOODS = 40;
	
	int N_CUSTOMERS=20;
	int N_ROUNDS=1000000;
	int RATING=10;

	double Ratings[40]; 
	int NRatings[40];

int main()
{	
	int GOOD, test=0;
	double Rating;
	//wypelnianie zerami
	for(int i=0; i<N_GOODS; i++)
	{
		Ratings[i]=i;
		NRatings[i]=0;
		//printf("%d\t%f\t%d\n",i,Ratings,NRatings);
	}
	//kupowanie i ocenianie
	for(int i=0; i<N_ROUNDS; i++)
	{
		srand(time(NULL)+i);
		GOOD = rand() % N_GOODS;
		//printf("%d\n", GOOD);
		NRatings[GOOD]++;
		srand(time(NULL)+i);
		Rating = rand() % (RATING + 1);
		//printf("%f\n",Rating);
		Ratings[GOOD] = ((Ratings[GOOD] * (NRatings[GOOD]-1)) + Rating) / NRatings[GOOD];
	}
	//sprawdzanie
	for(int i=0; i<N_GOODS; i++)
	{
		test = test + NRatings[i];
	}
	if(test == N_ROUNDS)
	{
		printf("Działa. Liczba zakupionych towarow: %d\n", test);
		test = 0;
	}
	else
	{
		printf("NIE Dziala. Liczba zkupionych towarow: %d\nPowino byc: %d\n", test, N_ROUNDS);
		test = 0;
	}
	//wyswietlanie
	printf("Srednie oceny kazdego produktu i liczba ich zakupow - klient1:\n");
	printf("Towar\tSrednia Ocena\tLiczba kupionych\n");
	for(int i=0; i<N_GOODS; i++)
		printf("%d\t%f\t%d\n", (i+1), Ratings[i], NRatings[i]);

	return 0;
}
