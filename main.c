#include <stdio.h>
#include "../header/Drawn-Frame.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../header/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../header/stb_image_resize2.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main (){
    
    FILE *arquivo;
    const char *nomearquivo = "saida.txt";

    int x, y, n, xRed, yRed;
    float fatorX = 0.35;
    float fatorY = 0.15;

    //Carrega a imagem em escala de cinza
    unsigned char *image = stbi_load("teste1.jpg", &x ,&y ,&n, 1);
    if(image == NULL){
        printf("Erro ao carregar imagem: %s\n", stbi_failure_reason());
        return 0;
    }

    //Redimensiona a imagem com o fator desejado
    xRed = (int)(x * fatorX);
    yRed = (int)(y * fatorY);

    unsigned char *imageR = stbir_resize_uint8_srgb(image, x, y, 0, NULL, xRed, yRed, 0, STBIR_1CHANNEL);
    
    Frame quadro = {Create_frame(yRed+2, xRed+2), yRed+2, xRed+2, 1};

    arquivo = fopen(nomearquivo,"w");
    if(arquivo == NULL){
        printf("deu n");
        return 0;
    }

    char matrizF[yRed][xRed];

    for(int y = 0; y < yRed; y++){
        for(int x = 0; x < xRed; x++){

            int indice = (y * xRed + x) * 1;
            unsigned tom = imageR[indice];
    
            //Distribui os caracteres baseado nas cores
            if(tom >= 0 && tom <= 25){
                matrizF[y][x] = '@';
            }else if(tom >= 26 && tom <= 50){
                matrizF[y][x] = '%';
            }else if(tom >= 51 && tom <= 75){
                matrizF[y][x] = '#';
            }else if(tom >= 76 && tom <= 100){
                matrizF[y][x] = '*';
            }else if(tom >= 101 && tom <= 125){
                matrizF[y][x] = '+';
            }else if(tom >= 126 && tom <= 150){
                matrizF[y][x] = '=';
            }else if(tom >= 151 && tom <= 175){
                matrizF[y][x] = '-';
            }else if(tom >= 176 && tom <= 200){
                matrizF[y][x] = ':';
            }else if(tom >= 201 && tom <= 225){
                matrizF[y][x] = '.';
            }else if(tom >= 226 && tom <= 255){
                matrizF[y][x] = ' ';
            }
        }
    }


    Drawns_EsPoint(yRed, xRed, &quadro, matrizF, 1, 1);

    Drawn_frame(quadro, arquivo);


    printf("%d | %d| %d\n",yRed, xRed, n);

    stbi_write_jpg("Cu.jpg", xRed, yRed, 1, imageR, 90);

    fclose(arquivo);
    stbi_image_free(imageR);
    stbi_image_free(image);
}

