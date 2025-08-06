#ifndef NO_H
#define NO_H

#include "Aresta.h"
#include <vector>

class No {
private:
    char id;
    int peso;  // para grafos ponderados nos vértices
    Aresta* primeiraAresta;
    No* proximo;

public:
    No(char id);
    ~No();

    char getId();
    int getPeso();
    void setPeso(int peso);

    Aresta* getPrimeiraAresta();
    void adicionarAresta(No* destino, int peso);
    No* getProximo();
    void setProximo(No* proximo);
};

#endif
