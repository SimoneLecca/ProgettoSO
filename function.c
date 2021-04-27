#include "function.h"
#include "define.h"

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
		mvprintw(new_obj.y,new_obj.x,new_obj.sprite);
		curs_set(0);
		refresh();
		pthread_mutex_unlock(&mutex_scrn);// sblocco mutex  per lo schermo
}
void cancellaOggetto(struct oggetto old_obj, int dim){												// contatore			
		pthread_mutex_lock(&mutex_scrn); // blocco mutex per lo schermo		
		for(int i=0; i < dim;i++){
			mvprintw(old_obj.y,old_obj.x+i," ");		
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
			if( ((all_obj[i].x >= obj.x && all_obj[i].x <= obj.x + obj.dim) 
			|| (all_obj[i].x+all_obj[i].dim >= obj.x && all_obj[i].x+all_obj[i].dim<= obj.x + obj.dim)) 
			&& obj.y == all_obj[i].y){
				/*COLLISIONE RILEVATA*/
				return all_obj[i].id;
			}
		}
	}

	/*BORDO Y*/	
	if(obj.y <= MINY ) return BORDO;
	/*BORDO X*/
	if(obj.y >= MAXY -2 ) return BORDO;

	return -1;	
	
}

/*si occupa di: leggere dal buffer, stampare a video gli oggetti in gioco, controllare le collisioni */
void controllore(){
	int numero;
	int colliso;
	 
	struct oggetto new_obj; 		// contiene l'oggetto che viene tolto dal buffer
	struct oggetto old_obj; 		// contiene la precedente posizione dell'oggetto che viene tolto dal buffer
	struct oggetto all_obj[OBJ_ON_SCREEN];	// contiene tutti gli oggetti attivi nello schermo

	while(1){
		new_obj = (struct oggetto)elimina(); // prendo un oggetto dal buffer
		old_obj = all_obj[new_obj.id];
		all_obj[new_obj.id]=new_obj;

		pthread_mutex_lock(&mutex_collision);
		
		cancellaOggetto(old_obj, new_obj.dim);
		stampaOggetto(new_obj);

		/*fine---------------------------------------------------------------------------------------------------------------------------------------------------*/

		/*CONTROLLO COLLISIONI (se avenute): solo se vite > 0*/
		if(new_obj.vite > 0){
			/*Trovo id oggetto colliso: -1 = nessuno*/			
			colliso = isColliding(new_obj, all_obj);
			if(colliso != -1){ 
				collision_m [new_obj.id][0]=1;
				collision_m [new_obj.id][1]=colliso;
				
			}
		}
		/*Se le vite sono 0 cancello l'oggetto dallo schermo*/
		else{ cancellaOggetto(new_obj, new_obj.dim); }
	
		
		pthread_mutex_unlock(&mutex_collision);
	}
}
