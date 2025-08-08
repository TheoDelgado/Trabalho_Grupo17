#include "Guloso.h"
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <ctime>
#include <limits>
#include <random>
#include <numeric>
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

set<char> Guloso::gulosoRandomizadoAdaptativo(Grafo* grafo, float alpha) {
    const int numIter = 10;
    set<char> melhorSolucao;
    int melhorTamanho = numeric_limits<int>::max();

    set<char> vertices = grafo->getVertices();
    srand(time(0));

    for (int iter = 0; iter < numIter; ++iter) {
        set<char> solucao;
        set<char> dominados;
        set<char> LC = vertices;

        while (dominados.size() < vertices.size() && !LC.empty()) {
            vector<pair<char, int>> candidatosValidos;

            // Avalia os vértices candidatos
            for (char v : LC) {
                if (dominados.count(v)) continue;

                // Verifica independência (v não pode ser adjacente a ninguém já na solução)
                bool independente = true;
                for (char vizinho : grafo->getAdjacentes(v)) {
                    if (solucao.count(vizinho)) {
                        independente = false;
                        break;
                    }
                }
                if (!independente) continue;

                // Calcula o ganho: quantos vértices v e seus vizinhos dominam
                int ganho = 0;
                if (!dominados.count(v)) ganho++;
                for (char vizinho : grafo->getAdjacentes(v)) {
                    if (!dominados.count(vizinho)) ganho++;
                }

                candidatosValidos.emplace_back(v, ganho);
            }

            if (candidatosValidos.empty()) break;

            // Ordena os candidatos por maior ganho
            sort(candidatosValidos.begin(), candidatosValidos.end(),
                [](const auto& a, const auto& b) { return a.second > b.second; });

            int limite = max(1, int(alpha * candidatosValidos.size()));
            int k = rand() % limite;
            char escolhido = candidatosValidos[k].first;

            // Adiciona o vértice à solução e marca ele e seus vizinhos como dominados
            solucao.insert(escolhido);
            dominados.insert(escolhido);
            for (char vizinho : grafo->getAdjacentes(escolhido))
                dominados.insert(vizinho);

            LC.erase(escolhido); // remove da lista de candidatos
        }

        // Atualiza a melhor solução até agora
        if (solucao.size() < melhorTamanho) {
            melhorSolucao = solucao;
            melhorTamanho = solucao.size();
        }
    }

    return melhorSolucao;
}

set<char> Guloso::gulosoRandomizadoAdaptativoReativo(Grafo* grafo) {
    const int numIter = 10;
    const int bloco = 1;
    vector<float> alfas = {0.05f, 0.1f, 0.15f, 0.3f, 0.5f};
    int m = alfas.size();

    vector<float> M(m, 0.0f);   // médias
    vector<float> P(m, 1.0f / m); // probabilidades iniciais
    set<char> melhorSolucao;
    int melhorTamanho = numeric_limits<int>::max();

    set<char> vertices = grafo->getVertices();
    srand(time(0));

    auto escolherAlphaIndex = [&]() -> int {
        float r = static_cast<float>(rand()) / RAND_MAX;
        float acumulado = 0.0f;
        for (int i = 0; i < m; ++i) {
            acumulado += P[i];
            if (r <= acumulado) return i;
        }
        return m - 1;
    };

    for (int iter = 1; iter <= numIter; ++iter) {
        if (iter % bloco == 0) {
            float soma = 0.0f;
            for (int i = 0; i < m; ++i)
                soma += 1.0f / (M[i] + 1e-6f); // evitar divisão por zero

            for (int i = 0; i < m; ++i)
                P[i] = (1.0f / (M[i] + 1e-6f)) / soma;
        }

        int alphaIndex = escolherAlphaIndex();
        float alpha = alfas[alphaIndex];

        set<char> solucao;
        set<char> dominados;
        set<char> LC = vertices;

        while (dominados.size() < vertices.size() && !LC.empty()) {
            vector<pair<char, int>> candidatosValidos;

            for (char v : LC) {
                if (dominados.count(v)) continue;

                bool independente = true;
                for (char vizinho : grafo->getAdjacentes(v)) {
                    if (solucao.count(vizinho)) {
                        independente = false;
                        break;
                    }
                }
                if (!independente) continue;

                int ganho = 0;
                if (!dominados.count(v)) ganho++;
                for (char vizinho : grafo->getAdjacentes(v)) {
                    if (!dominados.count(vizinho)) ganho++;
                }

                candidatosValidos.emplace_back(v, ganho);
            }

            if (candidatosValidos.empty()) break;

            sort(candidatosValidos.begin(), candidatosValidos.end(),
                [](const auto& a, const auto& b) { return a.second > b.second; });

            int limite = max(1, int(alpha * candidatosValidos.size()));
            int k = rand() % limite;
            char escolhido = candidatosValidos[k].first;

            solucao.insert(escolhido);
            dominados.insert(escolhido);
            for (char vizinho : grafo->getAdjacentes(escolhido))
                dominados.insert(vizinho);

            LC.erase(escolhido);
        }

        int tamSol = solucao.size();
        M[alphaIndex] = (M[alphaIndex] * (iter - 1) + tamSol) / iter;

        if (tamSol < melhorTamanho) {
            melhorTamanho = tamSol;
            melhorSolucao = solucao;
        }
    }

    return melhorSolucao;
}