/*_________________________________________________________________________
  |  Percettrone_ms: rete di percettroni a due strati
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

#include <stdlib.h>
#include <stdio.h>
#include "ss_snn_lib.h"

/*___________________________
 *| lettore_cifre
 *| Francesco Sisini (c) 2019
 */

/* Strato (layer) 1*/
#define L1_ND 784
#define L1_NP 15

/* Strato (layer) 2*/
#define L2_ND 15
#define L2_NP 10

/* Numero campioni di addestrameno */
#define N_C 500

/* Numeri di ripetizioni dell'addestramento */
#define N_T 1250

/* Velocità apprendimento*/
#define RATE 0.2

int main()
{

  /*Velocita' apprendimento*/
  double rate=RATE;

  /* Immagine da file CSV */
  int img[L1_ND];
  /* Label da file CSV (desiderd output) */
  int label;
  
  /*Strato 1*/
  double v_x0[L1_ND+1];/* input dei percettroni del layer 1*/
  double v_t[(L1_ND+1)*L1_NP];/* NP vettori di peso denditrico*/
  double v_Dt[(L1_ND+1)*L1_NP];/* Variazione v_t */
  double v_s1[L1_NP]; /*NP valori input*/
  double v_y1[L1_NP];/* NP output uno per percettrone*/

  /*Strato 2*/
  double v_x1[L2_ND+1];/* input dei percettroni del layer 2*/
  double v_u[(L2_ND+1)*L2_NP];/* NP vettori di peso denditrico*/
  double v_Du[(L2_ND+1)*L2_NP];/* Variazione v_u*/
  double v_s2[L2_NP]; /*NP valori input*/
  double v_y2[L2_NP];/* NP output uno per percettrone*/

  /*Output desiderato*/
  double v_d[L2_NP];/* NP output desiderato uno per percettrone*/
  
  /*Inizializza la rete dal file di input*/

  char comment[300];
  
  
  /*Carica dal file le configurazioni iniziali della rete*/

  srand(22);
  /*2) bias+pesi strato 1*/
  for(int i=0;i<(L1_ND+1)*L1_NP;i++)
    v_t[i]=(double)rand()/(1000.*(double)RAND_MAX);
  //print_object(v_t+1,L1_ND+1, L1_NP,1,1);
  
  /*3) bias+pesi strato 2*/
  for(int i=0;i<(L2_ND+1)*L2_NP;i++)
    v_u[i]=(double)rand()/(1000.*(double)RAND_MAX);
  
  for(int ii=0;ii<N_T;ii++)
    {
      if(ii%10==0)printf("Epoca %d di %d\n",ii,N_T);
      FILE* stream = fopen("train.csv", "r");
      if(stream==0) exit(1);
      /*Carica i dati di training ed esegue il training*/
      for(int jj=0;jj<N_C;jj++)
        {
          
          get_image(img,&label,stream);
                  
          /* conversione immagine da int a double */
          v_x0[0]=1;
          for(int i=0;i<L1_ND;i++)
            {
              if(img[i]>90)v_x0[i+1]=1;//((double)img[i]/255.);
              else v_x0[i+1]=0;
              //v_x0[i+1]=(double)img[i];
              
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
          
          /* Valutazione output */
          int imax;
          double fmax=0;
          if(ii==N_T-1 && 0)
            {
              
              printf("\x1b[2J");
              print_object(v_x0+1,28, 28,1,1);
              printf ("\nCiclo:  label:%d",label);
              printf("\tOutput: ");
              for(int i=0; i<L2_NP;i++)
                {
                  if(v_y2[i]>fmax)
                    {
                      fmax=v_y2[i];
                      imax=i;
                    }
                  printf ("%d (%f vs %f)\t",i,v_y2[i],v_d[i]);
                }
              printf("\n");
              printf("Riconosciuto il numero <%d>\n",imax);
              fflush(stdout);
              getchar();      
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

