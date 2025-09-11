#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct{
    char *ch;
}name;

int main(){
    // name p1[10];
    // // printf("%d %d.\n", sizeof(p1), sizeof(p1.ch));
    // p1.ch=malloc(10000);
    // printf("%d %d.\n", sizeof(p1), sizeof(p1.ch));

    char ch1=1, ch0=0;
    if(!ch1){
        if(ch0) printf("11\n");
        else printf("10\n");
    }
    else{
        if(ch0) printf("01\n");
        else printf("00\n");
    }

    char *cc="\0";
    if(cc[0]=='\0') printf("Yes!!!\n");
    else printf("No!!!\n");
}