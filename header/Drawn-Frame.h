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

//Função de desenha o quadro no txt
void Drawn_frame_txt(Frame frame, FILE *arquivo);

//Função que desenha os asteriscos aleatoriamente
void Generate_asterisks(Frame* frame, int quantidade);

//Função que desenha a matriz que foi passada pelo parâmetro
void GenerateALL_drawns(int xldesenho, int yCdesenho, Frame *frame, char desenho[xldesenho][yCdesenho], int quantidade);

//Função que desenha em um ponto que o usuario dejesar
void Drawns_EsPoint(int xldesenho, int yCdesenho, Frame *frame, char desenho[xldesenho][yCdesenho], int x, int y);

#endif
