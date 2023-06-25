//author: Cezary Kwella
#include <stdio.h>
#include <math.h>

/*
    Program potrzebuje pliku hash.txt w tym samym folderze co plik skrot.c

    Plik hash.txt musi miec postac:

        * tytul (wywołanie funkcji skrótu, np sha1sum)
        * skrot1 (stworzony na podstawie personal.txt)
        * skrot2 (stworzony na podstawie personal_.txt)
        (puste linie pomiedzy kazdym zestawem)

*/

int calculateDifference(char a, char b){
    int diff = 0;
    int a1 = a;
    int b1 = b;
    if(a1 >= 97){
        a1 -= 87;
    }else{
        a1 -= 48;
    }
    if(b1 >= 97){
        b1 -= 87;
    }else{
        b1 -= 48;
    }
    int i = 0;
    while(i < 4){
        if((a1 & 1) != (b1 & 1)){
            diff++;
        }
        a1 >>= 1;
        b1 >>= 1;
        i++;
    }
    return diff;


}

int main(){
    FILE *fp = fopen("hash.txt", "r");
    if(fp == NULL){
        printf("Brak pliku hash.txt");
        return 1;
    }
    FILE *fp2 = fopen("diff.txt", "w");
    char title[100];
    char hash1[130];
    char hash2[130];
    while(fscanf(fp, "%s %s %s", title, hash1, hash2) != EOF){
        int i = 0;
        int diff = 0;
        while(hash1[i] != '\0'){
            diff+=calculateDifference(hash1[i], hash2[i]);
            i++;
        }
        fprintf(fp2, "%s\n%s\n%s\nLiczba rozniacych sie bitow: %d z %d. Procentowo: %d%%\n\n", title, hash1, hash2, diff, i*4, (int)(100-round((1-(float)diff/(i*4))*100)));
    }

}