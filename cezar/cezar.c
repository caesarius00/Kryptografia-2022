#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//caesar cipher encrypt function
void encryptCaesar(char *text, int key);
void decryptCaesar(char *text, int key);
void encryptAffine(char *text, int key1, int key2);
void decryptAffine(char *text, int key1, int reverseKey2);
int oppositeIn26(int n);
void apparentTestCaesar();
void apparentTestAffine();
void cryptographicTestCaesar();
void cryptographicTestAffine();

int main(int argc, char *argv[]){
    //argc - number of arguments
    //argv - array of arguments
    //argv[0] - program name
    //argv[1] - first argument

    //check if there is enough arguments
    if(argc < 2){
        printf("Not enough arguments");
        return 1;
    }

    

    //główny program
    if(strcmp(argv[1], "-c") == 0){
        //caesar
        if(strcmp(argv[2], "-e") == 0){

            //pobranie klucza z pliku
            FILE *keyFile = fopen("key.txt", "r");
            if(keyFile == NULL){
                printf("Blad pliku!");
                return 1;
            }
            int key;
            fscanf(keyFile, "%d", &key);
            fclose(keyFile);

            if(key < 0 || key > 25){
                printf("Zly klucz!");
                return 1;
            }
            FILE *plain = fopen("plain.txt", "r");
            if(plain == NULL){
                printf("Blad pliku!");
                return 1;
            }
            FILE *crypto = fopen("crypto.txt", "w");
            if(crypto == NULL){
                printf("Blad pliku!");
                return 1;
            }
            //szyfrowanie tekstu i zapisanie do pliku
            char text[100];
            while(fgets(text, 100, plain) != NULL){
                encryptCaesar(text, key);
                fputs(text, crypto);
            }
            fclose(crypto);
            fclose(plain);
        }
        else if(strcmp(argv[2], "-d") == 0){
            //pobranie klucza z pliku
            FILE *keyFile = fopen("key.txt", "r");
            if(keyFile == NULL){
                printf("Blad pliku!");
                return 1;
            }
            int key;
            fscanf(keyFile, "%d", &key);
            fclose(keyFile);

            if(key < 0 || key > 25){
                printf("Zly klucz!");
                return 1;
            }
            //decrypt
            FILE *decrypt = fopen("decrypt.txt", "w");
            if(decrypt == NULL){
                printf("Blad pliku!");
                return 1;
            }
            FILE *crypto = fopen("crypto.txt", "r");
            if(crypto == NULL){
                printf("Blad pliku!");
                return 1;
            }
            char text[100];
            while(fgets(text, 100, crypto) != NULL){
                decryptCaesar(text, key);
                fputs(text, decrypt);
            }
            fclose(crypto);
            fclose(decrypt);
        }
        else if(strcmp(argv[2], "-j") == 0){
            apparentTestCaesar();
        }
        else if(strcmp(argv[2], "-k") == 0){
            cryptographicTestCaesar();
        }
        else{
            printf("Zły argument");
            return 1;
        }
    }
    else if(strcmp(argv[1], "-a") == 0){
        //afiniczny
        if(strcmp(argv[2], "-e") == 0){
            //pobranie klucza z pliku
            FILE *keyFile = fopen("key.txt", "r");
            if(keyFile == NULL){
                printf("Blad pliku!");
                return 1;
            }
            int key, afinKey;
            fscanf(keyFile, "%d", &key);
            fscanf(keyFile, "%d", &afinKey);
            fclose(keyFile);

            FILE *plain = fopen("plain.txt", "r");
            if(plain == NULL){
                printf("Blad pliku!");
                return 1;
            }
            //check if key is valid
            if(key < 0 || key > 25 || afinKey < 0 || afinKey > 25 || afinKey%2 == 0 || afinKey==13){
                printf("Zly klucz");
                return 1;
            }
            int keyReverse = oppositeIn26(afinKey);
            if(keyReverse == -1){
                printf("Zly klucz");
                return 1;
            }
            FILE *crypto = fopen("crypto.txt", "w");
            if(crypto == NULL){
                printf("Blad pliku!");
                return 1;
            }
            //szyfrowanie tekstu i zapisanie do pliku
            char text[100];
            while(fgets(text, 100, plain) != NULL){
                encryptAffine(text, key, afinKey);
                fputs(text, crypto);
            }
            fclose(crypto);
            fclose(plain);
        }
        else if(strcmp(argv[2], "-d") == 0){
            //pobranie klucza z pliku
            FILE *keyFile = fopen("key.txt", "r");
            if(keyFile == NULL){
                printf("Blad pliku!");
                return 1;
            }
            int key, afinKey;
            fscanf(keyFile, "%d", &key);
            fscanf(keyFile, "%d", &afinKey);
            fclose(keyFile);
            
            //check if key is valid
            if(key < 0 || key > 25 || afinKey < 0 || afinKey > 25 || afinKey%2 == 0 || afinKey==13){
                printf("Zly klucz");
                return 1;
            }
            int afinKeyReverse = oppositeIn26(afinKey);
            if(afinKeyReverse == -1){
                printf("Zly klucz");
                return 1;
            }
            //decrypt
            FILE *decrypt = fopen("decrypt.txt", "w");
            if(decrypt == NULL){
                printf("Blad pliku!");
                return 1;
            }
            FILE *crypto = fopen("crypto.txt", "r");
            if(crypto == NULL){
                printf("Blad pliku!");
                return 1;
            }
            char text[100];
            while(fgets(text, 100, crypto) != NULL){
                decryptAffine(text, key, afinKeyReverse);
                fputs(text, decrypt);
            }
            fclose(crypto);
            fclose(decrypt);
        }
        else if(strcmp(argv[2], "-j") == 0){
            apparentTestAffine();
        }
        else if(strcmp(argv[2], "-k") == 0){
            cryptographicTestAffine();
        }
        else{
            printf("Zły argument");
            return 1;
        }
    }
    else{
        printf("Zly argument");
        return 1;
    }

    return 0;
}

void encryptCaesar(char *text, int key){
    int i;
    for(i=0; text[i] != '\0'; ++i){
        char ch = text[i];

        if(ch >= 'a' && ch <= 'z'){
            ch = (ch - 'a' + key) % 26 + 'a';

            text[i] = ch;
        }
        else if(ch >= 'A' && ch <= 'Z'){
            ch = (ch - 'A' + key) % 26 + 'A';

            text[i] = ch;
        }
    }
}

void decryptCaesar(char *text, int key){
    int i;
    for(i=0; text[i] != '\0'; ++i){
        char ch = text[i];

        if(ch >= 'a' && ch <= 'z'){
            ch = ch - key;

            if(ch < 'a'){
                ch = ch + 'z' - 'a' + 1;
            }

            text[i] = ch;
        }
        else if(ch >= 'A' && ch <= 'Z'){
            ch = ch - key;

            if(ch < 'A'){
                ch = ch + 'Z' - 'A' + 1;
            }

            text[i] = ch;
        }
    }
}

void encryptAffine(char *text, int key1, int key2){
    int i;
    for(i=0; text[i] != '\0'; ++i){
        char ch = text[i];

        if(ch >= 'a' && ch <= 'z'){
            ch = ((ch-'a')*key2 + key1)%26 + 'a';

            text[i] = ch;
        }
        else if(ch >= 'A' && ch <= 'Z'){
            ch = ((ch-'A')*key2 + key1)%26 + 'A';
            //ch = ((int)ch*key1 + key2)%26 + 'A';

            text[i] = ch;
        }
    }
}

void decryptAffine(char *text, int key1, int reverseKey2){
    int i;
    for(i=0; text[i] != '\0'; ++i){
        char ch = text[i];

        if(ch >= 'a' && ch <= 'z'){
            ch = (((ch-'a') - key1 + 26)*reverseKey2)%26 + 'a';

            text[i] = ch;
        }
        else if(ch >= 'A' && ch <= 'Z'){
            ch = (((ch-'A') - key1 + 26)*reverseKey2)%26 + 'A';

            text[i] = ch;
        }
    }
}

int oppositeIn26(int key){
    int i;
    for(i=1; i<26; i++){
        if((key*i)%26 == 1){
            return i;
        }
    }
    return -1;
}

void apparentTestCaesar(){
    FILE *extra = fopen("extra.txt", "r");
    if(extra == NULL){
        printf("Blad pliku!");
        return;
    }
    FILE *crypto = fopen("crypto.txt", "r");
    if(crypto == NULL){
        printf("Blad pliku!");
        return;
    }
    char chApparent = fgetc(extra);
    fclose(extra);
    char chCrypto = fgetc(crypto);
    FILE *decrypt = fopen("decrypt.txt", "w");
    if(decrypt == NULL){
        printf("Blad pliku!");
        return;
    }
    for(int i=0; i<26; i++){
        if((chApparent+i - 'a')%26 + 'a' == chCrypto || (chApparent+i - 'A')%26 + 'A' == chCrypto){
            //open file key-new.txt
            FILE *key = fopen("key-new.txt", "w");
            if(key == NULL){
                printf("Blad pliku!");
                return;
            }
            fprintf(key, "%d", i);
            fclose(key);
            fseek(crypto, 0, SEEK_SET);
            char text[100];
            while(fgets(text, 100, crypto) != NULL){
                decryptCaesar(text, i);
                fputs(text, decrypt);
            }
            i=27;
        }
    }
    fclose(crypto);
    fclose(decrypt);
}

void apparentTestAffine(){
    FILE *extra = fopen("extra.txt", "r");
    if(extra == NULL){
        printf("Blad pliku!");
        return;
    }
    FILE *crypto = fopen("crypto.txt", "r");
    if(crypto == NULL){
        printf("Blad pliku!");
        return;
    }
    char chApparent1 = fgetc(extra);
    char chApparent2 = fgetc(extra);
    fclose(extra);
    char chCrypto1 = fgetc(crypto);
    char chCrypto2 = fgetc(crypto);
    FILE *decrypt = fopen("decrypt.txt", "w");
    if(decrypt == NULL){
        printf("Blad pliku!");
        return;
    }
    for(int i=0; i<26; i++){
        for(int j=0; j<26; j++){
            if(chApparent1 == (((chCrypto1 -'a') - i + 26)*oppositeIn26(j))%26 + 'a' || chApparent1 == (((chCrypto1 -'A') - i + 26)*oppositeIn26(j))%26 + 'A'){
                if(chApparent2 == (((chCrypto2 -'a') - i + 26)*oppositeIn26(j))%26 + 'a' || chApparent2 == (((chCrypto2 -'A') - i + 26)*oppositeIn26(j))%26 + 'A'){
                    FILE *key = fopen("key-new.txt", "w");
                    if(key == NULL){
                        printf("Blad pliku!");
                        return;
                    }
                    fprintf(key, "%d %d", i, j);
                    fclose(key);
                    fseek(crypto, 0, SEEK_SET);
                    char text[100];
                    while(fgets(text, 100, crypto) != NULL){
                        decryptAffine(text, i, oppositeIn26(j));
                        fputs(text, decrypt);
                    }
                    i=27;
                    j=27;
                }
            }
        }
    }
    fclose(crypto);
    fclose(decrypt);
}

void cryptographicTestCaesar(){
    FILE *crypto = fopen("crypto.txt", "r");
    if(crypto == NULL){
        printf("Blad pliku!");
        return;
    }
    FILE *decrypt = fopen("decrypt.txt", "w");
    if(decrypt == NULL){
        printf("Blad pliku!");
        return;
    }

    for(int i=0; i<26; i++){
        fprintf(decrypt, "Klucz: %d", i);
        fputc('\n', decrypt);
        char text[100];
        while(fgets(text, 100, crypto) != NULL){
            decryptCaesar(text, i);
            fputs(text, decrypt);
        }
        fputc('\n', decrypt);
        fputc('\n', decrypt);

        fseek(crypto, 0, SEEK_SET);
    }

    fclose(crypto);
    fclose(decrypt);
}

void cryptographicTestAffine(){
    FILE *crypto = fopen("crypto.txt", "r");
    if(crypto == NULL){
        printf("Blad pliku!");
        return;
    }
    FILE *decrypt = fopen("decrypt.txt", "w");
    if(decrypt == NULL){
        printf("Blad pliku!");
        return;
    }

    for(int i=1; i<26; i++){
        for(int j=0; j<26; j++){

            if (j%2!=0 && j!=13){
                fprintf(decrypt, "Klucze: %d %d", i, j);
                fputc('\n', decrypt);
                char text[100];
                while(fgets(text, 100, crypto) != NULL){
                    decryptAffine(text, i, oppositeIn26(j));
                    fputs(text, decrypt);
                }
                fputc('\n', decrypt);
                fputc('\n', decrypt);

                fseek(crypto, 0, SEEK_SET);
            }
        }
    }

    fclose(crypto);
    fclose(decrypt);
}