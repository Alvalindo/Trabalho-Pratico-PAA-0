#include "../header/Drawn-Frame.h"


char** Create_frame(int yLinha, int xColuna){

    // Alocando linha e colunas da matriz;
    char** frame = malloc(yLinha * sizeof(char *));
    if(frame == NULL){
            perror("Erro na alocação");
            return NULL;
        }

    for(int i = 0; i < yLinha; i++){
        frame[i] = malloc(xColuna * sizeof(char));

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

    for(int i = 0; i < yLinha; i++){
        for(int j = 0; j < xColuna; j++){
            if(i == 0 || i == yLinha-1){
                frame[i][j] = '-';
            }else if (j == 0 || j == xColuna-1){
                frame[i][j] = '|';                
            }else
                frame[i][j] = ' '; 
        }
    }

    return frame;
}

void Drawn_frame(Frame frame){

    for(int i = 0; i < frame.yLinha; i++){
        for(int j = 0; j < frame.xColuna; j++){
            printf("%c", frame.frame[i][j]);
        }
        printf("\n");
    }

}

void Drawn_frame_txt(Frame frame, FILE *arquivo){

    for(int i = 0; i < frame.yLinha; i++){
        for(int j = 0; j < frame.xColuna; j++){
            fprintf(arquivo, "%c", frame.frame[i][j]);
        }
        fprintf(arquivo,"\n");
    }

}

void GenerateALL_drawns(int xldesenho, int yCdesenho, Frame *frame, char desenho[xldesenho][yCdesenho], int quantidade){

    srand(time(NULL));

    //Verifica se a quantidade de desenho cabe no quadro
    if(((xldesenho * yCdesenho) * quantidade) <= ((frame->yLinha - 2) * (frame->xColuna - 2))){
        //Colar o desenho na quadro
        while(quantidade > 0){
            int xRand = rand() % (frame->yLinha);
            int yRand = rand() % (frame->xColuna);

            //Bloqueia as bordas
            if(xRand > 0 && xRand < frame->yLinha-1 && yRand > 0 && yRand < frame->xColuna-1){

                //Identifica se o desenho cabe na posição gerada aleatoriamente
                if(xRand + xldesenho < frame->yLinha && yRand + yCdesenho < frame->xColuna){

                    int colision = 0;

                    //Verifica colisão
                    for(int i = 0; i < xldesenho; i++){
                        for(int j = 0; j < yCdesenho; j++){
                            if(frame->frame[i+xRand][j+yRand] != ' ' && desenho[i][j] != ' '){
                                colision = 1;
                               //printf("Colidiu\n");
                            }
                        }
                    }

                    //Se não colidiu desenha o desenho
                    if(colision == 0){
                    for(int i = 0; i < xldesenho; i++){
                        for(int j = 0; j < yCdesenho; j++){
                                frame->frame[i+xRand][j+yRand] = desenho[i][j];
                                colision = 0;
                            }
                        }
                        quantidade--;
                    }    
                }
            } 
        }
    }else{
        printf("Quantidade exedida, utilize no maximo: %d figuras\n", (frame->yLinha - 2) * (frame->xColuna - 2));
        return;
    }
}

void Drawns_EsPoint(int xldesenho, int yCdesenho, Frame *frame, char desenho[xldesenho][yCdesenho], int x, int y){

    for(int i = 0; i < xldesenho; i++){
        for(int j = 0; j < yCdesenho; j++){
            frame->frame[i+x][j+y] = desenho[i][j];                        
        }
    }
}