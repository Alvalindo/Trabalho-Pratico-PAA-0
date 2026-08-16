#include <stdio.h>
#include "../header/JGP-Convert-ASCII.h"

int main (){
    
    FILE *arquivo;

    const char *nomearquivo = "saidas/saida.txt";

    const char *nomeimage = "teste.jpg";
    
    Frame quadro;

    arquivo = fopen(nomearquivo,"w");
    if(arquivo == NULL){
        printf("deu n");
        return 0;
    }

    Convert_to_ASCII(nomeimage, &quadro);
    
    Drawn_frame_txt(quadro, arquivo);

    fclose(arquivo);
   
}

