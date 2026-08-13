#ifndef DRAWN_FRAME_H
#define DRAWN_FRAME_H

#include <stdio.h>
#include <stdlib.h>

//Função para desenhar o quadro
char** Create_frame(int xLinha, int yColuna);

//Função de desenha o quadro no terminal
void Drawn_frame(char** frame, int xLinha, int yColuna);



#endif
