#include "Guloso.h"
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <ctime>
#include <limits>
#include <random>
#include <numeric>
#include <unordered_set>
#include <chrono>
#include <iostream>
using namespace std;

vector<char> Guloso::gulosoAdaptativo(Grafo* grafo) {
    auto inicio = chrono::high_resolution_clock::now();

    vector<char> conjuntoDominante;
    unordered_set<char> conjuntoDominanteSet;
    unordered_set<char> verticesDominados;
    map<char, No*> mapa;

    for (char v : grafo->getVertices()) {
        mapa[v] = grafo->buscarNo(v);
    }

    while (verticesDominados.size() < mapa.size()) {
        char melhorNo = 0;
        int melhorGanho = -1;

        for (const auto& [id, no] : mapa) {
            if (conjuntoDominanteSet.count(id) || verticesDominados.count(id)) continue;

            unordered_set<char> candidatos;
            candidatos.insert(id);

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

        if (melhorNo == 0) break;

        conjuntoDominante.push_back(melhorNo);
        conjuntoDominanteSet.insert(melhorNo);
        verticesDominados.insert(melhorNo);

        Aresta* a = mapa[melhorNo]->getPrimeiraAresta();
        while (a) {
            verticesDominados.insert(a->getDestino()->getId());
            a = a->getProxima();
        }
    }

    auto fim = chrono::high_resolution_clock::now();
    auto duracao = chrono::duration_cast<chrono::microseconds>(fim - inicio).count();
    cout << "Tempo Guloso Adaptativo: " << duracao << " microsegundos\n";

    return conjuntoDominante;
}

vector<char> Guloso::gulosoRandomizadoAdaptativo(Grafo* grafo, float alpha) {
    auto inicio = chrono::high_resolution_clock::now();

    const int numIter = 1;
    vector<char> melhorSolucao;
    int melhorTamanho = numeric_limits<int>::max();

    set<char> vertices = grafo->getVertices();
    srand(time(0));

    for (int iter = 0; iter < numIter; ++iter) {
        vector<char> solucao;
        set<char> dominados;
        set<char> LC = vertices;

        while (dominados.size() < vertices.size() && !LC.empty()) {
            vector<pair<char, int>> candidatosValidos;

            for (char v : LC) {
                if (dominados.count(v)) continue;

                bool independente = true;
                for (char vizinho : grafo->getAdjacentes(v)) {
                    if (find(solucao.begin(), solucao.end(), vizinho) != solucao.end()) {
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

            solucao.push_back(escolhido);
            dominados.insert(escolhido);
            for (char vizinho : grafo->getAdjacentes(escolhido))
                dominados.insert(vizinho);

            LC.erase(escolhido);
        }

        if ((int)solucao.size() < melhorTamanho) {
            melhorSolucao = solucao;
            melhorTamanho = solucao.size();
        }
    }

    auto fim = chrono::high_resolution_clock::now();
    auto duracao = chrono::duration_cast<chrono::microseconds>(fim - inicio).count();
    cout << "Tempo Guloso Randomizado Adaptativo: " << duracao << " microsegundos\n";

    return melhorSolucao;
}

vector<char> Guloso::gulosoRandomizadoAdaptativoReativo(Grafo* grafo) {
    auto inicio = chrono::high_resolution_clock::now();

    const int numIter = 1;
    const int bloco = 1;
    vector<float> alfas = {0.2f, 0.5f, 0.7f};
    int m = alfas.size();

    vector<float> M(m, 0.0f);
    vector<float> P(m, 1.0f / m);
    vector<char> melhorSolucao;
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
                soma += 1.0f / (M[i] + 1e-6f);
            for (int i = 0; i < m; ++i)
                P[i] = (1.0f / (M[i] + 1e-6f)) / soma;
        }

        int alphaIndex = escolherAlphaIndex();
        float alpha = alfas[alphaIndex];

        vector<char> solucao;
        set<char> dominados;
        set<char> LC = vertices;

        while (dominados.size() < vertices.size() && !LC.empty()) {
            vector<pair<char, int>> candidatosValidos;

            for (char v : LC) {
                if (dominados.count(v)) continue;

                bool independente = true;
                for (char vizinho : grafo->getAdjacentes(v)) {
                    if (find(solucao.begin(), solucao.end(), vizinho) != solucao.end()) {
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

            solucao.push_back(escolhido);
            dominados.insert(escolhido);
            for (char vizinho : grafo->getAdjacentes(escolhido))
                dominados.insert(vizinho);

            LC.erase(escolhido);
        }

        int tamSol = solucao.size();
        M[alphaIndex] = (M[alphaIndex] * (iter - 1) + tamSol) / iter;

        if (tamSol < melhorTamanho && tamSol > 0) {
            melhorTamanho = tamSol;
            melhorSolucao = solucao;
        }
    }

    auto fim = chrono::high_resolution_clock::now();
    auto duracao = chrono::duration_cast<chrono::microseconds>(fim - inicio).count();
    cout << "Tempo Guloso Randomizado Adaptativo Reativo: " << duracao << " microsegundos\n";

    return melhorSolucao;
}
