#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define GNIAZDKO_SERWERA "/tmp/gniazdko_serwera846"
#define GNIAZDKO_KLIENTA "/tmp/gniazdko_klienta298"

#define N_GOODS  40
	
	int N_CUSTOMERS=40;
	int N_ROUNDS=100;
	int RATING=10;
	int d;

int klient1();

int main()
{
	double Ratings[N_GOODS]; 
	int NRatings[N_GOODS], GOOD_S, kupionych, Rating_S;

	int n, sock;
	socklen_t serv_len, cli_len;
	struct sockaddr_un serv_addrstr, cli_addrstr;
	char buf[BUFSIZ]; // bufor o domysl.rozm.

	//wypelnianie zerami
	for(int i=0; i<N_GOODS; i++)
	{
		Ratings[i]=0;
		NRatings[i]=0;
		
	}	

	//przygotowanie gniazdka i struktury adres.
	sock = socket(PF_UNIX, SOCK_DGRAM, 0);
	serv_addrstr.sun_family = AF_UNIX;
	strcpy(serv_addrstr.sun_path, GNIAZDKO_SERWERA);
	
	//rejestracja adresu/utworzenie gniazdka
	unlink(GNIAZDKO_SERWERA);
	serv_len = sizeof(serv_addrstr);
	if (-1 == bind(sock,(struct sockaddr*)&serv_addrstr,serv_len)) 
	{
		perror("blad bind");
		exit(-1);
	} 
	
	//zakupy
	for (d=1; d <=N_CUSTOMERS;d++)
	{
       	if(fork()==0)
        {
            klient1();
			exit(1);
        }
    }
	//nieskonczona petla obslugi klientow
	printf("Serwer: zbieranie informacji od klientow\n");
	cli_len = sizeof(cli_addrstr);
	
	for(int i = 0; i<N_ROUNDS*N_CUSTOMERS; i++)//while(1)
	{	
		//odbieranie
		n=recvfrom(sock, buf, sizeof(buf), 0,(struct sockaddr *)&cli_addrstr,&cli_len);
		if (n == -1)
		{
			perror("blad read");
			printf("Serwer: blad odczytu, kontynuuje.\n");
		} 
		else
		{
			memcpy((void *)&GOOD_S, (void *)buf, sizeof(int));
			memcpy((void *)&Rating_S, (void *)(buf+sizeof(int)), sizeof(int));
			NRatings[GOOD_S]++;
			Ratings[GOOD_S] = ((Ratings[GOOD_S] * (NRatings[GOOD_S] - 1)) + Rating_S) / NRatings[GOOD_S];
		}  
		//printf("Serwer: odebral  GOOD nr %d, i ocene %d\n",GOOD_S, Rating_S);
	}		
	//wyswietlanie
	printf("\n\nSrednie oceny kazdego produktu i liczba ich zakupow:\n");
	printf("Towar\tSrednia Ocena\tLiczba kupionych\n");
	for(int i=0; i<N_GOODS; i++)
	{
		printf("%d\t%f\t%d\n", (i+1), Ratings[i], NRatings[i]);
		kupionych = kupionych+NRatings[i];
	}
	printf("Kupionych: %d\n", kupionych);
	
	close(sock);
	return 0;
	
}

int klient1(){	

	int GOOD, Rating;
	
	int n, sock, serv_len, cli_len;
	struct sockaddr_un serv_addrstr, cli_addrstr;
	char buf[BUFSIZ]; // bufor o domysl.rozm.
	struct timespec timeout = {0, 100000000};

	//przygotowanie struktur adres
	serv_addrstr.sun_family = AF_UNIX;
	strcpy(serv_addrstr.sun_path, GNIAZDKO_SERWERA);
	serv_len = sizeof(serv_addrstr);
	
	unlink(GNIAZDKO_KLIENTA);
	sock = socket(PF_UNIX, SOCK_DGRAM, 0);
	cli_addrstr.sun_family = AF_UNIX;
	sprintf(cli_addrstr.sun_path, GNIAZDKO_KLIENTA,getpid());
	cli_len = sizeof(cli_addrstr);
	unlink(cli_addrstr.sun_path);
	if (bind(sock, (struct sockaddr *)&cli_addrstr,cli_len)==-1) 
	{
		perror("blad bind");
		return -1;
	}

	//kupowanie, ocenianie i wysyłanie
	for(int i=0; i<N_ROUNDS; i++)
	{
		srand(time(NULL)+i+d);
		GOOD = rand() % N_GOODS;
		srand(time(NULL)+i+d);
		Rating = rand() % (RATING + 1);
		//printf("wyslano good: %d, raating: %d\n", GOOD, Rating);
		
		memcpy((void *)buf, (void *)&GOOD, sizeof(int));
		memcpy((void *)(buf+sizeof(int)), (void *)&Rating, sizeof(int));
		sendto(sock, buf, sizeof(buf), 0,(struct sockaddr *) &serv_addrstr,serv_len);
		
	}
	nanosleep((struct timespec *)&timeout,(struct timespec *)0);
	
	close(sock);
	return 0;
}
