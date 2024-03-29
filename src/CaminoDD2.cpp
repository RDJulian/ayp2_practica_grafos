#include <set>
#include "CaminoDD2.h"
#include "Lector.h"

using namespace std;

CaminoDD2::CaminoDD2() {
    pair<size_t, vector<Camino*>> data = Lector::leerArchivo(PATH_ARCHIVO_CSV);
    nodos = data.first;
    caminos = data.second;

    grafo = new Grafo(nodos);
    grafoRoto = new Grafo(nodos);
    grafoMejorado = new Grafo(nodos * 2);

    for (Camino* camino: caminos) {
        Arista arista = camino->calcularArista();
        Arista aristaRota = camino->calcularAristaRota();

        grafo->agregarAristaPonderada(arista);
        grafoRoto->agregarAristaPonderada(aristaRota);

        if (camino->esCaminoRoto()) {
            arista.destino += nodos;
        }
        grafoMejorado->agregarAristaPonderada(arista);
        aristaRota.origen += nodos;
        aristaRota.destino += nodos;
        grafoMejorado->agregarAristaPonderada(aristaRota);
    }
    grafoMejorado->agregarAristaPonderada(Arista((nodos * 2) - 1, nodos - 1, 0));
}

Camino* CaminoDD2::buscarCamino(size_t origen, size_t destino) {
    size_t i = 0;
    Camino* camino = nullptr;
    while (!camino && i < caminos.size()) {
        if (*caminos[i] == std::pair<size_t, size_t>(origen, destino)) {
            camino = caminos[i];
        } else {
            i++;
        }
    }
    return camino;
}

Camino* CaminoDD2::buscarCaminoRoto(const vector<size_t>& caminoInicial) {
    size_t i = 0;
    Camino* camino;
    Camino* aux;
    camino = nullptr;
    while (!camino && i < caminoInicial.size() - 1) {
        aux = buscarCamino(caminoInicial[i], caminoInicial[i + 1]);
        if (aux && aux->esCaminoRoto()) {
            camino = aux;
        } else {
            i++;
        }
    }
    return camino;
}

pair<std::vector<size_t>, int> CaminoDD2::obtenerCaminoAproximado() {
    vector<size_t> camino = grafo->obtenerCaminoMinimo(0, nodos - 1);
    Camino* caminoRoto = buscarCaminoRoto(camino);
    if (caminoRoto) {
        return caminoRoto->unirCaminos(grafo, grafoRoto, nodos - 1);
    } else {
        int peso = grafo->obtenerPesoMinimo(0, nodos - 1);
        return {camino, peso};
    }
}

pair<std::vector<size_t>, int> CaminoDD2::obtenerCaminoMejorado() {
    vector<size_t> camino, resultado;
    camino = grafoMejorado->obtenerCaminoMinimo(0, nodos - 1);
    set<size_t> filtro;
    for (auto i: camino) {
        if (i >= nodos) {
            i -= nodos;
        }
        filtro.insert(i);
    }
    for (auto i: filtro) {
        resultado.push_back(i);
    }
    int peso = grafoMejorado->obtenerPesoMinimo(0, nodos - 1);
    return {resultado, peso};
}

CaminoDD2::~CaminoDD2() {
    for (Camino* camino: caminos) {
        delete camino;
    }
    delete grafo;
    delete grafoRoto;
}