#include "function.h"	
#include "define.h"

/*
----------------------------------------------------------------------------------------
 Thread bomba - simile cannoneLaser
----------------------------------------------------------------------------------------
*/ 

void* t_bomba(void* arg){
	//int* parBomba= (int *) malloc(sizeof(int)*4);
	int* parBomba= (int *)arg;
	struct oggetto bomba;
	bomba.id = parBomba[2];
	bomba.sprite = "*";
	bomba.dim=1;
	bomba.vite=1;
	bomba.tipo=BOMBA;
	bomba.x = parBomba[0] + (int)(parBomba[3]/2);
	bomba.y = parBomba[1] +1;


	while(bomba.vite > 0){
		/*Controllo se la bomba ha Colliso*/
		pthread_mutex_lock(&mutex_collision);
		if(collision_m[bomba.id][0]==1){
			collision_m[bomba.id][0]=0;
			bomba.vite--;
			aggiungi_job(bomba);
		}
		pthread_mutex_unlock(&mutex_collision);

		/*Aggiorno posizione bomba e lo passo ad aggiungi_job se e' ancora vivo*/
		if(bomba.vite>0){ 
			bomba.y++;
			aggiungi_job(bomba);
		}
		usleep(DELAY);
	}
	return NULL;
} 


/*
------------------------------------------------------------------------------------------
 Funzione Astronave LV2 - simile missili e astronave madre
------------------------------------------------------------------------------------------
*/ 

void* t_astronave2 (void* arg){
	//lista per i thread_id delle bombe
	typedef struct Node t_node;
	//Dichiarazione e inizializzazione a NULL di l
    t_node *l = NULL;
	t_node *new = NULL;
	
	pthread_t bomba;
	int parBomba[4];
	int id_bomba = generatorId();
	/*recupero stronave LV1*/	
	struct oggetto astronaveLV1 = *(struct oggetto*)arg;

	/*INIZIALIZZAZIONE ASTRONAVE DI LV2*/
	struct oggetto astronave; // contiene i dati del giocatore
	astronave.id = generatorId(); //*(int *)arg;
	astronave.sprite = "oo";
	astronave.dim=2;
	astronave.x = astronaveLV1.x;
	astronave.y = astronaveLV1.y;
	astronave.vite=2;
	astronave.tipo= ASTRONAVE2;

	int countB = 1; 	/*contatore per bomba*/
	int direction=1;    /* Spostamento orizzontale */
	
	while(astronave.vite > 0){
		/*Controllo se l'astronave ha Colliso*/
		pthread_mutex_lock(&mutex_collision);
		if(collision_m[astronave.id][0]==1){
			collision_m[astronave.id][0]=0;

			/*COLLISIONE CON MISSILE*/
			if(collision_m[astronave.id][1]==MISSILE){
				astronave.vite--;
				beep();
				aggiungi_job(astronave);
			}	
		}
		pthread_mutex_unlock(&mutex_collision);

		//Se la navicella e' ancora viva effettuo lo spostamento
		if(astronave.vite>0){
			//se' raggiunge il bordo cambia direzione e scende di y
			if(astronave.x + direction == MAXX - astronave.dim || astronave.x + direction == MINX) {
				direction = -direction;
				astronave.y++;
			}
			else{//altrimenti proseguo nella stessa direzione		
				astronave.x += direction;
			}
			aggiungi_job(astronave);
		
			//lancio una bomba ogni 50 cicli
			if(countB % 50 == 0){
				/*setto vettore da passare per la creazione di una bomba*/
				parBomba[0]= astronave.x;
				parBomba[1]= astronave.y;
				parBomba[2]= id_bomba;
				parBomba[3]= astronave.dim;
	
				pthread_create(&bomba, NULL, &t_bomba, parBomba);

				//Allocazione di l nella memoria
    			new = malloc(sizeof(t_node));
    			//Assegnazione del campo info
    			new->info = bomba;
    			//Assegnamento del campo next a NULL
    			new->next = l;
				//l adesso rappresenta la testa della lista 
        		l = new;
			}
			countB++;
		}
		usleep(DELAY);
	}

	while (l != NULL) {
		pthread_join (l->info, NULL);
        l = l->next;
    }

	return NULL;
}

/*----------------------------------------------------------------------------------------
/*
------------------------------------------------------------------------------------------
 Funzione Astronave LV1 - simile missili e astronave madre
------------------------------------------------------------------------------------------
*/ 

void* t_astronave1 (void* arg){
	//lista per i thread_id delle bombe
	typedef struct Node t_node;
	//Dichiarazione e inizializzazione a NULL di l
    t_node *l = NULL;
	t_node *new = NULL;

	pthread_t astronaveLV2;
	pthread_t bomba;
	int parBomba[4];
	int id_bomba = generatorId();

	/*INIZIALIZZAZIONE ASTRONAVE DI LV1*/
	struct oggetto astronave; // contiene i dati del giocatore
	astronave.id = generatorId(); //*(int *)arg;
	astronave.sprite = "<@>";
	astronave.dim=3;
	astronave.x = MINX;
	astronave.y = MINY;
	astronave.tipo= ASTRONAVE1;
	astronave.vite=1;
	
	int countB = 1; 	/*contatore per bomba*/
	int direction=1;    /* Spostamento orizzontale */
	
	while(astronave.vite > 0){		
		/*Controllo se l'astronave ha Colliso*/
		pthread_mutex_lock(&mutex_collision);
		if(collision_m[astronave.id][0]==1){
			collision_m[astronave.id][0]=0;

			/*COLLISIONE CON MISSILE*/
			if(collision_m[astronave.id][1]==MISSILE){
				astronave.vite--;
				beep();
				aggiungi_job(astronave);

				pthread_create(&astronaveLV2, NULL, &t_astronave2, &astronave); 

			}	
		}
		pthread_mutex_unlock(&mutex_collision);

		//Se la navicella e' ancora viva effettuo lo spostamento
		if(astronave.vite>0){
			//se' raggiunge il bordo cambia direzione e scende di y
			if(astronave.x + direction == MAXX - astronave.dim || astronave.x + direction == MINX) {
				direction = -direction;
				astronave.y++;
			}
			else{//altrimenti proseguo nella stessa direzione		
				astronave.x += direction;
			}
			aggiungi_job(astronave);

			//lancio una bomba ogni 50 cicli
			if(countB % 50 == 0){
				/*setto vettore da passare per la creazione di una bomba*/
				parBomba[0]= astronave.x;
				parBomba[1]= astronave.y;
				parBomba[2]= id_bomba;
				parBomba[3]= astronave.dim;
	
				pthread_create(&bomba, NULL, &t_bomba, parBomba);
	
				/*
				pthread_mutex_lock(&mutex_boom);
				boom[0]= 1;// bool
				boom[1]= astronave.x;
				boom[2]= astronave.y;

				pthread_mutex_unlock(&mutex_boom);*/

				//Allocazione di l nella memoria
    			new = malloc(sizeof(t_node));
    			//Assegnazione del campo info
    			new->info = bomba;
    			//Assegnamento del campo next a NULL
    			new->next = l;
				//l adesso rappresenta la testa della lista 
        		l = new;
			}
			countB++;
		}
		usleep(DELAY);
	}

	pthread_join (astronaveLV2, NULL);
	
	while (l != NULL) {
		pthread_join (l->info, NULL);
        l = l->next;
    }

	return NULL;
}

/*
------------------------------------------------------------------------------------------
 Funzione generatrice delle Astronavi LV1 - simile lanciatore missili
------------------------------------------------------------------------------------------
*/ 
void* t_generatore_astronavi(void* arg){
	int m = 4; //*(int *)arg;
	int count = 0;

	pthread_t astronave[m]; 
	int* id_astronavi;

	while(count < m) {
		id_astronavi= (int *) malloc(sizeof(int));
		*id_astronavi = count +10;
		if(pthread_create(&astronave[count], NULL, t_astronave1, id_astronavi)){ endwin(); exit;}		

		count++;
		usleep(DELAY_ASTRONAVI);
	}

	
	count = 0;
	while(count < m) {
		pthread_join (astronave[count], NULL);
		count++;
	}
	end_enemis = true;
}

/*------------------------------------------------------------------------------------------
 THREAD LANCIATORE BOMBE - ogni astronave nemica ne ha uno
------------------------------------------------------------------------------------------*/ 
/*
void* t_lanciatore_bomba(void* arg){
	pthread_t bomba; // id thread dei missili
	bool flag=false; // flag generico
	int* id_missile = (int *) malloc(sizeof(int)*2);
	id_missile[0] = generatorId();
	int* id_missile2 = (int *) malloc(sizeof(int)*2);
	id_missile2[0] = generatorId();
	
	while(1){
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
}*/





 

