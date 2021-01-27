/*   (c) Markus Schulze, 2007, 2008, 2009, 2010, 2011                          */
/*   markus.schulze@alumni.tu-berlin.de                                        */
/*   draft, 4 April 2011                                                       */
/*                                                                             */
/* This program has been written in Microsoft Visual C++ 10.0.                 */
/*                                                                             */
/* This program calculates the Schulze proportional ranking with proportional  */
/* completion, as defined in this series of papers:                            */
/*                                                                             */
/*              http://m-schulze.webhop.net/schulze1.pdf                       */
/*              http://m-schulze.webhop.net/schulze2.pdf                       */
/*              http://m-schulze.webhop.net/schulze3.zip                       */
/*              http://m-schulze.webhop.net/schulze4.pdf                       */
/*              http://m-schulze.webhop.net/schulze5.pdf                       */

/*                                                                             */
/* Changelog:                                                                  */
/*                                                                             */
/* - Extend Format 1 to support cnadidate names and print the winners          */
/* - Reworked CLI interface                                                    */
/* - Only read candidates[] where it exists: it holds 26 names, so a poll with */
/*   more choices than that used to read past the end of it                    */
/*                                                                             */
/*                                                                             */

//#include <stdafx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define FOPEN() { \
    if (output_filename) { \
    datafile=fopen(output_filename,"a+"); \
    } else { \
        datafile=stdout;\
    } \
}

#define FCLOSE() { \
    if (output_filename) { \
    fclose(datafile); \
    } \
}


const double eps1 = 0.000000000000001;
const double eps2 = 0.0000000001;
const double eps3 = 0.0000000005;
const double eps4 = 0.0000001;

struct ArcElement
{
 unsigned int start;
 unsigned int end;
 double Value;
 double Cap;
 unsigned int upper;
 unsigned int lower;
};

struct VertexElement
{
 bool marked;
 bool direction;
 unsigned int upper;
};

struct IndiffElement
{
 unsigned int Value;
 unsigned int upper;
 unsigned int lower;
};

struct VotesElement
{
 double Value;
 bool marked;
};

unsigned int M;     /* number of seats      */
unsigned int C;     /* number of candidates */
unsigned int N;     /* number of voters     */
unsigned int **Vote;/* Vote[i][j] is the preference of voter i for candidate j */

unsigned int N2,N3,N4,Length1,Length2,Length3,Length4,Length5,
             i95,i96,i97,i98,i99;
double       Output1,d99;
bool         bool1;

unsigned int **Vote2,*Value2,*Value3,*Value5,*Indif4,*Test3,*Test8,
             *Where,*Kombi,*Kombi4,*Kombi5,*sorted7,*sorted7a,*sorted8,
             *sorted8a;
char         **Vote3,**Vote4,**Vote5,**Test;
double       **Matrix,*Value4,*Value7,*Value8,*NewValue,**path,**p1;
bool         **Vote7,**Vote8,*cool4,*Test5,*elected,*winner,*Votes1;

struct IndiffElement *Indiff7,*Indiff8;
struct ArcElement    *Arcs7,*Arcs8;
struct VertexElement *Vertices7,*Vertices8;
struct VotesElement  *Votes;

FILE *datafile;
char output_filename_buffer[1024]= "output1.txt";
char *output_filename = 0;//[1024]= "output1.txt";

char candidates[26][256] = {0,};

/*******************************************************************************/

void Print1(unsigned int i1, FILE *fp = datafile)
{
 if (C<27)
   {
    switch(i1)
      {
       case  0: fprintf(fp,"A");
                break;

       case  1: fprintf(fp,"B");
                break;

       case  2: fprintf(fp,"C");
                break;

       case  3: fprintf(fp,"D");
                break;

       case  4: fprintf(fp,"E");
                break;

       case  5: fprintf(fp,"F");
                break;

       case  6: fprintf(fp,"G");
                break;

       case  7: fprintf(fp,"H");
                break;

       case  8: fprintf(fp,"I");
                break;

       case  9: fprintf(fp,"J");
                break;

       case 10: fprintf(fp,"K");
                break;

       case 11: fprintf(fp,"L");
                break;

       case 12: fprintf(fp,"M");
                break;

       case 13: fprintf(fp,"N");
                break;

       case 14: fprintf(fp,"O");
                break;

       case 15: fprintf(fp,"P");
                break;

       case 16: fprintf(fp,"Q");
                break;

       case 17: fprintf(fp,"R");
                break;

       case 18: fprintf(fp,"S");
                break;

       case 19: fprintf(fp,"T");
                break;

       case 20: fprintf(fp,"U");
                break;

       case 21: fprintf(fp,"V");
                break;

       case 22: fprintf(fp,"W");
                break;

       case 23: fprintf(fp,"X");
                break;

       case 24: fprintf(fp,"Y");
                break;

       case 25: fprintf(fp,"Z");
                break;
      }
   }
 else
 fprintf(fp,"%d",i1+1);
}

/*******************************************************************************/

void Reading_the_Input(const char *filename)
{
 unsigned int i1,i2,i3,i4,i5;
 char c1, c2;

 if (!filename) {
 char Problemname[256];

 printf("Please insert the name of the data file.\n");
 scanf("%s",(char*)&Problemname);

 datafile =fopen(Problemname,"r");
 } else {
     datafile = fopen(filename, "r");
 }

 while (!feof(datafile))
   {
    c1=fgetc(datafile);
    switch(c1)
      {
       case 'M': fscanf(datafile,"%d\n",&M);
                 break;

       case 'C': fscanf(datafile,"%d\n",&C);
                 break;

       case 'N': fscanf(datafile,"%d\n",&N);
                 break;

       case 'F': fscanf(datafile,"%d\n",&i2);
                 break;

       case 'B': do c1=fgetc(datafile);
                 while (c1!='\n');

                 Vote=new unsigned int*[N];
                 for (i1=0;i1<N;i1++)
                 Vote[i1]=new unsigned int[C];

                 switch(i2)
                   {
                    case 1: for (i1=0;i1<N;i1++)
                              {
                               fscanf(datafile,"%d",&i3);

                               for (i3=0;i3<C;i3++)
                               fscanf(datafile,"%d",&Vote[i1][i3]);

                               fscanf(datafile,"\n");
                              }

                            break;

                    case 2: for (i1=0;i1<N;i1++)
                              {
                               fscanf(datafile,"%d",&i3);

                               for (i3=0;i3<C;i3++)
                               Vote[i1][i3]=UINT_MAX;

                               i3=1;

                               if (C<27)
                                 {
                                  c1=fgetc(datafile);
                                  while (c1!='\n')
                                    {
                                     switch(c1)
                                       {
                                        case 'A': if (Vote[i1][0]==UINT_MAX)
                                                  Vote[i1][0]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][0])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][0];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'B': if (Vote[i1][1]==UINT_MAX)
                                                  Vote[i1][1]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][1])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][1];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'C': if (Vote[i1][2]==UINT_MAX)
                                                  Vote[i1][2]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][2])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][2];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'D': if (Vote[i1][3]==UINT_MAX)
                                                  Vote[i1][3]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][3])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][3];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'E': if (Vote[i1][4]==UINT_MAX)
                                                  Vote[i1][4]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][4])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][4];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'F': if (Vote[i1][5]==UINT_MAX)
                                                  Vote[i1][5]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][5])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][5];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'G': if (Vote[i1][6]==UINT_MAX)
                                                  Vote[i1][6]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][6])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][6];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'H': if (Vote[i1][7]==UINT_MAX)
                                                  Vote[i1][7]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][7])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][7];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'I': if (Vote[i1][8]==UINT_MAX)
                                                  Vote[i1][8]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][8])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][8];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'J': if (Vote[i1][9]==UINT_MAX)
                                                  Vote[i1][9]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][9])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][9];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'K': if (Vote[i1][10]==UINT_MAX)
                                                  Vote[i1][10]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][10])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][10];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'L': if (Vote[i1][11]==UINT_MAX)
                                                  Vote[i1][11]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][11])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][11];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'M': if (Vote[i1][12]==UINT_MAX)
                                                  Vote[i1][12]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][12])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][12];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'N': if (Vote[i1][13]==UINT_MAX)
                                                  Vote[i1][13]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][13])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][13];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'O': if (Vote[i1][14]==UINT_MAX)
                                                  Vote[i1][14]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][14])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][14];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'P': if (Vote[i1][15]==UINT_MAX)
                                                  Vote[i1][15]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][15])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][15];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'Q': if (Vote[i1][16]==UINT_MAX)
                                                  Vote[i1][16]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][16])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][16];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'R': if (Vote[i1][17]==UINT_MAX)
                                                  Vote[i1][17]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][17])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][17];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'S': if (Vote[i1][18]==UINT_MAX)
                                                  Vote[i1][18]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][18])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][18];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'T': if (Vote[i1][19]==UINT_MAX)
                                                  Vote[i1][19]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][19])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][19];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'U': if (Vote[i1][20]==UINT_MAX)
                                                  Vote[i1][20]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][20])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][20];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'V': if (Vote[i1][21]==UINT_MAX)
                                                  Vote[i1][21]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][21])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][21];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'W': if (Vote[i1][22]==UINT_MAX)
                                                  Vote[i1][22]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][22])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][22];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'X': if (Vote[i1][23]==UINT_MAX)
                                                  Vote[i1][23]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][23])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][23];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'Y': if (Vote[i1][24]==UINT_MAX)
                                                  Vote[i1][24]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][24])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][24];
                                                    }
                                                  i3++;
                                                  break;

                                        case 'Z': if (Vote[i1][25]==UINT_MAX)
                                                  Vote[i1][25]=i3;
                                                  else
                                                  for (i4=0;i4<C;i4++)
                                                    {
                                                     if (Vote[i1][i4]>Vote[i1][25])
                                                     if (Vote[i1][i4]!=UINT_MAX)
                                                     Vote[i1][i4]=Vote[i1][25];
                                                    }
                                                  i3++;
                                                  break;
                                       }

                                     c1=fgetc(datafile);
                                    }
                                 }
                               else
                                 {
                                  i5=0;

                                  c1='A';

                                  while (c1!='\n')
                                    {
                                     c1=fgetc(datafile);
                                     switch(c1)
                                       {
                                        case '0':  i5=10*i5;
                                                   break;

                                        case '1':  i5=10*i5+1;
                                                   break;

                                        case '2':  i5=10*i5+2;
                                                   break;

                                        case '3':  i5=10*i5+3;
                                                   break;

                                        case '4':  i5=10*i5+4;
                                                   break;

                                        case '5':  i5=10*i5+5;
                                                   break;

                                        case '6':  i5=10*i5+6;
                                                   break;

                                        case '7':  i5=10*i5+7;
                                                   break;

                                        case '8':  i5=10*i5+8;
                                                   break;

                                        case '9':  i5=10*i5+9;
                                                   break;

                                        case ' ':  if (i5>0)
                                                     {
                                                      i5--;
                                                      if (Vote[i1][i5]==UINT_MAX)
                                                      Vote[i1][i5]=i3;
                                                      else
                                                      for (i4=0;i4<C;i4++)
                                                        {
                                                         if (Vote[i1][i4]>Vote[i1][i5])
                                                         if (Vote[i1][i4]!=UINT_MAX)
                                                         Vote[i1][i4]=Vote[i1][i5];
                                                        }
                                                      i5=0;
                                                      i3++;
                                                     }
                                                   break;

                                        case '\n': if (i5>0)
                                                     {
                                                      i5--;
                                                      if (Vote[i1][i5]==UINT_MAX)
                                                      Vote[i1][i5]=i3;
                                                      else
                                                      for (i4=0;i4<C;i4++)
                                                        {
                                                         if (Vote[i1][i4]>Vote[i1][i5])
                                                         if (Vote[i1][i4]!=UINT_MAX)
                                                         Vote[i1][i4]=Vote[i1][i5];
                                                        }
                                                     }
                                                   break;
                                       }
                                    }
                                 }
                              }

                            break;
                   }

                 break;

       case 'E': do c1=fgetc(datafile);
                 while (c1!='\n');
                 break;

       case 'X': c1=fgetc(datafile); // space
                 c1=fgetc(datafile); // letter
                 c2 = c1 - 'A';      // idx to candidates
                 do
                 {
                    c1=fgetc(datafile);
                    if (c2 >= 0 && c2 < 26) {
                        if (c1 == ' ') {
                            char *tmp = (char*)&(candidates[c2][0]);
                            fgets(candidates[c2], 255, datafile);
                            candidates[c2][strlen(candidates[c2]) - 1] = '\0'; // replace file endline by eos
                            break;
                        }
                    }

                 } while (c1!='\n');
                 break;

       case '#': do c1=fgetc(datafile);
                 while (c1!='\n');
                 break;
      }
   }

 fclose(datafile);
}

/*******************************************************************************/

void Analyzing_the_Input()
{
 unsigned int i1,i2,i3,i4,i5,i6,i7,i8;
 bool j1;

 Vote2 =new unsigned int*[N];

 for (i1=0;i1<N;i1++)
 Vote2 [i1]=new unsigned int[C];

 Value2=new unsigned int[N];

 for (i1=0;i1<C;i1++)
 Vote2[0][i1]=Vote[0][i1];

 Value2[0]=1;

 N2=1;

 for (i1=1;i1<N;i1++)
   {
    i2=0;
    j1=false;

    do
      {
       j1=true;

       for (i3=0;i3<C-1;i3++)
         {
          i4=Vote [i1][i3];
          i5=Vote2[i2][i3];

          for (i6=i3+1;i6<C;i6++)
            {
             i7=Vote [i1][i6];
             i8=Vote2[i2][i6];

             if (((i4 >i7) && (i5<=i8))
                  ||
                 ((i4 <i7) && (i5>=i8))
                  ||
                 ((i4==i7) && (i5!=i8)))
               {
                j1=false;
                i3=C-1;
                i6=C;
               }
            }
         }

       if (j1==false)
       i2++;
       else
       Value2[i2]++;
      }
    while ((i2<N2) && (j1==false));

    if (j1==false)
      {
       Value2[N2]=1;

       for (i2=0;i2<C;i2++)
       Vote2[N2][i2]=Vote[i1][i2];

       N2++;
      }
   }

 delete(Vote);
}

/*******************************************************************************/

void PropCompletion()
{
 unsigned int i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,totValue;
 char c1;
 bool j1,j2,j3,j4;

 j1=false;

 i14=i99;

 while ((j1==false) && (i99<2*i14))
   {
    i99++;
    i95++;

    j1=true;

    i2=0;

    for (i3=i97;i3<N4;i3++)
    if (Indif4[i3]==i98)
      {
       j1=false;

       i1=i98;
       i2=i3;
       i3=N4;
      }

    if (j1==true)
      {
       i1=0;
       i98--;

       for (i3=0;i3<N4;i3++)
       if (Indif4[i3]>i1)
         {
          j1=false;

          i1=Indif4[i3];
          i2=i3;

          if (i1==i98)
          i3=N4;
         }

       i98=i1;
      }

    i97=i2;

    if (j1==false)
      {
       if (N4-i2>Length3)
         {
          delete(Test);
          delete(NewValue);
          delete(Where);

          Length3=2*(N4-i2);

          Test =new char*[Length3];
          for (i3=0;i3<Length3;i3++)
          Test[i3]=new char[C];

          NewValue=new double      [Length3];
          Where   =new unsigned int[Length3];
         }

       NewValue[0]=Value4[i2];

       i1 =0;
       i10=0;
       for (i3=0;i3<M;i3++)
         {
          if (Vote4[i2][i3]==0)
            {
             Kombi4[i1]=i3;
             i1++;
            }
          else
            {
             Kombi5[i10]=i3;
             Test[0][i10]=Vote4[i2][i3];
             i10++;
            }
         }

       i12=1;

       Where[0]=i2;

       for (i3=i2+1;i3<N4;i3++)
       if (Indif4[i3]==i98)
         {
          j2=true;

          for (i4=0;i4<i1;i4++)
          if (Vote4[i3][Kombi4[i4]]!=0)
            {
             j2=false;
             i4=i1;
            }

          if (j2==true)
            {
             NewValue[i12]=Value4[i3];

             Where[i12]=i3;

             for (i4=0;i4<i10;i4++)
             Test[i12][i4]=Vote4[i3][Kombi5[i4]];

             i12++;
            }
         }

       totValue=0;
       i5=0;

       for (i3=0;i3<N3;i3++)
         {
          j2=false;

          for (i4=0;i4<i1;i4++)
          if (Vote3[i3][Kombi4[i4]]!=0)
            {
             j2=true;
             i4=i1;
            }

          if (j2==true)
            {
             totValue=totValue+Value3[i3];

             i6=0;
             j2=false;

             while (i6<i5)
               {
                j2=true;

                for (i4=0;i4<i1;i4++)
                if (Vote3[i3][Kombi4[i4]]!=Vote5[i6][i4])
                  {
                   j2=false;
                   i4=i1;
                  }

                if (j2==true)
                  {
                   Value5[i6]=Value5[i6]+Value3[i3];
                   i6=i5;
                  }
                else
                i6++;
               }

             if (j2==false)
               {
                i8=0;

                for (i4=0;i4<i1;i4++)
                  {
                   c1=Vote3[i3][Kombi4[i4]];
                   Vote5[i5][i4]=c1;

                   if (c1==0)
                   i8++;
                  }

                Test8[i5]=i8;

                Value5[i5]=Value3[i3];

                i5++;
               }
            }
         }

       if (i5==0)
         {
          i5=i1+1;

          totValue=i1+1;

          for (i3=0;i3<i1;i3++)
            {
             for (i4=0;i4<i1;i4++)
             Vote5[i3][i4]=0;

             Vote5[i3][i3]=-1;

             Test8[i3]=i1-1;

             Value5[i3]=1;
            }

          for (i3=0;i3<i1;i3++)
          Vote5[i1][i3]=1;

          Test8[i1]=0;

          Value5[i1]=1;
         }

       i7=N4+(i5-1)*i12;

       if (i7>Length4)
         {
          char **Vote6;
          double *Value6;
          unsigned int *Indif6;

          Vote6=new char*[N4];

          for (i3=0;i3<N4;i3++)
          Vote6[i3]=new char[C];

          Value6=new double      [N4];
          Indif6=new unsigned int[N4];

          for (i3=0;i3<N4;i3++)
            {
             Value6[i3]=Value4[i3];
             Indif6[i3]=Indif4[i3];

             for (i4=0;i4<M;i4++)
             Vote6[i3][i4]=Vote4[i3][i4];
            }

          delete(Vote4 );
          delete(Value4);
          delete(Indif4);
          delete(cool4 );

          Length4=2*i7;

          Vote4=new char*[Length4];

          for (i3=0;i3<Length4;i3++)
          Vote4[i3]=new char[C];

          Value4=new double      [Length4];
          Indif4=new unsigned int[Length4];
          cool4 =new bool        [Length4];

          for (i3=0;i3<N4;i3++)
            {
             Value4[i3]=Value6[i3];
             Indif4[i3]=Indif6[i3];

             for (i4=0;i4<M;i4++)
             Vote4[i3][i4]=Vote6[i3][i4];
            }

          delete(Vote6 );
          delete(Value6);
          delete(Indif6);

          delete(Test3);
          Test3=new unsigned int[Length4];
         }

       for (i3=0;i3<i12;i3++)
         {
          i8=Where[i12-i3-1];
          i7=N4-i3-1;

          Value4[i8]=Value4[i7];
          Indif4[i8]=Indif4[i7];

          for (i4=0;i4<M;i4++)
          Vote4[i8][i4]=Vote4[i7][i4];
         }

       N4=N4-i12;

       i4=N4;

       for (i13=0;i13<i12;i13++)
         {
          i11=0;

          for (i9=0;i9<N4;i9++)
            {
             j4=true;

             for (i3=0;i3<i10;i3++)
             if (Test[i13][i3]!=Vote4[i9][Kombi5[i3]])
               {
                j4=false;
                i3=i10;
               }

             if (j4==true)
               {
                Test3[i11]=i9;
                i11++;
               }
            }

          for (i9=0;i9<i5;i9++)
            {
             j3=false;

             i6=0;

             while (i6<i11)
               {
                i7=Test3[i6];

                if (Indif4[i7]==Test8[i9])
                  {
                   j3=true;

                   for (i3=0;i3<i1;i3++)
                   if (Vote5[i9][i3]!=Vote4[i7][Kombi4[i3]])
                     {
                      j3=false;
                      i3=i1;
                     }

                   if (j3==true)
                     {
                      Value4[i7]=Value4[i7]+(NewValue[i13]*Value5[i9])/totValue;
                      i11--;
                      Test3[i6]=Test3[i11];
                      i6=i11;
                     }
                   else
                   i6++;
                  }
                else
                i6++;
               }

             if (j3==false)
               {
                Value4[i4]=(NewValue[i13]*Value5[i9])/totValue;

                for (i3=0;i3<i1;i3++)
                Vote4[i4][Kombi4[i3]]=Vote5[i9][i3];

                for (i3=0;i3<i10;i3++)
                Vote4[i4][Kombi5[i3]]=Test[i13][i3];

                Indif4[i4]=Test8[i9];

                i4++;
               }
            }
         }
       N4=i4;
      }
   }

 bool1=j1;
}

/*******************************************************************************/

void PropCompletionSimple()
{
 unsigned int i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i15,IndiffMax,totValue;
 char c1;
 bool j1,j2,j3,j4,j5,j6;

 j1=false;

 IndiffMax=M;
 i15=0;

 while (j1==false)
   {
    j1=true;

    i2=0;

    for (i3=i15;i3<N4;i3++)
    if (cool4 [i3]==false)
    if (Indif4[i3]==IndiffMax)
      {
       j1=false;

       i1=IndiffMax;
       i2=i3;
       i3=N4;
      }

    if (j1==true)
      {
       i1=0;
       IndiffMax--;

       for (i3=0;i3<N4;i3++)
       if (cool4 [i3]==false)
       if (Indif4[i3]>i1)
         {
          j1=false;

          i1=Indif4[i3];
          i2=i3;

          if (i1==IndiffMax)
          i3=N4;
         }

       IndiffMax=i1;
      }

    i15=i2;

    if (j1==false)
      {
       if (N4-i2>Length3)
         {
          delete(Test);
          delete(NewValue);
          delete(Where);

          Length3=2*(N4-i2);

          Test =new char*[Length3];
          for (i3=0;i3<Length3;i3++)
          Test[i3]=new char[C];

          NewValue=new double      [Length3];
          Where   =new unsigned int[Length3];
         }

       NewValue[0]=Value4[i2];

       i1 =0;
       i10=0;
       for (i3=0;i3<M;i3++)
         {
          if (Vote4[i2][i3]==0)
            {
             Kombi4[i1]=i3;
             i1++;
            }
          else
            {
             Kombi5[i10]=i3;
             Test[0][i10]=Vote4[i2][i3];
             i10++;
            }
         }

       i12=1;

       Where[0]=i2;

       for (i3=i2+1;i3<N4;i3++)
       if (cool4 [i3]==false)
       if (Indif4[i3]==IndiffMax)
         {
          j2=true;

          for (i4=0;i4<i1;i4++)
          if (Vote4[i3][Kombi4[i4]]!=0)
            {
             j2=false;
             i4=i1;
            }

          if (j2==true)
            {
             NewValue[i12]=Value4[i3];

             Where[i12]=i3;

             for (i4=0;i4<i10;i4++)
             Test[i12][i4]=Vote4[i3][Kombi5[i4]];

             i12++;
            }
         }

       totValue=0;
       i5=0;

       for (i3=0;i3<N3;i3++)
         {
          j2=false;

          for (i4=0;i4<i1;i4++)
          if (Vote3[i3][Kombi4[i4]]!=0)
            {
             j2=true;
             i4=i1;
            }

          if (j2==true)
            {
             totValue=totValue+Value3[i3];

             i6=0;
             j2=false;

             while (i6<i5)
               {
                j2=true;

                for (i4=0;i4<i1;i4++)
                if (Vote3[i3][Kombi4[i4]]!=Vote5[i6][i4])
                  {
                   j2=false;
                   i4=i1;
                  }

                if (j2==true)
                  {
                   Value5[i6]=Value5[i6]+Value3[i3];
                   i6=i5;
                  }
                else
                i6++;
               }

             if (j2==false)
               {
                i8=0;
                j6=false;

                for (i4=0;i4<i1;i4++)
                  {
                   c1=Vote3[i3][Kombi4[i4]];
                   Vote5[i5][i4]=c1;

                   if (c1==-1)
                   j6=true;
                   else
                   if (c1==0)
                   i8++;
                  }

                Test8[i5]=i8;
                Test5[i5]=j6;

                Value5[i5]=Value3[i3];

                i5++;
               }
            }
         }

       if (i5==0)
         {
          i5=i1+1;

          totValue=i1+1;

          for (i3=0;i3<i1;i3++)
            {
             for (i4=0;i4<i1;i4++)
             Vote5[i3][i4]=0;

             Vote5[i3][i3]=-1;

             Test8[i3]=i1-1;
             Test5[i3]=true;

             Value5[i3]=1;
            }

          for (i3=0;i3<i1;i3++)
          Vote5[i1][i3]=1;

          Test8[i1]=0;
          Test5[i1]=false;

          Value5[i1]=1;
         }

       i7=N4+(i5-1)*i12;

       if (i7>Length4)
         {
          char **Vote6;
          double *Value6;
          unsigned int *Indif6;
          bool *cool6;

          Vote6=new char*[N4];

          for (i3=0;i3<N4;i3++)
          Vote6[i3]=new char[C];

          Value6=new double      [N4];
          Indif6=new unsigned int[N4];
          cool6 =new bool        [N4];

          for (i3=0;i3<N4;i3++)
            {
             Value6[i3]=Value4[i3];
             Indif6[i3]=Indif4[i3];
             cool6 [i3]=cool4 [i3];

             for (i4=0;i4<M;i4++)
             Vote6[i3][i4]=Vote4[i3][i4];
            }

          delete(Vote4 );
          delete(Value4);
          delete(Indif4);
          delete(cool4 );

          Length4=2*i7;

          Vote4=new char*[Length4];

          for (i3=0;i3<Length4;i3++)
          Vote4[i3]=new char[C];

          Value4=new double      [Length4];
          Indif4=new unsigned int[Length4];
          cool4 =new bool        [Length4];

          for (i3=0;i3<N4;i3++)
            {
             Value4[i3]=Value6[i3];
             Indif4[i3]=Indif6[i3];
             cool4 [i3]=cool6 [i3];

             for (i4=0;i4<M;i4++)
             Vote4[i3][i4]=Vote6[i3][i4];
            }

          delete(Vote6 );
          delete(Value6);
          delete(Indif6);
          delete(cool6 );

          delete(Test3);
          Test3=new unsigned int[Length4];
         }

       for (i3=0;i3<i12;i3++)
         {
          i8=Where[i12-i3-1];
          i7=N4-i3-1;

          Value4[i8]=Value4[i7];
          cool4 [i8]=cool4 [i7];
          Indif4[i8]=Indif4[i7];

          for (i4=0;i4<M;i4++)
          Vote4[i8][i4]=Vote4[i7][i4];
         }

       N4=N4-i12;

       i4=N4;

       for (i13=0;i13<i12;i13++)
         {
          i11=0;

          for (i9=0;i9<N4;i9++)
            {
             j4=true;

             for (i3=0;i3<i10;i3++)
             if (Test[i13][i3]!=Vote4[i9][Kombi5[i3]])
               {
                j4=false;
                i3=i10;
               }

             if (j4==true)
               {
                Test3[i11]=i9;
                i11++;
               }
            }

          j5=false;
          for (i3=0;i3<i10;i3++)
          if (Test[i13][i3]==-1)
            {
             j5=true;
             i3=i10;
            }

          for (i9=0;i9<i5;i9++)
            {
             j4=j5;

             if (j4==false)
             j4=Test5[i9];
             j3=false;

             i6=0;

             while (i6<i11)
               {
                i7=Test3[i6];

                if ((cool4[i7]==j4) && (Indif4[i7]==Test8[i9]))
                  {
                   j3=true;

                   for (i3=0;i3<i1;i3++)
                   if (Vote5[i9][i3]!=Vote4[i7][Kombi4[i3]])
                     {
                      j3=false;
                      i3=i1;
                     }

                   if (j3==true)
                     {
                      Value4[i7]=Value4[i7]+(NewValue[i13]*Value5[i9])/totValue;
                      i11--;
                      Test3[i6]=Test3[i11];
                      i6=i11;
                     }
                   else
                   i6++;
                  }
                else
                i6++;
               }

             if (j3==false)
               {
                Value4[i4]=(NewValue[i13]*Value5[i9])/totValue;

                for (i3=0;i3<i1;i3++)
                Vote4[i4][Kombi4[i3]]=Vote5[i9][i3];

                for (i3=0;i3<i10;i3++)
                Vote4[i4][Kombi5[i3]]=Test[i13][i3];

                Indif4[i4]=Test8[i9];
                cool4 [i4]=j4;

                i4++;
               }
            }
         }
       N4=i4;
      }
   }
}

/*******************************************************************************/

void EKarp()
{
 unsigned int i1,i2,i3,i47,i48,i57,i58,i6,i7,i8,i9,i12,i13,i15a,i15b,i16,i17a,
     i17b;
 unsigned int N7,N8,NArcs7,NArcs8,NVertices7,NVertices8,Source7,Source8,Drain7,
     Drain8,IndiffMax;
 bool j1,j2,j3,j4,j5,j6;
 double Flow,PossFlow,d1,d2;
 double LowerBounce7,UpperBounce7,UpperBounce7a,
        LowerBounce8,UpperBounce8,UpperBounce8a;
 struct ArcElement *p;
 struct VertexElement *q;

 if (N4>Length5)
   {
    delete(Vote7);
    delete(Vote8);
    delete(Value7);
    delete(Value8);
    delete(Indiff7);
    delete(Indiff8);
    delete(sorted7);
    delete(sorted7a);
    delete(sorted8);
    delete(sorted8a);

    Length5=2*N4;

    Vote7=new bool*[Length5];
    Vote8=new bool*[Length5];

    for (i1=0;i1<Length5;i1++)
      {
       Vote7[i1]=new bool[C];
       Vote8[i1]=new bool[C];
      }

    Value7  =new double              [Length5];
    Value8  =new double              [Length5];
    Indiff7 =new struct IndiffElement[Length5];
    Indiff8 =new struct IndiffElement[Length5];
    sorted7 =new unsigned int        [Length5];
    sorted7a=new unsigned int        [Length5];
    sorted8 =new unsigned int        [Length5];
    sorted8a=new unsigned int        [Length5];
   }

 for (i1=0;i1<M;i1++)
   {
    if (Vote4[0][i1]==1)
    Vote8[0][i1]=true;
    else
    Vote8[0][i1]=false;
   }

 Value8[0]=Value4[0];

 N8=1;

 for (i1=1;i1<N4;i1++)
   {
    i2=0;
    j1=false;

    while (i2<N8)
      {
       j1=true;

       for (i3=0;i3<M;i3++)
       if (((Vote8[i2][i3]==false) && (Vote4[i1][i3]==1))
        || ((Vote8[i2][i3]==true ) && (Vote4[i1][i3]!=1)))
         {
          j1=false;
          i3=M;
         }

       if (j1==true)
         {
          Value8[i2]=Value8[i2]+Value4[i1];
          i2=N8;
         }
       else
       i2++;
      }

    if (j1==false)
      {
       for (i3=0;i3<M;i3++)
         {
          if (Vote4[i1][i3]==1)
          Vote8[N8][i3]=true;
          else
          Vote8[N8][i3]=false;
         }

       Value8[N8]=Value4[i1];

       N8++;
      }
   }

 NArcs8=M;
 LowerBounce8 =0.0;
 UpperBounce8a=N+0.0;

 for (i1=0;i1<M;i1++)
 Votes1[i1]=false;

 i58=0;
 for (i1=0;i1<N8;i1++)
   {
    i2=1;

    for (i3=0;i3<M;i3++)
    if (Vote8[i1][i3]==false)
      {
       i2++;
       Votes1[i3]=true;
      }

    Indiff8[i1].Value=i2;

    if (i2==1)
      {
       i48=i1;
       i58=1;
       UpperBounce8a=UpperBounce8a-Value8[i1];
      }
    else
    NArcs8=NArcs8+i2;
   }

 if (UpperBounce8a>d99)
 UpperBounce8a=d99;

 j3=true;
 for (i1=0;i1<M;i1++)
 if (Votes1[i1]==false)
 j3=false;

 if (j3==true)
   {
    for (i1=0;i1<M;i1++)
      {
       if (Vote4[0][i1]==-1)
       Vote7[0][i1]=false;
       else
       Vote7[0][i1]=true;
      }

    Value7[0]=Value4[0];

    N7=1;

    for (i1=1;i1<N4;i1++)
      {
       i2=0;
       j1=false;

       while (i2<N7)
         {
          j1=true;

          for (i3=0;i3<M;i3++)
          if (((Vote7[i2][i3]==false) && (Vote4[i1][i3]!=-1))
           || ((Vote7[i2][i3]==true ) && (Vote4[i1][i3]==-1)))
            {
             j1=false;
             i3=M;
            }

          if (j1==true)
            {
             Value7[i2]=Value7[i2]+Value4[i1];
             i2=N7;
            }
          else
          i2++;
         }

       if (j1==false)
         {
          for (i3=0;i3<M;i3++)
            {
             if (Vote4[i1][i3]==-1)
             Vote7[N7][i3]=false;
             else
             Vote7[N7][i3]=true;
            }

          Value7[N7]=Value4[i1];

          N7++;
         }
      }

    NArcs7=M;
    LowerBounce7 =0.0;
    UpperBounce7a=N+0.0;

    for (i1=0;i1<M;i1++)
    Votes1[i1]=false;

    i57=0;
    for (i1=0;i1<N7;i1++)
      {
       i2=1;

       for (i3=0;i3<M;i3++)
       if (Vote7[i1][i3]==false)
         {
          i2++;
          Votes1[i3]=true;
         }

       Indiff7[i1].Value=i2;

       if (i2==1)
         {
          i47=i1;
          i57=1;
          UpperBounce7a=UpperBounce7a-Value7[i1];
         }
       else
       NArcs7=NArcs7+i2;
      }

    if (UpperBounce7a>d99)
    UpperBounce7a=d99;

    j2=true;
    for (i1=0;i1<M;i1++)
    if (Votes1[i1]==false)
    j2=false;

    if ((NArcs7>Length1) || (NArcs8>Length1))
      {
       delete(Arcs7);
       delete(Arcs8);

       Length1=2*NArcs7;
       if (NArcs8>NArcs7)
       Length1=2*NArcs8;

       Arcs7=new struct ArcElement[Length1];
       Arcs8=new struct ArcElement[Length1];
      }

    NVertices7=N7-i57+2+M;
    NVertices8=N8-i58+2+M;

    if ((NVertices7>Length2) || (NVertices8>Length2))
      {
       delete(Vertices7);
       delete(Vertices8);

       Length2=2*NVertices7;
       if (NVertices8>NVertices7)
       Length2=2*NVertices8;

       Vertices7=new struct VertexElement[Length2];
       Vertices8=new struct VertexElement[Length2];
      }

    Source7 = N7-i57;
    Drain7  = Source7+M+1;
    Source8 = N8-i58;
    Drain8  = Source8+M+1;

    UpperBounce7=UpperBounce7a/(M+0.0);
    UpperBounce8=UpperBounce8a/(M+0.0);

    if (j2==true)
      {
       Flow=0.0;

       for (i1=0;i1<M;i1++)
       Votes[i1].Value=0.0;

       Indiff7[0].upper=N7-1;
       Indiff7[0].lower=1;

       for (i1=1;i1<N7;i1++)
         {
          Indiff7[i1].upper=i1-1;
          Indiff7[i1].lower=i1+1;
         }

       Indiff7[N7-1].lower=0;

       if (i57==1)
         {
          i12=Indiff7[i47].upper;
          i13=Indiff7[i47].lower;

          Indiff7[i12].lower=i13;
          Indiff7[i13].upper=i12;
         }

       i9=0;

       IndiffMax=2;

       i1=0;
       i2=0;

       if (i57==1)
         {
          i1=1;

          if (i47==0)
          i2=1;
         }

       while (i1<N7)
         {
          i13=Indiff7[i2].lower;

          if (Indiff7[i2].Value==IndiffMax)
            {
             sorted7[i1]=i2;
             i1++;

             i12=Indiff7[i2].upper;

             Indiff7[i12].lower=i13;
             Indiff7[i13].upper=i12;
            }

          if (i2>=i13)
          IndiffMax++;

          i2=i13;
         }

       i17a=N7+1-i57;
       for (i1=i57;i1<N7;i1++)
         {
          i2=sorted7[i1];

          d1=Value7[i2];

          for (i6=0;i6<M;i6++)
          Votes[i6].marked=Vote7[i2][i6];

          i16=0;
          for (i7=1;i7<Indiff7[i2].Value;i7++)
            {
             for (i6=i16;i6<M;i6++)
             if (Votes[i6].marked==false)
               {
                i8 =i6;
                i16=i6;
                i6 =M;
               }

             p = &Arcs7[i9];

             p -> Cap   = Value7[i2];

             if (d1 > 0.0)
               {
                for (i6=i8+1;i6<M;i6++)
                if ((Votes[i6].marked==false)
                 && (Votes[i6].Value<Votes[i8].Value))
                i8=i6;

                d2 = UpperBounce7 - Votes[i8].Value;

                if (d2 < eps1)
                p -> Value = 0.0;
                else
                  {
                   if (d2 > d1) d2 = d1 ;

                   p -> Value = d2;

                   if (p -> Cap - d2 < eps1)
                   p -> Value = p -> Cap;

                   d1=d1-d2;

                   if (d1<eps1)
                   d1=0.0;

                   Votes[i8].Value=Votes[i8].Value+d2;
                  }
               }
             else
             p -> Value = 0.0;

             Votes[i8].marked=true;

             if (i57==1)
               {
                if (i2<i47)
                p -> start = i2;
                else
                p -> start = i2-1;
               }
             else
             p -> start = i2;

             p -> end   = i17a+i8;

             i9++;
            }

          sorted7a[i1]=i9;

          p = &Arcs7[i9];

          p -> start = Source7;
          p -> Cap   = Value7[i2];
          p -> Value = p -> Cap - d1;

          if (i57==1)
            {
             if (i2<i47)
             p -> end = i2;
             else
             p -> end = i2-1;
            }
          else
          p -> end=i2;

          i9++;
         }

       for (i1=0;i1<M;i1++)
         {
          p = &Arcs7[i9];

          p -> start = i17a+i1;
          p -> end   = Drain7;
          p -> Value = Votes[i1].Value;
          p -> Cap   = UpperBounce7;

          if (p -> Cap - p -> Value < eps1)
          p -> Value = p -> Cap;

          Flow = Flow + p -> Value;

          i9++;
         }

       j1=true;

       i15a=NArcs7-M;

       if (Flow<=UpperBounce7a-eps2)
       do
         {
          for (i1=0;i1<NVertices7;i1++)
          Vertices7[i1].marked=false;

          Arcs7[0].upper=NArcs7-1;
          Arcs7[0].lower=1;

          for (i1=1;i1<NArcs7;i1++)
            {
             Arcs7[i1].upper=i1-1;
             Arcs7[i1].lower=i1+1;
            }

          Arcs7[NArcs7-1].lower=0;

          for (i1=i57;i1<N7;i1++)
            {
             i2=sorted7a[i1];

             p = &Arcs7[i2];

             i12 = p -> upper;
             i13 = p -> lower;

             if (p -> Value < p -> Cap - eps1)
               {
                q = &Vertices7[p -> end];

                q -> marked    = true;
                q -> upper     = i2;
                q -> direction = true;
               }

             Arcs7[i12].lower=i13;
             Arcs7[i13].upper=i12;
            }

          i1 =NArcs7-1;

          j4=true;

          j5=true;

          do
            {
             p = &Arcs7[i1];
             i12 = p -> upper;

             if (Vertices7[p -> start].marked==true)
               {
                if (p -> Value < p -> Cap - eps1)
                  {
                   q = &Vertices7[p -> end];

                   q -> marked    = true;
                   q -> upper     = i1;
                   q -> direction = true;

                   j4=false;
                  }

                i13 = p -> lower;
                Arcs7[i12].lower=i13;
                Arcs7[i13].upper=i12;
               }

             if (i1<i12)
             j5=false;

             i1=i12;
            }
          while (j5==true);

          if (j4==false)
            {
             j4=true;

             j5=true;

             do
               {
                j6=true;

                do
                  {
                   p = &Arcs7[i1];
                   i12 = p -> upper;

                   if (Vertices7[p -> start].marked==true)
                     {
                      if (p -> Value < p -> Cap - eps1)
                        {
                         q = &Vertices7[Drain7];

                         q -> marked    = true;
                         q -> upper     = i1;
                         q -> direction = true;
                        }

                      i13 = p -> lower;
                      Arcs7[i12].lower=i13;
                      Arcs7[i13].upper=i12;
                     }

                   if (i12<i15a)
                   j6=false;

                   i1=i12;
                  }
                while ((Vertices7[Drain7].marked==false) && (j6==true));

                if (Vertices7[Drain7].marked==false)
                  {
                   j6=true;

                   do
                     {
                      p = &Arcs7[i1];
                      i12 = p -> upper;

                      if (Vertices7[p -> end].marked==true)
                        {
                         q = &Vertices7[p -> start];

                         if ((q -> marked == false) && (p -> Value > eps1))
                           {
                            q -> marked    = true;
                            q -> upper     = i1;
                            q -> direction = false;

                            j4=false;
                           }

                         i13 = p -> lower;
                         Arcs7[i12].lower=i13;
                         Arcs7[i13].upper=i12;
                        }

                      if (i1<i12)
                        {
                         if (j4==true)
                         j5=false;

                         j4=true;

                         j6=false;
                        }

                      i1=i12;
                     }
                   while (j6==true);

                   if (j5==true)
                     {
                      j6=true;

                      do
                        {
                         p = &Arcs7[i1];
                         i12 = p -> upper;

                         if (Vertices7[p -> start].marked==true)
                           {
                            q = &Vertices7[p -> end];

                            if ((q -> marked == false)
                             && (p -> Value < p -> Cap - eps1))
                              {
                               q -> marked    = true;
                               q -> upper     = i1;
                               q -> direction = true;

                               j4=false;
                              }

                            i13 = p -> lower;
                            Arcs7[i12].lower=i13;
                            Arcs7[i13].upper=i12;
                           }

                         if (i1<i12)
                           {
                            if (j4==true)
                            j5=false;

                            j4=true;

                            j6=false;
                           }

                         i1=i12;
                        }
                      while (j6==true);
                     }
                  }
               }
             while ((Vertices7[Drain7].marked==false) && (j5==true));
            }

          if (Vertices7[Drain7].marked==true)
            {
             i1=Drain7;

             PossFlow=N+0.0;

             do
               {
                p = &Arcs7[Vertices7[i1].upper];

                if (Vertices7[i1].direction==true)
                  {
                   if (p -> Cap - p -> Value < PossFlow)

                   PossFlow = p -> Cap - p -> Value;

                   i1 = p -> start;
                  }

                else
                  {
                   if (p -> Value < PossFlow)

                   PossFlow = p -> Value;

                   i1 = p -> end;
                  }
               }
             while (i1!=Source7);

             i1=Drain7;

             do
               {
                p = &Arcs7[Vertices7[i1].upper];

                if (Vertices7[i1].direction==true)
                  {
                   p -> Value = p -> Value + PossFlow;

                   if (p -> Value > p -> Cap - eps1)

                   p -> Value = p -> Cap;

                   i1 = p -> start;
                  }

                else
                  {
                   p -> Value = p -> Value - PossFlow;

                   if (p -> Value < eps1)

                   p -> Value = 0.0;

                   i1 = p -> end;
                  }
               }
             while (i1!=Source7);

             Flow=Flow+PossFlow;
            }

          else
          j1=false;

          if (Flow>UpperBounce7a-eps2)
          j1=false;
         }
       while (j1==true);

       LowerBounce7=N+0.0;
       for (i1=0;i1<M;i1++)
       if (Arcs7[i15a+i1].Value<LowerBounce7)
       LowerBounce7=Arcs7[i15a+i1].Value;

       UpperBounce7a=Flow;
       UpperBounce7 =UpperBounce7a/(M+0.0);
      }
    else
      {
       LowerBounce7 =0.0;
       UpperBounce7 =0.0;
       UpperBounce7a=0.0;
      }

    Flow=0.0;

    for (i1=0;i1<M;i1++)
    Votes[i1].Value=0.0;

    Indiff8[0].upper=N8-1;
    Indiff8[0].lower=1;

    for (i1=1;i1<N8;i1++)
      {
       Indiff8[i1].upper=i1-1;
       Indiff8[i1].lower=i1+1;
      }

    Indiff8[N8-1].lower=0;

    if (i58==1)
      {
       i12=Indiff8[i48].upper;
       i13=Indiff8[i48].lower;

       Indiff8[i12].lower=i13;
       Indiff8[i13].upper=i12;
      }

    i9=0;

    IndiffMax=2;

    i1=0;
    i2=0;

    if (i58==1)
      {
       i1=1;

       if (i48==0)
       i2=1;
      }

    while (i1<N8)
      {
       i13=Indiff8[i2].lower;

       if (Indiff8[i2].Value==IndiffMax)
         {
          sorted8[i1]=i2;
          i1++;

          i12=Indiff8[i2].upper;

          Indiff8[i12].lower=i13;
          Indiff8[i13].upper=i12;
         }

       if (i2>=i13)
       IndiffMax++;

       i2=i13;
      }

    i17b=N8+1-i58;
    for (i1=i58;i1<N8;i1++)
      {
       i2=sorted8[i1];

       d1=Value8[i2];

       for (i6=0;i6<M;i6++)
       Votes[i6].marked=Vote8[i2][i6];

       i16=0;
       for (i7=1;i7<Indiff8[i2].Value;i7++)
         {
          for (i6=i16;i6<M;i6++)
          if (Votes[i6].marked==false)
            {
             i8 =i6;
             i16=i6;
             i6 =M;
            }

          p = &Arcs8[i9];

          p -> Cap = Value8[i2];

          if (d1 > 0.0)
            {
             for (i6=i8+1;i6<M;i6++)
             if ((Votes[i6].marked==false)
              && (Votes[i6].Value<Votes[i8].Value))
             i8=i6;

             d2 = UpperBounce8 - Votes[i8].Value;

             if (d2 < eps1)
             p -> Value = 0.0;
             else
               {
                if (d2 > d1) d2 = d1 ;

                p -> Value = d2;

                if (p -> Cap - d2 < eps1)
                p -> Value = p -> Cap;

                d1=d1-d2;

                if (d1<eps1)
                d1=0.0;

                Votes[i8].Value=Votes[i8].Value+d2;
               }
            }
          else
          p -> Value = 0.0;

          Votes[i8].marked=true;

          if (i58==1)
            {
             if (i2<i48)
             p -> start = i2;
             else
             p -> start = i2-1;
            }
          else
          p -> start = i2;

          p -> end   = i17b+i8;

          i9++;
         }

       sorted8a[i1]=i9;

       p = &Arcs8[i9];

       p -> start = Source8;
       p -> Cap   = Value8[i2];
       p -> Value = p -> Cap - d1;

       if (i58==1)
         {
          if (i2<i48)
          p -> end = i2;
          else
          p -> end = i2-1;
         }
       else
       p -> end=i2;

       i9++;
      }

    for (i1=0;i1<M;i1++)
      {
       p = &Arcs8[i9];

       p -> start = i17b+i1;
       p -> end   = Drain8;
       p -> Value = Votes[i1].Value;
       p -> Cap   = UpperBounce8;

       if (p -> Cap - p -> Value < eps1)
       p -> Value = p -> Cap;

       Flow = Flow + p -> Value;

       i9++;
      }

    j1=true;

    i15b=NArcs8-M;

    if (Flow<=UpperBounce8a-eps2)
    do
      {
       for (i1=0;i1<NVertices8;i1++)
       Vertices8[i1].marked=false;

       Arcs8[0].upper=NArcs8-1;
       Arcs8[0].lower=1;

       for (i1=1;i1<NArcs8;i1++)
         {
          Arcs8[i1].upper=i1-1;
          Arcs8[i1].lower=i1+1;
         }

       Arcs8[NArcs8-1].lower=0;

       for (i1=i58;i1<N8;i1++)
         {
          i2=sorted8a[i1];

          p = &Arcs8[i2];

          i12 = p -> upper;
          i13 = p -> lower;

          if (p -> Value < p -> Cap - eps1)
            {
             q = &Vertices8[p -> end];

             q -> marked    = true;
             q -> upper     = i2;
             q -> direction = true;
            }

          Arcs8[i12].lower=i13;
          Arcs8[i13].upper=i12;
         }

       i1 =NArcs8-1;

       j4=true;

       j5=true;

       do
         {
          p = &Arcs8[i1];
          i12 = p -> upper;

          if (Vertices8[p -> start].marked==true)
            {
             if (p -> Value < p -> Cap - eps1)
               {
                q = &Vertices8[p -> end];

                q -> marked    = true;
                q -> upper     = i1;
                q -> direction = true;

                j4=false;
               }

             i13 = p -> lower;
             Arcs8[i12].lower=i13;
             Arcs8[i13].upper=i12;
            }

          if (i1<i12)
          j5=false;

          i1=i12;
         }
       while (j5==true);

       if (j4==false)
         {
          j4=true;

          j5=true;

          do
            {
             j6=true;

             do
               {
                p = &Arcs8[i1];
                i12 = p -> upper;

                if (Vertices8[p -> start].marked==true)
                  {
                   if (p -> Value < p -> Cap - eps1)
                     {
                      q = &Vertices8[Drain8];

                      q -> marked    = true;
                      q -> upper     = i1;
                      q -> direction = true;
                     }

                   i13 = p -> lower;
                   Arcs8[i12].lower=i13;
                   Arcs8[i13].upper=i12;
                  }

                if (i12<i15b)
                j6=false;

                i1=i12;
               }
             while ((Vertices8[Drain8].marked==false) && (j6==true));

             if (Vertices8[Drain8].marked==false)
               {
                j6=true;

                do
                  {
                   p = &Arcs8[i1];
                   i12 = p -> upper;

                   if (Vertices8[p -> end].marked==true)
                     {
                      q = &Vertices8[p -> start];

                      if ((q -> marked == false) && (p -> Value > eps1))
                        {
                         q -> marked    = true;
                         q -> upper     = i1;
                         q -> direction = false;

                         j4=false;
                        }

                      i13 = p -> lower;
                      Arcs8[i12].lower=i13;
                      Arcs8[i13].upper=i12;
                     }

                   if (i1<i12)
                     {
                      if (j4==true)
                      j5=false;

                      j4=true;

                      j6=false;
                     }

                   i1=i12;
                  }
                while (j6==true);

                if (j5==true)
                  {
                   j6=true;

                   do
                     {
                      p = &Arcs8[i1];
                      i12 = p -> upper;

                      if (Vertices8[p -> start].marked==true)
                        {
                         q = &Vertices8[p -> end];

                         if ((q -> marked == false)
                          && (p -> Value < p -> Cap - eps1))
                           {
                            q -> marked    = true;
                            q -> upper     = i1;
                            q -> direction = true;

                            j4=false;
                           }

                         i13 = p -> lower;
                         Arcs8[i12].lower=i13;
                         Arcs8[i13].upper=i12;
                        }

                      if (i1<i12)
                        {
                         if (j4==true)
                         j5=false;

                         j4=true;

                         j6=false;
                        }

                      i1=i12;
                     }
                   while (j6==true);
                  }
               }
            }
          while ((Vertices8[Drain8].marked==false) && (j5==true));
         }

       if (Vertices8[Drain8].marked==true)
         {
          i1=Drain8;

          PossFlow=N+0.0;

          do
            {
             p = &Arcs8[Vertices8[i1].upper];

             if (Vertices8[i1].direction==true)
               {
                if (p -> Cap - p -> Value < PossFlow)

                PossFlow = p -> Cap - p -> Value;

                i1 = p -> start;
               }

             else
               {
                if (p -> Value < PossFlow)

                PossFlow = p -> Value;

                i1 = p -> end;
               }
            }
          while (i1!=Source8);

          i1=Drain8;

          do
            {
             p = &Arcs8[Vertices8[i1].upper];

             if (Vertices8[i1].direction==true)
               {
                p -> Value = p -> Value + PossFlow;

                if (p -> Value > p -> Cap - eps1)

                p -> Value = p -> Cap;

                i1 = p -> start;
               }

             else
               {
                p -> Value = p -> Value - PossFlow;

                if (p -> Value < eps1)

                p -> Value = 0.0;

                i1 = p -> end;
               }
            }
          while (i1!=Source8);

          Flow=Flow+PossFlow;
         }

       else
       j1=false;

       if (Flow>UpperBounce8a-eps2)
       j1=false;
      }
    while (j1==true);

    LowerBounce8=N+0.0;
    for (i1=0;i1<M;i1++)
    if (Arcs8[i15b+i1].Value<LowerBounce8)
    LowerBounce8=Arcs8[i15b+i1].Value;

    UpperBounce8a=Flow;
    UpperBounce8 =UpperBounce8a/(M+0.0);

    while (((LowerBounce7+eps3<UpperBounce7) || (LowerBounce8+eps3<UpperBounce8))
           && (LowerBounce7+5.0*eps3<UpperBounce8)
           && (UpperBounce7>LowerBounce8-3.0*eps3))
      {
       if ((LowerBounce7+eps3<UpperBounce7)
           && (LowerBounce7+5.0*eps3<UpperBounce8)
           && (UpperBounce7>LowerBounce8-3.0*eps3))
         {
          Flow=0.0;

          for (i1=0;i1<M;i1++)
          Votes[i1].Value=0.0;

          i9=0;

          for (i1=i57;i1<N7;i1++)
            {
             i2=sorted7[i1];

             d1=Value7[i2];

             for (i6=0;i6<M;i6++)
             Votes[i6].marked=Vote7[i2][i6];

             i16=0;
             for (i7=1;i7<Indiff7[i2].Value;i7++)
               {
                for (i6=i16;i6<M;i6++)
                if (Votes[i6].marked==false)
                  {
                   i8 =i6;
                   i16=i6;
                   i6 =M;
                  }

                p = &Arcs7[i9];

                if (d1 > 0.0)
                  {
                   for (i6=i8+1;i6<M;i6++)
                   if ((Votes[i6].marked==false)
                    && (Votes[i6].Value<Votes[i8].Value))
                   i8=i6;

                   d2 = UpperBounce7 - Votes[i8].Value;

                   if (d2 < eps1)
                   p -> Value = 0.0;
                   else
                     {
                      if (d2 > d1) d2 = d1 ;

                      p -> Value = d2;

                      if (p -> Cap - d2 < eps1)
                      p -> Value = p -> Cap;

                      d1=d1-d2;

                      if (d1<eps1)
                      d1=0.0;

                      Votes[i8].Value=Votes[i8].Value+d2;
                     }
                  }
                else
                p -> Value = 0.0;

                Votes[i8].marked=true;

                p -> end = i17a+i8;

                i9++;
               }

             p = &Arcs7[i9];

             p -> Value = p -> Cap - d1;

             i9++;
            }

          for (i1=0;i1<M;i1++)
            {
             p = &Arcs7[i9];

             p -> Value = Votes[i1].Value;
             p -> Cap   = UpperBounce7;

             if (p -> Cap - p -> Value < eps1)
             p -> Value = p -> Cap;

             Flow = Flow + p -> Value;

             i9++;
            }

          j1=true;

          if (Flow<=UpperBounce7a-eps2)
          do
            {
             for (i1=0;i1<NVertices7;i1++)
             Vertices7[i1].marked=false;

             Arcs7[0].upper=NArcs7-1;
             Arcs7[0].lower=1;

             for (i1=1;i1<NArcs7;i1++)
               {
                Arcs7[i1].upper=i1-1;
                Arcs7[i1].lower=i1+1;
               }

             Arcs7[NArcs7-1].lower=0;

             for (i1=i57;i1<N7;i1++)
               {
                i2=sorted7a[i1];

                p = &Arcs7[i2];

                i12 = p -> upper;
                i13 = p -> lower;

                if (p -> Value < p -> Cap - eps1)
                  {
                   q = &Vertices7[p -> end];

                   q -> marked    = true;
                   q -> upper     = i2;
                   q -> direction = true;
                  }

                Arcs7[i12].lower=i13;
                Arcs7[i13].upper=i12;
               }

             i1 =NArcs7-1;

             j4=true;

             j5=true;

             do
               {
                p = &Arcs7[i1];
                i12 = p -> upper;

                if (Vertices7[p -> start].marked==true)
                  {
                   if (p -> Value < p -> Cap - eps1)
                     {
                      q = &Vertices7[p -> end];

                      q -> marked    = true;
                      q -> upper     = i1;
                      q -> direction = true;

                      j4=false;
                     }

                   i13 = p -> lower;
                   Arcs7[i12].lower=i13;
                   Arcs7[i13].upper=i12;
                  }

                if (i1<i12)
                j5=false;

                i1=i12;
               }
             while (j5==true);

             if (j4==false)
               {
                j4=true;

                j5=true;

                do
                  {
                   j6=true;

                   do
                     {
                      p = &Arcs7[i1];
                      i12 = p -> upper;

                      if (Vertices7[p -> start].marked==true)
                        {
                         if (p -> Value < p -> Cap - eps1)
                           {
                            q = &Vertices7[Drain7];

                            q -> marked    = true;
                            q -> upper     = i1;
                            q -> direction = true;
                           }

                         i13 = p -> lower;
                         Arcs7[i12].lower=i13;
                         Arcs7[i13].upper=i12;
                        }

                      if (i12<i15a)
                      j6=false;

                      i1=i12;
                     }
                   while ((Vertices7[Drain7].marked==false) && (j6==true));

                   if (Vertices7[Drain7].marked==false)
                     {
                      j6=true;

                      do
                        {
                         p = &Arcs7[i1];
                         i12 = p -> upper;

                         if (Vertices7[p -> end].marked==true)
                           {
                            q = &Vertices7[p -> start];

                            if ((q -> marked == false) && (p -> Value > eps1))
                              {
                               q -> marked    = true;
                               q -> upper     = i1;
                               q -> direction = false;

                               j4=false;
                              }

                            i13 = p -> lower;
                            Arcs7[i12].lower=i13;
                            Arcs7[i13].upper=i12;
                           }

                         if (i1<i12)
                           {
                            if (j4==true)
                            j5=false;

                            j4=true;

                            j6=false;
                           }

                         i1=i12;
                        }
                      while (j6==true);

                      if (j5==true)
                        {
                         j6=true;

                         do
                           {
                            p = &Arcs7[i1];
                            i12 = p -> upper;

                            if (Vertices7[p -> start].marked==true)
                              {
                               q = &Vertices7[p -> end];

                               if ((q -> marked == false)
                                && (p -> Value < p -> Cap - eps1))
                                 {
                                  q -> marked    = true;
                                  q -> upper     = i1;
                                  q -> direction = true;

                                  j4=false;
                                 }

                               i13 = p -> lower;
                               Arcs7[i12].lower=i13;
                               Arcs7[i13].upper=i12;
                              }

                            if (i1<i12)
                              {
                               if (j4==true)
                               j5=false;

                               j4=true;

                               j6=false;
                              }

                            i1=i12;
                           }
                         while (j6==true);
                        }
                     }
                  }
                while ((Vertices7[Drain7].marked==false) && (j5==true));
               }

             if (Vertices7[Drain7].marked==true)
               {
                i1=Drain7;

                PossFlow=N+0.0;

                do
                  {
                   p = &Arcs7[Vertices7[i1].upper];

                   if (Vertices7[i1].direction==true)
                     {
                      if (p -> Cap - p -> Value < PossFlow)

                      PossFlow = p -> Cap - p -> Value;

                      i1 = p -> start;
                     }

                   else
                     {
                      if (p -> Value < PossFlow)

                      PossFlow = p -> Value;

                      i1 = p -> end;
                     }
                  }
                while (i1!=Source7);

                i1=Drain7;

                do
                  {
                   p = &Arcs7[Vertices7[i1].upper];

                   if (Vertices7[i1].direction==true)
                     {
                      p -> Value = p -> Value + PossFlow;

                      if (p -> Value > p -> Cap - eps1)

                      p -> Value = p -> Cap;

                      i1 = p -> start;
                     }

                   else
                     {
                      p -> Value = p -> Value - PossFlow;

                      if (p -> Value < eps1)

                      p -> Value = 0.0;

                      i1 = p -> end;
                     }
                  }
                while (i1!=Source7);

                Flow=Flow+PossFlow;
               }

             else
             j1=false;

             if (Flow>UpperBounce7a-eps2)
             j1=false;
            }
          while (j1==true);

          LowerBounce7=N+0.0;
          for (i1=0;i1<M;i1++)
          if (Arcs7[i15a+i1].Value<LowerBounce7)
          LowerBounce7=Arcs7[i15a+i1].Value;

          UpperBounce7a=Flow;
          UpperBounce7 =UpperBounce7a/(M+0.0);
         }

       if ((LowerBounce8+eps3<UpperBounce8)
           && (LowerBounce7+5.0*eps3<UpperBounce8)
           && (UpperBounce7>LowerBounce8-3.0*eps3))
         {
          Flow=0.0;

          for (i1=0;i1<M;i1++)
          Votes[i1].Value=0.0;

          i9=0;

          for (i1=i58;i1<N8;i1++)
            {
             i2=sorted8[i1];

             d1=Value8[i2];

             for (i6=0;i6<M;i6++)
             Votes[i6].marked=Vote8[i2][i6];

             i16=0;
             for (i7=1;i7<Indiff8[i2].Value;i7++)
               {
                for (i6=i16;i6<M;i6++)
                if (Votes[i6].marked==false)
                  {
                   i8 =i6;
                   i16=i6;
                   i6 =M;
                  }

                p = &Arcs8[i9];

                if (d1 > 0.0)
                  {
                   for (i6=i8+1;i6<M;i6++)
                   if ((Votes[i6].marked==false)
                    && (Votes[i6].Value<Votes[i8].Value))
                   i8=i6;

                   d2 = UpperBounce8 - Votes[i8].Value;

                   if (d2 < eps1)
                   p -> Value = 0.0;
                   else
                     {
                      if (d2 > d1) d2 = d1 ;

                      p -> Value = d2;

                      if (p -> Cap - d2 < eps1)
                      p -> Value = p -> Cap;

                      d1=d1-d2;

                      if (d1<eps1)
                      d1=0.0;

                      Votes[i8].Value=Votes[i8].Value+d2;
                     }
                  }
                else
                p -> Value = 0.0;

                Votes[i8].marked=true;

                p -> end = i17b+i8;

                i9++;
               }

             p = &Arcs8[i9];

             p -> Value = p -> Cap - d1;

             i9++;
            }

          for (i1=0;i1<M;i1++)
            {
             p = &Arcs8[i9];

             p -> Value = Votes[i1].Value;
             p -> Cap = UpperBounce8;

             if (p -> Cap - p -> Value < eps1)
             p -> Value = p -> Cap;

             Flow = Flow + p -> Value;

             i9++;
            }

          j1=true;

          if (Flow<=UpperBounce8a-eps2)
          do
            {
             for (i1=0;i1<NVertices8;i1++)
             Vertices8[i1].marked=false;

             Arcs8[0].upper=NArcs8-1;
             Arcs8[0].lower=1;

             for (i1=1;i1<NArcs8;i1++)
               {
                Arcs8[i1].upper=i1-1;
                Arcs8[i1].lower=i1+1;
               }

             Arcs8[NArcs8-1].lower=0;

             for (i1=i58;i1<N8;i1++)
               {
                i2=sorted8a[i1];

                p = &Arcs8[i2];

                i12 = p -> upper;
                i13 = p -> lower;

                if (p -> Value < p -> Cap - eps1)
                  {
                   q = &Vertices8[p -> end];

                   q -> marked    = true;
                   q -> upper     = i2;
                   q -> direction = true;
                  }

                Arcs8[i12].lower=i13;
                Arcs8[i13].upper=i12;
               }

             i1 =NArcs8-1;

             j4=true;

             j5=true;

             do
               {
                p = &Arcs8[i1];
                i12 = p -> upper;

                if (Vertices8[p -> start].marked==true)
                  {
                   if (p -> Value < p -> Cap - eps1)
                     {
                      q = &Vertices8[p -> end];

                      q -> marked    = true;
                      q -> upper     = i1;
                      q -> direction = true;

                      j4=false;
                     }

                   i13 = p -> lower;
                   Arcs8[i12].lower=i13;
                   Arcs8[i13].upper=i12;
                  }

                if (i1<i12)
                j5=false;

                i1=i12;
               }
             while (j5==true);

             if (j4==false)
               {
                j4=true;

                j5=true;

                do
                  {
                   j6=true;

                   do
                     {
                      p = &Arcs8[i1];
                      i12 = p -> upper;

                      if (Vertices8[p -> start].marked==true)
                        {
                         if (p -> Value < p -> Cap - eps1)
                           {
                            q = &Vertices8[Drain8];

                            q -> marked    = true;
                            q -> upper     = i1;
                            q -> direction = true;
                           }

                         i13 = p -> lower;
                         Arcs8[i12].lower=i13;
                         Arcs8[i13].upper=i12;
                        }

                      if (i12<i15b)
                      j6=false;

                      i1=i12;
                     }
                   while ((Vertices8[Drain8].marked==false) && (j6==true));

                   if (Vertices8[Drain8].marked==false)
                     {
                      j6=true;

                      do
                        {
                         p = &Arcs8[i1];
                         i12 = p -> upper;

                         if (Vertices8[p -> end].marked==true)
                           {
                            q = &Vertices8[p -> start];

                            if ((q -> marked == false) && (p -> Value > eps1))
                              {
                               q -> marked    = true;
                               q -> upper     = i1;
                               q -> direction = false;

                               j4=false;
                              }

                            i13 = p -> lower;
                            Arcs8[i12].lower=i13;
                            Arcs8[i13].upper=i12;
                           }

                         if (i1<i12)
                           {
                            if (j4==true)
                            j5=false;

                            j4=true;

                            j6=false;
                           }

                         i1=i12;
                        }
                      while (j6==true);

                      if (j5==true)
                        {
                         j6=true;

                         do
                           {
                            p = &Arcs8[i1];
                            i12 = p -> upper;

                            if (Vertices8[p -> start].marked==true)
                              {
                               q = &Vertices8[p -> end];

                               if ((q -> marked == false)
                                && (p -> Value < p -> Cap - eps1))
                                 {
                                  q -> marked    = true;
                                  q -> upper     = i1;
                                  q -> direction = true;

                                  j4=false;
                                 }

                               i13 = p -> lower;
                               Arcs8[i12].lower=i13;
                               Arcs8[i13].upper=i12;
                              }

                            if (i1<i12)
                              {
                               if (j4==true)
                               j5=false;

                               j4=true;

                               j6=false;
                              }

                            i1=i12;
                           }
                         while (j6==true);
                        }
                     }
                  }
                while ((Vertices8[Drain8].marked==false) && (j5==true));
               }

             if (Vertices8[Drain8].marked==true)
               {
                i1=Drain8;

                PossFlow=N+0.0;

                do
                  {
                   p = &Arcs8[Vertices8[i1].upper];

                   if (Vertices8[i1].direction==true)
                     {
                      if (p -> Cap - p -> Value < PossFlow)

                      PossFlow = p -> Cap - p -> Value;

                      i1 = p -> start;
                     }

                   else
                     {
                      if (p -> Value < PossFlow)

                      PossFlow = p -> Value;

                      i1 = p -> end;
                     }
                  }
                while (i1!=Source8);

                i1=Drain8;

                do
                  {
                   p = &Arcs8[Vertices8[i1].upper];

                   if (Vertices8[i1].direction==true)
                     {
                      p -> Value = p -> Value + PossFlow;

                      if (p -> Value > p -> Cap - eps1)

                      p -> Value = p -> Cap;

                      i1 = p -> start;
                     }

                   else
                     {
                      p -> Value = p -> Value - PossFlow;

                      if (p -> Value < eps1)

                      p -> Value = 0.0;

                      i1 = p -> end;
                     }
                  }
                while (i1!=Source8);

                Flow=Flow+PossFlow;
               }

             else
             j1=false;

             if (Flow>UpperBounce8a-eps2)
             j1=false;
            }
          while (j1==true);

          LowerBounce8=N+0.0;
          for (i1=0;i1<M;i1++)
          if (Arcs8[i15b+i1].Value<LowerBounce8)
          LowerBounce8=Arcs8[i15b+i1].Value;

          UpperBounce8a=Flow;
          UpperBounce8 =UpperBounce8a/(M+0.0);
         }
      }

    if (LowerBounce7+5.0*eps3>=UpperBounce8)
      {
       bool1=true;
       Output1=(LowerBounce7+UpperBounce8)/2.0;
      }
   }
 else
   {
    bool1=true;
    Output1=0.0;
   }

 d99=UpperBounce8a;
}

/*******************************************************************************/

void EKarpFinal()
{
 unsigned int i1,i2,i3,i48,i58,i6,i7,i8,i9,i12,i13,i15b,i16,i17b;
 unsigned int NArcs8,NVertices8,Source8,Drain8,IndiffMax;
 bool j1,j3,j4,j5,j6;
 double Flow,PossFlow,d1,d2;
 double LowerBounce8,UpperBounce8,UpperBounce8a;
 struct ArcElement *p;
 struct VertexElement *q;

 if (N4>Length5)
   {
    delete(Vote7);
    delete(Vote8);
    delete(Value7);
    delete(Value8);
    delete(Indiff7);
    delete(Indiff8);
    delete(sorted7);
    delete(sorted7a);
    delete(sorted8);
    delete(sorted8a);

    Length5=2*N4;

    Vote7=new bool*[Length5];
    Vote8=new bool*[Length5];

    for (i1=0;i1<Length5;i1++)
      {
       Vote7[i1]=new bool[C];
       Vote8[i1]=new bool[C];
      }

    Value7  =new double              [Length5];
    Value8  =new double              [Length5];
    Indiff7 =new struct IndiffElement[Length5];
    Indiff8 =new struct IndiffElement[Length5];
    sorted7 =new unsigned int        [Length5];
    sorted7a=new unsigned int        [Length5];
    sorted8 =new unsigned int        [Length5];
    sorted8a=new unsigned int        [Length5];
   }

 NArcs8=M;
 LowerBounce8 =0.0;
 UpperBounce8a=N+0.0;

 for (i1=0;i1<M;i1++)
 Votes1[i1]=false;

 i58=0;
 for (i1=0;i1<N4;i1++)
   {
    i2=1;

    for (i3=0;i3<M;i3++)
    if (Vote4[i1][i3]==-1)
      {
       i2++;
       Votes1[i3]=true;
      }

    Indiff8[i1].Value=i2;

    if (i2==1)
      {
       i48=i1;
       i58=1;
       UpperBounce8a=UpperBounce8a-Value4[i1];
      }
    else
    NArcs8=NArcs8+i2;
   }

 if (UpperBounce8a>d99)
 UpperBounce8a=d99;

 j3=true;
 for (i1=0;i1<M;i1++)
 if (Votes1[i1]==false)
 j3=false;

 if (j3==true)
   {
    if (NArcs8>Length1)
      {
       delete(Arcs7);
       delete(Arcs8);

       Length1=2*NArcs8;

       Arcs7=new struct ArcElement[Length1];
       Arcs8=new struct ArcElement[Length1];
      }

    NVertices8=N4-i58+2+M;
    if (NVertices8>Length2)
      {
       delete(Vertices7);
       delete(Vertices8);

       Length2=2*NVertices8;

       Vertices7=new struct VertexElement[Length2];
       Vertices8=new struct VertexElement[Length2];
      }

    Source8 = N4-i58;
    Drain8  = Source8+M+1;

    UpperBounce8=UpperBounce8a/(M+0.0);

    Flow=0.0;

    for (i1=0;i1<M;i1++)
    Votes[i1].Value=0.0;

    Indiff8[0].upper=N4-1;
    Indiff8[0].lower=1;

    for (i1=1;i1<N4;i1++)
      {
       Indiff8[i1].upper=i1-1;
       Indiff8[i1].lower=i1+1;
      }

    Indiff8[N4-1].lower=0;

    if (i58==1)
      {
       i12=Indiff8[i48].upper;
       i13=Indiff8[i48].lower;

       Indiff8[i12].lower=i13;
       Indiff8[i13].upper=i12;
      }

    i9=0;

    IndiffMax=2;

    i1=0;
    i2=0;

    if (i58==1)
      {
       i1=1;

       if (i48==0)
       i2=1;
      }

    while (i1<N4)
      {
       i13=Indiff8[i2].lower;

       if (Indiff8[i2].Value==IndiffMax)
         {
          sorted8[i1]=i2;
          i1++;

          i12=Indiff8[i2].upper;

          Indiff8[i12].lower=i13;
          Indiff8[i13].upper=i12;
         }

       if (i2>=i13)
       IndiffMax++;

       i2=i13;
      }

    i17b=N4+1-i58;
    for (i1=i58;i1<N4;i1++)
      {
       i2=sorted8[i1];

       d1=Value4[i2];

       for (i6=0;i6<M;i6++)
         {
          if (Vote4[i2][i6]==-1)
          Votes[i6].marked=false;
          else
          Votes[i6].marked=true;
         }

       i16=0;
       for (i7=1;i7<Indiff8[i2].Value;i7++)
         {
          for (i6=i16;i6<M;i6++)
          if (Votes[i6].marked==false)
            {
             i8 =i6;
             i16=i6;
             i6 =M;
            }

          p = &Arcs8[i9];

          p -> Cap = Value4[i2];

          if (d1 > 0.0)
            {
             for (i6=i8+1;i6<M;i6++)
             if ((Votes[i6].marked==false)
              && (Votes[i6].Value<Votes[i8].Value))
             i8=i6;

             d2 = UpperBounce8 - Votes[i8].Value;

             if (d2 < eps1)
             p -> Value = 0.0;
             else
               {
                if (d2 > d1) d2 = d1 ;

                p -> Value = d2;

                if (p -> Cap - d2 < eps1)
                p -> Value = p -> Cap;

                d1=d1-d2;

                if (d1<eps1)
                d1=0.0;

                Votes[i8].Value=Votes[i8].Value+d2;
               }
            }
          else
          p -> Value = 0.0;

          Votes[i8].marked=true;

          if (i58==1)
            {
             if (i2<i48)
             p -> start = i2;
             else
             p -> start = i2-1;
            }
          else
          p -> start = i2;

          p -> end   = i17b+i8;

          i9++;
         }

       sorted8a[i1]=i9;

       p = &Arcs8[i9];

       p -> start = Source8;
       p -> Cap   = Value4[i2];
       p -> Value = p -> Cap - d1;

       if (i58==1)
         {
          if (i2<i48)
          p -> end = i2;
          else
          p -> end = i2-1;
         }
       else
       p -> end = i2;

       i9++;
      }

    for (i1=0;i1<M;i1++)
      {
       p = &Arcs8[i9];

       p -> start = i17b+i1;
       p -> end   = Drain8;
       p -> Value = Votes[i1].Value;
       p -> Cap   = UpperBounce8;

       if (p -> Cap - p -> Value < eps1)
       p -> Value = p -> Cap;

       Flow = Flow + p -> Value;

       i9++;
      }

    j1=true;

    i15b=NArcs8-M;

    if (Flow<=UpperBounce8a-eps2)
    do
      {
       for (i1=0;i1<NVertices8;i1++)
       Vertices8[i1].marked=false;

       Arcs8[0].upper=NArcs8-1;
       Arcs8[0].lower=1;

       for (i1=1;i1<NArcs8;i1++)
         {
          Arcs8[i1].upper=i1-1;
          Arcs8[i1].lower=i1+1;
         }

       Arcs8[NArcs8-1].lower=0;

       for (i1=i58;i1<N4;i1++)
         {
          i2=sorted8a[i1];

          p = &Arcs8[i2];

          i12 = p -> upper;
          i13 = p -> lower;

          if (p -> Value < p -> Cap - eps1)
            {
             q = &Vertices8[p -> end];

             q -> marked    = true;
             q -> upper     = i2;
             q -> direction = true;
            }

          Arcs8[i12].lower=i13;
          Arcs8[i13].upper=i12;
         }

       i1 =NArcs8-1;

       j4=true;

       j5=true;

       do
         {
          p = &Arcs8[i1];
          i12 = p -> upper;

          if (Vertices8[p -> start].marked==true)
            {
             if (p -> Value < p -> Cap - eps1)
               {
                q = &Vertices8[p -> end];

                q -> marked    = true;
                q -> upper     = i1;
                q -> direction = true;

                j4=false;
               }

             i13 = p -> lower;
             Arcs8[i12].lower=i13;
             Arcs8[i13].upper=i12;
            }

          if (i1<i12)
          j5=false;

          i1=i12;
         }
       while (j5==true);

       if (j4==false)
         {
          j4=true;

          j5=true;

          do
            {
             j6=true;

             do
               {
                p = &Arcs8[i1];
                i12 = p -> upper;

                if (Vertices8[p -> start].marked==true)
                  {
                   if (p -> Value < p -> Cap - eps1)
                     {
                      q = &Vertices8[Drain8];

                      q -> marked    = true;
                      q -> upper     = i1;
                      q -> direction = true;
                     }

                   i13 = p -> lower;
                   Arcs8[i12].lower=i13;
                   Arcs8[i13].upper=i12;
                  }

                if (i12<i15b)
                j6=false;

                i1=i12;
               }
             while ((Vertices8[Drain8].marked==false) && (j6==true));

             if (Vertices8[Drain8].marked==false)
               {
                j6=true;

                do
                  {
                   p = &Arcs8[i1];
                   i12 = p -> upper;

                   if (Vertices8[p -> end].marked==true)
                     {
                      q = &Vertices8[p -> start];

                      if ((q -> marked == false) && (p -> Value > eps1))
                        {
                         q -> marked    = true;
                         q -> upper     = i1;
                         q -> direction = false;

                         j4=false;
                        }

                      i13 = p -> lower;
                      Arcs8[i12].lower=i13;
                      Arcs8[i13].upper=i12;
                     }

                   if (i1<i12)
                     {
                      if (j4==true)
                      j5=false;

                      j4=true;

                      j6=false;
                     }

                   i1=i12;
                  }
                while (j6==true);

                if (j5==true)
                  {
                   j6=true;

                   do
                     {
                      p = &Arcs8[i1];
                      i12 = p -> upper;

                      if (Vertices8[p -> start].marked==true)
                        {
                         q = &Vertices8[p -> end];

                         if ((q -> marked == false)
                          && (p -> Value < p -> Cap - eps1))
                           {
                            q -> marked    = true;
                            q -> upper     = i1;
                            q -> direction = true;

                            j4=false;
                           }

                         i13 = p -> lower;
                         Arcs8[i12].lower=i13;
                         Arcs8[i13].upper=i12;
                        }

                      if (i1<i12)
                        {
                         if (j4==true)
                         j5=false;

                         j4=true;

                         j6=false;
                        }

                      i1=i12;
                     }
                   while (j6==true);
                  }
               }
            }
          while ((Vertices8[Drain8].marked==false) && (j5==true));
         }

       if (Vertices8[Drain8].marked==true)
         {
          i1=Drain8;

          PossFlow=N+0.0;

          do
            {
             p = &Arcs8[Vertices8[i1].upper];

             if (Vertices8[i1].direction==true)
               {
                if (p -> Cap - p -> Value < PossFlow)

                PossFlow = p -> Cap - p -> Value;

                i1 = p -> start;
               }

             else
               {
                if (p -> Value < PossFlow)

                PossFlow = p -> Value;

                i1 = p -> end;
               }
            }
          while (i1!=Source8);

          i1=Drain8;

          do
            {
             p = &Arcs8[Vertices8[i1].upper];

             if (Vertices8[i1].direction==true)
               {
                p -> Value = p -> Value + PossFlow;

                if (p -> Value > p -> Cap - eps1)

                p -> Value = p -> Cap;

                i1 = p -> start;
               }

             else
               {
                p -> Value = p -> Value - PossFlow;

                if (p -> Value < eps1)

                p -> Value = 0.0;

                i1 = p -> end;
               }
            }
          while (i1!=Source8);

          Flow=Flow+PossFlow;
         }

       else
       j1=false;

       if (Flow>UpperBounce8a-eps2)
       j1=false;
      }
    while (j1==true);

    LowerBounce8=N+0.0;
    for (i1=0;i1<M;i1++)
    if (Arcs8[i15b+i1].Value<LowerBounce8)
    LowerBounce8=Arcs8[i15b+i1].Value;

    UpperBounce8a=Flow;
    UpperBounce8 =UpperBounce8a/(M+0.0);

    while (LowerBounce8+eps3<UpperBounce8)
      {
       Flow=0.0;

       for (i1=0;i1<M;i1++)
       Votes[i1].Value=0.0;

       i9=0;

       for (i1=i58;i1<N4;i1++)
         {
          i2=sorted8[i1];

          d1=Value4[i2];

          for (i6=0;i6<M;i6++)
            {
             if (Vote4[i2][i6]==-1)
             Votes[i6].marked=false;
             else
             Votes[i6].marked=true;
            }

          i16=0;
          for (i7=1;i7<Indiff8[i2].Value;i7++)
            {
             for (i6=i16;i6<M;i6++)
             if (Votes[i6].marked==false)
               {
                i8 =i6;
                i16=i6;
                i6 =M;
               }

             p = &Arcs8[i9];

             if (d1 > 0.0)
               {
                for (i6=i8+1;i6<M;i6++)
                if ((Votes[i6].marked==false)
                 && (Votes[i6].Value<Votes[i8].Value))
                i8=i6;

                d2 = UpperBounce8 - Votes[i8].Value;

                if (d2 < eps1)
                p -> Value = 0.0;
                else
                  {
                   if (d2 > d1) d2 = d1 ;

                   p -> Value = d2;

                   if (p -> Cap - d2 < eps1)
                   p -> Value = p -> Cap;

                   d1=d1-d2;

                   if (d1<eps1)
                   d1=0.0;

                   Votes[i8].Value=Votes[i8].Value+d2;
                  }
               }
             else
             p -> Value = 0.0;

             Votes[i8].marked=true;

             p -> end = i17b+i8;

             i9++;
            }

          p = &Arcs8[i9];

          p -> Value = p -> Cap - d1;

          i9++;
         }

       for (i1=0;i1<M;i1++)
         {
          p = &Arcs8[i9];

          p -> Value = Votes[i1].Value;
          p -> Cap   = UpperBounce8;

          if (p -> Cap - p -> Value < eps1)
          p -> Value = p -> Cap;

          Flow = Flow + p -> Value;

          i9++;
         }

       j1=true;

       if (Flow<=UpperBounce8a-eps2)
       do
         {
          for (i1=0;i1<NVertices8;i1++)
          Vertices8[i1].marked=false;

          Arcs8[0].upper=NArcs8-1;
          Arcs8[0].lower=1;

          for (i1=1;i1<NArcs8;i1++)
            {
             Arcs8[i1].upper=i1-1;
             Arcs8[i1].lower=i1+1;
            }

          Arcs8[NArcs8-1].lower=0;

          for (i1=i58;i1<N4;i1++)
            {
             i2=sorted8a[i1];

             p = &Arcs8[i2];

             i12 = p -> upper;
             i13 = p -> lower;

             if (p -> Value < p -> Cap - eps1)
               {
                q = &Vertices8[p -> end];

                q -> marked    = true;
                q -> upper     = i2;
                q -> direction = true;
               }

             Arcs8[i12].lower=i13;
             Arcs8[i13].upper=i12;
            }

          i1 =NArcs8-1;

          j4=true;

          j5=true;

          do
            {
             p = &Arcs8[i1];
             i12 = p -> upper;

             if (Vertices8[p -> start].marked==true)
               {
                if (p -> Value < p -> Cap - eps1)
                  {
                   q = &Vertices8[p -> end];

                   q -> marked    = true;
                   q -> upper     = i1;
                   q -> direction = true;

                   j4=false;
                  }

                i13 = p -> lower;
                Arcs8[i12].lower=i13;
                Arcs8[i13].upper=i12;
               }

             if (i1<i12)
             j5=false;

             i1=i12;
            }
          while (j5==true);

          if (j4==false)
            {
             j4=true;

             j5=true;

             do
               {
                j6=true;

                do
                  {
                   p = &Arcs8[i1];
                   i12 = p -> upper;

                   if (Vertices8[p -> start].marked==true)
                     {
                      if (p -> Value < p -> Cap - eps1)
                        {
                         q = &Vertices8[Drain8];

                         q -> marked    = true;
                         q -> upper     = i1;
                         q -> direction = true;
                        }

                      i13 = p -> lower;
                      Arcs8[i12].lower=i13;
                      Arcs8[i13].upper=i12;
                     }

                   if (i12<i15b)
                   j6=false;

                   i1=i12;
                  }
                while ((Vertices8[Drain8].marked==false) && (j6==true));

                if (Vertices8[Drain8].marked==false)
                  {
                   j6=true;

                   do
                     {
                      p = &Arcs8[i1];
                      i12 = p -> upper;

                      if (Vertices8[p -> end].marked==true)
                        {
                         q = &Vertices8[p -> start];

                         if ((q -> marked == false) && (p -> Value > eps1))
                           {
                            q -> marked    = true;
                            q -> upper     = i1;
                            q -> direction = false;

                            j4=false;
                           }

                         i13 = p -> lower;
                         Arcs8[i12].lower=i13;
                         Arcs8[i13].upper=i12;
                        }

                      if (i1<i12)
                        {
                         if (j4==true)
                         j5=false;

                         j4=true;

                         j6=false;
                        }

                      i1=i12;
                     }
                   while (j6==true);

                   if (j5==true)
                     {
                      j6=true;

                      do
                        {
                         p = &Arcs8[i1];
                         i12 = p -> upper;

                         if (Vertices8[p -> start].marked==true)
                           {
                            q = &Vertices8[p -> end];

                            if ((q -> marked == false)
                             && (p -> Value < p -> Cap - eps1))
                              {
                               q -> marked    = true;
                               q -> upper     = i1;
                               q -> direction = true;

                               j4=false;
                              }

                            i13 = p -> lower;
                            Arcs8[i12].lower=i13;
                            Arcs8[i13].upper=i12;
                           }

                         if (i1<i12)
                           {
                            if (j4==true)
                            j5=false;

                            j4=true;

                            j6=false;
                           }

                         i1=i12;
                        }
                      while (j6==true);
                     }
                  }
               }
             while ((Vertices8[Drain8].marked==false) && (j5==true));
            }

          if (Vertices8[Drain8].marked==true)
            {
             i1=Drain8;

             PossFlow=N+0.0;

             do
               {
                p = &Arcs8[Vertices8[i1].upper];

                if (Vertices8[i1].direction==true)
                  {
                   if (p -> Cap - p -> Value < PossFlow)

                   PossFlow = p -> Cap - p -> Value;

                   i1 = p -> start;
                  }

                else
                  {
                   if (p -> Value < PossFlow)

                   PossFlow = p -> Value;

                   i1 = p -> end;
                  }
               }
             while (i1!=Source8);

             i1=Drain8;

             do
               {
                p = &Arcs8[Vertices8[i1].upper];

                if (Vertices8[i1].direction==true)
                  {
                   p -> Value = p -> Value + PossFlow;

                   if (p -> Value > p -> Cap - eps1)

                   p -> Value = p -> Cap;

                   i1 = p -> start;
                  }

                else
                  {
                   p -> Value = p -> Value - PossFlow;

                   if (p -> Value < eps1)

                   p -> Value = 0.0;

                   i1 = p -> end;
                  }
               }
             while (i1!=Source8);

             Flow=Flow+PossFlow;
            }

          else
          j1=false;

          if (Flow>UpperBounce8a-eps2)
          j1=false;
         }
       while (j1==true);

       LowerBounce8=N+0.0;
       for (i1=0;i1<M;i1++)
       if (Arcs8[i15b+i1].Value<LowerBounce8)
       LowerBounce8=Arcs8[i15b+i1].Value;

       UpperBounce8a=Flow;
       UpperBounce8 =UpperBounce8a/(M+0.0);
      }

    Output1=(LowerBounce8+UpperBounce8)/2.0;
   }
 else
 Output1=0.0;
}

/*******************************************************************************/

void EKarpSimple()
{
 unsigned int i1,i2,i3,i4,i5,i6,i7,i8,i9,i12,i13,i15,i16,i17;
 unsigned int N8,NArcs8,NVertices8,Source8,Drain8,IndiffMax;
 bool j1,j2,j4,j5,j6;
 double Flow,PossFlow,MaxPossFlow,UpperBounce8,d1,d2;
 struct ArcElement *p;
 struct VertexElement *q;

 NArcs8=M;
 Flow=0.0;
 MaxPossFlow=N+0.0;
 i5=0;

 if (N4>Length5)
   {
    delete(Vote7);
    delete(Vote8);
    delete(Value7);
    delete(Value8);
    delete(Indiff7);
    delete(Indiff8);
    delete(sorted7);
    delete(sorted7a);
    delete(sorted8);
    delete(sorted8a);

    Length5=2*N4;

    Vote7=new bool*[Length5];
    Vote8=new bool*[Length5];

    for (i1=0;i1<Length5;i1++)
      {
       Vote7[i1]=new bool[C];
       Vote8[i1]=new bool[C];
      }

    Value7  =new double              [Length5];
    Value8  =new double              [Length5];
    Indiff7 =new struct IndiffElement[Length5];
    Indiff8 =new struct IndiffElement[Length5];
    sorted7 =new unsigned int        [Length5];
    sorted7a=new unsigned int        [Length5];
    sorted8 =new unsigned int        [Length5];
    sorted8a=new unsigned int        [Length5];
   }

 for (i1=0;i1<M;i1++)
   {
    if (Vote4[0][i1]==-1)
    Vote8[0][i1]=false;
    else
    Vote8[0][i1]=true;
   }

 Value8[0]=Value4[0];

 N8=1;

 for (i1=1;i1<N4;i1++)
   {
    i2=0;
    j1=false;

    while (i2<N8)
      {
       j1=true;

       for (i3=0;i3<M;i3++)
       if (((Vote8[i2][i3]==false) && (Vote4[i1][i3]!=-1))
        || ((Vote8[i2][i3]==true ) && (Vote4[i1][i3]==-1)))
         {
          j1=false;
          i3=M;
         }

       if (j1==true)
         {
          Value8[i2]=Value8[i2]+Value4[i1];
          i2=N8;
         }
       else
       i2++;
      }

    if (j1==false)
      {
       for (i3=0;i3<M;i3++)
         {
          if (Vote4[i1][i3]==-1)
          Vote8[N8][i3]=false;
          else
          Vote8[N8][i3]=true;
         }

       Value8[N8]=Value4[i1];

       N8++;
      }
   }

 for (i1=0;i1<M;i1++)
 Votes1[i1]=false;

 for (i1=0;i1<N8;i1++)
   {
    i2=1;

    for (i3=0;i3<M;i3++)
    if (Vote8[i1][i3]==false)
      {
       i2++;
       Votes1[i3]=true;
      }

    Indiff8[i1].Value=i2;

    if (i2==1)
      {
       i4=i1;
       i5=1;
       MaxPossFlow=MaxPossFlow-Value8[i1];
      }
    else
    NArcs8=NArcs8+i2;
   }

 j2=true;
 for (i1=0;i1<M;i1++)
 if (Votes1[i1]==false)
 j2=false;

 if (j2==true)
   {
    for (i1=0;i1<M;i1++)
    Votes[i1].Value=0.0;

    if (NArcs8>Length1)
      {
       delete(Arcs7);
       delete(Arcs8);

       Length1=2*NArcs8;

       Arcs7=new struct ArcElement[Length1];
       Arcs8=new struct ArcElement[Length1];
      }

    NVertices8=N8-i5+2+M;
    if (NVertices8>Length2)
      {
       delete(Vertices7);
       delete(Vertices8);

       Length2=2*NVertices8;

       Vertices7=new struct VertexElement[Length2];
       Vertices8=new struct VertexElement[Length2];
      }

    Source8 = N8-i5;
    Drain8  = Source8+M+1;
    UpperBounce8=MaxPossFlow/(M+0.0);

    Indiff8[0].upper=N8-1;
    Indiff8[0].lower=1;

    for (i1=1;i1<N8;i1++)
      {
       Indiff8[i1].upper=i1-1;
       Indiff8[i1].lower=i1+1;
      }

    Indiff8[N8-1].lower=0;

    if (i5==1)
      {
       i12=Indiff8[i4].upper;
       i13=Indiff8[i4].lower;

       Indiff8[i12].lower=i13;
       Indiff8[i13].upper=i12;
      }

    i9=0;

    IndiffMax=2;

    i1=0;
    i2=0;

    if (i5==1)
      {
       i1=1;

       if (i4==0)
       i2=1;
      }

    while (i1<N8)
      {
       i13=Indiff8[i2].lower;

       if (Indiff8[i2].Value==IndiffMax)
         {
          sorted8[i1]=i2;
          i1++;

          i12=Indiff8[i2].upper;

          Indiff8[i12].lower=i13;
          Indiff8[i13].upper=i12;
         }

       if (i2>=i13)
       IndiffMax++;

       i2=i13;
      }

    i17=N8+1-i5;
    for (i1=i5;i1<N8;i1++)
      {
       i2=sorted8[i1];

       d1=Value8[i2];

       for (i6=0;i6<M;i6++)
       Votes[i6].marked=Vote8[i2][i6];

       i16=0;
       for (i7=1;i7<Indiff8[i2].Value;i7++)
         {
          for (i6=i16;i6<M;i6++)
          if (Votes[i6].marked==false)
            {
             i8 =i6;
             i16=i6;
             i6 =M;
            }

          p = &Arcs8[i9];

          p -> Cap = Value8[i2];

          if (d1 > 0.0)
            {
             for (i6=i8+1;i6<M;i6++)
             if ((Votes[i6].marked==false)
              && (Votes[i6].Value<Votes[i8].Value))
             i8=i6;

             d2 = UpperBounce8 - Votes[i8].Value;

             if (d2 < eps1)
             p -> Value = 0.0;
             else
               {
                if (d2 > d1) d2 = d1 ;

                p -> Value = d2;

                if (p -> Cap - d2 < eps1)
                p -> Value = p -> Cap;

                d1=d1-d2;

                if (d1<eps1)
                d1=0.0;

                Votes[i8].Value=Votes[i8].Value+d2;
               }
            }
          else
          p -> Value = 0.0;

          Votes[i8].marked=true;

          if (i5==1)
            {
             if (i2<i4)
             p -> start = i2;
             else
             p -> start = i2-1;
            }
          else
          p -> start = i2;

          p -> end   = i17+i8;

          i9++;
         }

       sorted8a[i1]=i9;

       p = &Arcs8[i9];

       p -> start = Source8;
       p -> Cap   = Value8[i2];
       p -> Value = p -> Cap - d1;

       if (i5==1)
         {
          if (i2<i4)
          p -> end = i2;
          else
          p -> end = i2-1;
         }
       else
       p -> end = i2;

       i9++;
      }

    for (i1=0;i1<M;i1++)
      {
       p = &Arcs8[i9];

       p -> start = i17+i1;
       p -> end   = Drain8;
       p -> Value = Votes[i1].Value;
       p -> Cap   = UpperBounce8;

       if (p -> Cap - p -> Value < eps1)
       p -> Value = p -> Cap;

       Flow = Flow + p -> Value;

       i9++;
      }

    i15=NArcs8-M;

    j1=true;

    if (Flow<=MaxPossFlow-eps2)
    do
      {
       for (i1=0;i1<NVertices8;i1++)
       Vertices8[i1].marked=false;

       Arcs8[0].upper=NArcs8-1;
       Arcs8[0].lower=1;

       for (i1=1;i1<NArcs8;i1++)
         {
          Arcs8[i1].upper=i1-1;
          Arcs8[i1].lower=i1+1;
         }

       Arcs8[NArcs8-1].lower=0;

       for (i1=i5;i1<N8;i1++)
         {
          i2=sorted8a[i1];

          p = &Arcs8[i2];

          i12 = p -> upper;
          i13 = p -> lower;

          if (p -> Value < p -> Cap - eps1)
            {
             q = &Vertices8[p -> end];

             q -> marked    = true;
             q -> upper     = i2;
             q -> direction = true;
            }

          Arcs8[i12].lower=i13;
          Arcs8[i13].upper=i12;
         }

       i1 =NArcs8-1;

       j4=true;

       j5=true;

       do
         {
          p = &Arcs8[i1];
          i12 = p -> upper;

          if (Vertices8[p -> start].marked==true)
            {
             if (p -> Value < p -> Cap - eps1)
               {
                q = &Vertices8[p -> end];

                q -> marked    = true;
                q -> upper     = i1;
                q -> direction = true;

                j4=false;
               }

             i13 = p -> lower;
             Arcs8[i12].lower=i13;
             Arcs8[i13].upper=i12;
            }

          if (i1<i12)
          j5=false;

          i1=i12;
         }
       while (j5==true);

       if (j4==false)
         {
          j4=true;

          j5=true;

          do
            {
             j6=true;

             do
               {
                p = &Arcs8[i1];
                i12 = p -> upper;

                if (Vertices8[p -> start].marked==true)
                  {
                   if (p -> Value < p -> Cap - eps1)
                     {
                      q = &Vertices8[Drain8];

                      q -> marked    = true;
                      q -> upper     = i1;
                      q -> direction = true;
                     }

                   i13 = p -> lower;
                   Arcs8[i12].lower=i13;
                   Arcs8[i13].upper=i12;
                  }

                if (i12<i15)
                j6=false;

                i1=i12;
               }
             while ((Vertices8[Drain8].marked==false) && (j6==true));

             if (Vertices8[Drain8].marked==false)
               {
                j6=true;

                do
                  {
                   p = &Arcs8[i1];
                   i12 = p -> upper;

                   if (Vertices8[p -> end].marked==true)
                     {
                      q = &Vertices8[p -> start];

                      if ((q -> marked == false) && (p -> Value > eps1))
                        {
                         q -> marked    = true;
                         q -> upper     = i1;
                         q -> direction = false;

                         j4=false;
                        }

                      i13 = p -> lower;
                      Arcs8[i12].lower=i13;
                      Arcs8[i13].upper=i12;
                     }

                   if (i1<i12)
                     {
                      if (j4==true)
                      j5=false;

                      j4=true;

                      j6=false;
                     }

                   i1=i12;
                  }
                while (j6==true);

                if (j5==true)
                  {
                   j6=true;

                   do
                     {
                      p = &Arcs8[i1];
                      i12 = p -> upper;

                      if (Vertices8[p -> start].marked==true)
                        {
                         q = &Vertices8[p -> end];

                         if ((q -> marked == false)
                          && (p -> Value < p -> Cap - eps1))
                           {
                            q -> marked    = true;
                            q -> upper     = i1;
                            q -> direction = true;

                            j4=false;
                           }

                         i13 = p -> lower;
                         Arcs8[i12].lower=i13;
                         Arcs8[i13].upper=i12;
                        }

                      if (i1<i12)
                        {
                         if (j4==true)
                         j5=false;

                         j4=true;

                         j6=false;
                        }

                      i1=i12;
                     }
                   while (j6==true);
                  }
               }
            }
          while ((Vertices8[Drain8].marked==false) && (j5==true));
         }

       if (Vertices8[Drain8].marked==true)
         {
          i1=Drain8;

          PossFlow=N+0.0;

          do
            {
             p = &Arcs8[Vertices8[i1].upper];

             if (Vertices8[i1].direction==true)
               {
                if (p -> Cap - p -> Value < PossFlow)

                PossFlow = p -> Cap - p -> Value;

                i1 = p -> start;
               }

             else
               {
                if (p -> Value < PossFlow)

                PossFlow = p -> Value;

                i1 = p -> end;
               }
            }
          while (i1!=Source8);

          i1=Drain8;

          do
            {
             p = &Arcs8[Vertices8[i1].upper];

             if (Vertices8[i1].direction==true)
               {
                p -> Value = p -> Value + PossFlow;

                if (p -> Value > p -> Cap - eps1)

                p -> Value = p -> Cap;

                i1 = p -> start;
               }

             else
               {
                p -> Value = p -> Value - PossFlow;

                if (p -> Value < eps1)

                p -> Value = 0.0;

                i1 = p -> end;
               }
            }
          while (i1!=Source8);

          Flow=Flow+PossFlow;
         }

       else
       j1=false;

       if  (Flow>MaxPossFlow-eps2)
       j1=false;
      }
    while (j1==true);

    if (Flow>MaxPossFlow-eps2)
    Output1=UpperBounce8;
    else
      {
       i96++;

       i97=0;

       i98=M-1;

       i99=10;
       i1=(0.35355*i95)/i96;
       if (i99<i1)
       i99=i1;

       d99=N+0.0;

       bool1=false;

       do
         {
          PropCompletion();

          if (bool1==false)
          EKarp();
          else
          EKarpFinal();
         }
       while (bool1==false);
      }
   }
 else
   {
    i96++;

    i97=0;

    i98=M-1;

    i99=10;
    i1=(0.35355*i95)/i96;
    if (i99<i1)
    i99=i1;

    d99=N+0.0;

    bool1=false;

    do
      {
       PropCompletion();

       if (bool1==false)
       EKarp();
       else
       EKarpFinal();
      }
    while (bool1==false);
   }
}

/*******************************************************************************/

void VoteMana()
{
 unsigned int i1,i2,i3,i4,i5,i6;
 char c1;
 bool j1,j2;

 i1=Vote2[0][Kombi[0]];
 i4=0;
 j2=false;

 for (i2=1;i2<=M;i2++)
   {
    if (Vote2[0][Kombi[i2]]<i1)
      {
       Vote3[0][i2-1]=-1;
       Vote4[0][i2-1]=-1;
       j2=true;
      }
    else
      {
       if (Vote2[0][Kombi[i2]]>i1)
         {
          Vote3[0][i2-1]=1;
          Vote4[0][i2-1]=1;
         }
       else
         {
          Vote3[0][i2-1]=0;
          Vote4[0][i2-1]=0;
          i4++;
         }
      }
   }

 Value3[0]=Value2[0];
 Indif4[0]=i4;
 cool4 [0]=j2;

 N3=1;

 for (i1=1;i1<N2;i1++)
   {
    i2=0;

    j1=false;

    while ((i2<N3) && (j1==false))
      {
       j1=true;

       i6=Vote2[i1][Kombi[0]];

       for (i3=1;i3<=M;i3++)
         {
          i5=Vote2[i1][Kombi[i3]];
          c1=Vote3[i2][i3-1];

          if (((i5>i6) && (c1!=1))
              || ((i5<i6) && (c1!=-1))
              || ((i5==i6) && (c1!=0)))
            {
             j1=false;
             i3=M+1;
            }
         }

       if (j1==false)
       i2++;
       else
       Value3[i2]=Value3[i2]+Value2[i1];
      }

    if (j1==false)
      {
       i6=Vote2[i1][Kombi[0]];

       i4=0;
       j2=false;

       for (i2=1;i2<=M;i2++)
         {
          if (Vote2[i1][Kombi[i2]]<i6)
            {
             Vote3[N3][i2-1]=-1;
             Vote4[N3][i2-1]=-1;
             j2=true;
            }
          else
            {
             if (Vote2[i1][Kombi[i2]]>i6)
               {
                Vote3[N3][i2-1]=1;
                Vote4[N3][i2-1]=1;
               }
             else
               {
                Vote3[N3][i2-1]=0;
                Vote4[N3][i2-1]=0;
                i4++;
               }
            }
         }

       Value3[N3]=Value2[i1];
       Indif4[N3]=i4;
       cool4 [N3]=j2;

       N3++;
      }
   }

 N4=N3;

 for (i1=0;i1<N4;i1++)
 Value4[i1]=Value3[i1]+0.0;

 PropCompletionSimple();
 EKarpSimple();
}

/*******************************************************************************/

void CheckCombination()
{
 unsigned int i1,i2;

 for (i1=0;i1<C;i1++)
 if (elected[i1]==false)
   {
    FOPEN();
    Print1(i1);
    FCLOSE();

    Kombi[1]=i1;

    for (i2=0;i2<C;i2++)
    if (i1!=i2)
      {
       if (elected[i2]==false)
         {
          Kombi[0]=i2;
          VoteMana();
          Matrix[i1][i2]=Output1;

       FOPEN();
          fprintf(datafile," %f",Output1);
          FCLOSE();
         }
      }
    else
      {
       Output1=0.0;

       FOPEN();
       fprintf(datafile," %f",Output1);
       FCLOSE();
      }

       FOPEN();
    fprintf(datafile,"\n");
    FCLOSE();
   }
}

/*******************************************************************************/

void Filling_the_next_place()
{
 unsigned int i1,i2,i3;
 double d1;
 bool j1,j2;

 M++;

 i95=0;
 i96=0;

 for (i1=0;i1<C;i1++)
 for (i2=0;i2<C;i2++)
 Matrix[i1][i2]=0.0;

 CheckCombination();

 for (i1=0;i1<C;i1++)
 if (elected[i1]==false)
 for (i2=0;i2<C;i2++)
 if (i1!=i2)
 if (elected[i2]==false)
 p1[i1][i2]=Matrix[i1][i2];

 for (i1=0;i1<C;i1++)
 if (elected[i1]==false)
 for (i2=0;i2<C;i2++)
 if (i1!=i2)
 if (elected[i2]==false)
 for (i3=0;i3<C;i3++)
 if (i1!=i3)
 if (i2!=i3)
 if (elected[i3]==false)
   {
    d1=p1[i2][i1];
    if (p1[i1][i3]<d1)
    d1=p1[i1][i3];

    if (p1[i2][i3]<d1)
    p1[i2][i3]=d1;
   }

 if (M==1)
 for (i1=0;i1<C;i1++)
 for (i2=0;i2<C;i2++)
 if (i1!=i2)
 path[i1][i2]=p1[i1][i2];

 for (i1=0;i1<C;i1++)
 if (elected[i1]==false)
 winner[i1]=true;

 for (i1=0;i1<C;i1++)
 if (elected[i1]==false)
 for (i2=0;i2<C;i2++)
 if (i1!=i2)
 if (elected[i2]==false)
 if (p1[i2][i1]>p1[i1][i2]+eps4)
 winner[i1]=false;

 j1=false;

 FOPEN();

 for (i1=0;i1<C;i1++)
 if (elected[i1]==false)
 if (winner [i1]==true )
   {
    j2=true;

    for (i2=0;i2<C;i2++)
    if (elected[i2]==false)
    if (winner [i2]==true )
    if (i1!=i2)
    if (path[i2][i1]>path[i1][i2]+eps4)
    j2=false;

    if (j2==true )
    if (j1==false)
      {
       i3=i1;
       j1=true;

       if (datafile != stdout) {
        fprintf(stdout,"The %2d. place goes to candidate ",M);
        Print1(i1, stdout);
        if (i1 < 26 && candidates[i1][0])
        fprintf(stdout,": %s", candidates[i1]);
        fprintf(stdout,"\n");
       }

       fprintf(datafile,"The %d. place goes to candidate ",M);
       Print1(i1);
       fprintf(datafile,".\n");
      }
   }

 fprintf(datafile,"\n");
 FCLOSE();

 elected[i3]=true;

 Kombi[1+M]=i3;
}

/*******************************************************************************/

int main(int argc, const char **argv)
{
 unsigned int i1,i2,i3;


 // CLI syntax: $> prog [data_file [output_file]]
 //
 //      data_file   - input ballots in format 1 or 2
 //      output_file - output file for detailed output
 //
 //      both parameters can be ommitted to run in interactive mode
 //      if output_file is given a summary is printed to stdout

 if (argc < 2) {
 Reading_the_Input(0);
 } else {
     Reading_the_Input(argv[1]);
 }
 if (argc > 2) {
     strncpy(output_filename_buffer, argv[2], 1023);
     output_filename = output_filename_buffer;
 }
 Analyzing_the_Input();

 M=0;

 i1=N2;

 if (C+1>N2)
 i1=C+1;

 i95    =0;
 i96    =0;
 Length1=i1;
 Length2=i1;
 Length3=i1;
 Length4=i1;
 Length5=i1;

 Vote3=new char*[i1];
 Vote4=new char*[i1];
 Vote5=new char*[i1];
 Vote7=new bool*[i1];
 Vote8=new bool*[i1];
 Test =new char*[i1];

 for (i3=0;i3<i1;i3++)
   {
    Vote3[i3]=new char[C];
    Vote4[i3]=new char[C];
    Vote5[i3]=new char[C];
    Vote7[i3]=new bool[C];
    Vote8[i3]=new bool[C];
    Test [i3]=new char[C];
   }

 Test5    =new bool                [i1 ];
 Value3   =new unsigned int        [i1 ];
 Value4   =new double              [i1 ];
 Indif4   =new unsigned int        [i1 ];
 cool4    =new bool                [i1 ];
 Kombi    =new unsigned int        [C+1];
 Kombi4   =new unsigned int        [C  ];
 Kombi5   =new unsigned int        [C  ];
 Test8    =new unsigned int        [i1 ];
 Value5   =new unsigned int        [i1 ];
 Test3    =new unsigned int        [i1 ];
 Indiff7  =new struct IndiffElement[i1 ];
 Indiff8  =new struct IndiffElement[i1 ];
 Arcs7    =new struct ArcElement   [i1 ];
 Arcs8    =new struct ArcElement   [i1 ];
 Vertices7=new struct VertexElement[i1 ];
 Vertices8=new struct VertexElement[i1 ];
 Votes    =new struct VotesElement [C  ];
 Votes1   =new bool                [C  ];
 NewValue =new double              [i1 ];
 Where    =new unsigned int        [i1 ];
 Value7   =new double              [i1 ];
 Value8   =new double              [i1 ];
 sorted7  =new unsigned int        [i1 ];
 sorted7a =new unsigned int        [i1 ];
 sorted8  =new unsigned int        [i1 ];
 sorted8a =new unsigned int        [i1 ];

 Matrix=new double*[C];
 p1    =new double*[C];
 path  =new double*[C];
 for (i2=0;i2<C;i2++)
   {
    Matrix[i2]=new double[C];
    p1    [i2]=new double[C];
    path  [i2]=new double[C];
   }

 winner =new bool[C];
 elected=new bool[C];

 for (i1=0;i1<C;i1++)
 elected[i1]=false;

 if (output_filename) {
 datafile=fopen(output_filename,"w+");
 fclose(datafile);
 }

 for (i1=0;i1<C-1;i1++)
 Filling_the_next_place();

 FOPEN();
 for (i1=0;i1<C;i1++)
 if (elected[i1]==false)
   {
       if (datafile != stdout) {
        fprintf(stdout,"The %2d. place goes to candidate ",M+1);
        Print1(i1, stdout);
        if (i1 < 26 && candidates[i1][0])
        fprintf(stdout,": %s", candidates[i1]);
        fprintf(stdout,"\n");
       }


    fprintf(datafile,"The %d. place goes to candidate ",M+1);
    Print1(i1);
    fprintf(datafile,".\n");
   }
 FCLOSE();

 delete(Kombi);
 delete(Vote2);
 delete(Value2);
 delete(Matrix);
 delete(Vote3);
 delete(Value3);
 delete(Vote4);
 delete(Value4);
 delete(Indif4);
 delete(cool4);
 delete(Kombi4);
 delete(Kombi5);
 delete(Test8);
 delete(Vote5);
 delete(Value5);
 delete(Test);
 delete(Test3);
 delete(Test5);
 delete(Indiff7);
 delete(Indiff8);
 delete(Arcs7);
 delete(Arcs8);
 delete(Vertices7);
 delete(Vertices8);
 delete(Votes);
 delete(Votes1);
 delete(NewValue);
 delete(Where);
 delete(Vote7);
 delete(Value7);
 delete(Vote8);
 delete(Value8);
 delete(sorted7);
 delete(sorted7a);
 delete(sorted8);
 delete(sorted8a);
 delete(p1);
 delete(winner);
 delete(elected);
 delete(path);

 return 0;
}
