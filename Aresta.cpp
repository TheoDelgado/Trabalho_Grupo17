#include "Aresta.h"
#include "No.h"

Aresta::Aresta(No* destino, int peso)
    : destino(destino), peso(peso), proxima(nullptr) {}

Aresta::~Aresta() {}

No* Aresta::getDestino() { return destino; }
int Aresta::getPeso() { return peso; }
Aresta* Aresta::getProxima() { return proxima; }
void Aresta::setProxima(Aresta* p) { proxima = p; }