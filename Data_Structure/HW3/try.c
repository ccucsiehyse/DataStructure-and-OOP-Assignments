#include<stdio.h>
#include<stdlib.h>
#define P printf("line: %d.\n", __LINE__)

int read_line(char str[], int n){
	int ch, i = 0;
    P;
	while ((ch = getchar()) != '\n')
	    if (i < n)
	        str[i++] = ch;
	str[i] = '\0';   /* terminates string */
	return i;        /* number of characters stored */
}

int main(){
    int n, l, t, r;
    //input 4 variables
    scanf("%d %d %d %d", &n, &l, &t, &r);
    
    //input each node's memory
    int memory[t][n], id_tmp, mem_tmp;
    for(int i=0; i<n; i++){
        scanf("%d %d", &id_tmp, &mem_tmp);
        for(int j=0; j<t; j++) memory[j][id_tmp] = mem_tmp;
    }

    //input your output
    int tmp[7] = {0};
    char str[100];
    
    while(1){
        //reset tmp
        for(int i=0; i<7; i++) tmp[i]=0;

        //scan line
        int ch, k = 0;
        while ((ch = getchar()) != '\n')
            if (k < n)
                str[k++] = ch;
        str[k] = '\0';
        if(str[0]=='-') break;

        printf("str[]: %s.\n", str);

        //save read number
        int now=0, ct=0;
        for(int i=0; i<7; i++){
            if(str[now]=='\0' || str[now]==EOF) break;
            while(str[now]>='0' && str[now]<='9'){
                tmp[i] = tmp[i]*10 + (str[now]-'0');
                now++;
            }
            
            if(str[now]!='\0') now++;
            ct++;
        }
        
        //3 input in line
        if(ct == 3){
            memory[tmp[2]-1][tmp[0]]--;
            memory[tmp[2]-1][tmp[1]]--;
        }
        //7 input in line
        else if(ct == 7){
            if(tmp[3] == tmp[4]){
                memory[tmp[6]-1][tmp[0]]--;
                memory[tmp[6]-1][tmp[1]]--;
            }
        }

    }

    //print memory
    for(int i=0;i<t;i++){
        printf("%d\n",i);
        for(int j=0;j<n;j++){
            printf("%d ",memory[i][j]);
        }
        printf("\n");
    }
}










