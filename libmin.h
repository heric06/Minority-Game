#ifndef MINORITY_C_H
#define MINORITY_C_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define GOTO 	1
#define STAY	-1

#define STRATLEN(bitsize)	((int)pow(2,bitsize))

extern unsigned long long memory;
extern unsigned int tot;
extern double sigma ;
extern double sigmax;
extern int *attendance;
extern int A;

typedef unsigned int uint;
typedef struct agent
{
	uint **strat;		/* strategie della forma S(i)={1, 1, 0, 1, ...,[2^M]} */
	uint bests;   		/* indice di posizione della strategia con punteggio migliore */	
	int *score; 		/* punteggio di ogni propria strategia */
	int decision;		/* le azioni sono 1 e -1 */
	struct agent* prev;	/* puntatori al successivo e precedente di un membro della lista, */
	struct agent* next;	/* usati per retrocedere dalla fine per liberare la memoria       */
}
llist_t;

/* aggiunge N nuove structs in coda alla lista */
void queue(llist_t* head, uint N);
/* cancella e libera la memoria per N structs dalla fine */
void delete(llist_t* head, uint N, uint S);
/* inizializza la sim.ne per N agenti, S strategie e M memoria */
void init(llist_t* head, uint N, uint S, uint M);
/* processore dei dati: gli agenti agiscono secondo memoria */
void mkcho(llist_t* head, uint M, uint S);
/* aggiorna un po' tutto */
void update(llist_t* head, uint D, uint M);
/* interfaccia utente: mostra i resultati */
void summary(llist_t* head, uint M);
/* ritorna una stringa del formato binario di una variabile */
char* tobin(uint what_to, uint bitof);
/* media delle affluenze delle ultime giornate in memoria */
float average(uint M);
/* ritorna il numero di agenti che hanno scelto di andare */
uint going(llist_t* head);

#endif /* MINORITY_C_H */
