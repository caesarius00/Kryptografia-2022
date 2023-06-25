#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    if(argc != 2){
        printf("Zla liczba argumentow");
        return 1;
    }

    if(strcmp(argv[1], "-p") == 0){
        //przygotowanie do szyfrowania

        //open file orig.txt
        FILE *orig = fopen("orig.txt", "r");
        if(orig == NULL){
            printf("Nie udalo sie otworzyc pliku orig.txt");
            return 1;
        }
        //open file plain.txt
        FILE *plain = fopen("plain.txt", "w");
        if(plain == NULL){
            printf("Nie udalo sie otworzyc pliku plain.txt");
            fclose(orig);
            return 1;
        }

        //read from orig.txt and write to plain.txt
        char c;
        int i=-1;
        while((c = fgetc(orig)) != EOF){
            if(i%64 == 63){
                fputc('\n', plain);
                i=-1;
            }

            if(c >='a' && c <= 'z' || c==' '){
                fputc(c, plain);
                i++;
            }
            else if(c >='A' && c <= 'Z'){
                fputc(c+32, plain);
                i++;
            }
        }
        fclose(orig);
        fclose(plain);

    }
    else if(strcmp(argv[1], "-e") == 0){
        //szyfrowanie
        //open plain
        FILE *plain = fopen("plain.txt", "r");
        if(plain == NULL){
            printf("Nie udalo sie otworzyc pliku plain.txt");
            return 1;
        }
        //open binary file crypto.bin
        FILE *crypto = fopen("crypto.bin", "wb");
        if(crypto == NULL){
            printf("Nie udalo sie otworzyc pliku crypto.bin");
            fclose(plain);
            return 1;
        }
        //open file key.txt
        FILE *keyFile = fopen("key.txt", "r");
        if(keyFile == NULL){
            printf("Nie udalo sie otworzyc pliku key.txt");
            fclose(plain);
            fclose(crypto);
            return 1;
        }
        //read from key.txt to char* key
        char key[64];
        for(int i=0; i<64; i++){
            key[i] = fgetc(keyFile);
        }

        //read from plain.txt and write to crypto.bin
        char c;
        int i=0;
        while((c = fgetc(plain)) != EOF){
            if(c!='\n'){
                //fputc(c^key[i], crypto);
                char buffer[2] = {c^key[i]};
                //printf("%s %d\t", buffer, i);
                fwrite(buffer, sizeof(char), 1, crypto);
                i++;
                if(i==64)
                    i=0;
            }else{
                //fputc(c, crypto);
                char buffer[2] = {c};
                //printf("%s", buffer);
                fwrite(buffer, sizeof(char), 1, crypto);
            }
        }


    }
    else if(strcmp(argv[1], "-k") == 0){
        //kryptoanaliza
        //read file crypto.bin to char**
        FILE *crypto = fopen("crypto.bin", "rb");
        if(crypto == NULL){
            printf("Nie udalo sie otworzyc pliku crypto.bin");
            return 1;
        }
        //read crypto fo string array with realloc
        char **cryptoArray = NULL;
        int cryptoArraySize = 0;
        char w[2];
        int i=0;
        while(fread(w, sizeof(char), 1, crypto) != 0){
            cryptoArraySize++;
            cryptoArray = realloc(cryptoArray, cryptoArraySize*sizeof(char*));
            cryptoArray[cryptoArraySize-1] = malloc(64*sizeof(char));
            for(int j=0; j<64; j++){
                cryptoArray[cryptoArraySize-1][j] = w[0];
                if(fread(w, sizeof(char), 1, crypto) == 0) break;
            }
            //fseek(crypto, 1, SEEK_CUR);
        }

        //create copy of cryptoArray
        char **result = NULL;
        result = malloc(cryptoArraySize*sizeof(char*));
        for(int i=0; i<cryptoArraySize; i++){
            result[i] = malloc(64*sizeof(char));
        }

        char spaceFound[64];
        //fill spaceFound with 0
        for(int i=0; i<64; i++){
            spaceFound[i] = 0;
        }
        for(int i=0; i<64; i++){
            for(int j=0; j<cryptoArraySize-2; j++){
                int xor= cryptoArray[j][i]^cryptoArray[j+1][i];
                if(xor >=64 && xor <= 95 && j<cryptoArraySize-3){
                    //one is space
                    int xor2 = cryptoArray[j][i]^cryptoArray[j+2][i];
                    if(xor2 >=64 && xor2 <= 95){
                        //one is space
                        int xor3 = cryptoArray[j+1][i]^cryptoArray[j+2][i];
                        if((xor3 <64 || xor3 > 95) && xor3!=0){
                            result[j][i] = ' ';
                            spaceFound[i] = cryptoArray[j][i];
                            break;
                        }
                    }
                    else{
                        if(xor2!=0 && (xor2 <64 || xor2 > 95)){
                            result[j+1][i] = ' ';
                            spaceFound[i] = cryptoArray[j+1][i];
                            break;
                        }
                    }
                }


            }
        }

        for(int i=0; i<cryptoArraySize; i++){
            for(int j=0; j<64; j++){
                if(result[i][j] != ' '){
                    result[i][j] = cryptoArray[i][j]^spaceFound[j]^' ';
                }
                if(spaceFound[j] == 0){
                    result[i][j] = '.';
                }
            }
        }


        //save result to file decrypt.txt
        FILE *decrypt = fopen("decrypt.txt", "w");
        if(decrypt == NULL){
            printf("Nie udalo sie otworzyc pliku decrypt.txt");
            return 1;
        }
        for(int i=0; i<cryptoArraySize-1; i++){
            for(int j=0; j<64; j++){
                fputc(result[i][j], decrypt);
            }
            fputc('\n', decrypt);
        }
        for(int j=0; j<64; j++){
            //jeśli ostatnia linia jest zapisana niepełna, to przestań zapisywać znaki
            //w większości przypadków działa dobrze
            //może się zepsuć tylko wtedy, gdy klucz i znak do zakodowania są w danej kolumnie takie same
            //przez co kod tego znaku będzie równy 0
            //uznałem, że jest to mała strata w przeciwieństwie do zapisywania linii z losowymi znakami
            if(cryptoArray[cryptoArraySize-1][j] == 0) break;

            fputc(result[cryptoArraySize-1][j], decrypt);
        }



        

    } 
    /*else if(strcmp(argv[1], "-d") == 0){
        //deszyfrowanie
        //open file crypto.bin
        FILE *crypto = fopen("crypto.bin", "rb");
        if(crypto == NULL){
            printf("Nie udalo sie otworzyc pliku crypto.bin");
            return 1;
        }
        //open file key.txt
        FILE *keyFile = fopen("key.txt", "r");
        if(keyFile == NULL){
            printf("Nie udalo sie otworzyc pliku key.txt");
            fclose(crypto);
            return 1;
        }
        //open file decrypt.txt
        FILE *decrypt = fopen("decrypt.txt", "w");
        if(decrypt == NULL){
            printf("Nie udalo sie otworzyc pliku decrypt.txt");
            fclose(crypto);
            fclose(keyFile);
            return 1;
        }
        //read from key.txt to char* key
        char key[64];
        for(int i=0; i<64; i++){
            key[i] = fgetc(keyFile);
        }
        //read from crypto.bin and write to decrypt.txt
        char c[2];
        int i=0;
        while(fread(c, sizeof(char), 1, crypto) != 0){
            if(i<64){
                fputc(c[0]^key[i], decrypt);
                printf("%c", c[0]^key[i]);
                i++;
            }else{
                fputc(c[0], decrypt);
                printf("%c", c[0]);
                i=0;
            }
        }
    }*/
    else {
        printf("Zly argument");
        return 1;
    }



    return 0;
}