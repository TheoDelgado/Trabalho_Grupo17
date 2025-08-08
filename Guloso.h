#ifndef GULOSO_H
#define GULOSO_H

#include "Grafo.h"
#include <vector>
#include <set>
using namespace std;

class Guloso {
public:
    // Retorna um conjunto dominante independente usando algoritmo guloso adaptativo
    static set<char> gulosoAdaptativo(Grafo* grafo);
    static set<char> gulosoRandomizadoAdaptativo(Grafo* grafo, float alpha);
    static set<char> gulosoRandomizadoAdaptativoReativo(Grafo* grafo);
};

#endif // GULOSO_H
