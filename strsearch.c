/*functions for searching substring in string with Boyer Moore (1st heuristic) 
 *need to implement it into ial.c and ial.h //TODO */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compute_jumps(char* P,unsigned *char_jump){
	for(int i=0;i<128;i++){
		char_jump[i]=strlen(P);
	}
	for(int k=0;k<(int)strlen(P);k++){
		char_jump[(int)P[k]]=(int)strlen(P)-k-1;
	}
return;
}

int bma(char *P,char*T,unsigned *char_jump){
	int index;
	int j=(int)strlen(P)-1;
	int k=(int)strlen(P)-1;
	while(j<(int)strlen(T) && k>=0){
		if(T[j]==P[k]){
			j=j-1;
			k=k-1;
		}
		else{
			j=j+char_jump[(int)T[j]];
			k=(int)strlen(P)-1;
		}
	}
	if (k==-1) {
		index=j+1;
	}
	else {
		index=-1;
	}		
return index;

}


int main() {
int index;
unsigned array[128];
//testing strings, edit if you want
char P[4]="rf";
char T[20]="sadfesdfaaaef";
compute_jumps(P,array);
index=bma(P,T,array);
if(index==-1){
	printf("Nenaslo sa\n");
}
else{
printf("index: %d\n",index);	
}
return 0;
}
