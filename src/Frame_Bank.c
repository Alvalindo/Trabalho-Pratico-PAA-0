#include <stdio.h>
#include "../header/Frame_Bank.h"


void Isertion_bank(Frame_Bank* Bframe, Frame Uframe){
    
    //Alocando um nó
    node* novo = (node*) malloc(sizeof(node));
    if(novo == NULL){
        perror("Erro na alocação");
        return;
    }

    //Atribuindo os dados
    novo->Uframe = Uframe;
    novo->prox = NULL;

    //Verificando para enserir um novo nó
    if(Bframe->inicio == Bframe->fim){
        Bframe->inicio = novo;
        Bframe->fim = Bframe->inicio;
    }else{
        Bframe->fim->prox = novo;
        Bframe->fim = novo;
    }

    Bframe->qt_frames++;
    
}

node* Search_bank(Frame_Bank* Bframe, int pos){
    if(Bframe->inicio == NULL){
        printf("Lista vazia\n");
        return NULL;
    }
    node* aux = Bframe->inicio;
    while(aux != NULL){
        if(aux->Uframe.pos == pos){
            return aux;
        }
        aux = aux->prox;
    }

    printf("elemento não existe\n");
    return NULL;
}

void Remove_bank(Frame_Bank* Bframe, int pos){
    if(Bframe->inicio == NULL){
        printf("Lista vazia\n");
        return;
    }

    node* aux = Bframe->inicio;
    node* anterior = NULL;

    while(aux != NULL && aux->Uframe.pos != pos){
        anterior = aux;
        aux = aux->prox;
    }

    if(aux == NULL){
        printf("Elemeno não existe!\n");
    }

    // Removendo do início
    if(anterior == NULL){
        Bframe->inicio = aux->prox;

    // Removendo meio ou final
    }else{
        anterior->prox = aux->prox;
    }

    if(aux == Bframe->fim){
        Bframe->fim = anterior;
    }

    free(aux);
    
}

void Free_bank(Frame_Bank* Bframe){
    if(Bframe->inicio == NULL){
        printf("Lista vazia\n");
        return;
    }

    node* aux = Bframe->inicio;

    while(aux != NULL){

        node* proximo = aux->prox;

        for(int i = 0; i < aux->Uframe.yLinha; i++){
            free(aux->Uframe.frame[i]);
        }
        free(aux->Uframe.frame);

        aux = proximo;

        free(aux);
    }

    Bframe->inicio = NULL;
    Bframe->fim = NULL;
}