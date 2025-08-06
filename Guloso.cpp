#include "Guloso.h"
#include <set>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

// Implementação do algoritmo guloso adaptativo
set<char> Guloso::gulosoAdaptativo(Grafo* grafo) {
    set<char> conjuntoDominante;
    set<char> verticesDominados;
    map<char, No*> mapa;

    // Cria um mapa de id para ponteiro de vértice
    for (char v : grafo->getVertices()) {
        mapa[v] = grafo->buscarNo(v);
    }

    // Enquanto nem todos os vértices estiverem dominados
    while (verticesDominados.size() < mapa.size()) {
        char melhorNo = 0;
        int melhorGanho = -1;

        // Para cada vértice ainda não dominado nem no conjunto
        for (const auto& [id, no] : mapa) {
            if (conjuntoDominante.count(id) || verticesDominados.count(id)) continue;

            set<char> candidatos;
            candidatos.insert(id);

            // Adiciona os vizinhos que ainda não foram dominados
            Aresta* a = no->getPrimeiraAresta();
            while (a) {
                char vizinho = a->getDestino()->getId();
                if (!verticesDominados.count(vizinho)) {
                    candidatos.insert(vizinho);
                }
                a = a->getProxima();
            }

            int ganho = candidatos.size();
            if (ganho > melhorGanho) {
                melhorGanho = ganho;
                melhorNo = id;
            }
        }

        if (melhorNo == 0) break; // segurança contra erro

        // Adiciona o melhor vértice ao conjunto dominante
        conjuntoDominante.insert(melhorNo);
        verticesDominados.insert(melhorNo);

        // Marca os vizinhos como dominados
        Aresta* a = mapa[melhorNo]->getPrimeiraAresta();
        while (a) {
            verticesDominados.insert(a->getDestino()->getId());
            a = a->getProxima();
        }
    }

    return conjuntoDominante;
}
