#include "function.h"	
#include "define.h"

/*
----------------------------------------------------------------------------------------
 Thread bomba - simile cannoneLaser
----------------------------------------------------------------------------------------
*/ 

void* t_bomba(void* arg){
	int* parBomba= (int *)arg;
	struct oggetto bomba;
	bomba.id = parBomba[2];//generatorId();
	bomba.sprite[0] = "*";
	bomba.dim=1;
	bomba.dimy=1;
	bomba.vite=1;
	bomba.tipo=BOMBA;
	bomba.x = parBomba[0] + (int)(parBomba[3]/2);
	bomba.y = parBomba[1] +1;


	while(bomba.vite > 0 && !partitaFinita()){
		/*Controllo se la bomba ha Colliso*/
		pthread_mutex_lock(&mutex_collision);
		int colliso= collision_m[bomba.id][0];
		pthread_mutex_unlock(&mutex_collision);
		if(colliso && !partitaFinita()){
			pthread_mutex_lock(&mutex_collision);
			collision_m[bomba.id][0]=0;
			pthread_mutex_unlock(&mutex_collision);
			bomba.vite--;
		}
		

		/*Aggiorno posizione bomba e lo passo ad aggiungi_job se e' ancora vivo*/
		if(bomba.vite>0){ 
			bomba.y++;
		}
		aggiungi_job(bomba);
		usleep(DELAY);
	}
	//return NULL;
} 

/*
------------------------------------------------------------------------------------------
 Funzione Astronave LV2 - simile missili e astronave madre
------------------------------------------------------------------------------------------
*/ 

void* t_astronave2 (void* arg){
	int* parAstronave2= (int *)arg;
	
	//lista per i thread_id delle bombe
	typedef struct Node t_node;
	//Dichiarazione e inizializzazione a NULL di l
    	t_node *l = NULL;
	t_node *new = NULL;
	
	pthread_t bomba;
	int parBomba[4];
	int id_bomba = generatorId();

	int sxup=1; 	bool nav_sxup=true;
	int sxdown=1; 	bool nav_sxdown=true;
	int dxup=1;	bool nav_dxup=true;
	int dxdown=1;	bool nav_dxdown=true;
	
	/*INIZIALIZZAZIONE ASTRONAVE DI LV2*/
	struct oggetto astronave; // contiene i dati del giocatore
	astronave.id = generatorId(); //*(int *)arg;
	astronave.sprite[0] = "00";
	astronave.sprite[1] = "00";
	astronave.dim=2;
	astronave.dimy=2;
	astronave.x = parAstronave2[0];
	astronave.y = parAstronave2[1];
	astronave.vite=2;
	astronave.tipo= ASTRONAVE2;
	int direction=parAstronave2[2];    /* Spostamento orizzontale */
	
	srand((int)time(0)); 	//inizializza generatore numeri casuali
	int countB = 1; 	/*contatore per bomba*/
	int r;			//ver per numero random, 0 o 1
	
	while(astronave.vite > 0 && !partitaFinita()){
		/*Controllo se l'astronave ha Colliso*/
		
		pthread_mutex_lock(&mutex_collision);
		int colliso= collision_m[astronave.id][0];
		int tipoColliso = collision_m[astronave.id][1];
		int xColliso = collision_m[astronave.id][2];
		int yColliso = collision_m[astronave.id][3];
		pthread_mutex_unlock(&mutex_collision);
		if(colliso == 1){
			pthread_mutex_lock(&mutex_collision);
			collision_m[astronave.id][0]=0;
			pthread_mutex_unlock(&mutex_collision);
			/*COLLISIONE CON MISSILE*/
			if(tipoColliso==MISSILE){
				direction *= (-1);
				astronave.vite--;
				//Inividuo la navicella colpita
                                if(astronave.x == xColliso){ //sxdown
                                	if(astronave.y == yColliso){ //sxup
                                		if(sxup >0){
                                			sxup--;
                                			aggiornaPunteggio(5);
                                			astronave.vite--;
                                			if(sxup == 0){nav_sxup=false;}
                                		}
                                	}
                                	else{//sxdown
                                		if(sxdown >0){
                                			sxdown--;
							aggiornaPunteggio(5);
                                			astronave.vite--;
							//beep();
                                			if(sxdown == 0) {
                                				//se viene distrutta una navicella di sotto viene sostituita da quella sopra
                                				//nav_sxdown=false;
                                				nav_sxdown=nav_sxup;
                                				nav_sxup=false;
                                				sxdown = sxup; 
                                			}
                                		}
                                	}
				}
				else{//dx
					if(astronave.y == yColliso){ //dxup
						if(dxup >0){
                                			dxup--;
                                			aggiornaPunteggio(5);
                                			astronave.vite--;
							//beep();
							if(dxup == 0){ nav_dxup=false;}
                              				
                                		}
                                	}
                                	else{//dxdown
                                		if(dxdown >0){
                                			dxdown--;
                                			aggiornaPunteggio(5);
                                			astronave.vite--;
							//beep();
                                			if(dxdown == 0) {
                                				//se viene distrutta una navicella di sotto viene sostituita da quella sopra
                                				//nav_sxdown=false;
                                				nav_dxdown=nav_dxup;
                                				nav_dxup=false;
                                				dxdown = dxup; 
                       
                                			}
                                		}
                                	}		
				}
				if(!nav_sxup){
					if(!nav_dxup) astronave.sprite[0]="^^";
					else astronave.sprite[0]="^0";
				}
				else if(!nav_dxup) astronave.sprite[0]="0^";
				if(!nav_sxdown){
					if(!nav_dxdown) astronave.sprite[1]="^^";
					else astronave.sprite[1]="^0";
				}
				else if(!nav_dxdown) astronave.sprite[1]="0^";
				
				//aggiungi_job(astronave);
			
				
			}
			
			/*COLLISIONE CON ASTRONAVE*/
			if(tipoColliso==ASTRONAVE1 || tipoColliso==ASTRONAVE2){
				direction *= (-1);
			}	
		}
		
		
		//Se la navicella e' ancora viva effettuo lo spostamento
		if(astronave.vite>0 && !partitaFinita()){
			//se' raggiunge il bordo cambia direzione e scende di y
			if(astronave.x + direction == MAXX - astronave.dim || astronave.x + direction == MINX) {
				direction = -direction;
				astronave.y+=2;
				if(astronave.y >= MAXY -2) {
					pthread_mutex_lock(&mutex_end);
					end_layer=true;
					pthread_mutex_unlock(&mutex_end);
				}
			}
			else{//altrimenti proseguo nella stessa direzione		
				astronave.x += direction;
			}


			//aggiungi_job(astronave);
		
			//lancio una bomba in un intervallo tra i 50 e i 100 cicli
			r = random()%2;
			countB += r; //al contatore aggiungo randomicamente un +1
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
		aggiungi_job(astronave);
		usleep(DELAY);
	}
	
	if(astronave.vite <=0){//navicella distrutta Animazione
		astronave.vite=1;
		astronave.sprite[0]="**";
		astronave.sprite[1]="**";
		aggiungi_job(astronave);
		usleep(DELAY*3);
		astronave.sprite[0]="~~";
		astronave.sprite[1]="~~";
		aggiungi_job(astronave);
		usleep(DELAY*3);
		astronave.vite=1;
		astronave.sprite[0]="**";
		astronave.sprite[1]="**";
		aggiungi_job(astronave);
		usleep(DELAY*3);
		astronave.sprite[0]="~~";
		astronave.sprite[1]="~~";
		aggiungi_job(astronave);
		usleep(DELAY*3);
		astronave.vite=0;
		aggiungi_job(astronave);
	
		aggiornaPunteggio(40);
	}
	
	while (l != NULL) {
		pthread_join (l->info, NULL);
        	l = l->next;
    	}

	pthread_mutex_lock(&mutex_scrn); 
	mvprintw(MAXY +10, 2, "t_astronave2 %d:ok", astronave.id); 
	pthread_mutex_unlock(&mutex_scrn);
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

	int parBomba[4];	//parametro da passare alla bomba
	int parAstronave2[3]; 	//parametro da passare al'astronave di lv2

	int id_bomba = generatorId();

	/*INIZIALIZZAZIONE ASTRONAVE DI LV1*/
	struct oggetto astronave; // contiene i dati del giocatore
	astronave.id = generatorId(); //*(int *)arg;
	astronave.sprite[0] = "<@>";
	astronave.dim=3;
	astronave.dimy=1;
	astronave.x = MINX;
	astronave.y = MINY+10;
	astronave.tipo= ASTRONAVE1;
	astronave.vite=1;
	
	int direction=1;    /* Spostamento orizzontale */
	
	srand((int)time(0)); 	//inizializza generatore numeri casuali
	int countB = 1; 	/*contatore per bomba*/
	int r;			//ver per numero random, 0 o 1
	
	while(astronave.vite > 0  && !partitaFinita()){		
		/*Controllo se l'astronave ha Colliso*/
		pthread_mutex_lock(&mutex_collision);
		int colliso= collision_m[astronave.id][0];
		int tipoColliso = collision_m[astronave.id][1];
		int xColliso = collision_m[astronave.id][2];
		int yColliso = collision_m[astronave.id][3];
		pthread_mutex_unlock(&mutex_collision);
		if(colliso==1){
			pthread_mutex_lock(&mutex_collision);
			collision_m[astronave.id][0]=0;
			pthread_mutex_unlock(&mutex_collision);
			/*COLLISIONE CON MISSILE*/
			if(tipoColliso==MISSILE){
				direction *= (-1);
				astronave.vite--;
				//beep();
				//aggiungi_job(astronave);
				/*setto vettore da passare per la creazione di una bomba*/
				parAstronave2[0] = astronave.x;
				parAstronave2[1] = astronave.y;
				parAstronave2[2] = direction;

				aggiornaPunteggio(20);
				
				pthread_create(&astronaveLV2, NULL, &t_astronave2, parAstronave2); 

			}	
				
			/*COLLISIONE CON ASTRONAVE*/
			if(tipoColliso==ASTRONAVE1 || tipoColliso==ASTRONAVE2){
				direction *= (-1);
			}
		}

		//Se la navicella e' ancora viva effettuo lo spostamento
		if(astronave.vite>0){
			//se' raggiunge il bordo cambia direzione e scende di y
			if(astronave.x + direction == MAXX - astronave.dim || astronave.x + direction == MINX) {
				direction = -direction;
				astronave.y+=2;
				if(astronave.y >= MAXY -2) {
					pthread_mutex_lock(&mutex_end);
					end_layer=true;
					pthread_mutex_unlock(&mutex_end);
				}
			}
			else{//altrimenti proseguo nella stessa direzione		
				astronave.x += direction;
			}
			//aggiungi_job(astronave);

			//lancio una bomba in un intervallo tra i 50 e i 100 cicli
			r = random()%2;
			countB += r; //al contatore aggiungo randomicamente un +1
			if(countB % 100 == 0){
				/*setto vettore da passare per la creazione di una bomba*/
				parBomba[0]= astronave.x;
				parBomba[1]= astronave.y;
				parBomba[2]= id_bomba; //generatorId()?
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
		
		//AGGIORNO LA POSIZIONE
		aggiungi_job(astronave);
		usleep(DELAY);
	}

	//se ha 0 vite (quindi si 'e creata la lv2, aspetto che finisca)
	if(astronave.vite <= 0){pthread_join (astronaveLV2, NULL);}
	
	
	while (l != NULL) {
		pthread_join (l->info, NULL);
        	l = l->next;
    	}
	pthread_mutex_lock(&mutex_scrn); 
	mvprintw(MAXY +9, 2, "t_astronave1 %d:ok", astronave.id); 
	pthread_mutex_unlock(&mutex_scrn);

	return NULL;
}

/*
------------------------------------------------------------------------------------------
 Funzione generatrice delle Astronavi LV1 - simile lanciatore missili
------------------------------------------------------------------------------------------
*/ 
void* t_generatore_astronavi(void* arg){
	int count = 0;
	pthread_t astronave[N_ASTONAVI_NEMICHE]; 

	while(count < N_ASTONAVI_NEMICHE && !partitaFinita()) {
		if(pthread_create(&astronave[count], NULL, t_astronave1, NULL)){ endwin(); exit;}		
		count++;
		usleep(DELAY_ASTRONAVI);
	}
	
	while(count > 0){
		pthread_join (astronave[count-1], NULL);
		count--;
	}
	
	if(!partitaFinita()){
		pthread_mutex_lock(&mutex_end);
		end_enemis = true;
		pthread_mutex_unlock(&mutex_end);
	}
	
	pthread_mutex_lock(&mutex_scrn); 
	mvprintw(MAXY +8, 2, "t_generatore_astronavi:ok"); 
	pthread_mutex_unlock(&mutex_scrn);
}



 

