#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "libmin.h"

/* uso variabili globali per motivi di efficienza */
uint tot = 0;
unsigned long long memory = 0;
double sigma  = 0;
double sigmax = 0;
int *attendance;
int A;

void init(llist_t* head, uint N, uint S, uint M)
{
	uint i, j;
	/* utilizzo un puntatore a struct per iterare 
	 * all'interno della lista di agenti */
	llist_t *it = NULL;

	/* allocazione e collegamento dei restanti agenti */
	queue(head, N-1);
	tot = N;
	attendance = malloc((M+1)*sizeof(int));

	for(i=0; i<M; ++i){
		attendance[i] = rand()%(N+1);
	}

	/* inizializzo la memoria di partenza in base alle
	 * affluenze generate randomicamente */
	for(i=0; i<M; ++i){
		memory <<= 1;
		memory |= (attendance[M-1-i] < N/2? 0<<0 : 1<<0);
	}

	/* converto l'affluenza in A(t) */
	A = -(tot-attendance[0]) + attendance[0];

	for(it=head; it!=NULL; it=it->next)
	{
		/* inizializzazione degli agenti */
		it->strat = malloc(S*sizeof(int*));
		it->score = malloc(S*sizeof(int));
		it->bests = 0;
		
		/* inizializzazione delle strategie di lunghezza
		 * 2^M che gestirà ogni possibile combinazione binaria
		 * della memoria globale */
		for(i=0; i<S; ++i){
			it->score[i] = 0;
			it->strat[i] = malloc(STRATLEN(M)*sizeof(int));
			for(j=0; j<STRATLEN(M); ++j){
				it->strat[i][j] = rand()%2;
			}
		}
	}
}

void mkcho(llist_t* head, uint M, uint S)
{
	uint i;
	uint membin = 0;
	llist_t *it = NULL;
	int max;

	/* l'istruzione si chiede se nella variabile memory
	 * ci sia un 1 nella posizione i-esima da destra
	 * e l'espressione assegna 1 nella posizione i-esima
	 * da destra nella variabile membin. Così facendo di fatto
	 * converto la memoria globale nei suoi primi M bit */
	for(i=0; i<M; ++i){
		membin |= memory & 1<<i;
	}

	/* aggiorno la srategia migliore per ogni agente cercando
	 * la posizione di quella con il punteggio più alto */
	for(it=head; it!=NULL; it=it->next)
	{
		max = it->score[it->bests];
		for(i=0; i<S; ++i){
			if(max < it->score[i]){
				max = it->score[i];
				it->bests = i;
			}
		}
		/* la decisione è presa all'interno della best strategy 
		 * considerando l'elemeno membin-esimo, di cui ora sono sicuro 
		 * rientri nel range, avendone ristretto il valore nella sola
		 * area di interesse da 0 a 2^M-1 
		 * se nella strategia l'elemento previsione [membin] è true==1,
		 * allora l'azione è "non andare"==-1, se nella strategia l'elemento
		 * previsione [membin] è false==0, allora l'azione è "andare"==1 */
		it->decision = (it->strat[it->bests][membin]? STAY : GOTO);
	}
}

void update(llist_t* head, uint D, uint M)
{
	uint i;
	llist_t *it = NULL;
	A = 0;
	
	/* aggiorno la memoria con shift di un posto e l'affluenza A(t) 
	 * come sommatoria delle scelte degli agenti presi in esame */
	for(it=head; it!=NULL; it=it->next){
		A += it->decision;
	}
	memory <<= 1;
	memory |= (A < 0? 0<<0 : 1<<0);
	
	/* aggiorno l'affluenza per l'interfaccia */
	for(i=M; i>0; --i){
		attendance[i] = attendance[i-1];
	}
	attendance[0] = going(head);

	/* aggiorno i punteggi delle strategie usate */
	for(it=head; it!=NULL; it=it->next){
		it->score[it->bests] += -(it->decision*A);
	}

	/* aggiorno i parametri delle deviazioni standard:
	 * sigma  = dev. std della A(t) corrente,
	 * sigmax = dev. std media di tutte le A(t) 
	 * il cui valore sarà incrementato in tempo reale, ma
	 * definitivo solo a fine simulazione */
	sigma = (double)(A*A)/tot;
	sigmax += sigma/D;
}

float average(uint M)
{
	uint i;
	float average = 0;
	for(i=0; i<M; ++i){
		average += attendance[i+1];
	}
	return average/M;
}

void queue(llist_t* head, uint N)
{
	llist_t *pt = head;
	uint i;
	for(pt=head; pt->next!=NULL; pt=pt->next){/* itera fino alla fine */}
	
	/* metto in coda tutti gli agenti */
	for(i=0; i<N; ++i){
		pt->next = malloc(sizeof(llist_t));
		pt->next->prev = pt;
		pt = pt->next;
		pt->next = NULL;
	}
}

void delete(llist_t* head, uint N, uint S)
{
	llist_t *pt = NULL;
	uint i, j;
	for(pt=head; pt->next!=NULL; pt=pt->next){/* itera fino alla fine */}
	
	/* eseguo il free() della memoria di tutti i puntatori 
	 * allocati dinamicamente e infine li punto a NULL */
	for(i=0; i<N; ++i)
	{
		/* caso del primo nodo, head */
		if(pt->prev==NULL)
		{
			free(pt->score);
			pt->score = NULL;

			for(j=0; j<S; ++j){
				free(pt->strat[j]);
				pt->strat[j] = NULL;
			}

			free(pt->strat);
			pt->strat = NULL;
			free(pt);
			pt = NULL;
		}
		else
		{
			pt = pt->prev;
			free(pt->next->score);
			pt->next->score = NULL;

			for(j=0; j<S; ++j){
				free(pt->next->strat[j]);
				pt->next->strat[j] = NULL;
			}
			
			free(pt->next->strat);
			pt->next->strat = NULL;

			free(pt->next);
			pt->next = NULL;
		}
	}
}

void summary(llist_t* head, uint M)
{
	uint i;

	printf(
"\
Current weekend details:\t\
σ = A²/N: %.6f\t⟨σ⟩ = ⟨A²/N⟩: %.6f\n\
Total going:\t%d/%d\n\n\
", sigma, sigmax, going(head), tot);

	printf(
"\
Last %d weeks:\t\t\t\
Average:  %.3f\n\
", M, average(M));

	for(i=0; i<M; ++i){
		printf("-%d Attn.: %d\n", i+1, attendance[i+1]);
	}
}

char* tobin(uint what_to, uint bitof)
{
	uint i;
	char* bin;
	bin = malloc(bitof*sizeof(char));
	for(i=0; i<bitof; ++i){
		bin[bitof-i-1] = (what_to & 1<<i ? '1' : '0');
	}
	return bin;
}

uint going(llist_t* head)
{
	llist_t *it = NULL;
	uint cnt = 0;
	
	for(it=head; it!=NULL; it=it->next){
		/* per ogni agente, il contatore aumenta
		 * solo in caso la decisione sia GOTO == 1 */
		cnt += (it->decision == GOTO ? 1 : 0);
	}

	return cnt;
}

