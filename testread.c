#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "ecrypt-sync.h" 

int main( int argc, char** argv )
{
    int i;
    FILE *fp_k;
    u8 key[16]= {0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
    u8 iv[16] = {0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    //u8 tmp;

    fp_k = fopen("keyfile_enc","r");
    if(fp_k == NULL){
         printf("open fail");
         return 1;
    }else{
         for(i=0; i<16; i++){
             fscanf(fp_k,"%02hhX", &key[i]);
	     //key[i]=tmp;
	     printf("%02X", key[i]);
	}
	//fscanf(fp_k,"%c", &tmp);
	printf("\n");
         for(i=0; i<16; i++){                
             fscanf(fp_k,"%02hhX", &iv[i]);
	     //iv[i]=tmp;
	      printf("%02X", iv[i]); 
	}
	//fscanf(fp_k,"%c", &tmp);    
	printf("\n");      
     }

        for(i=0; i<16; i++){
	     printf("%02X", key[i]);
	}
	printf("\n");
        for(i=0; i<16; i++)
            printf("%02X", iv[i]);
	printf("\n");
     fclose(fp_k) ;
     return 0;
}
