/*_________________________________________________________________________
  |  Addestra percettrone multristrato (apms)
  |  Copyright (C) 2018  Francesco Sisini (francescomichelesisini@gmail.com)
  |
  |  This program is free software: you can redistribute it and/or modify
  |  it under the terms of the GNU General Public License as published by
  |  the Free Software Foundation, either version 3 of the License, or
  |  (at your option) any later version.
  |
  |  This program is distributed in the hope that it will be useful,
  |  but WITHOUT ANY WARRANTY; without even the implied warranty of
  |  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  |  GNU General Public License for more details.
  |
  |  You should have received a copy of the GNU General Public License
  |  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
// ./cognitiva_apms mnist_train.csv 7 60000 0.2 0.01 9 100 

#include <stdlib.h>
#include <stdio.h>
#include "cognitiva_lib.h"

/*___________________________
 *| suite:  cognitiva
 *| programma: apms
 *| Francesco Sisini (c) 2019
 */


/* Strato (layer) 1*/
#define L1_ND 784
#define L1_NP 100

/* Strato (layer) 2*/
#define L2_ND 100
#define L2_NP 10

/* Velocità apprendimento*/
#define RATE 0.2

int main(int argc,char *argv[])
{

  /* Epoche e campioni*/
  int epoche=1;
  int campioni=1;

  printf("\x1b[2J\x1b[1;1H\ncognitiva_: addestra percettrone multistrato (apms) \n");
  printf("\n(C)Scuola_Sisini 2019 https://pumar.it/cognitiva\n");
  fflush(stdout);  
  
  if(argc<4)
    {
      printf("\nUSO: cognitiva_apms <file.csv>  <numero_epoche> <numero_campioni> <lerning_rate> <max_sinapsi> <rnd_seed>\n");
      printf("\nPARAMETRI:\n");
      printf("<file.csv>         dati di addestramento e label\n");
      printf("<numero_epoche>    numero (intero) di epoche di addestramento\n");
      printf("<numero_campioni>  numero (intero) campioni da analizzare\n");
      printf("<learning_rate>    rateo (decimale) di apprendimento\n");
      printf("<max_sinapsi>      valore massimo di init. sinapsi (opz)\n");
      printf("<rnd_seed>         seme per numeri casuali (opz)\n");
      exit(1);
    }
  if(sscanf(argv[2],"%i",&epoche)!=1)
    {
      printf("Attenzione: %s non e' un valore intero\n",argv[2]);
      exit(1);
    }
   if(sscanf(argv[3],"%i",&campioni)!=1)
    {
      printf("Attenzione: %s non e' un intero\n",argv[3]);
      exit(1);
    }
  
  /*Velocita' apprendimento*/
  double rate=RATE;

  if(argc>=5)
    {
    
      if(sscanf(argv[4],"%lf",&rate)!=1)
        {
          printf("Attenzione: %s non e' un float\n",argv[4]);
          exit(1);
        }
    }

   /*Estremo superiore del valore iniziale delle sinapsi (0 to ...)*/
  double sinapsi=0.001;

  if(argc>=6)
    {
      if(sscanf(argv[5],"%lf",&sinapsi)!=1)
        {
          printf("Attenzione %s non e' un decimale\n",argv[5]);
          exit(1);
        }
    }

   /*Estremo superiore del valore iniziale delle sinapsi (0 to ...)*/
  int seed=1;

  if(argc>=7)
    {
      if(sscanf(argv[6],"%i",&seed)!=1)
        {
          printf("Attenzione %s non e' un intero\n",argv[6]);
          exit(1);
        }
    }

  
  /* Immagine da file CSV */
  int img[L1_ND];
  /* Label da file CSV (desiderd output) */
  int label;
  
  /*Strato 1*/
  double v_x0[L1_ND+1];/* input dei percettroni del layer 1*/
  double v_t[(L1_ND+1)*L1_NP];/* NP vettori di peso dendritico*/
  double v_Dt[(L1_ND+1)*L1_NP];/* Variazione v_t */
  double v_s1[L1_NP]; /*NP valori input*/
  double v_y1[L1_NP];/* NP output uno per percettrone*/

  /*Strato 2*/
  double v_x1[L2_ND+1];/* input dei percettroni del layer 2*/
  double v_u[(L2_ND+1)*L2_NP];/* NP vettori di peso dendritico*/
  double v_Du[(L2_ND+1)*L2_NP];/* Variazione v_u*/
  double v_s2[L2_NP]; /*NP valori input*/
  double v_y2[L2_NP];/* NP output uno per percettrone*/

  /*Output desiderato*/
  double v_d[L2_NP];/* NP output desiderato uno per percettrone*/
  
  /*Carica dal file le configurazioni iniziali della rete*/

  srand(seed);
  /*2) bias+pesi strato 1*/
  for(int i=0;i<(L1_ND+1)*L1_NP;i++)
    v_t[i]=sinapsi*(double)rand()/(double)RAND_MAX;
  //print_object(v_t+1,L1_ND+1, L1_NP,1,1);

  printf("\n____________________________________________");
  printf("\n| addestramento layers: layer1.w e layer2.w\n");
  printf("\nCampioni: %d, epoche: %d, l-rate: %f\n",campioni, epoche, rate);
  
  /*3) bias+pesi strato 2*/
  for(int i=0;i<(L2_ND+1)*L2_NP;i++)
    v_u[i]=sinapsi*(double)rand()/(double)RAND_MAX;
  
  for(int ii=0;ii<epoche;ii++)
    {
      printf("\x1b[15;1HEpoca %d di %d\n",ii+1,epoche);
      
      FILE* stream = fopen(argv[1], "r");
      if(stream==0) exit(1);
      /*Carica i dati di training ed esegue il training*/
      for(int jj=0;jj<campioni;jj++)
        {
          if(jj%100==0)printf("\x1b[16;1HCampione %d di %d\n",jj,campioni);
          get_image(img,&label,stream);
                  
          /* conversione immagine da int a double */
          v_x0[0]=1;
          for(int i=0;i<L1_ND;i++)
            {
              //if(img[i]>90)v_x0[i+1]=1;
              //else v_x0[i+1]=0;
              
              v_x0[i+1]=((double)img[i])/255;
              
            }                 
          /*4 output desiderato*/
          for(int i=0;i<L2_NP;i++)
            {
              if(i == label)
                v_d[i]=1;
              else
                v_d[i]=0;
            }
          
          /*** PROPAGAZIONE AVANTI ***/
          
          /** Feed Forward: Input->L1->output to L2*/
          layer_feed_forward(v_s1,v_y1,v_t,v_x0,L1_NP,L1_ND);
      
          /** Mappa y1 in x1 aggiungendo l'elemento x1_0=1*/
          layer_map_out_in(v_x1, v_y1,L2_ND);
          
          /** Feed Forward: L1->L2->output*/
          layer_feed_forward(v_s2,v_y2,v_u,v_x1,L2_NP,L2_ND);
          
          
          /*** CORREZIONE PER PROPAGAZIONE INVERSA ***/
          
          /** Propagazione inversa dell'errore in L2  (v_u  <- v_y2) */
          for(int i=0;i<L2_NP;i++)
            {
              /* correzione dei pesi (v_u) del percettrone i-esimo */
              perc_correzione( v_u+i*(L2_ND+1),v_x1,v_s2[i],v_d[i]-v_y2[i], rate,L2_ND);
            }
          
          /** Propagazione inversa dell'errore in L1  (v_t <- v_y2)*/
          for(int i=0;i<L1_NP;i++)
            {
              double dd=0;
              for(int j=0;j<L2_NP;j++)
                {
                  /* w: peso del i-esimo dendrite del j-esimo percettrone dello strato più esterno */
                  double w=v_u[j*(L2_ND+1)+i];
                  /* correzione   */
                  dd=dd+w*(v_d[j]-v_y2[j])*Dactiv_function(v_s2[j]);
                }
              
              /* correzione del percettrone i-esimo*/
              perc_correzione( v_t+i*(L1_ND+1),v_x0,v_s1[i],dd, rate,L1_ND);
            }
        }
      fclose(stream);
    }

  /*_____________________________________
    | Salva la rete
   */
  FILE* sw = fopen("layer1.w", "w");
  layer_writedown(sw,v_t, L1_ND, L1_NP);
  fclose(sw);
  
  sw = fopen("layer2.w", "w");
  layer_writedown(sw,v_u, L2_ND, L2_NP);
  fclose(sw);
  
}

