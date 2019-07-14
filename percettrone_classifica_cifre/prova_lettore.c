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
 *| prova_lettore
 *| Francesco Sisini (c) 2019
 */

/* Strato (layer) 1*/
#define L1_ND 784
#define L1_NP 15

/* Strato (layer) 2*/
#define L2_ND 15
#define L2_NP 10

int main(int argc,char* argv[])
{

  if(argc<4)
    {
      printf("uso: ss_lettore <digit.csv> <layer1.w> <layer2.w>");
      exit(1);
    }
  
  
  /* Immagine da file CSV */
  int img[L1_ND];

  int label;

   /*Strato 1*/
  double v_x0[L1_ND+1];/* input dei percettroni del layer 1*/
  double v_t[(L1_ND+1)*L1_NP];/* NP vettori di peso denditrico*/
  double v_s1[L1_NP]; /*NP valori input*/
  double v_y1[L1_NP];/* NP output uno per percettrone*/

  /*Strato 2*/
  double v_x1[L2_ND+1];/* input dei percettroni del layer 2*/
  double v_u[(L2_ND+1)*L2_NP];/* NP vettori di peso denditrico*/
  double v_s2[L2_NP]; /*NP valori input*/
  double v_y2[L2_NP];/* NP output uno per percettrone*/

  /* carica i pesi v_t e v_u dai file */
  FILE* w=fopen(argv[2],"rb");
  if(w==0)
    {
      printf("File %s non trovato\n",argv[2]);
      exit (1);
    }
  
  layer_read(w,v_t, L1_ND, L1_NP);
  fclose(w);

  w=fopen(argv[3],"rb");
  if(w==0)
    {
      printf("File %s non trovato\n",argv[3]);
      exit (1);
    }
  
  layer_read(w,v_u, L2_ND, L2_NP);
  fclose(w);

  w=fopen(argv[1],"r");
  if(w==0)
    {
      printf("File %s non trovato\n",argv[1]);
      exit (1);
    }
  get_image(img,&label,w);
  fclose(w);

  /* conversione immagine da int a double */
  v_x0[0]=1;
  for(int i=0;i<L1_ND;i++)
    {
      if(img[i]>90)v_x0[i+1]=1;//((double)img[i]/255.);
      else v_x0[i+1]=0;
    }      
  
  /*** PROPAGAZIONE AVANTI ***/
          
  /** Feed Forward: Input->L1->output to L2*/
  layer_feed_forward(v_s1,v_y1,v_t,v_x0,L1_NP,L1_ND);
  
  /** Mappa y1 in x1 aggiungendo l'elemento x1_0=1*/
  layer_map_out_in(v_x1, v_y1,L2_ND);
  
  /** Feed Forward: L1->L2->output*/
  layer_feed_forward(v_s2,v_y2,v_u,v_x1,L2_NP,L2_ND);
  
   /* Valutazione output */
          int imax;
          double fmax=0;
         
          for(int i=0; i<L2_NP;i++)
            {
              if(v_y2[i]>fmax)
                {
                  fmax=v_y2[i];
                  imax=i;
                }
            }
          //print_object(v_x0+1,28, 28,1,1);
          printf("\n________________________________________");
          printf("\n| Miglior risultato %d\n\n",imax,fmax*100);
          printf("\n________________________________________");
          for(int i=0; i<L2_NP;i++)
            {
              printf("\n| %d (%.1lf\%)\n",i,v_y2[i]*100);
            }
          printf("\n________________________________________");
          printf("\n| Campioni 40000\n| Epoche 5\n| Addestramento 16 (min)");
          fflush(stdout);  
}





