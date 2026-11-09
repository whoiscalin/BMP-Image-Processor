#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp_io_rgb.h"

double clamp(double val) {
   if(val < 0.0) return 0.0;
   if(val > 255.0) return 255.0;
   return val;
}

struct culoare {
   int Red;
   int Green;
   int Blue;
 }color;

void border(unsigned char ***img, int *N, int *M, int *left, int *right, int *top, int *bottom, int *border_width,struct culoare *color) {
   for(int l = *left; l <= *left+*border_width; l++) {
      for(int i = *top; i <= *bottom; i++) {
         img[i][l][0]=color->Red;
         img[i][l][1]=color->Green;
         img[i][l][2]=color->Blue;
      }
   }
   for(int l = *right; l >= *right-*border_width; l--) {
      for(int i = *top; i <= *bottom; i++) {
         img[i][l][0]=color->Red;
         img[i][l][1]=color->Green;
         img[i][l][2]=color->Blue;
      }
   }
   for(int l = *top; l <= *top + *border_width; l++) {
      for(int j = *left; j <= *right; j++) {
         img[l][j][0]=color->Red;
         img[l][j][1]=color->Green;
         img[l][j][2]=color->Blue;
      }
   }
   for(int l = *bottom; l >= *bottom - *border_width; l--) {
      for(int j=*left; j <= *right; j++) {
         img[l][j][0]=color->Red;
         img[l][j][1]=color->Green;
         img[l][j][2]=color->Blue;

   }
   }
}

unsigned char*** crop(unsigned char ***img, int *N, int *M,  int *left, int *right, int *top, int *bottom) {
   int new_Height = *bottom - *top;
   int new_Width = *right - *left;
   unsigned char ***cropped_img = (unsigned char ***)malloc(new_Height * sizeof(unsigned char **));
   for(int i = 0; i < new_Height; i++) {
      cropped_img[i]=(unsigned char **)malloc(new_Width * sizeof(unsigned char *));
         for(int j = 0; j < new_Width; j++) {
            cropped_img[i][j]=(unsigned char *)malloc(3 * sizeof(unsigned char));
         }
   }
   for(int i = 0; i < new_Height; i++) {
      for(int j = 0; j < new_Width; j++) {
         cropped_img[i][j][0]=img[*top+i][*left+j][0];
         cropped_img[i][j][1]=img[*top+i][*left+j][1];
         cropped_img[i][j][2]=img[*top+i][*left+j][2];
      }
   }
   *N = new_Height;
   *M = new_Width;
   return cropped_img;
}

void pixel_swap(unsigned char ***img, int *N, int *M) {
   for(int i = 0; i < *N; i++) {
      for(int j = 0; j < *M; j++) {
         int medie = (img[i][j][0]+img[i][j][1]+img[i][j][2])/3;
         for(int k = 0; k < 3; k++) {
            img[i][j][k]=medie;
         }
      }
   }
}

unsigned char ***convolutional_filter(unsigned char ***img, int *N, int *M) {
   printf("Introdu coeficientul si filtrul dorit: \n");
   double coeficient;
   scanf("%lf", &coeficient);
   double **filter=malloc(3 * sizeof(double *));
   if(filter == NULL) {
      free(filter);
      return NULL;

   }
   for(int i = 0; i < 3; i++) {
      filter[i]=calloc(3, sizeof(double ));
      if(filter[i] == NULL) {
         for(int j = 0; j < i; j++) {
            free(filter[j]);
         }
         free(filter);
         return NULL;
      }
   }
      for(int i = 0; i < 3; i++) {
         for(int j = 0; j < 3; j++) {
            scanf("%lf", &filter[i][j]);
       } 
      }
      for(int i = 0; i < 3; i++) {
         for(int j = 0; j < 3; j++) {
            filter[i][j]*=coeficient;
       } 
      }

   unsigned char ***new_img = malloc(*N * sizeof(unsigned char **));
if(new_img == NULL) {
    return NULL; 
}

for(int i = 0; i < *N; i++) {
    
    new_img[i] = calloc(*M, sizeof(unsigned char *));
    
    if(new_img[i] == NULL) {
        
        for(int j = 0; j < i; j++) {
            free(new_img[j]);
        }
        free(new_img);
        return NULL;
    }
    
    
    for(int j = 0; j < *M; j++) {
        
        new_img[i][j] = calloc(3, sizeof(unsigned char));
        
        if(new_img[i][j] == NULL) {
            
            for(int k = 0; k < j; k++) {
                free(new_img[i][k]);
            }
            free(new_img[i]);
            
            
            for(int l = 0; l < i; l++) {
                for(int k = 0; k < *M; k++) {
                    free(new_img[l][k]);
                }
                free(new_img[l]);
            }
            
            // 3. Eliberam matricea mare
            free(new_img);
            return NULL;
        }
    }
}
   for(int i = 1; i < *N-1; i++) {
      for(int j = 1; j < *M-1; j++) {

         float sum_Red = 0.0, sum_Green = 0.0, sum_Blue = 0.0;
         
         
         for(int k = -1; k <= 1; k++) {
            for(int l = -1; l <=1; l++) {
               double filtru = filter[k+1][l+1];
                  sum_Red += img[i+k][j+l][0] * filtru;
                  sum_Green += img[i+k][j+l][1] * filtru;
                  sum_Blue += img[i+k][j+l][2] * filtru;
            }
         }
         new_img[i][j][0] = (unsigned char)clamp(round(sum_Red));
         new_img[i][j][1] = (unsigned char)clamp(round(sum_Green));
         new_img[i][j][2] = (unsigned char)clamp(round(sum_Blue));
      }
   }
   for(int i = 0; i < 3; i++)
      free(filter[i]);
   free(filter);
   return new_img;
}

void freeImage(unsigned char ***img, int H, int W) {
   if(img == NULL) {
      return;
   }
   for(int i = 0; i < H; i++) {
      for(int j = 0; j < W; j++) {
         free(img[i][j]);
      }
      free(img[i]);
   }
   free(img);
}

unsigned char ***instagramable(unsigned char ***img, int *N, int *M) {
   int new_Height, new_Width;
   if(*N > *M) {
       new_Height = *N;
       new_Width = *M + (*N - *M);
   }
   if(*N < *M) {
       new_Height = *N + (*M- *N);
       new_Width = *M;
   }
   unsigned char ***squared=(unsigned char ***)malloc(new_Height * sizeof(unsigned char **));

   for(int i = 0; i < new_Height; i++) {
      squared[i]=(unsigned char **)malloc(new_Width * sizeof(unsigned char *));
      for(int j = 0; j < new_Width; j++) {
         squared[i][j]=(unsigned char *)malloc(3 * sizeof(unsigned char ));
      }
   }
   for(int i = 0; i <(new_Height-*N)/2; i++) {
      for(int j = 0; j < *M; j++) {
         squared[i][j][0] = img[0][j][0];
         squared[i][j][1] = img[0][j][1];
         squared[i][j][2] = img[0][j][2];
      }
   }
   for(int i = (new_Height - *N)/2, k = 0; i < *N +(new_Height-*N)/2; i++, k++) {
      for(int j = 0; j < *M; j++) {
         squared[i][j][0]=img[k][j][0];
         squared[i][j][1]=img[k][j][1];
         squared[i][j][2]=img[k][j][2];
      }
   }
   for(int i = *N+(new_Height-*N)/2; i < new_Height; i++) {
      for(int j = 0; j < *M; j++) {
         squared[i][j][0] = img[*N-1][j][0];
         squared[i][j][1] = img[*N-1][j][1];
         squared[i][j][2] = img[*N-1][j][2];
      }
   }
  
   *N = new_Height;
   *M = new_Width;
   return squared;
}

int main() {
   int M, N;
   unsigned char ***img = bmp_read("alpaca.bmp", &N, &M);
   if(!img) {
    return 1;

   } 

   int top = 10;
   int bottom = 500;
   int left = 300;
   int right = 550;
   int border_width;
   int prevN = N;
   int prevM = M;
//  printf("Insert the border's width: ");
//  scanf("%d", &border_width);
//  printf("\n");
//  printf("Insert the border's color's( R G B): ") ;

 //scanf("%d %d %d", &color.Red, &color.Green, &color.Blue);

 //Border(img, &left, &right, &top, &bottom, &border_width, &color);

//  unsigned char ***cropped = crop(img, &N, &M, &left, &right, &top, &bottom);
//  if(cropped == NULL) {
//    freeImage(img, prevN, prevM);
//    return 1;
//  }
unsigned char ***squared = instagramable(img, &N, &M);
if(squared == NULL) {
   freeImage(img, prevN, prevM);
   return 1;
}
 freeImage(img, prevN, prevM);
 img = squared;
 //pixel_swap(img, &N, &M); // Black & White filter
 unsigned char ***filtered = convolutional_filter(img, &N, &M);
 if(filtered == NULL) {
   freeImage(img, N, M);
   return 1;
 }
 freeImage(img, prevN, prevM);
 img = filtered;
bmp_write("output.bmp", img, N, M);
 freeImage(img, N, M);
 return 0;
}