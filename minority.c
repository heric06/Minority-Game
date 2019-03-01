#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "libmin.h"

/* valori predefiniti in caso di NULL input */
#define STD_N 		2001
#define STD_D 		250
#define STD_M		3
#define STD_S		7
/* limiti per evitare sovraccarichi di memoria */
#define LIM_N		32767
#define LIM_D		16384
#define LIM_M		6
#define LIM_S		16
#define all			tot
/* uso questo meccanismo bit-wise per risparmio memoria */
#define ATT 		(1<<0) /* se attiva, mostra il valore corrente di A             */
#define BIN 		(1<<1) /* se attiva, mostra il formato binario della memoria    */
#define ERR 		(1<<2) /* accertamento di presa visione dei warning             */
#define FST 		(1<<3) /* se attiva, non mostra l'interfaccia della simulazione */
#define EXE 		"minority"

void print_usage()
{
printf("\
Uso: %s [OPZIONI] [VALORI]\n\
Opzioni:\n\
 -n <num>\t--num <num>\tSimula per num agenti\n\
 -d <days>\t--day <days>\tSimula per days giorni\n\
 -m <size>\t--mem <size>\tSimula con grandezza memoria size\n\
 -s <val>\t--str <val>\tSimula con val strategie per agente\n\
 -f\t\t--fast\t\tSimula senza interfaccia\n\
 -h\t\t--help\t\tMostra questo messaggio\n", EXE);	
}

int main(int argc, char *argv[])
{
	/* lista concatenata di agenti */
	llist_t *agents = NULL;
	uint num = STD_N, days = STD_D, mem = STD_M, strats = STD_S;
	uint i, d = 0, iter = 0;
	char prop = 0b0000;			/* variabile di 8 bit che gestisce 4 proprietà */
	char path[] = "results/";	/* i file dei risultati verranno salvati qui   */
	char *filename;				/* nome finale del file di output ATT_xx.dat   */
	char c;
	FILE *fp;
	
	int opt;
	static struct option long_options[] = 
	{
		{"num",		required_argument,	NULL, 'n'},
		{"day",		required_argument, 	NULL, 'd'},
		{"mem",		required_argument,	NULL, 'm'},
		{"str", 	required_argument, 	NULL, 's'},
		{"fast",	no_argument,		NULL, 'f'},
		{"help", 	no_argument, 		NULL, 'h'},
		{NULL, 0, NULL, 0}
	};
	
	while( (opt = getopt_long(argc, argv, "n:d:m:s:fhv", long_options, NULL)) != -1)
	{
		iter++;
		switch(opt)
		{
			case 'n':
				/* ottengo il numero totale di agenti per la simulazione
				 * dal primo argomento, se esiste, della funzione main, 
				 * altrimenti il numero di default è STD_N */
				if(argv[++iter] && atoi(argv[iter])>1 && atoi(argv[iter])<=LIM_N){
					num = atoi(argv[iter]);
				}
				else if(argv[iter] && atoi(argv[iter]) > LIM_N){
					printf("Agents number was exceeding max value %d\n", LIM_N);
					printf("Input will be resized to that value\n");
					prop |= ERR;
					num = LIM_N;
				}
				break;
				
			case 'd':
				/* ottengo il numero totale di giorni per la simulazione
				 * dal secondo argomento, se esiste, della funzione main, 
				 * altrimenti il numero di default è STD_D */
				if(argv[++iter] && atoi(argv[iter])>1 && atoi(argv[iter])<=LIM_D){
					days = atoi(argv[iter]);
				}
				else if(argv[iter] && atoi(argv[iter]) > LIM_D){
					printf("Days number was exceeding max value %d\n", LIM_D);
					printf("Input will be resized to that value\n");
					prop |= ERR;
					days = LIM_D;
				}
				break;
				
			case 'm':
				/* ottengo il numero totale di giorni di memoria gobale
				 * dal terzo argomento, se esiste, della funzione main, 
				 * altrimenti il numero di default è STD_M */
				if(atoi(argv[++iter])>1 && atoi(argv[iter])<=LIM_M){
					mem = atoi(argv[iter]);
				}
				else if(atoi(argv[iter]) > LIM_M){
					printf("Memory size was exceeding max value %d\n", LIM_M);
					printf("Input will be resized to that value\n");
					prop |= ERR;
					mem = LIM_M;
				}
				break;
				
			case 's':
				/* ottengo il numero totale di strategie di ogni agente
				 * dal quarto argomento, se esiste, della funzione main, 
				 * altrimenti il numero di default è STD_S */
				if(atoi(argv[++iter])>1 && atoi(argv[iter])<=LIM_S){
					strats = atoi(argv[iter]);
				}
				else if(atoi(argv[iter]) > LIM_S){
					printf("Strategy number was exceeding max value %d\n", LIM_S);
					printf("Input will be resized to that value\n");
					prop |= ERR;
					strats = LIM_S;
				}
				break;
				
			case 'h': print_usage(); break;
			case 'f': prop |= FST; break;
			default: break;
		}
	}

	/* inizializzo il nodo centrale a cui concatenare gli altri agenti */
	agents = malloc(sizeof(llist_t));
	agents->next = agents->prev = NULL;
	
	/* ottengo il numero totale di agenti per la simulazione
	 * dal primo argomento, se esiste, della funzione main, 
	 * altrimenti il numero di default è STD_N 
	if(argv[1] && atoi(argv[1])>1 && atoi(argv[1])<=LIM_N){
		num = atoi(argv[1]);
	}
	else if(argv[1] && atoi(argv[1])>LIM_N){
		printf("Agents number was exceeding max value %d\n", LIM_N);
		printf("Input will be resized to that value\n");
		prop |= ERR;
		num = LIM_N;
	}
	else
		num = STD_N;

	/* ottengo il numero totale di giorni per la simulazione
	 * dal secondo argomento, se esiste, della funzione main, 
	 * altrimenti il numero di default è STD_D 
	if(argv[2] && atoi(argv[2])>0 && atoi(argv[2])<=LIM_D){
		days = atoi(argv[2]);
	}
	else if(argv[2] && atoi(argv[2])>LIM_D){
		printf("Days number was exceeding max value %d\n", LIM_D);
		printf("Input will be resized to that value\n");
		prop |= ERR;
		days = LIM_D;
	}
	else
		days = STD_D;

	/* ottengo il numero totale di giorni di memoria gobale
	 * dal terzo argomento, se esiste, della funzione main, 
	 * altrimenti il numero di default è STD_M 
	if(argv[3] && atoi(argv[3])>0 && atoi(argv[3])<=LIM_M){
		mem = atoi(argv[3]);
	}
	else if(argv[3] && atoi(argv[3])>LIM_M){
		printf("Memory size was exceeding max value %d\n", LIM_M);
		printf("Input will be resized to that value\n");
		prop |= ERR;
		mem = LIM_M;
	}
	else
		mem = STD_M;

	/* ottengo il numero totale di strategie di ogni agente
	 * dal quarto argomento, se esiste, della funzione main, 
	 * altrimenti il numero di default è STD_S 
	if(argv[4] && atoi(argv[4])>0 && atoi(argv[4])<=LIM_S){
		strats = atoi(argv[4]);
	}
	else if(argv[4] && atoi(argv[4])>LIM_S){
		printf("Strategy number was exceeding max value %d\n", LIM_S);
		printf("Input will be resized to that value\n");
		prop |= ERR;
		strats = LIM_S;
	}
	else
		strats = STD_S;

	/* ottengo l'opzione per la simulazione di sola stampa dei risultati
	 * dal quinto argomento, se esiste, della funzione main,
	 * utile per comparare velocemente i grafici delle affluenze 
	if(argv[5] && (strcmp(argv[5], "--fast")==0)){
		prop |= FST;
	}
	
	/* costruisco i filename considerando i valori custom della simulazione */
	filename = malloc(sizeof(char)*50);
	sprintf(filename, "%sATT_%d-%d-%d-%d.dat", path, num, days, mem, strats);
	fp = fopen(filename, "w");

	if(fp==NULL){
		printf("%s: An error occurred while opening %s\n", argv[0], filename);
		prop |= ERR;
	}
	if(prop & ERR){
		printf(" -Press ENTER to start simulation\n");
		printf(" -Enter 'q' to esc\n");
		if((c = getchar()) == 'q')
			return 0;
	}
	
	printf("Init simulation ...");
	/* questa indicazione svuota il buffer dello stdout e quindi
	 * faciliterà la stampa immediata sulla stessa riga */
	fflush(stdout);

	/* genero random le affluenze delle settimane precedenti */
	srand(123456789);
	clock_t start = clock();
	init(agents, num, strats, mem);
	printf("\rDone               \n");

	if(!(prop & FST))
	{
		do{
			system("clear");
			printf("Press ENTER to next week.\tEnter 'q' to stop simulation.\n\n");
			printf("Week %d/%d\n", d+1, days);
			
			mkcho(agents, mem, strats);

			/* aggiorno tutti i dati per essere stampati */
			update(agents, days, mem);
			
			/* ~Interfaccia~ */
			if(prop & ATT)
				printf("A(%d): %d\n", d+1, A);
			if(prop & BIN)
				printf("Memory bin: %s\n", tobin(memory>>1, mem));
			
			summary(agents, mem);

			if(fp!=NULL)
				fprintf(fp, "%d\n", A);

			switch(c = getchar())
			{
				/* shortcuts per riavviare/terminare il programma */
				case 'q': case 'Q': d = days; break;
				case 'r': case 'R': return main(argc, argv);
				/* interruttori per statistiche di debug */
				case 'a': case 'A': prop ^= ATT; break;
				case 'b': case 'B': prop ^= BIN; break;
				default : break;
			}
		}
		while(++d<days);
		if(fp!=NULL){
			printf("%s generated\n", filename);
			fclose(fp);
		}
	}
	else
	{
		do{
			/* ho inserito un caricamento che progredisce man mano
			 * che passano i giorni della simulazione in modo da
			 * dare un segnale che sia tutto a nella norma al posto 
			 * del solito cursore lampeggiante */
			printf("\r%d days simulation %d%%", days, 1+(100*d/days));
			fflush(stdout);
			mkcho(agents, mem, strats);
			update(agents, days, mem);
			if(fp!=NULL)
				fprintf(fp, "%d\n", A);
		}
		while(++d<days);
		
		/* ⟨σ⟩ = deviazione std media 
		 *  α  = (2^M)/N */
		printf("\r⟨σ⟩ = ⟨A²/N⟩: %.10f\nα = %.10f\n\n", sigmax, pow(2, mem)/tot);
		if(fp!=NULL){
			printf("%s generated\n", filename);
			fclose(fp);
		}
	}

	delete(agents, all, strats);
	free(filename);
	free(attendance);
	attendance = NULL;

	clock_t end = clock();
	printf("Completed in %f s\n", (double)(end-start)/CLOCKS_PER_SEC);

	return 0;
}
