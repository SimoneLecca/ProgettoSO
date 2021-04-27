#include "function.h"
#include "define.h"


/*thread che controlla un singolo missile*/
void* t_missile(void* arg){
	/*dichiaro e inizializzo il missile*/
	struct oggetto missile;
	missile.id = *(int *)arg;
	missile.sprite = "|";
	missile.dim=1;
	pthread_mutex_lock(&mutex_fire); // blocco mutex per la variabile globale
	if (missile.id == MISSILE2)
		missile.x=fire[1];
	else
		missile.x = fire[1]+5; // in posizione 1 la variabile globale contiene la cordinata di dove è stato lanciato il missile 
	pthread_mutex_unlock(&mutex_fire);// sblocco mutex per la variabile globale
	
	missile.y = MAXY-MINY-1;
	missile.vite=1;
	missile.id_thread = pthread_self();

	while(missile.vite > 0){
		/*Controllo se il missile ha Colliso*/
		pthread_mutex_lock(&mutex_collision);
		if(collision_m[missile.id][0]==1){
			collision_m[missile.id][0]=0;
			missile.vite--;
			aggiungi_job(missile);
		}
		pthread_mutex_unlock(&mutex_collision);

		/*Aggiorno posizione missile e lo passo ad aggiungi_job se e' ancora vivo*/
		if(missile.vite>0){ 
			missile.y--;
			aggiungi_job(missile);
		}
		usleep(DELAY);
	}

	return NULL;
	
}
/*thread che si occupa del lacio dei missili una volta che il giocatore preme spazio*/
void* t_lanciatore_missili(void* arg){
	pthread_t missile1,missile2; // id thread dei missili
	bool flag=false; // flag generico
	int* id_missile = (int *) malloc(sizeof(int));
	*id_missile = MISSILE1;
	int* id_missile2 = (int *) malloc(sizeof(int));
	*id_missile2 = MISSILE2;
	
	
	while(1){
		pthread_mutex_lock(&mutex_fire);
			flag= (bool)fire[0]; // fire variabile globale che in posizione 0 contine un flag che avvisa quando il giocatore preme spazio
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
}

/*si occupa di aggiornare la posizione della navicella del giocatore alla pressione dei tasti direzionali, aggiorna il fire "spara i missili" quando il giocatore preme spazio*/
void* t_giocatore(void* arg)
{
	struct oggetto giocatore; // contiene i dati del giocatore
	bool flag;
	char c; // contiene il caratte premuto da tastiera
	// inizializzo il giocatore
	giocatore.id = GAMER;
	giocatore.sprite = "<|--|>";
	giocatore.dim=6;
	giocatore.x = MAXX/2-giocatore.dim/2;
	giocatore.y = MAXY-MINY;
	giocatore.vite=3;
	giocatore.id_thread = pthread_self();
	
	
	flag=false; 	
	aggiungi_job(giocatore); // invio nel bufffer la prima posizione
	
	while(giocatore.vite > 0) {

		/*Controllo se il missile ha Colliso*/
		pthread_mutex_lock(&mutex_collision);
		if(collision_m[giocatore.id][0]==1 && collision_m[giocatore.id][1]==BOMBA){
			collision_m[giocatore.id][0]=0;
			giocatore.vite--; flash(); beep();
		}
		pthread_mutex_unlock(&mutex_collision);

		flag = false;
		// controllo quale tastio viene premuto
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
		
		default:
			flag=true;
			break;
		}
		// solo se utente ha premuto i tasti direzionali allora invio nel buffer la nuova posizione
		if (!flag)
			aggiungi_job(giocatore);

		
	}
}


