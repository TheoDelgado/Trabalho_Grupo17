#ifndef ARESTA_H
#define ARESTA_H

class No; // declaração antecipada

class Aresta {
private:
    No* destino;
    int peso;
    Aresta* proxima;

public:
    Aresta(No* destino, int peso);
    ~Aresta();

    No* getDestino();
    int getPeso();
    Aresta* getProxima();
    void setProxima(Aresta* proxima);
};

#endif