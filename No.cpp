#include "No.h"

No::No(char id) : id(id), peso(0), primeiraAresta(nullptr), proximo(nullptr) {}

No::~No() {
    Aresta* atual = primeiraAresta;
    while (atual != nullptr) {
        Aresta* temp = atual;
        atual = atual->getProxima();
        delete temp;
    }
}

char No::getId() { return id; }
int No::getPeso() { return peso; }
void No::setPeso(int p) { peso = p; }

Aresta* No::getPrimeiraAresta() { return primeiraAresta; }

void No::adicionarAresta(No* destino, int peso) {
    Aresta* nova = new Aresta(destino, peso);
    nova->setProxima(primeiraAresta);
    primeiraAresta = nova;
}

No* No::getProximo() { return proximo; }
void No::setProximo(No* p) { proximo = p; }
