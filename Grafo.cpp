#include "Grafo.h"
#include "Aresta.h"
#include <iostream>
#include <queue>
#include <stack>
#include <limits>
#include <algorithm>
#include <tuple>
#include <set>
#include <map>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

// construtor
Grafo::Grafo(bool dir, bool pV, bool pA)
    : primeiro(nullptr), // primeiro nó (lista encadeada de vértices)
      direcionado(dir),
      ponderadoVertices(pV),
      ponderadoArestas(pA) {}

// destrutor
Grafo::~Grafo() {
    No* atual = primeiro;
    while (atual) {
        No* tmp = atual;
        atual = atual->getProximo();
        delete tmp; // libera cada vértice (que também libera suas arestas)
    }
}

void Grafo::adicionarNo(char id) {
    if (buscarNo(id)) return; // já existe
    No* novo = new No(id);
    novo->setProximo(primeiro); // insere na lista encadeada de vértices
    primeiro = novo;
}

No* Grafo::buscarNo(char id) const {
    No* atual = primeiro;
    while (atual) {
        if (atual->getId() == id) return atual;
        atual = atual->getProximo();
    }
    return nullptr;
}

void Grafo::adicionarAresta(char origem, char destino, int peso) {
    adicionarNo(origem);  // garante que os nós existam
    adicionarNo(destino);
    No* o = buscarNo(origem);
    No* d = buscarNo(destino);
    if (!ponderadoArestas) peso = 1;
    o->adicionarAresta(d, peso);
    if (!direcionado) d->adicionarAresta(o, peso); // se não direcionado, adiciona volta
}

set<char> Grafo::getVertices() const {
    set<char> v;
    No* atual = primeiro;
    while (atual) {
        v.insert(atual->getId());
        atual = atual->getProximo();
    }
    return v;
}

set<char> Grafo::getAdjacentes(char id) const {
    std::set<char> adjacentes;
    No* no = buscarNo(id);
    if (!no) return adjacentes;

    Aresta* aresta = no->getPrimeiraAresta();
    while (aresta) {
        adjacentes.insert(aresta->getDestino()->getId());
        aresta = aresta->getProxima();
    }
    return adjacentes;
}


bool Grafo::getPonderadoArestas() const {
    return ponderadoArestas;
}


// imprime um vetor de caracteres
void Grafo::imprimirVetor(const vector<char>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i];
        if (i + 1 < vec.size()) cout << ",";
    }
    cout << "\n";
}

// imprime a lista de adjacência completa
void Grafo::imprimir() const {
    No* atual = primeiro;
    while (atual) {
        cout << atual->getId() << ": ";
        Aresta* a = atual->getPrimeiraAresta();
        bool primeiroVizinho = true;
        while (a) {
            if (!primeiroVizinho) cout << " -> ";
            cout << a->getDestino()->getId();
            primeiroVizinho = false;
            a = a->getProxima();
        }
        cout << "\n";
        atual = atual->getProximo();
    }
}

// fecho transitivo direto por DFS
vector<char> fechoTransitivoDFS(No* inicio, map<char, No*>& mapa) {
    set<char> visitado;
    stack<No*> pilha;
    vector<char> resultado;

    pilha.push(inicio);
    visitado.insert(inicio->getId());

    while (!pilha.empty()) {
        No* atual = pilha.top(); pilha.pop();
        Aresta* a = atual->getPrimeiraAresta();
        while (a) {
            char dest = a->getDestino()->getId();
            if (!visitado.count(dest)) {
                visitado.insert(dest);
                resultado.push_back(dest);
                pilha.push(a->getDestino());
            }
            a = a->getProxima();
        }
    }
    return resultado;
}

// fecho transitivo indireto
vector<char> fechoTransitivoIndireto(No* alvo, const map<char, No*>& mapa) {
    set<char> resultado;
    for (const auto& [chave, no] : mapa) {
        vector<char> fecho = fechoTransitivoDFS(no, const_cast<map<char, No*>&>(mapa));
        for (char c : fecho) {
            if (c == alvo->getId()) {
                resultado.insert(chave);
                break;
            }
        }
    }
    return vector<char>(resultado.begin(), resultado.end());
}

// Dijkstra
vector<char> caminhoMinimoDijkstra(No* origem, No* destino, const map<char, No*>& mapa) {
    map<char, int> dist;                            // mapeia o menor custo conhecido de origem até cada vértice
    map<char, char> anterior;                       // armazena o pai (vértice anterior) no caminho mais curto
    const int INF = numeric_limits<int>::max();     // representa valor infinito para inicialização

    // inicializa distâncias com infinito
    for (auto &[id, _] : mapa) dist[id] = INF;
    
    dist[origem->getId()] = 0;

    // fila de prioridade (min-heap) que prioriza vértices com menor distância conhecida
    auto cmp = [&dist](char a, char b) { return dist[a] > dist[b]; };
    priority_queue<char, vector<char>, decltype(cmp)> fila(cmp);
    fila.push(origem->getId());

    // enquanto houver vértices para processar
    while (!fila.empty()) {
        char atual = fila.top(); fila.pop();
        Aresta* a = mapa.at(atual)->getPrimeiraAresta();
        while (a) {
            char viz = a->getDestino()->getId();
            int peso = a->getPeso();
            if (dist[atual] + peso < dist[viz]) {
                dist[viz] = dist[atual] + peso;
                anterior[viz] = atual;
                fila.push(viz);
            }
            a = a->getProxima();
        }
    }

    // reconstrução do caminho
    vector<char> caminho;
    if (dist[destino->getId()] == INF) return caminho; // sem caminho

    // caminha de trás para frente, do destino até a origem
    for (char v = destino->getId(); v != origem->getId(); v = anterior[v]) {
        caminho.push_back(v);
    }
    caminho.push_back(origem->getId());
    reverse(caminho.begin(), caminho.end());
    return caminho;
}

// Floyd
vector<char> caminhoMinimoFloyd(No* origem, No* destino, const map<char, No*>& mapa) {
    const int INF = 1e8;
    map<char, int> idx;             // mapeia o id do vértice (char) para um índice numérico
    vector<char> idxToChar;         // mapeia índice de volta para o char original (reverso)
    int n = 0;

    // preenche os mapeamentos idx e idxToChar
    for (auto &[c, _] : mapa) {
        idx[c] = n++;
        idxToChar.push_back(c);
    }

    // inicializa a matriz de distâncias com INF e a matriz de próximos com -1
    vector<vector<int>> dist(n, vector<int>(n, INF));
    vector<vector<int>> next(n, vector<int>(n, -1));

    // preenche a matriz de distâncias com pesos diretos das arestas
    for (auto &[c, no] : mapa) {
        int i = idx[c];
        dist[i][i] = 0;
        Aresta* a = no->getPrimeiraAresta();
        while (a) {
            int j = idx[a->getDestino()->getId()];
            dist[i][j] = a->getPeso();
            next[i][j] = j;
            a = a->getProxima();
        }
    }

    // computa todos os caminhos mínimos entre todos os pares
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }

    // reconstrução do caminho mínimo a partir de next[][]
    int i = idx[origem->getId()], j = idx[destino->getId()];
    if (next[i][j] == -1) return {};

    vector<char> caminho;
    // caminha de i até j seguindo os ponteiros next[]
    for (; i != j; i = next[i][j])
        caminho.push_back(idxToChar[i]);
    caminho.push_back(idxToChar[j]);
    return caminho;
}


// Algoritmo de Prim para AGM
Grafo AGM_Prim(const map<char, No*>& mapa) {
    Grafo agm(false, false, true); // grafo de saída, não direcionado, ponderado em arestas
    if (mapa.empty()) return agm;

    set<char> visitados;

    // fila de prioridade de arestas (peso, origem, destino)
    auto cmp = [](const tuple<int, char, char>& a, const tuple<int, char, char>& b) {
        return get<0>(a) > get<0>(b); // menor peso primeiro
    };
    priority_queue<tuple<int, char, char>, vector<tuple<int, char, char>>, decltype(cmp)> pq(cmp);

    // inicia a AGM a partir do primeiro vértice
    char inicio = mapa.begin()->first;
    visitados.insert(inicio);

    // adiciona arestas do vértice inicial à fila
    Aresta* a = mapa.at(inicio)->getPrimeiraAresta();
    while (a) {
        pq.emplace(a->getPeso(), inicio, a->getDestino()->getId());
        a = a->getProxima();
    }

    // enquanto tiver vértices não visitados
    while (!pq.empty() && visitados.size() < mapa.size()) {
        auto [peso, u, v] = pq.top(); pq.pop();
        if (visitados.count(v)) continue; // já foi incluso

        agm.adicionarAresta(u, v, peso);
        visitados.insert(v);

        // adiciona novas arestas a partir de v
        Aresta* a = mapa.at(v)->getPrimeiraAresta();
        while (a) {
            char viz = a->getDestino()->getId();
            if (!visitados.count(viz))
                pq.emplace(a->getPeso(), v, viz);
            a = a->getProxima();
        }
    }

    // garante todos os nós incluídos
    for (const auto& [id, _] : mapa)
        agm.adicionarNo(id);

    return agm;
}

// Algoritmo de Kruskal para AGM
Grafo AGM_Kruskal(const map<char, No*>& mapa) {
    Grafo agm(false, false, true);
    map<char, char> pai;

    // função auxiliar para encontrar representante
    function<char(char)> encontrar = [&](char u) {
        return pai[u] == u ? u : pai[u] = encontrar(pai[u]);
    };

    auto unir = [&](char u, char v) {
        pai[encontrar(u)] = encontrar(v);
    };

    vector<tuple<int, char, char>> arestas;
    // coleta todas as arestas do grafo
    for (const auto& [u, no] : mapa) {
        pai[u] = u;
        Aresta* a = no->getPrimeiraAresta();
        while (a) {
            char v = a->getDestino()->getId();
            if (u < v) // evita duplicar aresta em grafo não direcionado
                arestas.emplace_back(a->getPeso(), u, v);
            a = a->getProxima();
        }
    }

    // ordena arestas por peso
    sort(arestas.begin(), arestas.end());

    // une os conjuntos
    for (auto [peso, u, v] : arestas) {
        if (encontrar(u) != encontrar(v)) {
            unir(u, v);
            agm.adicionarAresta(u, v, peso);
        }
    }

    for (const auto& [id, _] : mapa)
        agm.adicionarNo(id);

    return agm;
}

// Gera a árvore de profundidade a partir de um vértice
Grafo Grafo::DFS_arvore(char origemId) const {
    Grafo arvoreDFS(direcionado, ponderadoVertices, ponderadoArestas);
    set<char> visitados;            // conjunto para controlar vértices já visitados
    stack<No*> pilha;               // pilha para controle da DFS

    No* origem = buscarNo(origemId);
    if (!origem) return arvoreDFS;

    pilha.push(origem);
    visitados.insert(origemId);
    arvoreDFS.adicionarNo(origemId);

    while (!pilha.empty()) {
        No* atual = pilha.top();
        pilha.pop();

        Aresta* aresta = atual->getPrimeiraAresta();
        while (aresta) {
            char vizinhoId = aresta->getDestino()->getId();
            if (!visitados.count(vizinhoId)) {
                visitados.insert(vizinhoId);
                pilha.push(aresta->getDestino());
                arvoreDFS.adicionarNo(vizinhoId);
                arvoreDFS.adicionarAresta(atual->getId(), vizinhoId, aresta->getPeso());
            }
            aresta = aresta->getProxima();
        }
    }

    return arvoreDFS;
}

// Calcula raio, diâmetro, centro e periferia do grafo
void Grafo::medidasGrafo() const {
    const int INF = 1e8;
    map<char, int> idx;           // mapeia id de vértice para índice numérico
    vector<char> idxToChar;       // índice numérico → id de vértice
    int n = 0;

    // atribui índices
    No* atual = primeiro;
    while (atual) {
        idx[atual->getId()] = n++;
        idxToChar.push_back(atual->getId());
        atual = atual->getProximo();
    }

    // matriz de distâncias inicial
    vector<vector<int>> dist(n, vector<int>(n, INF));
    for (int i = 0; i < n; ++i)
        dist[i][i] = 0;

    // preenche distâncias diretas
    atual = primeiro;
    while (atual) {
        int u = idx[atual->getId()];
        Aresta* a = atual->getPrimeiraAresta();
        while (a) {
            int v = idx[a->getDestino()->getId()];
            dist[u][v] = a->getPeso();
            if (!direcionado) dist[v][u] = a->getPeso();
            a = a->getProxima();
        }
        atual = atual->getProximo();
    }

    // floyd
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];

    // excentricidades
    vector<int> ecc(n, 0);
    int raio = INF, diametro = 0;
    for (int i = 0; i < n; ++i) {
        int maxDist = 0;
        for (int j = 0; j < n; ++j)
            if (dist[i][j] < INF)
                maxDist = max(maxDist, dist[i][j]);
        ecc[i] = maxDist;
        raio = min(raio, maxDist);
        diametro = max(diametro, maxDist);
    }

    // centro/periferia
    vector<char> centro, periferia;
    for (int i = 0; i < n; ++i) {
        if (ecc[i] == raio)
            centro.push_back(idxToChar[i]);
        if (ecc[i] == diametro)
            periferia.push_back(idxToChar[i]);
    }

    // saída
    cout << "Raio: " << raio << "\n";
    cout << "Diametro: " << diametro << "\n";

    cout << "Centro: ";
    for (char c : centro) cout << c << " ";
    cout << "\n";

    cout << "Periferia: ";
    for (char c : periferia) cout << c << " ";
    cout << "\n";
}