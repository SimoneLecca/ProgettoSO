#include "function.h"
#include "define.h"

/*void aggiornaVite(int vite){
	usleep(10);
	pthread_mutex_lock(&mutex_scrn);
	mvprintw(MAXY +4, 2, "Vite: %d    ", vite); 
	pthread_mutex_unlock(&mutex_scrn);
}
*/
/*thread che controlla un singolo missile*/
void* t_missile(void* arg){
	/*dichiaro e inizializzo il missile*/
	struct oggetto missile;
	int* param = arg;
	missile.id = param[0];
	missile.tipo=MISSILE;
	missile.sprite[0] = "|";
	missile.dim=1;
	missile.dimy=1;
	missile.x = param[1];
	missile.y = MAXY-MINY-2;
	missile.vite=1;

	while(missile.vite > 0 && !partitaFinita()){
		/*Controllo se il missile ha Colliso*/
		pthread_mutex_lock(&mutex_collision);
		int colliso= collision_m[missile.id][0];
		pthread_mutex_unlock(&mutex_collision);

		if(colliso==1 || partitaFinita()){
			pthread_mutex_lock(&mutex_collision);
			collision_m[missile.id][0]=0;
			pthread_mutex_unlock(&mutex_collision);
			missile.vite--;
		}
		else if(missile.vite>0) {missile.y--;} //Aggiorno posizione missile e lo passo ad aggiungi_job se e' ancora vivo
		
		aggiungi_job(missile);
		usleep(DELAY);
	}
	
	pthread_mutex_lock(&mutex_scrn); 
	mvprintw(MAXY +4, 2, "t_missili:ok"); 
	pthread_mutex_unlock(&mutex_scrn);

	//return NULL;
	
}
/*thread che si occupa del lacio dei missili una volta che il giocatore preme spazio*/
void* t_lanciatore_missili(void* arg){
	pthread_t missile1,missile2; // id thread dei missili
	bool flag=false; // flag generico
	int* id_missile = (int *) malloc(sizeof(int)*2);
	id_missile[0] = generatorId();
	int* id_missile2 = (int *) malloc(sizeof(int)*2);
	id_missile2[0] = generatorId();
	
	while(!partitaFinita()){
		pthread_mutex_lock(&mutex_fire);
		flag= (bool)fire[0]; // fire variabile globale che in posizione 0 contine un flag che avvisa quando il giocatore preme spazio
		id_missile[1]=fire[1];
		id_missile2[1]=fire[1]+5;
		pthread_mutex_unlock(&mutex_fire);
		// se il giocatore preme spazio allora faccio partire i thread dei missili
		if (flag){		
			pthread_create(&missile1, NULL, &t_missile, id_missile); 
			pthread_create(&missile2, NULL, &t_missile, id_missile2); 
		
			pthread_join (missile1, NULL);
			pthread_join (missile2, NULL);		
			pthread_mutex_lock(&mutex_fire);
			fire[0]= 0; // aggiorno il flag 
			pthread_mutex_unlock(&mutex_fire);	
		}
	}	
	free(id_missile);/// liberare memoria della malloc
	free(id_missile2);

	pthread_mutex_lock(&mutex_scrn); 
	mvprintw(MAXY +5, 2, "t_lanciatoreMissili:ok"); 
	pthread_mutex_unlock(&mutex_scrn);
	
	pthread_exit((void *) 0);
}

void* t_gStatus(void* arg){
	usleep(DELAY);
	pthread_mutex_lock(&mutex_player);
	struct oggetto giocatore= player;
	pthread_mutex_unlock(&mutex_player);
	aggiungi_job(giocatore); // invio nel bufffer la prima posizione
	
	while(giocatore.vite > 0 && !partitaFinita()) {
		

		/*Controllo se il missile ha Colliso*/
		pthread_mutex_lock(&mutex_collision);
		int colliso= collision_m[giocatore.id][0];
		int tipoColliso = collision_m[giocatore.id][1];
		pthread_mutex_unlock(&mutex_collision);

		if(colliso==1 && tipoColliso==BOMBA){
			
			giocatore.vite--; //flash(); beep();
			aggiornaPunteggio(-20);

			pthread_mutex_lock(&mutex_player);
			player.vite=giocatore.vite;
			pthread_mutex_unlock(&mutex_player);	
		}

		pthread_mutex_lock(&mutex_collision);
		collision_m[giocatore.id][0]=0;
		pthread_mutex_unlock(&mutex_collision);

		/*aggiorno la posizione del player*/
		pthread_mutex_lock(&mutex_player);
		giocatore= player;
		pthread_mutex_unlock(&mutex_player);
		aggiungi_job(giocatore);
		usleep(DELAY);

		
	}
	
	//Se il giocatore esce dal ciclo e non ha piu vite vuol dire che ha perso
	if(giocatore.vite <= 0) {
		pthread_mutex_lock(&mutex_scrn); 
		mvprintw(MAXY +6, 2, "t_GsTATUS:ok"); 
		pthread_mutex_unlock(&mutex_scrn);

		pthread_mutex_lock(&mutex_end);
		end_player = true;
		pthread_mutex_unlock(&mutex_end);
	}

	aggiungi_job(giocatore);

	pthread_mutex_lock(&mutex_scrn); 
	mvprintw(MAXY +6, 2, "t_GsTATUS:ok"); 
	pthread_mutex_unlock(&mutex_scrn);
	pthread_exit((void *) 0);
}

/*si occupa di aggiornare la posizione della navicella del giocatore alla pressione dei tasti direzionali, aggiorna il fire "spara i missili" quando il giocatore preme spazio*/
void* t_giocatore(void* arg)
{
	usleep(DELAY);
	pthread_mutex_lock(&mutex_player);
	struct oggetto giocatore= player;
	pthread_mutex_unlock(&mutex_player);

	char c; // contiene il caratte premuto da tastiera
	
	//finche la partita non finisce continua a prendere gli input dell'utente
	//usleep(DELAY*100);
	while(!partitaFinita()) {
		switch(c=getch()) {
			// se preme destra o sinistra aggiono la posizione della navicella
			case SINISTRA:
				if(giocatore.x>MINX)
					giocatore.x-=1;
				break;
			
			case DESTRA:
				if(giocatore.x< (MAXX-MINX-giocatore.dim)) 
					giocatore.x+=1;
				break;
			// se preme spazio aggiorno la variabile globale fire: fire[1] posizione dalla quale parte il missile, 
			//fire[0] flag letto dal thread lancio_missile che quando è true fa il lancio effetivo
			case SPACE:
				pthread_mutex_lock(&mutex_fire);
				if(fire[0]==0){
					fire[0]= 1;// bool
					fire[1]= giocatore.x;
				}
				pthread_mutex_unlock(&mutex_fire);
				break;
		}
		/*aggiorno la posizione del player*/
		pthread_mutex_lock(&mutex_player);
		player.x=giocatore.x;
		player.y=giocatore.y;
		pthread_mutex_unlock(&mutex_player);
		
	}
	pthread_mutex_lock(&mutex_scrn); 
	mvprintw(MAXY +7, 2, "t_giocatore:ok"); 
	pthread_mutex_unlock(&mutex_scrn);
}


