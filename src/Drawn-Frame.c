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
            if(i == 0 || i == xLinha-1){
                frame[i][j] = '-';
            }else if (j == 0 || j == yColuna-1){
                frame[i][j] = '|';                
            }else
                frame[i][j] = ' '; 
        }
    }

    return frame;
}

void Drawn_frame(Frame frame){

    for(int i = 0; i < frame.xLinha; i++){
        for(int j = 0; j < frame.yColuna; j++){
            printf("%c", frame.frame[i][j]);
        }
        printf("\n");
    }

}

void Generate_asterisks(Frame* frame, int quantidade){

    srand(time(NULL));

   while(quantidade > 0){
        int xRand = (rand() % frame->xLinha);;
        int yRand = (rand() % frame->yColuna);

        if(xRand > 0 && xRand < frame->xLinha-1 && yRand > 0 && yRand < frame->yColuna-1){
            if(frame->frame[xRand][yRand] == ' ' && quantidade > 0){
                    frame->frame[xRand][yRand] = '*';
                    quantidade--;
            }      
        } 
   }
}