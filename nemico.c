#include "function.h"	
#include "define.h"

/*
----------------------------------------------------------------------------------------
 Thread bomba - simile cannoneLaser
----------------------------------------------------------------------------------------
*/ 

void* t_bomba(void* arg){
	struct oggetto astronave = *(struct oggetto*)arg;

	struct oggetto bomba;
	bomba.id = generatorId();
	bomba.sprite = "*";
	bomba.dim=1;
	bomba.vite=1;
	bomba.tipo=BOMBA;
	bomba.x = astronave.x + (int)(astronave.dim/2);
	bomba.y = astronave.y +1;


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
	pthread_t bomba;
	/*recupero stronave LV1*/	
	struct oggetto astronaveLV1 = *(struct oggetto*)arg;

	/*INIZIALIZZAZIONE ASTRONAVE DI LV2*/
	struct oggetto astronave; // contiene i dati del giocatore
	astronave.id = ASTRONAVE2; //*(int *)arg;
	astronave.sprite = "oo";
	astronave.dim=2;
	astronave.x = astronaveLV1.x;
	astronave.y = astronaveLV1.y;
	astronave.vite=2;

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
				pthread_create(&bomba, NULL, &t_bomba, &astronave);
			}
			countB++;
		}
		usleep(DELAY);
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
	pthread_t astronaveLV2;
	pthread_t bomba;

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
				pthread_create(&bomba, NULL, &t_bomba, &astronave);
			}
			countB++;
		}
		usleep(DELAY);
	}

	pthread_join (astronaveLV2, NULL);
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







 

