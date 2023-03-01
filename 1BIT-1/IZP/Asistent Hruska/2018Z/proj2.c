/**
 * Projekt 2 - Iteracne vypocty
 * Tomas Moravcik xmorav41@stud.fit.vutbr.cz
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>


//Vypocita logaritmus z cisla "x" o "n" iteraciach cez taylorov polynom
double taylor_log(double x, unsigned long int n)
{
    if (x==0) return NAN;
    if (x==INFINITY) return INFINITY;
    if (x<0) return NAN;
    if (n<=0) return NAN;

    double y;
    double citatel = 1;
    double tl = 0.0;
    y = x - 1;

    if(n==1) return y;

    if (x < 1)
    {
      for (unsigned k = 1; k <= n; k++)
      {
          for (int l = k; l >= 1; l--)
              y *= y;
          tl = tl - (y / k);
      }
    }

    else
    {
      for (unsigned k = 1; k <= n; k++)
      {
       citatel *= (x - 1)/x;
       tl = tl + (citatel/k);
      }
    }

    return tl;
}

//Vypocita logaritmus z cisla "x" o "n" iteraciach cez zretazeny zlomok
double cfrac_log(double x, unsigned long int n)
{
  if (x==0) return NAN;
  if (x==INFINITY) return INFINITY;
  if (x<0) return NAN;
  if (n<=0) return NAN;

  double cf = 0.0;
  double lg = (x-1)/(x+1);
  double lglg = lg*lg;

  if(n==1) return 2*lg;

  for (long int k = n+1; k >= 1; k--)
  {
     cf = (2*k-1) - k*k*lglg/cf;
  }

  return 2*lg / cf;
}

//exponencionalna funkcia s taylorovym logaritmom
double taylor_pow(double x, double y, unsigned long int n)
{
    if (x==0) return 0;
    if (x<0) return NAN;

    double citatel = y * taylor_log(x, n);
    double tl = 1.0;
    double sucet = 0.0;

    for (unsigned k = 1; k < n; k++)
    {
      tl = tl * citatel / k;
      sucet = sucet + tl;
    }

    return sucet + 1;
}

//exponencionalna funkcia s logaritmom zretazeneho zlomku
double taylorcf_pow(double x, double y, unsigned long int n)
{
    if (x==0) return 0.0;
    if (x<0) return NAN;

    double citatel = y * cfrac_log(x, n);
    double tl = 1.0;
    double sucet = 0.0;

    for (unsigned k = 1; k < n; k++)
    {
      tl = tl * citatel / k;
      sucet = sucet + tl;
    }

    return sucet + 1;
}

//skontroluje ci su iteracie nie zaporne
int check_n(char *argv)
{
    long int n = strtoul(argv,NULL,10);
    if (n < 0)
    {
      printf("Chyba v 'n'\n");
      return 0;
    }
    else return -1;
}

int main(int argc, char *argv[])
{
   //skontroluje pritomnost prikazu
   if (strcmp(argv[1],"--log") != 0 && strcmp(argv[1],"--pow") != 0)
   {
     printf("Nezadany --log alebo --pow prikaz\n");
     return -1;
   }

   //skontroluje spravnost ciselnych argumentov
   for (int i = 2; i<argc; i++)
   {
     for (unsigned j = 0; j<(strlen(argv[i])); j++)
     {
       int c = argv[i][j];
       if ((c >= '0' && c <= '9') || c == '.')
         ;

       else
       {
        printf("%d. argument '%s' obsahuje chybu '%c'\n",i,argv[i],c);
        return -1;
       }
     }
   }


   //logaritmus
   if (strcmp(argv[1],"--log") == 0)
   {
     if (argc != 4)
     {
       printf("Zly pocet argumentov\n");
       return -1;
     }

     if (!check_n(argv[3])) return -1;

     double m = strtod(argv[2],NULL);
     unsigned long int n = strtoul(argv[3],NULL,10);

     double log_tl = taylor_log(m,n);
     double log_fc = cfrac_log(m,n);

     printf("       log(%.5g) = %.12g\n",m,log(m));
     printf(" cfrac_log(%.5g) = %.12g\n",m,log_fc);
     printf("taylor_log(%.5g) = %.12g\n",m,log_tl);

   }


   //exponencionalna funkcia
   if (strcmp(argv[1],"--pow") == 0)
   {
     if (argc != 5)
     {
       printf("Zly pocet argumentov\n");
       return -1;
     }

     if (!check_n(argv[4])) return -1;

     double x = strtod(argv[2],NULL);
     double y = strtod(argv[3],NULL);
     unsigned long int n = strtoul(argv[4],NULL,10);

     double tay_pow = taylor_pow(x,y,n);
     double cf_pow = taylorcf_pow(x,y,n);

     printf("         pow(%.5g,%.5g) = %.12g\n",x,y,pow(x,y));
     printf("  taylor_pow(%.5g,%.5g) = %.12g\n",x,y,tay_pow);
     printf("taylorcf_pow(%.5g,%.5g) = %.12g\n",x,y,cf_pow);
   }
   
   return 0;
}
