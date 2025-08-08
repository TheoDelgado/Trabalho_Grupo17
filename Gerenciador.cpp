#include "Gerenciador.h"
#include "Grafo.h"
#include "Guloso.h"
#include <iomanip>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

// Função para imprimir um vetor de char
void imprimirVetor(const vector<char>& v) {
    for (char c : v) {
        cout << c << " ";
    }
    cout << endl;
}

// Função auxiliar para salvar um vetor de char em um arquivo .txt
void salvarResultadoEmArquivo(const vector<char>& resultado) {
    char opcao;
    cout << "Deseja salvar o resultado em um arquivo? (s/n): ";
    cin >> opcao;

    if (opcao == 's' || opcao == 'S') {
        string nomeArquivo;
        cout << "Digite o nome do arquivo (ex: resultado.txt): ";
        cin >> nomeArquivo;

        ofstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            cerr << "Erro ao criar o arquivo!\n";
            return;
        }

        for (char c : resultado) {
            arquivo << c << " ";
        }
        arquivo << endl;
        arquivo.close();
        cout << "Resultado salvo com sucesso em \"" << nomeArquivo << "\".\n";
    }
}

// Função auxiliar para salvar a representação textual de um grafo
void salvarGrafoEmArquivo(const Grafo& g) {
    char opcao;
    cout << "Deseja salvar o grafo em um arquivo? (s/n): ";
    cin >> opcao;

    if (opcao == 's' || opcao == 'S') {
        string nomeArquivo;
        cout << "Digite o nome do arquivo (ex: grafo.txt): ";
        cin >> nomeArquivo;

        ofstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            cerr << "Erro ao criar o arquivo!\n";
            return;
        }

        for (char v : g.getVertices()) {
            arquivo << v << ": ";
            No* n = g.buscarNo(v);
            Aresta* a = n->getPrimeiraAresta();
            while (a) {
                arquivo << a->getDestino()->getId();
                if (g.getPonderadoArestas()) arquivo << "(" << a->getPeso() << ")";
                if (a->getProxima()) arquivo << " -> ";
                a = a->getProxima();
            }
            arquivo << "\n";
        }

        arquivo.close();
        cout << "Grafo salvo com sucesso em \"" << nomeArquivo << "\".\n";
    }
}

// Função auxiliar para salvar medidas do grafo (raio, diâmetro, centro, periferia)
void salvarMedidasEmArquivo(const Grafo& g) {
    char opcao;
    cout << "Deseja salvar as medidas do grafo em um arquivo? (s/n): ";
    cin >> opcao;

    if (opcao == 's' || opcao == 'S') {
        string nomeArquivo;
        cout << "Digite o nome do arquivo (ex: medidas.txt): ";
        cin >> nomeArquivo;

        ofstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            cerr << "Erro ao criar o arquivo!\n";
            return;
        }

        // Calcula medidas
        stringstream buffer;
        streambuf* antigo = cout.rdbuf();
        cout.rdbuf(buffer.rdbuf());
        const_cast<Grafo&>(g).medidasGrafo();
        cout.rdbuf(antigo);

        arquivo << buffer.str();
        arquivo.close();
        cout << "Medidas salvas com sucesso em \"" << nomeArquivo << "\".\n";
    }
}

/*
void salvarConjuntoEmArquivo(const set<char>& resultado) {
    char opcao;
    cout << "Deseja salvar o resultado em um arquivo? (s/n): ";
    cin >> opcao;

    if (opcao == 's' || opcao == 'S') {
        string nomeArquivo;
        cout << "Digite o nome do arquivo (ex: resultado.txt): ";
        cin >> nomeArquivo;

        ofstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            cerr << "Erro ao criar o arquivo!\n";
            return;
        }

        for (char c : resultado) {
            arquivo << c << " ";
        }
        arquivo << endl;
        arquivo.close();
        cout << "Resultado salvo com sucesso em \"" << nomeArquivo << "\".\n";
    }
}
*/

char Gerenciador::get_id_entrada() {
    char id;
    cout << "Digite o ID do no: ";
    cin >> id;
    return id;
}

void Gerenciador::comandos(Grafo* grafo) {
    map<char, No*> mapa;
    for (char v : grafo->getVertices())
        mapa[v] = grafo->buscarNo(v);

    cout << "\nDigite uma das opcoes abaixo e pressione enter:\n"
         << "(a) Fecho transitivo direto de um no;\n"
         << "(b) Fecho transitivo indireto de um no;\n"
         << "(c) Caminho minimo (Dijkstra);\n"
         << "(d) Caminho minimo (Floyd);\n"
         << "(e) AGM (Prim);\n"
         << "(f) AGM (Kruskal);\n"
         << "(g) Arvore DFS a partir de um no;\n"
         << "(h) Raio, diametro, centro e periferia;\n"
         << "(i) CDI - Guloso Adaptativo;\n"
         << "(j) CDI - Guloso Randomizado Adaptativo;\n"
         << "(k) CDI - Guloso Randomizado Adaptativo Reativo;\n"
         << "(0) Sair.\n"
         << ">> ";

    char resp;
    cin >> resp;

    switch (resp) {
        case 'a': {
            char id = get_id_entrada();
            vector<char> resultado = fechoTransitivoDFS(grafo->buscarNo(id), mapa);
            imprimirVetor(resultado);
            salvarResultadoEmArquivo(resultado);
            break;
        }
        case 'b': {
            char id = get_id_entrada();
            vector<char> resultado = fechoTransitivoIndireto(grafo->buscarNo(id), mapa);
            imprimirVetor(resultado);
            salvarResultadoEmArquivo(resultado);
            break;
        }
        case 'c': {
            char u = get_id_entrada();
            char v = get_id_entrada();
            vector<char> resultado = caminhoMinimoDijkstra(mapa[u], mapa[v], mapa);
            imprimirVetor(resultado);
            salvarResultadoEmArquivo(resultado);
            break;
        }
        case 'd': {
            char u = get_id_entrada();
            char v = get_id_entrada();
            vector<char> resultado = caminhoMinimoFloyd(mapa[u], mapa[v], mapa);
            imprimirVetor(resultado);
            salvarResultadoEmArquivo(resultado);
            break;
        }
        case 'e': {
            Grafo agm = AGM_Prim(mapa);
            agm.imprimir();
            salvarGrafoEmArquivo(agm);
            break;
        }
        case 'f': {
            Grafo agm = AGM_Kruskal(mapa);
            agm.imprimir();
            salvarGrafoEmArquivo(agm);
            break;
        }
        case 'g': {
            char id = get_id_entrada();
            Grafo arvoreDFS = grafo->DFS_arvore(id);
            cout << "Arvore DFS a partir do no " << id << ":\n";
            arvoreDFS.imprimir();
            salvarGrafoEmArquivo(arvoreDFS);
            break;
        }
        case 'h': {
            grafo->medidasGrafo();
            salvarMedidasEmArquivo(*grafo);
            break;
        }
        case 'i': {
            vector<char> resultado = Guloso::gulosoAdaptativo(grafo);
            cout << "CDI (Guloso Adaptativo): ";
            for (char c : resultado) cout << c << " ";
            cout << endl;
            salvarResultadoEmArquivo(resultado); // já funciona para vector<char>
            break;
        }

        case 'j': {
            float alpha;
            cout << "Digite o valor de alpha (0 <= alpha <= 1): ";
            cin >> alpha;
            if (alpha < 0.0f || alpha > 1.0f) {
                cout << "Valor de alpha inválido. Deve estar entre 0 e 1.\n";
                break;
            }
            vector<char> resultado = Guloso::gulosoRandomizadoAdaptativo(grafo, alpha);
            cout << "\nConjunto Dominante Independente (Guloso Randomizado Adaptativo): ";
            for (char c : resultado) cout << c << " ";
            cout << endl;
            salvarResultadoEmArquivo(resultado); // usa função para vetor, mantém ordem de inserção
            break;
        }

        case 'k': {
            vector<char> resultado = Guloso::gulosoRandomizadoAdaptativoReativo(grafo);
            cout << "CDI (Guloso Randomizado Reativo Adaptativo): ";
            for (char c : resultado) cout << c << " ";
            cout << "\nTamanho da solução: " << resultado.size() << endl;
            salvarResultadoEmArquivo(resultado);
            break;
        }
                
        case '0': exit(0);
        default:
            cout << "Opcao invalida.\n";
    }

    comandos(grafo); // loop interativo
}