#include <iostream>
#include <queue>
#include <string>
#include <chrono>
#include <vector>

using namespace std;

// --------------------------------------
//   TIEMPO GLOBAL DESDE QUE INICIA EL PROGRAMA
// --------------------------------------
auto start_time = chrono::steady_clock::now();

int añosTranscurridos() {
    auto now = chrono::steady_clock::now();
    auto diff = chrono::duration_cast<chrono::seconds>(now - start_time).count();
    return (int)diff; // 1 segundo = 1 año
}

// --------------------------------------
//           NODO DEL ÁRBOL
// --------------------------------------
struct Nodo {
    string nombre;
    string tipo;    // Agua, Fuego, etc.
    string genero;  // Hombre/Mujer
    string estado;  // Vivo/Muerto
    int nacimiento; // segundo en el que nació

    Nodo* padre;
    Nodo* izquierda;  // hijo 1
    Nodo* derecha;    // hijo 2

    Nodo(string n, string t, string g, string e, Nodo* p)
        : nombre(n), tipo(t), genero(g), estado(e), padre(p),
          izquierda(nullptr), derecha(nullptr) {
        nacimiento = añosTranscurridos();
    }

    int edadActual() {
        return añosTranscurridos() - nacimiento;
    }

    int hijos() {
        int h = 0;
        if (izquierda) h++;
        if (derecha) h++;
        return h;
    }
};

// --------------------------------------
//         ÁRBOL GENEALÓGICO
// --------------------------------------
struct Arbol {
    Nodo* raiz;

    Arbol() {
        // Raíz automática: Asteroide
        raiz = new Nodo("Asteroide", "Roca", "None", "Vivo", nullptr);

        // Hijos iniciales muertos
        raiz->izquierda = new Nodo("Agua", "Agua", "None", "Muerto", raiz);
        raiz->derecha   = new Nodo("Fuego", "Fuego", "None", "Muerto", raiz);
    }

    // Buscar por nombre
    Nodo* buscar(string nombre) {
        if (!raiz) return nullptr;

        queue<Nodo*> q;
        q.push(raiz);

        while (!q.empty()) {
            Nodo* act = q.front(); q.pop();
            if (act->nombre == nombre) return act;
            if (act->izquierda) q.push(act->izquierda);
            if (act->derecha) q.push(act->derecha);
        }
        return nullptr;
    }

    // Listar padres válidos (0 o 1 hijo)
    vector<Nodo*> padresDisponibles() {
        vector<Nodo*> lista;
        queue<Nodo*> q;
        q.push(raiz);

        while(!q.empty()) {
            Nodo* act = q.front(); q.pop();
            if (act->hijos() < 2) lista.push_back(act);
            if (act->izquierda) q.push(act->izquierda);
            if (act->derecha) q.push(act->derecha);
        }
        return lista;
    }

    // Insertar pidiendo padre
    void insertar() {
        string nombre, tipo, genero, estado;

        cout << "\n=== Nueva insercion ===\n";
        cout << "Nombre: "; cin >> nombre;

        if (buscar(nombre)) {
            cout << "ERROR: Ya existe un personaje con ese nombre.\n";
            return;
        }

        cout << "Tipo (Agua/Fuego/etc): ";
        cin >> tipo;
        cout << "Genero (Hombre/Mujer): ";
        cin >> genero;
        cout << "Estado (Vivo/Muerto): ";
        cin >> estado;

        vector<Nodo*> disponibles = padresDisponibles();

        if (disponibles.empty()) {
            cout << "No hay padres disponibles.\n";
            return;
        }

        cout << "\nSeleccione padre:\n";
        for (int i = 0; i < disponibles.size(); i++) {
            cout << i+1 << ". " << disponibles[i]->nombre
                 << " (hijos: " << disponibles[i]->hijos() << ")\n";
        }

        int op;
        cin >> op;
        if (op < 1 || op > disponibles.size()) {
            cout << "Opción inválida.\n";
            return;
        }

        Nodo* padreSel = disponibles[op-1];
        Nodo* nuevo = new Nodo(nombre, tipo, genero, estado, padreSel);

        if (!padreSel->izquierda) padreSel->izquierda = nuevo;
        else padreSel->derecha = nuevo;

        cout << "Insertado correctamente bajo el padre: " << padreSel->nombre << "\n";
    }

    // Eliminar nodo si está permitido
    void eliminar() {
        string nombre;
        cout << "\nNombre del personaje a eliminar: ";
        cin >> nombre;

        Nodo* objetivo = buscar(nombre);
        if (!objetivo) {
            cout << "No existe ese personaje.\n";
            return;
        }

        if (objetivo == raiz) {
            cout << "ERROR: No puedes eliminar la raíz (Asteroide).\n";
            return;
        }

        if (objetivo->hijos() > 0) {
            cout << "ERROR: No se puede eliminar, tiene hijos.\n";
            return;
        }

        int edad = objetivo->edadActual();
        if (edad < 60) {
            cout << "ERROR: Solo puede eliminarse si tiene más de 60 años.\n";
            return;
        }

        // Desconectar del padre
        if (objetivo->padre->izquierda == objetivo)
            objetivo->padre->izquierda = nullptr;
        else
            objetivo->padre->derecha = nullptr;

        delete objetivo;

        cout << "Eliminado exitosamente.\n";
    }

    // Mostrar por niveles (generaciones)
    void mostrarGeneraciones() {
        cout << "\n=== ÁRBOL POR GENERACIONES ===\n";

        queue<pair<Nodo*, int>> q;
        q.push({raiz, 0});

        int nivelActual = -1;

        while(!q.empty()) {
            auto [act, nivel] = q.front(); q.pop();

            if (nivel != nivelActual) {
                nivelActual = nivel;
                cout << "\n--- GENERACION " << nivel << " ---\n";
            }

            cout << "ID: " << act 
                 << " | Nombre: " << act->nombre
                 << " | Tipo: " << act->tipo
                 << " | Genero: " << act->genero
                 << " | Estado: " << act->estado
                 << " | Padre: " << (act->padre ? act->padre->nombre : "Ninguno")
                 << " | Hijos: " << act->hijos()
                 << " | Edad: " << act->edadActual()
                 << "\n";

            if (act->izquierda) q.push({act->izquierda, nivel+1});
            if (act->derecha) q.push({act->derecha, nivel+1});
        }
    }
};

// --------------------------------------
//                MAIN
// --------------------------------------
int main() {
    Arbol arbol;
    int op;

    do {
        cout << "\n===== MENU =====\n";
        cout << "1. Insertar personaje\n";
        cout << "2. Eliminar personaje\n";
        cout << "3. Mostrar por generaciones\n";
        cout << "4. Salir\n";
        cout << "Opcion: ";
        cin >> op;

        switch(op) {
            case 1: arbol.insertar(); break;
            case 2: arbol.eliminar(); break;
            case 3: arbol.mostrarGeneraciones(); break;
        }

    } while(op != 4);

    return 0;
}

