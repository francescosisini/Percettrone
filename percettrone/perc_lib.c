#include <stdio.h>

void print_js(double x,double y, int cls, double w[],char label[],int iter)
{
  double m,q;
  if(w[2]==0) return;
  m=-w[1]/w[2];
  q=-w[0]/w[2];
  printf("if(i==%d){",iter);
  printf("scrivi_messaggio('%s');\n",label);
  printf("disegna_punto(%lf,%lf,%d);\n",x,y,cls);
  printf("disegna_retta(%lf,%lf);\n}\n",m,q);
  
}
int read_input_output(FILE *fp,double inp[],double* out){
  int r;
  r=fscanf(fp,"%lf,%lf,%lf",inp+1,inp+2,out);
  return r;
}

int read_test(FILE *fp,double inp[]){
  int r;
  r=fscanf(fp,"%lf,%lf",inp+1,inp+2);
  return r;
}
