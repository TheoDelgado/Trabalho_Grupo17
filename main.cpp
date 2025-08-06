#include "Grafo.h"
#include "No.h"
#include "Gerenciador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

/*
MANUAL DE EXECUÇÃO:

1. Digitar "g++ *.cpp -o execGrupo17" no terminal;
2. Digitar "./execGrupo17 instancias_t1/<arquivo_entrada>" no terminal.       // Para instâncias do trab1
2.1. Digitar "./execGrupo17 instancias_t2/<arquivo_entrada>" no terminal.     // Para instâncias do trab2
*/


int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <caminho_arquivo_grafo>" << endl;
        return 1;
    }

    string caminho = argv[1];
    ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << caminho << endl;
        return 1;
    }

    int direcionado, ponderadoAresta, ponderadoVertice, numVertices;
    arquivo >> direcionado >> ponderadoAresta >> ponderadoVertice;
    arquivo >> numVertices;

    Grafo* grafo = new Grafo(direcionado, ponderadoVertice, ponderadoAresta);

    for (int i = 0; i < numVertices; ++i) {
        char id;
        int peso;
        arquivo >> id;
        if (ponderadoVertice) arquivo >> peso; // lido, mas ignorado
        grafo->adicionarNo(id);
    }

    string linha;
    getline(arquivo, linha);
    while (getline(arquivo, linha)) {
        istringstream ss(linha);
        char origem, destino;
        int peso = 1;
        ss >> origem >> destino;
        if (ponderadoAresta) ss >> peso;
        grafo->adicionarAresta(origem, destino, peso);
    }

    arquivo.close();
    Gerenciador::comandos(grafo);
    delete grafo;
    return 0;
}