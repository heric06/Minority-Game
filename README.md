# Minority Game

Introduzione
------------

Il Minority Game è un gioco proposto da Yi-Cheng Zhang e Damien Challet, ispirato dall' "El Farol Bar problem". Il cavillo nasce dal fatto che la capienza del bar non prevede l'affluenza di tutte le persone che vogliono frequentarlo, lasciando di fatto il pubblico insoddisfatto sia in caso di sorvannumero, sia in caso di partecipazione esigua.

L'obiettivo della simulazione proprosta sarà proprio quello di trovare il modo di rendere `N` agenti capaci di decidere se andare o meno al bar avendo a disposizione solo una memoria della partecipazione `A` di un certo numero `M` di giorni precedenti.

Ho provvisto ogni agente di `S` strategie con relativo punteggio, aggiornato per ognuno dei giorni della simulazione. Ad ogni turno `(t)` vincono coloro che fanno una scelta in minor numero (da qui il nome), ragion per cui si preferisce utilizzare `N` dispari.

Ai fini dei calcoli negli algoritmi, le azioni saranno definite come **1** "vado" e **-1** "non vado", mentre le strategie saranno una serie di stringhe in forma binaria. Tuttavia nell'interfaccia i risultati verranno mostrati in tempo reale con numeri più intuitivi.

Compilazione
------------

	./configure
	
	make

Esecuzione
----------

Il programma accetta fino a 5 parametri in ingresso, di cui:
	
- N = numero della popolazione
	
- D = numero dei giorni della simulazione
	
- M = numero dei giorni in memoria
	
- S = numero delle strategie proprie ad ogni agente
	
Il quinto è lasciato per simulazioni più spinte: `--fast` , la cui direttiva è quella di far stampare a schermo solo il valore della deviazione standard media di tutti i giorni simulati, il valore **α** = (2^M)/N, e poi dei valori di `A(t)` su file. 
	
Un esempio di esecuzione è quindi del tipo: `./minority -n 5093 -d 700 -m 4 -s 10 --fast` .

Il comando `./minority` procederà invece ad avviare una simulazione predefinita con parametri

- N = 2001
	
- D = 250
	
- M = 3
	
- S = 7

Interfaccia
-----------

L' "interfaccia" è semplicemente il modo in cui ho disposto la stampa dei risultati: al contrario della direttiva `--fast` , a schermo ci sarà ogni giorno della simulazione aggiornato  le affluenze e la media di tutti gli `M` giorni precedenti. In aggiunta saranno anche aggiornati e mostrati deviazione standard di `A(t)` e deviazione standard media.
A questo punto i comandi sono:
	
- `Invio`: procede al giorno successivo
	
- `Q + Invio`: termina la simulazione
	
- `R + Invio`: riavvia la simulazione
	
*comandi di debug*
	
- `A + Invio`: viene mostrata `A(t)` attuale
	
- `B + Invio`: viene mostrata la memoria in formato binario

Happy ending
------------

I risultati ottenuti verranno salvati infine nella directory 'results/', in cui ne ho proposti 2 di partenza: uno con i parametri predefiniti e l'altro con i parametri massimi (imposti nel #define del main).
