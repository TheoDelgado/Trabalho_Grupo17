#ifndef GULOSO_H
#define GULOSO_H

#include "Grafo.h"
#include <vector>
#include <set>
using namespace std;

class Guloso {
public:
    // Retorna um conjunto dominante independente usando algoritmos gulosos adaptativos
    static vector<char> gulosoAdaptativo(Grafo* grafo);
    static vector<char> gulosoRandomizadoAdaptativo(Grafo* grafo, float alpha);
    static vector<char> gulosoRandomizadoAdaptativoReativo(Grafo* grafo);
};

#endif // GULOSO_H
