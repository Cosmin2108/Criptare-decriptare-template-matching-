#include <stdio.h>
#include <stdlib.h>
#include<math.h>
struct imagine
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;

};

struct header
{
    int height;
    int width;

};

void liniarizare(char *numefisier, struct imagine **pixel, struct header **head,  unsigned char **padding)
{  printf("Liniarizare...\n");
   *head=(struct header*)malloc(sizeof(struct header));

   FILE *img;
   img=fopen(numefisier,"rb");

   if(img==NULL)
        {
          printf("Eroare la gasirea imaginii\n");
          return;
        }
        else
        printf("Imagine gasita\n") ;


          fseek(img,18,SEEK_SET);
          fread(&(*head)->width,sizeof(int),1,img);
		  fread(&(*head)->height,sizeof(int),1,img);
          fseek(img,54,SEEK_SET);


    int h=(*head)->height, w=(*head)->width;
    *pixel=(struct imagine*)malloc(h*w*sizeof(struct imagine));

    int p= ((*head)->width*3)%4;
    if(p!=0)
     {
      p=4-((*head)->width*3)%4;
      *padding=(unsigned char*)malloc((*head)->height*p*sizeof(unsigned char));
      }

    unsigned char o;
    unsigned k=0;
    int poz=h*w-1 , l, c,j;    /// poz-(l+1)*w+1  aici incepe pozitia unei linii dintr-o imagine cu susul in jos, " poz "  nu e unsigned pt ca o sa ia val-1!!

 for(l=0;l<h;l++)
 {
   int q=poz-(l+1)*w;
    for(c=0;c<w;c++)
    {
        q++;
        fread(&(*pixel)[q].blue,sizeof(char),1,img);
        fread(&(*pixel)[q].green,sizeof(char),1,img);
        fread(&(*pixel)[q].red,sizeof(char),1,img);


    }

    if(((*head)->width*3)%4!=0)
     {
          for(j=0;j<p;j++)
          {
             fread(&o,sizeof(char),1,img);
             (*padding)[k]=o;
             k++;

          }
     }
  }

    fclose(img);
    printf("->Imagine liniarizata\n");
}

void creeaza_img(char *numefisier,char *numecopie, struct imagine pixel[], struct header head[],  unsigned char *padding)
{  printf("Creare imagine...\n");

   FILE *img, *copy;
   img=fopen(numefisier,"rb");
   if(img==NULL)
   {
       printf("Eroare la gasirea imaginii de creat\n");
       return;
    }
    else
       printf("Imagine ce trebuie creata a fost gasita\n") ;

   copy=fopen(numecopie,"wb");
   copy==NULL? printf("Eroare la crearea noului fisier pentru copiere\n"): printf("Noul fisier pentru copierea imaginii a fost creat\n") ;

    unsigned int k=0;
	int poz=head->height*head->width-1 , l, c,j,i;
    unsigned char o;

   for(i=0;i<54;i++)
     {
        fread(&o,sizeof(char),1,img);
        fwrite(&o,sizeof(char),1,copy);

     }

	 for(l=0;l<head->height;l++)
	 {
         int q=poz-(l+1)*head->width;

    	 for(c=0;c<head->width;c++)
         {
	         q++;
             fwrite(&pixel[q].blue,sizeof(char),1,copy);
             fwrite(&pixel[q].green,sizeof(char),1,copy);
             fwrite(&pixel[q].red,sizeof(char),1,copy);


         }

		 if((head->width*3)%4!=0)
         {   printf("padding\n");
              int p= 4-(head->width*3)%4;
              for(j=0;j<p;j++)
              {
                fwrite(&padding[k],sizeof(char),1,copy);
                k++;
              }
         }
	 }
    fclose(img);
    fclose(copy);
    printf("->Imagine creata\n\n");

}

unsigned int XORSHIFT32(unsigned int val)
{
        val^=val<<13;
        val^=val>>17;
        val^=val<<5;
        return val;
}

void generare_vector ( char *numefisierk, struct header *head, unsigned int **R)
{
    printf("Generare numere aleatoare...\n");
    FILE *key=fopen(numefisierk, "r");
    if(key==NULL)
    {   printf("Eroare la gasirea fisierului %s\n", numefisierk);
        return; }
     else
      printf("Fisier %s gasit\n", numefisierk) ;

    (*R)=(unsigned int*)malloc((2*head->height*head->width-1)*sizeof(unsigned int));

    unsigned int seed,i;
    fscanf(key,"%u", &seed);

    (*R)[0]=seed;
    for(i=1;i<=2*head->height*head->width-1;i++)
    {
        (*R)[i]=XORSHIFT32((*R)[i-1]);
    }

    fclose(key);
}

void creaza_permutare(unsigned int *R,unsigned int **P, struct header *head)
{
     printf("Generare permutare...\n");
    *P=(unsigned int*)malloc((head->height*head->width-1)*sizeof(unsigned int));
    unsigned int i,k;

    for(i=0;i<head->height*head->width;i++)
     (*P)[i]=i;

    for(i=head->height*head->width-1;i>=1;i--)
    {
         k=R[head->height*head->width-i]%(i+1);
         unsigned int aux= (*P)[i];
        (*P)[i]=(*P)[k];
        (*P)[k]=aux;

    }
     printf("->Permutare creata\n");
}

void permutare_pixeli( struct imagine **pixel, struct header *head, unsigned int *P)
{   printf("Permutare pixeli...\n");

    unsigned int i;
    struct imagine *aux=(struct imagine*)malloc(head->height*head->width*sizeof(struct imagine));
    for(i=0;i<=head->height*head->width-1;i++)
    {
        aux[P[i]].blue=(*pixel)[i].blue;
        aux[P[i]].green=(*pixel)[i].green;
        aux[P[i]].red=(*pixel)[i].red;

    }
    free(*pixel);
    (*pixel)=aux;

     printf("->Pixelii au fost permutati\n");
}


void criptare(char *numefisierk,struct imagine **pixel, struct header *head,unsigned int *R)
{   printf("Criptare imagine...\n");
    FILE *key;
    key=fopen(numefisierk,"r");
    if(key==NULL)
    {   printf("Eroare la gasirea fisierului %s\n", numefisierk);
        return; }
     else
        printf("Fisier %s gasit\n", numefisierk) ;

    unsigned int SV;
    int x,y,i;
    unsigned int m=~(~0<<8);
    for(i=0;i<=head->height*head->width-1;i++)
      {
        if(i==0)
        {
          fseek(key,9,SEEK_SET);
          fscanf(key,"%u",&SV);
          x=SV&m;
          y=R[head->height*head->width+i]&m;
          (*pixel)[i].blue=x^(*pixel)[i].blue^y;
          x=(SV>>8)&m;
          y=(R[head->height*head->width+i]>>8)&m;
          (*pixel)[i].green=x^(*pixel)[i].green^y;
          x=(SV>>16)&m;
          y=(R[head->height*head->width+i]>>16)&m;
          (*pixel)[i].red=x^(*pixel)[i].red^y;

        }
        else
        {
          y=R[head->height*head->width+i]&m;
          (*pixel)[i].blue=(*pixel)[i-1].blue^(*pixel)[i].blue^y;
          y=(R[head->height*head->width+i]>>8)&m;
          (*pixel)[i].green=(*pixel)[i-1].green^(*pixel)[i].green^y;
          y=(R[head->height*head->width+i]>>16)&m;
          (*pixel)[i].red=(*pixel)[i-1].red^(*pixel)[i].red^y;

        }
      }

    fclose(key);
    printf("->Imagine criptata\n");
}

void decriptare( char *numefisierk,struct imagine **pixel, struct header *head,unsigned int *R)
{
    printf("Decriptare imagine...\n");

     FILE *key;
    key=fopen(numefisierk,"r");
    if(key==NULL)
    {   printf("Eroare la gasirea fisierului %s\n", numefisierk);
        return; }
     else
      printf("Fisier %s gasit\n", numefisierk) ;
    unsigned int SV;
    int x,y,i;
     unsigned int m=~(~0<<8);
    for(i=head->height*head->width-1;i>=0;i--)
       if(i==0)
        {
          fseek(key,9,SEEK_SET);
          fscanf(key,"%u",&SV);
          x=SV&m;
          y=R[head->height*head->width+i]&m;
          (*pixel)[i].blue=x^(*pixel)[i].blue^y;
          x=(SV>>8)&m;
          y=(R[head->height*head->width+i]>>8)&m;
          (*pixel)[i].green=x^(*pixel)[i].green^y;
          x=(SV>>16)&m;
          y=(R[head->height*head->width+i]>>16)&m;
          (*pixel)[i].red=x^(*pixel)[i].red^y;

        }
        else
        {
          y=R[head->height*head->width+i]&m;
          (*pixel)[i].blue=(*pixel)[i-1].blue^(*pixel)[i].blue^y;
          y=(R[head->height*head->width+i]>>8)&m;
          (*pixel)[i].green=(*pixel)[i-1].green^(*pixel)[i].green^y;
          y=(R[head->height*head->width+i]>>16)&m;
          (*pixel)[i].red=(*pixel)[i-1].red^(*pixel)[i].red^y;

        }

    fclose(key);
    printf("->Imagine decriptata\n");
}

void permutare_inversa_pixeli( struct imagine **pixel, struct header *head, unsigned int *P)
{
    printf("Permutare inversa pixeli...\n");
    struct imagine *aux=(struct imagine*)malloc(head->height*head->width*sizeof(struct imagine));
    int i;
    for(i=head->height*head->width-1;i>=0;i--)
    {
        aux[i].blue=(*pixel)[P[i]].blue;
        aux[i].green=(*pixel)[P[i]].green;
        aux[i].red=(*pixel)[P[i]].red;

    }
    free(*pixel);
    (*pixel)=aux;
     printf("->Pixelii au fost permutati in ordine inversa\n");
}


void testul_chi_patrat (struct imagine *pixel, struct header *head)
{
    int i;
    unsigned int *fb, *fg, *fr,j;
    fb=(unsigned int*)calloc(256, sizeof(unsigned int));
    fg=(unsigned int*)calloc(256, sizeof(unsigned int));
    fr=(unsigned int*)calloc(256, sizeof(unsigned int));

    for(i=0;i<=head->width*head->height-1;i++)
    {
        fb[pixel[i].blue]++;
        fg[pixel[i].green]++;
        fr[pixel[i].red]++;
    }

    double festim;
    festim=(head->height*head->width)/256;

    double x1=0.0, x2=0.0, x3=0.0;
    for(j=0;j<=255;j++)
    {
        x1= x1+ pow((double)fb[j]-festim,2)/festim;
        x2= x2+ pow((double)fg[j]-festim,2)/festim;
        x3= x3+ pow((double)fr[j]-festim,2)/festim;
    }

    printf("( %.2f %.2f %.2f )\n", x3, x2, x1);

free(fb);
free(fg);
free(fr);

}


void grayscale_image(char* nume_fisier_sursa,char* nume_fisier_destinatie)
{
   FILE *fin, *fout;
   unsigned int dim_img, latime_img, inaltime_img;
   unsigned char pRGB[3], header[54], aux;

   fin = fopen(nume_fisier_sursa, "rb");
   if(fin == NULL)
   	{
   		printf("Nu am gasit imaginea sursa din care citesc\n");
   		return;
   	}

   fout = fopen(nume_fisier_destinatie, "wb+");

   fseek(fin, 2, SEEK_SET);
   fread(&dim_img, sizeof(unsigned int), 1, fin);

   fseek(fin, 18, SEEK_SET);
   fread(&latime_img, sizeof(unsigned int), 1, fin);
   fread(&inaltime_img, sizeof(unsigned int), 1, fin);
   printf("Dimensiunea imaginii in pixeli (latime x inaltime): %u x %u\n",latime_img, inaltime_img);

   //copiaza octet cu octet imaginea initiala in cea noua
	fseek(fin,0,SEEK_SET);
	unsigned char c;
	while(fread(&c,1,1,fin)==1)
	{
		fwrite(&c,1,1,fout);
		fflush(fout);
	}
	fclose(fin);

	//calculam padding-ul pentru o linie
	int padding;
    if(latime_img % 4 != 0)
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

	fseek(fout, 54, SEEK_SET);
	int i,j;
	for(i = 0; i < inaltime_img; i++)
	{
		for(j = 0; j < latime_img; j++)
		{
			//citesc culorile pixelului
			fread(pRGB, 3, 1, fout);
			//fac conversia in pixel gri
			aux = 0.299*pRGB[2] + 0.587*pRGB[1] + 0.114*pRGB[0];
			pRGB[0] = pRGB[1] = pRGB[2] = aux;
        	fseek(fout, -3, SEEK_CUR);
        	fwrite(pRGB, 3, 1, fout);
        	fflush(fout);
		}
		fseek(fout,padding,SEEK_CUR);
	}
	fclose(fout);
}

double medie_val_gray(int ls, int li,int cs, int cd, struct imagine *pix_s, struct header *head_s)
{
    int i,j;
    double  s=0.0;
    for(i=ls;i<=li;i++)
     for(j=cs;j<=cd;j++)
            s+=(double)pix_s[i*head_s->width+j].blue;

    return s/(double)((li-ls+1)*(cd-cs+1));
}

double deviatia_standard(int ls, int li,int cs, int cd,struct imagine *pix, struct header *head)
{
  int i, j;
  double  s=0.0, medie;
  medie=medie_val_gray(ls,li,cs,cd,pix,head);

    for(i=ls;i<=li;i++)
        for(j=cs;j<=cd;j++)
           s+=((double)pix[i*head->width+j].blue-medie)*((double)pix[i*head->width+j].blue-medie);

    return sqrt((double)s/(double)((li-ls+1)*(cd-cs+1)-1));
}

double corelatie_sabloane(int ls, int li,int cs, int cd, struct imagine *pix, struct header *head, struct imagine *pix_s, struct header *head_s)
{
  int i, j;
  double s=0.0, fr, medie_s, medie;
  int ls_s, li_s, cs_s, cd_s;

  if(ls==0)
    ls_s=head_s->height-1-(li-ls);
   else
    ls_s=0;
  li_s=ls_s+(li-ls);

  if(cs==0)
     cs_s= head_s->width-1-(cd-cs);
   else
     cs_s=0;
   cd_s=cs_s+(cd-cs); /// ??!!!

    fr=(double)(deviatia_standard(ls,li,cs,cd,pix,head)*deviatia_standard(ls_s, li_s, cs_s,cd_s,pix_s,head_s ));

    medie_s=medie_val_gray(ls_s, li_s, cs_s,cd_s,pix_s,head_s);
    medie=medie_val_gray(ls,li,cs,cd,pix,head);

   for(i=ls;i<=li;i++)
   {
    int a=cs_s;
    for(j=cs;j<=cd;j++)
    {
        s+=((double)(pix[i*head->width+j].blue-medie)*(double)(pix_s[ls_s*head_s->width+a].blue-medie_s))/fr;
        a++;

    }

    ls_s++;
   }
   return s/(double)((li-ls+1)*(cd-cs+1));
}

struct detectii
{
    int i, j;
    int ls, li, cs, cd;
    double corr;
    unsigned char color_blue,color_green, color_red;

};

void template_matching(struct detectii **D,int *color, struct imagine *pix, struct header *head, struct imagine *pix_s, struct header *head_s, int *n)
{

    double prag=0.50;

    int ls, li, cs, cd,i, j;
    for(i=0;i<head->height;i++)
      for(j=0;j<head->width;j++)
      {
        ls=i-head_s->height/2;          /// Determin limitele portiunii din img care se suprapune cu sablonul
        if(ls<0)
         ls=i-(head_s->height/2+ls);

        li=i+head_s->height/2;
        if(li>head->height-1)
         li=i+(head_s->height/2+head->height-1-li);

        cs=j-head_s->width/2;
        if(cs<0)
          cs=j-(head_s->width/2+cs);

        cd=j+head_s->width/2;
        if(cd>head->width-1)
           cd=j+(head_s->width/2+head->width-1-cd);


          double x=corelatie_sabloane(ls,li,cs,cd,pix,head,pix_s,head_s);

          if(x>prag)
            {
               (*D)=(struct detectii*)realloc(*D,((*n)+1)*sizeof(struct detectii));
               (*D)[*n].i=i;
               (*D)[*n].j=j;
               (*D)[*n].ls=ls;
               (*D)[*n].li=li;
               (*D)[*n].cd=cd;
               (*D)[*n].cs=cs;
               (*D)[*n].corr=x;
               (*D)[*n].color_blue=color[2];
               (*D)[*n].color_green=color[1];
               (*D)[*n].color_red=color[0];
                (*n)++;
            }
      }
}


void desenare (struct imagine **pixel, struct header *head,struct detectii *D, int n)
{
   printf("Conturare detectii\n");
   int i , ls,li,cd,cs,j;
   for(i=0;i<n;i++)
   {
        ls=D[i].ls;
        li=D[i].li;
        cs=D[i].cs;
        cd=D[i].cd;

       for(j=ls;j<=li;j++)
       {
           (*pixel)[j*head->width+cs].red=D[i].color_red;
           (*pixel)[j*head->width+cs].green=D[i].color_green;
           (*pixel)[j*head->width+cs].blue=D[i].color_blue;

           (*pixel)[j*head->width+cd].red=D[i].color_red;
           (*pixel)[j*head->width+cd].green=D[i].color_green;
           (*pixel)[j*head->width+cd].blue=D[i].color_blue;

       }
       for(j=cs;j<=cd;j++)
       {
           (*pixel)[ls*head->width+j].red=D[i].color_red;
           (*pixel)[ls*head->width+j].green=D[i].color_green;
           (*pixel)[ls*head->width+j].blue=D[i].color_blue;

           (*pixel)[li*head->width+j].red=D[i].color_red;
           (*pixel)[li*head->width+j].green=D[i].color_green;
           (*pixel)[li*head->width+j].blue=D[i].color_blue;

       }

   }

}

int cmp(const void *a, const void *b)
{
   double x=((struct detectii*)a)->corr;
   double y=((struct detectii*)b)->corr;
    if(x<y)
        return 1;
    else
    if(x>y)
    return -1;
    else
    return 0;

}

double aria(int ls, int li, int cs, int cd)
{
    return (double)(abs(li-ls)+1)*(abs(cd-cs)+1);

}

void stergere(struct detectii **D, int j,int *n)
{
    int i;
    for(i=j;i<(*n)-1;i++)
        (*D)[i]=(*D)[i+1];

    (*D)=(struct detectii*)realloc((*D),((*n)-1)*sizeof(struct detectii));
    (*n)--;


}

void eliminare_non_maxime(struct detectii **D, int *n, struct header *head_s)
{
   printf("Eliminare non-maxime\n");
   int i,j, ls_i, li_i, cd_i, cs_i;
   double ar, ar1, aria_inters,suprap;

   for(i=0;i<(*n)-1;i++)
   {
        ar=aria((*D)[i].ls,(*D)[i].li, (*D)[i].cs,(*D)[i].cd);

    for(j=i+1;j<(*n);j++)
   {

       if(abs((*D)[i].ls-(*D)[j].ls)+1<=head_s->height&&abs((*D)[i].cs-(*D)[j].cs)+1<=head_s->width)
         {
            ar1=aria((*D)[j].ls,(*D)[j].li,(*D)[j].cs,(*D)[j].cd);

             if((*D)[i].ls<(*D)[j].ls)
             {
                 ls_i=(*D)[i].ls+((*D)[j].ls-(*D)[i].ls);
                 li_i=(*D)[j].ls+((*D)[i].li-(*D)[j].ls);
             }
               else
                {
                    ls_i=(*D)[j].ls+((*D)[i].ls-(*D)[j].ls);
                    li_i=(*D)[i].ls+((*D)[j].li-(*D)[i].ls);

                }

             if((*D)[i].cs<(*D)[j].cs)
              {
                  cs_i=(*D)[i].cs+((*D)[j].cs-(*D)[i].cs);
                  cd_i=(*D)[j].cs+((*D)[i].cd-(*D)[j].cs);

              }
              else
              {
                  cs_i=(*D)[j].cs+((*D)[i].cs-(*D)[j].cs);
                  cd_i=(*D)[i].cs+((*D)[j].cd-(*D)[i].cs);

              }

            aria_inters=(double)aria(ls_i,li_i,cs_i,cd_i);
            suprap= (double)aria_inters/(double)(ar1+ar-aria_inters);
            if(suprap>0.2)
                {
                    stergere(&(*D),j,&(*n));
                    j--;
                }
         }
   }

   }

}

int main()
{
   /// PROCES CRIPTARE
   struct imagine *pixel;
   struct header *head;
   unsigned char *padding=NULL;

   char *numefisier=(char*)"peppers.bmp";
   liniarizare(numefisier,&pixel,&head,&padding);
   printf("Dimensiounea imaginii in pixeli este width x height <=> %d x %d\n",head->width,head->height);
   printf("Valorile testului chi-patrat pentru img initiala: ");
   testul_chi_patrat(pixel,head);
   printf("\n");
   char *numecopie=(char*)"peppers_copy.bmp";
   creeaza_img(numefisier,numecopie,pixel,head,padding);

   unsigned int *R;
   char *numefisierk=(char*)"secret_key.txt";
   generare_vector(numefisierk, head,&R);

   unsigned int *P;
   creaza_permutare(R,&P,head);
   permutare_pixeli(&pixel,head,P);

   char *nume_img_amestecata=(char*)"peppers_mixed.bmp";

   creeaza_img(numefisier,nume_img_amestecata,pixel,head,padding);

   criptare(numefisierk,&pixel,head,R);

   char *nume_img_criptata=(char*)"enc_peppers.bmp";

   creeaza_img(numefisier,nume_img_criptata,pixel,head,padding);
   printf("Valorile testului chi-patrat pentru img criptata: ");
   testul_chi_patrat(pixel,head);
   printf("\nSucces criptare\n\nURMEAZA PROCESUL DE DECRIPTARE:\n\n");
   free(R);
   free(P);
   free(pixel);
   free(head);
   if(padding!=NULL)
   free(padding);
  /// PROCES DECRIPTARE
  liniarizare(nume_img_criptata,&pixel,&head,&padding);
  printf("Dimensiunea in pixeli a imaginii criptate este width x height <=> %d x %d\n",head->width,head->height);

  generare_vector(numefisierk, head,&R);
  decriptare(numefisierk,&pixel,head,R);
  char *nume_img_decriptata_amestecata="decripted_image_mixed.bmp";
  creeaza_img(nume_img_criptata,nume_img_decriptata_amestecata,pixel,head,padding);

  creaza_permutare(R,&P,head);
  permutare_inversa_pixeli(&pixel,head,P);
  char *nume_img_decriptata=(char*)"decripted_image.bmp";
  creeaza_img(nume_img_criptata,nume_img_decriptata,pixel,head,padding);

   free(R);
   free(P);
   free(pixel);
   free(head);
   if(padding!=NULL)
   free(padding);

  /// TEMPLATE MATCHING
  printf("\n\nTEMPLATE MATCHING\n\n");
  struct header *head_s=NULL;
  unsigned char *padding_s=NULL;
  struct imagine *pix_s=NULL;
  struct detectii *D=NULL;
  int n_det=0;

  char *nume_img_cifre="test.bmp";
  char *nume_img_gray="gray_img.bmp";
  grayscale_image(nume_img_cifre,nume_img_gray);

  liniarizare(nume_img_gray,&pixel,&head,&padding);

  FILE *s=fopen("sabloane.txt","r");
  if(s==NULL)
  {   printf("Nu exista sabloane\n");
      return 0;  }

  char *nume_sablon=(char*)malloc(11*sizeof(char));

  while(!feof(s))
{
  if(pix_s!=NULL)
  free(pix_s);
  if(head_s!=NULL)
  free(head_s);
  if(padding_s!=NULL)
  free(padding_s);

  fscanf(s,"%s",nume_sablon);
  int color[3];
  fscanf(s,"%d %d %d\n", &color[0], &color[1],&color[2]);

  printf("Nume sablon: %s\n ", nume_sablon);

  char *s_gray_aux="sablon_gray_aux.bmp";
  grayscale_image(nume_sablon,s_gray_aux);

  liniarizare(s_gray_aux,&pix_s,&head_s,&padding_s);

  template_matching(&D,color,pixel,head,pix_s, head_s, &n_det);

}
   qsort(D,n_det,sizeof(D[0]),cmp);

   printf("NUMAR DETETECTII %d\n", n_det);

   eliminare_non_maxime(&D,&n_det,head_s);

   printf("NUMAR DETETECTII %d\n", n_det);

   free(pixel);
   if(padding!=NULL)
   free(padding);
   free(head);

   liniarizare(nume_img_cifre,&pixel,&head,&padding);

   desenare(&pixel,head,D,n_det);

   char *nume_img_desenata="detectii.bmp";

   creeaza_img(nume_img_cifre,nume_img_desenata,pixel,head,padding);

   fclose(s);
   free(pixel);
   if(padding!=NULL)
   free(padding);
   free(head);

   if(pix_s!=NULL)
  free(pix_s);
  if(head_s!=NULL)
  free(head_s);
  if(padding_s!=NULL)
  free(padding_s);

   return 0;
}
