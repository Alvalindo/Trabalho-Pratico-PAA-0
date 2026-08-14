#include <stdio.h>
#include "../header/Drawn-Frame.h"

int main (){

    int y = 80;
    int x = 20;

    Frame quadro = {Create_frame(x,y), x, y, 1};

    Generate_asterisks(&quadro, 3);

    Drawn_frame(quadro);
}

