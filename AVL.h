#ifndef AVL_H
#define AVL_H
#include <iostream>
#include <string>
#include "Lista.h"

struct nodo {
    int clave;
    struct nodo *izquierda, *derecha;
    int altura;
    
    
    nodo() : clave(0), izquierda(nullptr), derecha(nullptr) {}
    nodo(int valor) : clave(valor), izquierda(nullptr), derecha(nullptr) {}
};

void imprimirArbol(struct nodo* raiz, std::string prefijo = "", bool esIzquierdo = true) {
    if (raiz == nullptr) {
        return;
    }

    std::cout << prefijo;
    std::cout << (esIzquierdo ? ".. " : ".. ");
    std::cout << raiz->clave << std::endl;

    imprimirArbol(raiz->derecha, prefijo + (esIzquierdo ? "¦   " : "    "), false);
    imprimirArbol(raiz->izquierda, prefijo + (esIzquierdo ? "¦   " : "    "), true);
}

int altura(struct nodo *N) {
    if (N == nullptr)
        return 0;
    return N->altura;
}

struct nodo* nuevoNodo(int clave) {
    struct nodo* nodo = new struct nodo;
    nodo->clave = clave;
    nodo->izquierda = nodo->derecha = nullptr;
    nodo->altura = 1; // nuevo nodo se añade al final del árbol
    return nodo;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

struct nodo* rotacionDerecha(struct nodo *y) {
    struct nodo *x = y->izquierda;
    struct nodo *T2 = x->derecha;
    // Realizar rotación
    x->derecha = y;
    y->izquierda = T2;
    // Actualizar alturas
    y->altura = max(altura(y->izquierda), altura(y->derecha)) + 1;
    x->altura = max(altura(x->izquierda), altura(x->derecha)) + 1;
    // Devolver nueva raíz
    return x;
}

struct nodo* rotacionIzquierda(struct nodo *x) {
    struct nodo *y = x->derecha;
    struct nodo *T2 = y->izquierda;
    // Realizar rotación
    y->izquierda = x;
    x->derecha = T2;
    // Actualizar alturas
    x->altura = max(altura(x->izquierda), altura(x->derecha)) + 1;
    y->altura = max(altura(y->izquierda), altura(y->derecha)) + 1;
    // Devolver nueva raíz
    return y;
}

// Obtener el factor de balanceo de un nodo N
int obtenerBalance(struct nodo *N) {
    if (N == nullptr)
        return 0;
    return altura(N->izquierda) - altura(N->derecha);
}

struct nodo* insertar(struct nodo* nodo, int clave) {
    // 1. Realizar la inserción normal
    if (nodo == nullptr)
        return nuevoNodo(clave);
    if (clave < nodo->clave)
        nodo->izquierda = insertar(nodo->izquierda, clave);
    else if (clave > nodo->clave)
        nodo->derecha = insertar(nodo->derecha, clave);
    else // Las claves iguales no se permiten
        return nodo;
    // 2. Actualizar la altura del nodo ancestro
    nodo->altura = 1 + max(altura(nodo->izquierda), altura(nodo->derecha));
    // 3. Obtener el factor de balanceo de este nodo ancestro para verificar si se desbalanceó
    int balance = obtenerBalance(nodo);
    // Si el nodo se desbalanceó, entonces hay 4 casos
    // Caso izquierda-izquierda
    if (balance > 1 && clave < nodo->izquierda->clave)
        return rotacionDerecha(nodo);
    // Caso derecha-derecha
    if (balance < -1 && clave > nodo->derecha->clave)
        return rotacionIzquierda(nodo);
    // Caso izquierda-derecha
    if (balance > 1 && clave > nodo->izquierda->clave) {
        nodo->izquierda = rotacionIzquierda(nodo->izquierda);
        return rotacionDerecha(nodo);
    }
    // Caso derecha-izquierda
    if (balance < -1 && clave < nodo->derecha->clave) {
        nodo->derecha = rotacionDerecha(nodo->derecha);
        return rotacionIzquierda(nodo);
    }
    // Devolver el puntero del nodo (sin cambios)
    return nodo;
}

struct nodo* buscar(struct nodo* raiz, int clave) {
    // Casos base: raiz es null o la clave está presente en la raiz
    if (raiz == nullptr || raiz->clave == clave)
        return raiz;
    // La clave es mayor que la clave de la raiz
    if (raiz->clave < clave)
        return buscar(raiz->derecha, clave);
    // La clave es menor que la clave de la raiz
    return buscar(raiz->izquierda, clave);
}

struct nodo* nodoConValorMaximo(struct nodo* nodo) {
    struct nodo* actual = nodo;
    while (actual->derecha != nullptr)
        actual = actual->derecha;
    return actual;
}

struct nodo* eliminar(struct nodo* raiz, int clave) {
    if (raiz == nullptr)
        return raiz;

    if (clave < raiz->clave)
        raiz->izquierda = eliminar(raiz->izquierda, clave);
    else if (clave > raiz->clave)
        raiz->derecha = eliminar(raiz->derecha, clave);
    else {
        if ((raiz->izquierda == nullptr) || (raiz->derecha == nullptr)) {
            struct nodo *temp = raiz->izquierda ? raiz->izquierda : raiz->derecha;
            if (temp == nullptr) {
                temp = raiz;
                raiz = nullptr;
            } else
                *raiz = *temp;
            delete temp;
        } else {
            struct nodo* temp = nodoConValorMaximo(raiz->izquierda);
            raiz->clave = temp->clave;
            raiz->izquierda = eliminar(raiz->izquierda, temp->clave);
        }
    }

    if (raiz == nullptr)
        return raiz;

    raiz->altura = 1 + max(altura(raiz->izquierda), altura(raiz->derecha));
    int balance = obtenerBalance(raiz);

    if (balance > 1) {
        if (obtenerBalance(raiz->izquierda) >= 0)
            return rotacionDerecha(raiz);
        else {
            raiz->izquierda = rotacionIzquierda(raiz->izquierda);
            return rotacionDerecha(raiz);
        }
    }
    if (balance < -1) {
        if (obtenerBalance(raiz->derecha) <= 0)
            return rotacionIzquierda(raiz);
        else {
            raiz->derecha = rotacionDerecha(raiz->derecha);
            return rotacionIzquierda(raiz);
        }
    }

    return raiz;
}

void preOrden(struct nodo* raiz) {
    if (raiz != nullptr) {
        std::cout << raiz->clave << " ";
        preOrden(raiz->izquierda);
        preOrden(raiz->derecha);
    }
}

void inOrden(struct nodo* raiz) {
    if (raiz != nullptr) {
        inOrden(raiz->izquierda);
        std::cout << raiz->clave << " ";
        inOrden(raiz->derecha);
    }
}

void postOrden(struct nodo* raiz) {
    if (raiz != nullptr) {
        postOrden(raiz->izquierda);
        postOrden(raiz->derecha);
        std::cout << raiz->clave << " ";
    }
}


struct ResultadoBusqueda {
    bool encontrado;
    int nivel;
    std::string posicion;
    Lista<int> nodosVisitados;
};

ResultadoBusqueda dfs(struct nodo* raiz, int valorBuscado) {
    if (raiz == nullptr) return {false, -1, "", Lista<int>()};
    
    struct NodoInfo {
        nodo* nodo;
        int nivel;
        std::string posicion;
    };
    
    Lista<NodoInfo> pila;
    Lista<int> nodosVisitados;
    pila.insertarAlFinal({raiz, 1, "raiz"});
    
    while (!pila.esVacia()) {
        NodoInfo actual = pila.getCabeza()->dato;
        pila.eliminarCabeza();
        
        nodosVisitados.insertarAlFinal(actual.nodo->clave);
        
        if (actual.nodo->clave == valorBuscado) {
            return {true, actual.nivel, actual.posicion, nodosVisitados};
        }
        
        if (actual.nodo->derecha != nullptr) {
            pila.insertarAlFinal({actual.nodo->derecha, actual.nivel + 1, actual.posicion + " -> derecha"});
        }
        if (actual.nodo->izquierda != nullptr) {
            pila.insertarAlFinal({actual.nodo->izquierda, actual.nivel + 1, actual.posicion + " -> izquierda"});
        }
    }
    
    return {false, -1, "", nodosVisitados};
}
  

ResultadoBusqueda bfs(struct nodo* raiz, int valorBuscado) {
    if (raiz == nullptr) return {false, -1, "", Lista<int>()};
    
    struct NodoInfo {
        nodo* nodo;
        int nivel;
        std::string posicion;
    };
    
    Lista<NodoInfo> cola;
    Lista<int> nodosVisitados;
    cola.insertarAlFinal({raiz, 1, "raiz"});
    
    while (!cola.esVacia()) {
        NodoInfo actual = cola.getCabeza()->dato;
        cola.eliminarCabeza();
        
        nodosVisitados.insertarAlFinal(actual.nodo->clave);
        
        if (actual.nodo->clave == valorBuscado) {
            return {true, actual.nivel, actual.posicion, nodosVisitados};
        }
        
        if (actual.nodo->izquierda != nullptr) {
            cola.insertarAlFinal({actual.nodo->izquierda, actual.nivel + 1, actual.posicion + " -> izquierda"});
        }
        if (actual.nodo->derecha != nullptr) {
            cola.insertarAlFinal({actual.nodo->derecha, actual.nivel + 1, actual.posicion + " -> derecha"});
        }
    }
    
    return {false, -1, "", nodosVisitados};
}


#endif
