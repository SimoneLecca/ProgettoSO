#include "function.h"
#include "define.h"

int main(){
	
	start();
	/*dichiarazione variabili*/
	pthread_t giocatore,lanciatore_missili, generatore_astronavi; // id dei thread che verranno lanciati
	int i=0; // contatore
	
	/*inizializzo schermo */
	initscr();
	noecho();
	curs_set(false);
	/*inizializzazione variabili, anche quelle pubbliche */
	for(i=0;i<OBJ_ON_SCREEN;i++){
		collision_m [i][0]=0;
		collision_m [i][1]=0;
	}
	
	print_home(); // stampa a video la home
	inizializza_coda(); // inizializza il semaforo
	pthread_create(&giocatore, NULL, &t_giocatore, NULL); // lancia il thread che controlla la navicella del giocatore
	pthread_create(&lanciatore_missili, NULL, &t_lanciatore_missili, NULL); // lancia il thread che controlla i missili che lancia la navicella del giocatore
	if(pthread_create(&generatore_astronavi, NULL, t_generatore_astronavi, NULL)){ endwin(); exit;} // lancia il thread che gestisce le navicelle nemiche

	controllore(); // si occupa di: leggere dal buffer, stampare a video gli oggetti in gioco, controllare le collisioni

	end_game(); // stampa la fine della partita

	endwin();
	return 0;
}
