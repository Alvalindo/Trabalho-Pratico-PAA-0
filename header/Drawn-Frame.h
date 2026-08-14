#ifndef DRAWN_FRAME_H
#define DRAWN_FRAME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../header/Frame_Bank.h"

//Função para desenhar o quadro
char** Create_frame(int xLinha, int yColuna);

//Função de desenha o quadro no terminal
void Drawn_frame(Frame frame);

//Função que desenha os asteriscos aleatoriamente
void Generate_asterisks(Frame* frame, int quantidade);




#endif
