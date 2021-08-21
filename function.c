#include "function.h"
#include "define.h"

bool partitaFinita(){
	pthread_mutex_lock(&mutex_end);
	bool risultato = (end_player || end_enemis || end_layer);
	pthread_mutex_unlock(&mutex_end);
	return risultato;
	
}

void aggiornaId(int punt){
	pthread_mutex_lock(&mutex_scrn);
	mvprintw(MAXY +2, 2, "numero Id: %d", punt); 
	usleep(100);
	mvprintw(MAXY +3, 2, "p: %d, e: %d, l: %d", end_player, end_enemis, end_layer); 
	pthread_mutex_unlock(&mutex_scrn);
}

//funzione per generare gli id degli oggetti in gioco
int generatorId(){
	int returnId=0;
	pthread_mutex_lock(&mutex_id);
	if(newId <= OBJ_ON_SCREEN){
		newId++;
		returnId=newId;
		aggiornaId(returnId);
	}
	
	pthread_mutex_unlock(&mutex_id);
	
	return returnId;
}



/*aggiunge un nuova posizine nel buffer*/
void aggiungi_job (struct oggetto new_obj){
	pthread_mutex_lock(&mutex_buffer); /* Blocco mutex */
	
	if(counter < DIM_BUFFER){
        buffer[counter] = new_obj;
		counter++;
		sem_post(&buffer_count); 
	}
	
	pthread_mutex_unlock(&mutex_buffer); /* Sblocco mutex */
}

/*elimina un oggetto dal buffer*/
struct oggetto elimina(){
	struct oggetto obj_rm; // contiene l'oggetto eliminato dalla 
	struct oggetto obj_null = {0,0,NULL,0,0,0,0};
	
	sem_wait (&buffer_count);  /* Nessun job: attendo */
	pthread_mutex_lock(&mutex_buffer);  /* Blocco mutex */
	
	obj_rm = buffer[counter-1];
	buffer[counter-1] = obj_null;
	
	counter--; /* Aggiorno contatore */

	pthread_mutex_unlock(&mutex_buffer); /* Sblocco mutex */ 
	return obj_rm; 
}

/*inizio: elimino dallo schermo l'oggetto nella vecchia posizione, aggiorno i dati con la nuova posizione e stampo il nuovo oggetto nella nuova posizione */
/*AGGIORNO POSIZIONE OGGETTO*/
void stampaOggetto(struct oggetto new_obj){
		pthread_mutex_lock(&mutex_scrn); // blocco mutex per lo schermo		
		mvprintw(new_obj.y,new_obj.x,new_obj.sprite[0]);
		if(new_obj.dimy==2){//dimy
			mvprintw(new_obj.y+1,new_obj.x,new_obj.sprite[1]);
		}
		curs_set(0);
		refresh();
		pthread_mutex_unlock(&mutex_scrn);// sblocco mutex  per lo schermo
}
void cancellaOggetto(struct oggetto old_obj, int dim){												// contatore			
		pthread_mutex_lock(&mutex_scrn); // blocco mutex per lo schermo		
		for(int i=0; i < dim;i++){
			mvprintw(old_obj.y,old_obj.x+i," ");	
			if(old_obj.dimy==2){//dimy
				mvprintw(old_obj.y+1,old_obj.x+i," ");
			}	
			curs_set(0);
			refresh();
		}
		pthread_mutex_unlock(&mutex_scrn);	
}

int isColliding(struct oggetto obj, struct oggetto all_obj[]){
	/*PER TUTTI GLI OGGETTI A SCHERMO*/
	for(int i =0; i<OBJ_ON_SCREEN; i++){
		/*CHE NON SIA L'OGGETTO ESAMINATO ORA "ASTRONAVE1"*/
		if(obj.id != all_obj[i].id && all_obj[i].vite >0){
			/*CONTROLLO SE C'E UNA COLLISIONE*/
			/*controllo se il punto iniziale e compreso tra x e x+dim*/
			/*controllo se il punto finale e compreso tra x e x+dim*/										
			if( ((all_obj[i].x >= obj.x && all_obj[i].x <= obj.x + (obj.dim -1)) 
			|| (all_obj[i].x+(all_obj[i].dim -1) >= obj.x && all_obj[i].x+(all_obj[i].dim-1)<= obj.x + (obj.dim -1)))){//&& obj.y == all_obj[i].y
				/*controlla dimy*/
				if( ((all_obj[i].y >= obj.y && all_obj[i].y <= obj.y + (obj.dimy-1)) 
				|| (all_obj[i].y+(all_obj[i].dimy-1) >= obj.y && all_obj[i].y+ (all_obj[i].dimy -1) <= obj.y + (obj.dimy -1)))){
					/**COLLISIONE RILEVATA*/
					pthread_mutex_lock(&mutex_collision);
					collision_m [obj.id][2] =all_obj[i].x;	//x per capire che parte della seconda navicella 'e stata colpita
					collision_m [obj.id][3] =all_obj[i].y;	//y
					pthread_mutex_unlock(&mutex_collision);
					return all_obj[i].tipo;
					
				}
				
				
			}
		}
	}

	/*BORDO Y*/	
	if(obj.y <= MINY ) return BORDO;
	/*BORDO X*/
	if(obj.y >= MAXY -2) return BORDO;

	return -1;	
	
}

void aggiornaPunteggio(int punt){
	pthread_mutex_lock(&mutex_punteggio);
	punteggio +=punt;
	if(punteggio < 0) punteggio =0;
	pthread_mutex_unlock(&mutex_punteggio);	
	pthread_mutex_lock(&mutex_scrn);
	mvprintw(MAXY +1, 2, "Punteggio: %d    ", punteggio); 
	pthread_mutex_unlock(&mutex_scrn);
}

/*si occupa di: leggere dal buffer, stampare a video gli oggetti in gioco, controllare le collisioni */
void controllore(){
	int numero;
	int colliso;
	 
	struct oggetto new_obj; 		// contiene l'oggetto che viene tolto dal buffer
	struct oggetto old_obj; 		// contiene la precedente posizione dell'oggetto che viene tolto dal buffer
	struct oggetto all_obj[OBJ_ON_SCREEN];	// contiene tutti gli oggetti attivi nello schermo

	//Finche non si verifica una condizione di fine parta continua
	while(!(end_player || end_enemis || end_layer)){
		new_obj = (struct oggetto)elimina(); // prendo un oggetto dal buffer
		old_obj = all_obj[new_obj.id];
		all_obj[new_obj.id]=new_obj;


		
		cancellaOggetto(old_obj, new_obj.dim);

		/*fine---------------------------------------------------------------------------------------------------------------------------------------------------*/

		/*CONTROLLO COLLISIONI (se avenute): solo se vite > 0*/
		if(new_obj.vite > 0){
			stampaOggetto(new_obj);
			/*Trovo id oggetto colliso: -1 = nessuno*/			
			colliso = isColliding(new_obj, all_obj);
			if(colliso != -1){ 
				pthread_mutex_lock(&mutex_collision);
				collision_m [new_obj.id][0]=1;
				collision_m [new_obj.id][1]=colliso;	
				pthread_mutex_unlock(&mutex_collision);			
			}
		}
		/*Se le vite sono 0 cancello l'oggetto dallo schermo*/
		
		
	}
}

void end_game(){
	int x=0,y=0; // x y ci aiuteranno a stampare a video nelle cordinate giuste
	int i=0; // contatore
	int n_mostri=5; // numero di sprite dentro la prima schermata
	int dir=1;
	char c;

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

	clear();
	print_box(); // richiamo la funzione di stampa bordo

	pthread_mutex_lock(&mutex_scrn);
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
	
	if(end_player || end_layer){
		/*altra stampa nella home*/
		mvprintw(19,50,"    Hai perso! Punteggio: %d", punteggio);
		mvprintw(20,50,"  ____                         ___                 ");
		mvprintw(21,50," / ___| __ _ _ __ ___   ___   / _ \\__   _____ _ __ ");
		mvprintw(22,50,"| |  _ / _` | '_ ` _ \\ / _ \\ | | | \\ \\ / / _ \\ '__|");
		mvprintw(23,50,"| |_| | (_| | | | | | |  __/ | |_| |\\ V /  __/ |   ");
		mvprintw(24,50," \\____|\\__,_|_| |_| |_|\\___|  \\___/  \\_/ \\___|_|   ");
		mvprintw(25,50,"        Press 'SPACE' for close the game                ");
	}
	else{
		/*altra stampa nella home*/
		mvprintw(19,50,"     Hai vinto! Punteggio: %d", punteggio);
		mvprintw(20,50,"__   __           __        ___       ");
		mvprintw(21,50,"\\ \\ / /__  _   _  \\ \\      / (_)_ __  ");
		mvprintw(22,50," \\ V / _ \\| | | |  \\ \\ /\\ / /| | '_ \\ ");
		mvprintw(23,50,"  | | (_) | |_| |   \\ V  V / | | | | |");
		mvprintw(24,50,"  |_|\\___/ \\__,_|    \\_/\\_/  |_|_| |_|");
		mvprintw(25,50,"        Press 'SPACE' for close the game                ");
	}

	curs_set(0);
	refresh();
	pthread_mutex_unlock(&mutex_scrn);
	// appena il giocatore preme un tasto finisce il gioco
        c = getch();     
	while(SPACE != getch());

   	
   	clear();
	curs_set(0);
	refresh();
   	print_box();
}

