#include "../header/JGP-Convert-ASCII.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../header/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../header/stb_image_resize2.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void Convert_to_ASCII(const char* NomeImage, Frame* frame){

    int xColuna, yLinha, canal, xRed, yRed;
    float fatorX = 0.35;
    float fatorY = 0.15;

    //Carrega a imagem para uma matriz de pixel. Usa escala de cinza.
    unsigned char *imageP = stbi_load(NomeImage, &xColuna ,&yLinha ,&canal, 1);
    if(imageP == NULL){
        printf("Erro ao carregar imagem %s\n", stbi_failure_reason());
        return;
    }

    //Redimensiona a imagem com o fator desejado.
    xRed = (int)(xColuna * fatorX);
    yRed = (int)(yLinha * fatorY);

    //Redimensiona a imagem carregada e gera outra matriz de pixel em escala de cinza.
    unsigned char *imageR = stbir_resize_uint8_srgb(imageP, xColuna, yLinha, 0, NULL, xRed, yRed, 0, STBIR_1CHANNEL);
    if(imageR == NULL){
        printf("Erro ao carregar imagem: %s\n", stbi_failure_reason());
        return;
    }

    //Abribui os dados no quadro.
    frame->frame = Create_frame(yRed+2, xRed+2);
    frame->xColuna = xRed+2;
    frame->yLinha = yRed+2;
    frame->pos = 1;

    //Cria um matriz para amarzenar a transformação para ASCII.
    char matrizF[yRed][xRed];

    //Convertes os pixeis para caracteres e guarda na matriz.
    for(int y = 0; y < yRed; y++){
        for(int x = 0; x < xRed; x++){

            //Calcula o índice para acessar a matriz.
            int indice = (y * xRed + x) * 1;
            unsigned tom = imageR[indice];
    
            //IDEIA: Implementar diferentes conjuntos de caracteres. 
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

    //Desenha a matriz final no quadro.
    Drawns_EsPoint(yRed, xRed, frame, matrizF, 1, 1);

    stbi_write_jpg("saidas/Saida.jpg", xRed, yRed, 1, imageR, 90);

    //Libera todas as imagens da memória.
    stbi_image_free(imageP);
    free(imageR);

    return;
}