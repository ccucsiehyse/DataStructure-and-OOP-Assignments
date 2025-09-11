#include<stdio.h>

int main(){
    // input
    int n;
    scanf("%d", &n);
    int a[2][n];
    for(int i=0; i<2; i++){
        for(int j=0; j<n; j++) scanf("%d", &a[i][j]);
    }

    // output
    printf("4\n"); // 4 times

    for(int j=0; j<n; j++) printf("%d ", a[0][j]); // old path

    printf("\n%d ", n-1);
    for(int j=1; j<n; j++) printf("%d ", a[0][j]); // s -> d

    printf("\n%d ", n-1);
    for(int j=1; j<n; j++) printf("%d ", a[1][j]); // change all except s
    printf("\n");

    for(int j=0; j<n; j++) printf("%d ", a[1][j]); // new path
}