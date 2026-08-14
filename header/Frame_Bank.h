#ifndef FRAME_BANK_H
#define FRAME_BANK_H

#include "../header/Frame.h"

typedef struct node{
    
    Frame Uframe;
    struct node* prox; 

}node;

typedef struct Frame_Bank{
    node* inicio;
    node* fim;
    int qt_frames; // Quantidade de quadros
}Frame_Bank;


//Lista enacadeada

void Isertion_bank(Frame_Bank* Bframe, Frame Uframe);

node* Search_bank(Frame_Bank* Bframe, int pos);

void Remove_bank(Frame_Bank* Bframe, int pos);

void Free_bank(Frame_Bank* Bframe);

#endif 