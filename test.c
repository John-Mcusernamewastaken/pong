#include <stdio.h>
#include <stdlib.h>

#define BOUNCE_MAXARC 180
#define BOUNCE_MINARC 0
int main()
{
    for(int i=0;i<1000;i++){
        double n = ((rand() % (BOUNCE_MAXARC-1-BOUNCE_MINARC))+BOUNCE_MINARC);
        if(BOUNCE_MINARC>=n||n>=BOUNCE_MAXARC)printf("%.2f\n",n);
    } 
}