#ifndef GERENCIADOR_H
#define GERENCIADOR_H

#include "Grafo.h"
#include <iostream>
#include <algorithm>
using namespace std;

class Gerenciador {
public:
    static void comandos(Grafo* grafo);
    static char get_id_entrada();
};


#endif //GERENCIADOR_H