#include "../header/Drawn-Frame.h"


char** Create_frame(int xLinha, int yColuna){

    // Alocando linha e colunas da matriz;
    char** frame = malloc(xLinha * sizeof(char *));
    if(frame == NULL){
            perror("Erro na alocação");
            return NULL;
        }

    for(int i = 0; i < xLinha; i++){
        frame[i] = malloc(yColuna * sizeof(char));

        //Verifica erro, se positivo, libera a memória já alocada
        if(frame[i] == NULL){
            perror("Erro na alocação");
            for(int j = 0; j < i; i++){
                free(frame[j]);
            }
            free(frame);
            return NULL;
        }
    }

    //Desenha a moldura do quadro

    for(int i = 0; i < xLinha; i++){
        for(int j = 0; j < yColuna; j++){
            if(i == (xLinha-xLinha) || i == (xLinha-1)){
                frame[i][j] = '-';
            }else if (j == (yColuna-yColuna) || j == (yColuna-1)){
                frame[i][j] = '|';                
            }else
                frame[i][j] = ' '; 
        }
    }

    return frame;
}

void Drawn_frame(char** frame, int xLinha, int yColuna){

    for(int i = 0; i < xLinha; i++){
        for(int j = 0; j < yColuna; j++){
            printf("%c", frame[i][j]);
        }
        printf("\n");
    }

}