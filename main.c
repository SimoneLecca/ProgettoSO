	#include "function.h"
#include "define.h"

int main(){
	
	start();
	/*dichiarazione variabili*/
	pthread_t giocatore, gStatus, lanciatore_missili, generatore_astronavi; // id dei thread che verranno lanciati
	
	/*inizializzo schermo */
	initscr();
	noecho();
	curs_set(false);
	
	
	print_home(); // stampa a video la home
	inizializza_coda(); // inizializza il semaforo
	pthread_create(&giocatore, NULL, &t_giocatore, NULL); // lancia il thread che controlla la navicella del giocatore
	pthread_create(&gStatus, NULL, &t_gStatus, NULL); // lancia il thread che controlla la navicella del giocatore
	pthread_create(&lanciatore_missili, NULL, &t_lanciatore_missili, NULL); // lancia il thread che controlla i missili che lancia la navicella del giocatore
	if(pthread_create(&generatore_astronavi, NULL, t_generatore_astronavi, NULL)){ endwin(); exit;} // lancia il thread che gestisce le navicelle nemiche

	controllore(); // si occupa di: leggere dal buffer, stampare a video gli oggetti in gioco, controllare le collisioni

	//flash();beep();
	//usleep(DELAY*100);
	
	pthread_join (gStatus, NULL);	//quando vince il giocatore
	pthread_join (lanciatore_missili, NULL); //quando vince il giocatore
	pthread_join (generatore_astronavi, NULL); //quando vince il nemico
	end_game(); // stampa la fine della partita
	pthread_join (giocatore, NULL);	
	
		
	
	

	//TERMINA I THREAD QUIIII
	
	
	endwin();
	return 0;
}
