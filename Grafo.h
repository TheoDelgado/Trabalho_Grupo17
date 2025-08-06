#ifndef GRAFO_H
#define GRAFO_H

#include "No.h"
#include <map>
#include <set>
#include <vector>
#include <string>
using namespace std;

class Grafo {
private:
    No* primeiro;
    bool direcionado;
    bool ponderadoVertices;
    bool ponderadoArestas;
    

public:
    Grafo(bool dir, bool pV, bool pA);
    ~Grafo();

    void adicionarNo(char id);
    void adicionarAresta(char origem, char destino, int peso = 1);
    No* buscarNo(char id) const;
    set<char> getVertices() const;
    void imprimir() const;
    void imprimirVetor(const vector<char>& vec);
    Grafo DFS_arvore(char origemId) const;
    void medidasGrafo() const;
    bool getPonderadoArestas() const;
};

vector<char> fechoTransitivoDFS(No* inicio, map<char, No*>& mapa);
vector<char> fechoTransitivoIndireto(No* alvo, const map<char, No*>& mapa);
vector<char> caminhoMinimoDijkstra(No* origem, No* destino, const map<char, No*>& mapa);
vector<char> caminhoMinimoFloyd(No* origem, No* destino, const map<char, No*>& mapa);
Grafo AGM_Prim(const map<char, No*>& mapa);
Grafo AGM_Kruskal(const map<char, No*>& mapa);

#endif