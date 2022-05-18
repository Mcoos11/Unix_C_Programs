#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

char polecenie[10];
char *wynik;

int main ()
{
    while (1) {
    printf("Podaj polecenie do wykonania [d,s,c,t,q]:\n");
    wynik=fgets(polecenie, sizeof(polecenie), stdin);
    //sscanf(buf, "%s", polecenie);
    if (polecenie[0]=='q') break;
	else if (polecenie[0]=='d' || polecenie[0]=='s' || polecenie[0]=='c' || polecenie[0]=='t'){
		if (fork()==0) {
            		/* ... wykonanie polecenia potomka ... */
			switch (polecenie[0]) {
            	case 'd':
					execlp("date","date",(char*)NULL);
					break;
				case 's':
					execlp("sh","sh",(char*)NULL);
					break;
				case 'c':
					execlp("xclock","xclock","-update","1",(char*)NULL);
					//system("xclock -update 1");
					break;
				case 't':
					execlp("xterm","xterm",(char*)NULL);
					break;
				}
            		exit(0); /* obowiazkowe zakonczenie potomka */
        	}
         	/* ... czekanie na potomka terminalowego ... */
        	/* ... lub sprzatanie zombie okienkowego ... */
        	switch (polecenie[0]) {
	 		case 'd':
				wait(NULL);
				break;
                        case 's':
				wait(NULL);
				break;
        	}
		}
	else printf("zle\n");
	//obsluga zombie
	while(waitpid(-1,NULL,WNOHANG));
    } /* nieskonczona petla rodzica */
	return 0;
}
