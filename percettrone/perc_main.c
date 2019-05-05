/*
    Percettrone: rete di percettroni ad un singolo strato
    Copyright (C) 2018  Francesco Sisini (francescomichelesisini@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FILE_NAME_SIZE 20 

/*Numero di neuroni di input*/
#define VROWS 2

/* Calcola il valore di attivazione del percettrone in base all'input e ai pesi*/
double calculate_activation(double weight[],double input[]);

/* determina l'uscita alta o bassa (0 o 1) del percettrone*/
int classify_activation(double activation_value); 

/*Stampa a video il grafico dei punti della retta separante le classi*/
void print_js(double x,double y, int cls, double w[],char label[],int i);

void vector_init(double res[VROWS],double val);
int read_input_output(FILE *fp,double inp[],double * out);
int read_test(FILE *fp,double test[]);

int main(int argc, char * argv[])
{
  /*Nomi dei file di addestramento e test*/
  char file_name[FILE_NAME_SIZE];
  memset(file_name,0,FILE_NAME_SIZE);
  strcpy(file_name,argv[0]);
  strcat(file_name,".dat");
  
  /*Carica i dati di addestramento*/
  FILE * f=fopen(file_name,"rt");
  if(f==0) exit(1);

  
  double x[VROWS+1];/*input cognitrone*/
  double out;/*outpput cognitrone*/
  double w[VROWS+1];/*pesi cognitrone*/
  double r;/*velocità apprendimento*/
  double a;
  int cls;
  
  /*Inizializza il vettore pesi del percettrone*/
  vector_init(w,1);
  w[0]=0.2;
  w[1]=.5;
  w[2]=-2.5;
  vector_init(x,1);
  r=0.05;
  
  /*Addestra il percettrone*/
  int count=0;
  while(read_input_output(f,x,&out)!=EOF)
    {
      a=calculate_activation(w,x);
      cls=classify_activation(a);
      /*Calcolo il nuovo vettore pesi*/
      for(int i=0; i<VROWS+1; i++)
        {
          w[i]=w[i]+r*(out-cls)*x[i];
        }
      print_js( x[1],x[2], out,  w,"Apprendimento",count);
      count ++;
    }
  
  fclose(f);

  /*Nome del file con i dati di test*/
  memset(file_name,0,FILE_NAME_SIZE);
  strcpy(file_name,argv[0]);
  strcat(file_name,".tst");
  
  f=fopen(file_name,"rt");
  if(f==0) exit(1);
  
  /*Testa il percettrone*/
  while(read_test(f,x)!=EOF)
    {
      a=calculate_activation(w,x);
      cls=classify_activation(a);
      print_js( x[1],x[2], cls,  w,"Test",count);
      count++;
    }
  fclose(f);
}

double calculate_activation(double w[],double x[])
{
  int c;
  double a=0;
  c=VROWS;
  for(int j=0;j<c+1;j++)
    {
      a=a+w[j]*x[j];
    }
  return a;
}

int classify_activation(double a)
{
  if(a>0) return 1; else return 0;
}
  
void vector_init(double res[VROWS],double val)
{ 
  for(int j=0;j<VROWS+1;j++)
    {
      res[j]=val;
    }
}

  





