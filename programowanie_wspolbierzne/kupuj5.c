#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <time.h>

#define MEM_SIZ 1024

	int N_GOODS = 40;
	
	int N_CUSTOMERS=20;
	int N_ROUNDS=1000000;
	int RATING=10;

	struct wsp_str{
		double Ratings[40];
		int NRatings[40];
		pthread_mutex_t lock;
	};
	struct wsp_str *pamiec;

int klient1();

int main()
{
	struct timespec startR, stopR, startCPU, stopCPU;
	long sekundy, nanosekundy;
    	double CPUczas, Rczas;
	int kupionych=0;
	//deklaracja zmiennych w pamieci wspoldzielonej
	int id;
	struct wsp_str* Pamiec_WSP = 0;

	id = shmget(0x193, MEM_SIZ, IPC_CREAT |  0666);
	if (id == -1) 
	{
		perror("blad shmget");
		exit(errno);
	}

	Pamiec_WSP = (struct wsp_str *)shmat(id, 0, 0);
	if (Pamiec_WSP == (void *)-1) {
		perror("blad shmat");
		exit(errno);
	}
	pamiec = Pamiec_WSP;

	//wypelnianie zerami
	for(int i=0; i<N_GOODS; i++)
	{
		pamiec->Ratings[i]=0;
		pamiec->NRatings[i]=0;
		//printf("%d\t%f\t%d\n",i,pamiec->Ratings[i],pamiec->NRatings[i]);
	}
	
	//ochrona mutex inicjalizacja
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&pamiec->lock, &attr);

	//zakupy
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startCPU);
	clock_gettime(CLOCK_REALTIME, &startR);

	for (int i=1; i <= N_CUSTOMERS;i++)
	{
       	 	if(fork()==0)
        	{
				pthread_mutex_lock(&pamiec->lock);
            	klient1();
				pthread_mutex_unlock(&pamiec->lock);
				exit(-1);
            }
        }
	//czekanie na koniec zaupow
	for (int i=1; i <= N_CUSTOMERS;i++)
	{
		//waitpid(Child_PID[i], NULL, 0);
		wait(NULL);
	}

	//pomiar czasu
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stopCPU);
	clock_gettime(CLOCK_REALTIME, &stopR);
	
	sekundy = stopCPU.tv_sec - startCPU.tv_sec;
    nanosekundy = stopCPU.tv_nsec - startCPU.tv_nsec;
    CPUczas = sekundy + nanosekundy*1e-9;
	
	sekundy = stopR.tv_sec - startR.tv_sec;
    nanosekundy = stopR.tv_nsec - startR.tv_nsec;
    Rczas = sekundy + nanosekundy*1e-9;



	//wyswietlanie
	printf("\nSrednie oceny kazdego produktu i liczba ich zakupow:\n");
	printf("Towar\tSrednia Ocena\tLiczba kupionych\n");
	for(int i=0; i<N_GOODS; i++)
	{
		printf("%d\t%f\t%d\n", (i+1), pamiec->Ratings[i], pamiec->NRatings[i]);
		kupionych = kupionych+pamiec->NRatings[i];
	}
	printf("\nLacznie kupionych: %d\nPowinno byc: %d\n", kupionych, (N_ROUNDS*N_CUSTOMERS));
	printf("POMIAR CZASU ZAKUPOW\n");
	printf("Czas rzeczywisty: %.3fs\n", Rczas);
	printf("Czas procesora: %.3fs\n", CPUczas);
	shmctl(id,IPC_RMID,NULL);

	pthread_mutex_destroy(&pamiec->lock);
	return 0;
}

int klient1(){	
	int GOOD;
	double Rating;
	//kupowanie i ocenianie
	for(int i=0; i<N_ROUNDS; i++)
	{
		srand(time(NULL)+i);
		GOOD = rand() % N_GOODS;
		pamiec->NRatings[GOOD]++;
		srand(time(NULL)+i);
		Rating = rand() % (RATING + 1);
		pamiec->Ratings[GOOD] = ((pamiec->Ratings[GOOD] * (pamiec->NRatings[GOOD] - 1)) + Rating) / pamiec->NRatings[GOOD];
	}
	
	return 0;
}
