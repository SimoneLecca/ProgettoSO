#include "function.h"
#include "define.h"

/*Inizializza le variabili globali*/
void start(){
	pthread_mutex_t mutex_scrn = PTHREAD_MUTEX_INITIALIZER; // mutex per accesso alla risorsa condivisa screen
	pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER; // mutex per l'accesso al buffer
	pthread_mutex_t mutex_collision = PTHREAD_MUTEX_INITIALIZER; // mutex per l'accesso alla matrice per la gestione delle collisioni
	pthread_mutex_t mutex_fire = PTHREAD_MUTEX_INITIALIZER; // mutex per l'accesso alla variabile utilizzata per quando si sprara
	pthread_mutex_t mutex_astronave = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_id = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_punteggio = PTHREAD_MUTEX_INITIALIZER;

	punteggio = 0;

	int counter=0; // contatore per il buffer

	int fire[2]= {0,0};// casin0000
	
	
	end_player= false;	//astronave del giocatore distrutta
	end_enemis= false;	//le astronavi nemiche sono tutte distrutte
	end_layer= false;	//le astronavi nemiche sono arrivate all'ultimo layer e vincono	
	
	newId=0; //varibile per generare gli id degli oggetti in gioco
}

/*inizializza il semaforo*/
void inizializza_coda(){
	counter = 0;
	sem_init(&buffer_count, 0, 0); /* Inizio da zero (nessun job) */
	
}

/*esegue la grafica della schermata iniziale e del countdown */
void print_home(){

	int x=0,y=0; // x y ci aiuteranno a stampare a video nelle cordinate giuste
	int i=0; // contatore
	int n_mostri=5; // numero di sprite dentro la prima schermata
	int dir=1;

	/*inizializzo i colori*/
	start_color();
	init_color(COLOR_MAGENTA, 700,300,0); // il magenta lo facccio diventare arancione
	init_pair(1,COLOR_RED,COLOR_RED);  // al numero uno abbiamo rosso su sfondo rosso
	init_pair(2,COLOR_MAGENTA,COLOR_MAGENTA); 
	init_pair(3,COLOR_YELLOW,COLOR_YELLOW);
	init_pair(4,COLOR_GREEN,COLOR_GREEN);
	init_pair(5,COLOR_BLUE,COLOR_BLUE);
	init_pair(6,COLOR_WHITE,COLOR_BLACK);
	
	x=27; // punto di inizio in ascissa degli sprite
	y=5; // punto di inizio in cordinata degli sprite
	print_box(); // richiamo la funzione di stampa bordo
	
	/*ciclo utilizzato per la stampa degli sprite*/
	for(i=0;i<n_mostri+1;i++){
		attron(COLOR_PAIR(i)); //funzioneche cambia il colore della prossima scritta
		
		mvprintw(y,x+2," "); 
		mvprintw(y,x+3," "); 
		mvprintw(y,x+4," "); 

		mvprintw(y+1,x+1," "); 
		mvprintw(y+1,x+2," "); 
		mvprintw(y+1,x+3," "); 
		mvprintw(y+1,x+4," "); 
		mvprintw(y+1,x+5," "); 

		mvprintw(y+2,x+0," "); 
		mvprintw(y+2,x+1," "); 
		mvprintw(y+2,x+3," "); 
		mvprintw(y+2,x+5," "); 
		mvprintw(y+2,x+6," "); 

		mvprintw(y+3,x+0," "); 
		mvprintw(y+3,x+1," "); 
		mvprintw(y+3,x+2," "); 
		mvprintw(y+3,x+3," "); 
		mvprintw(y+3,x+4," "); 
		mvprintw(y+3,x+5," "); 
		mvprintw(y+3,x+6," "); 


		mvprintw(y+4,x+2," "); 
		mvprintw(y+4,x+4," "); 

		mvprintw(y+5,x+1," "); 
		mvprintw(y+5,x+3," "); 
		mvprintw(y+5,x+5," "); 

		mvprintw(y+6,x+0," "); 
		mvprintw(y+6,x+6," "); 

		attroff(i);
		x+=16;
	}
	
	attron(COLOR_PAIR(6));
	
	/*altra stampa nella home*/
	mvprintw(20,50," ____                      ____  _             _   ");
	mvprintw(21,50,"|  _ \\ _ __ ___  ___ ___  / ___|| |_ __ _ _ __| |_ ");
	mvprintw(22,50,"| |_) | '__/ _ \\/ __/ __| \\___ \\| __/ _` | '__| __|");
	mvprintw(23,50,"|  __/| | |  __/\\__ \\__ \\  ___) | || (_| | |  | |_ ");
	mvprintw(24,50,"|_|   |_|  \\___||___/___/ |____/ \\__\\__,_|_|   \\__|");
	
	curs_set(0);
	refresh();
	
	getch(); // appena il giocatore preme un tasto inizia il gioco
	
	/* countdown prima dell'inizio gioco */
   	clear();
	print_box();
	mvprintw(17,70," _____ ");
	mvprintw(18,70,"|___ / ");
	mvprintw(19,70,"  |_ \\ ");
	mvprintw(20,70," ___) |");
	mvprintw(21,70,"|____/ ");
   	refresh();
   	sleep(COUNTDOWN_DELAY); 
   	
   	clear();
	print_box();
	mvprintw(17,70," ____  ");
	mvprintw(18,70,"|___ \\ ");
	mvprintw(19,70,"  __) |");
	mvprintw(20,70," / __/ ");
	mvprintw(21,70,"|_____|");
   	refresh();
   	sleep(COUNTDOWN_DELAY);
   	
	clear();
	print_box();
	mvprintw(17,73," _ ");
	mvprintw(18,73,"/ |");
	mvprintw(19,73,"| |");
	mvprintw(20,73,"| |");
	mvprintw(21,73,"|_|");
   	refresh();
   	sleep(COUNTDOWN_DELAY);
	
	clear();
	print_box();
	mvprintw(17,60,"     _             _     _   _ ");
	mvprintw(18,60," ___| |_ __ _ _ __| |_  | | | |");
	mvprintw(19,60,"/ __| __/ _` | '__| __| | | | |");
	mvprintw(20,60,"\\__ \\ || (_| | |  | |_  |_| |_|");
	mvprintw(21,60,"|___/\\__\\__,_|_|   \\__| (_) (_)");
   	refresh();
   	sleep(COUNTDOWN_DELAY); 
   	
   	clear();
	curs_set(0);
	refresh();
   	print_box();
}

// stampa il bordo
void print_box(){
	mvhline(MAXY-1, 1, ACS_HLINE, MAXX-1); // linea di sotto
	mvhline(1, 1, ACS_HLINE, MAXX-1); // linea di sopra
	
	mvvline(1, MAXX-1, ACS_VLINE, MAXY-1); // linea di destra
	mvvline(1, 1, ACS_VLINE, MAXY-1); //linea di sinistra
	
	mvhline(1, 1, ACS_ULCORNER, 1); // angolo TL (top left)
	mvhline(1, MAXX-1, ACS_URCORNER, 1); //angolo TR 
	
	mvvline(MAXY-1,1, ACS_LLCORNER, 1); // angolo BL
	mvvline(MAXY-1, MAXX-1, ACS_LRCORNER, 1); // BR

	aggiornaPunteggio(0);
   	refresh();
}
