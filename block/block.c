#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>

#define BLACK 0
#define WHITE -1

//trzeba zainstalować libssl-dev
//gcc block.c -o block -lcrypto

//autor - Cezary Kwella

typedef struct pixel{
    int red;
    int green;
    int blue;
}pixel;

int main(){
    //read bitmap file without header and store it in rgb pixel array 
    FILE *fptr;
    fptr = fopen("plain.bmp", "rb");
    if (fptr == NULL){
        printf("Error! opening file");
        return 1;
    }
    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, fptr);
    fclose(fptr);

    string[fsize] = 0;

    //read header
    char header[54];
    for (int i = 0; i < 54; i++){
        header[i] = string[i];
    }

    //read width and height
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];

    //read rgb pixel array
    int size = 3 * width * height;
    char rgb[size];
    for (int i = 0; i < size; i++){
        rgb[i] = string[i + 54];
    }

    pixel pixels[width][height];
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            pixels[i][j].blue = string[i * 3 + j * 3 * width + 54];
            pixels[i][j].green = string[i * 3 + j * 3 * width + 55];
            pixels[i][j].red = string[i * 3 + j * 3 * width + 56];
            //printf("(%d %d %d), ", pixels[i][j].red, pixels[i][j].green, pixels[i][j].blue);
        }
        //printf("\n");
    }

    //make array of 8x8 pixel blocks
    int block_width = width / 8;
    int block_height = height / 8;
    pixel blocks[block_width][block_height][8][8];
    for (int i = 0; i < block_width; i++){
        for (int j = 0; j < block_height; j++){
            for (int k = 0; k < 8; k++){
                for (int l = 0; l < 8; l++){
                    blocks[i][j][k][l] = pixels[i * 8 + k][j * 8 + l];
                }
            }
        }
    }



    //create copy of blocks array
    pixel blocks_copy[block_width][block_height][8][8];
    for (int i = 0; i < block_width; i++){
        for (int j = 0; j < block_height; j++){
            for (int k = 0; k < 8; k++){
                for (int l = 0; l < 8; l++){
                    blocks_copy[i][j][k][l] = blocks[i][j][k][l];
                }
            }
        }
    }



    ///////////////////////////////ECB
    for (int i = 0; i < block_width; i++){
        for (int j = 0; j < block_height; j++){
            unsigned char c[MD5_DIGEST_LENGTH];
            MD5((unsigned char*)&blocks_copy[i][j], sizeof(blocks_copy[i][j]), c);
            for (int k = 0; k < 8; k++){
                for (int l = 0; l < 8; l++){
                    if (c[k] % 2 == 0){
                        blocks_copy[i][j][k][l].red = BLACK;
                        blocks_copy[i][j][k][l].green = BLACK;
                        blocks_copy[i][j][k][l].blue = BLACK;
                    }
                    else{
                        blocks_copy[i][j][k][l].red = WHITE;
                        blocks_copy[i][j][k][l].green = WHITE;
                        blocks_copy[i][j][k][l].blue = WHITE;
                    }
                }
            }
        }
    }

    //save blocks_copy to pixels
    for (int i = 0; i < block_width; i++){
        for (int j = 0; j < block_height; j++){
            for (int k = 0; k < 8; k++){
                for (int l = 0; l < 8; l++){
                    pixels[i * 8 + k][j * 8 + l] = blocks_copy[i][j][k][l];
                }
            }
        }
    }

    //save pixel array to rgb pixel array
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            rgb[3 * (i + j*width)] = pixels[i][j].blue;
            rgb[3 * (i + j*width) + 1] = pixels[i][j].green;
            rgb[3 * (i + j*width) + 2] = pixels[i][j].red;
        }
    }

    //save rgb pixel array to file
    FILE *fptr2;
    fptr2 = fopen("ecb_crypto.bmp", "wb");
    if (fptr2 == NULL){
        printf("Error! opening file");
        return 1;
    }
    fwrite(header, 54, 1, fptr2);
    fwrite(rgb, size, 1, fptr2);
    fclose(fptr2);


    //////////////////////////////CBC
    pixel previous_block[8][8];
    for (int i = 0; i < block_width; i++){
        for (int j = 0; j < block_height; j++){
            unsigned char c[MD5_DIGEST_LENGTH];
            MD5((unsigned char*)&blocks[i][j], sizeof(blocks[i][j]), c);
            for (int k = 0; k < 8; k++){
                for (int l = 0; l < 8; l++){
                    if (c[k] % 2 == 0){
                        blocks[i][j][k][l].red = BLACK;
                        blocks[i][j][k][l].green = BLACK;
                        blocks[i][j][k][l].blue = BLACK;
                    }
                    else{
                        blocks[i][j][k][l].red = WHITE;
                        blocks[i][j][k][l].green = WHITE;
                        blocks[i][j][k][l].blue = WHITE;
                    }
                    if (i == 0 && j == 0){
                        previous_block[k][l] = blocks[i][j][k][l];
                    }
                    else{
                        blocks[i][j][k][l].red = blocks[i][j][k][l].red ^ previous_block[k][l].red;
                        blocks[i][j][k][l].green = blocks[i][j][k][l].green ^ previous_block[k][l].green;
                        blocks[i][j][k][l].blue = blocks[i][j][k][l].blue ^ previous_block[k][l].blue;
                        previous_block[k][l] = blocks[i][j][k][l];
                    }
                }
            }
        }
    }    

    //save blocks to pixels
    for (int i = 0; i < block_width; i++){
        for (int j = 0; j < block_height; j++){
            for (int k = 0; k < 8; k++){
                for (int l = 0; l < 8; l++){
                    pixels[i * 8 + k][j * 8 + l] = blocks[i][j][k][l];
                }
            }
        }
    }

    //save pixel array to rgb pixel array
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            rgb[3 * (i + j*width)] = pixels[i][j].blue;
            rgb[3 * (i + j*width) + 1] = pixels[i][j].green;
            rgb[3 * (i + j*width) + 2] = pixels[i][j].red;
        }
    }
    
    FILE *fptr3;
    fptr3 = fopen("cbc_crypto.bmp", "wb");
    if (fptr3 == NULL){
        printf("Error! opening file");
        return 1;
    }
    fwrite(header, 54, 1, fptr3);
    fwrite(rgb, size, 1, fptr3);
    fclose(fptr3);


    return 0;
}