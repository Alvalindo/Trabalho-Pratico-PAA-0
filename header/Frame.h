#ifndef FRAME_H
#define FRAME_H

typedef struct Bank_Frame{
    
    char*** fremes; // Armazena todos os desenhos
    int qt_frames; // Quantidade de desenhos

}Bank_Frame;


//Função para criar o banco de quadros, com apenas 1 quadro.

void Create_bank(Bank_Frame nubank, int xLinha, int yColuna);


#endif