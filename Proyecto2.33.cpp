#include <iostream>      // Librería estándar de entrada/salida (para usar cin/cout)
#include <queue>         // Librería para usar colas (queue) (necesario para BFS y mostrar por generaciones)
#include <string>        // Librería para manejar strings (cadenas de texto)
#include <ctime>         // Librería para manejar fechas y tiempo (para la "edad" de los nodos)
#include <vector>        // Librería para usar vectores dinámicos (para listas de padres disponibles)

using namespace std;     // Evita escribir std:: constantemente para tipos y funciones estándar

// =========================
// COLORES ANSI (para imprimir texto de colores en consola)
// =========================
#define RESET    "\033[0m"  // Código de escape ANSI para resetear el color a la configuración predeterminada
#define ROJO     "\033[31m" // Código de escape ANSI para el color rojo
#define VERDE    "\033[32m" // Código de escape ANSI para el color verde
#define AMARILLO "\033[33m" // Código de escape ANSI para el color amarillo
#define AZUL     "\033[34m"  // Código de escape ANSI para el color azul

// --------------------------------------
// TIEMPO GLOBAL DESDE QUE INICIA EL PROGRAMA
// --------------------------------------
time_t start_time = time(NULL);  // Almacena el número de segundos transcurridos desde 1/1/1970 al inicio del programa

// Función para calcular los "años" (segundos) transcurridos desde el inicio del programa
int yearsElapsed() {
    return (int)(difftime(time(NULL), start_time));  // Calcula la diferencia entre el tiempo actual y el tiempo inicial, y lo devuelve como entero
}

// --------------------------------------
// NODO DEL ÁRBOL
// --------------------------------------
struct Nodo {
    string nombre;   // Nombre único del personaje
    string tipo;     // Tipo de elemento del personaje (Agua, Fuego, Roca, etc.)
    string genero;   // Género del personaje (Hombre, Mujer)
    string estado;   // Estado del personaje (Vivo, Muerto)
    int nacimiento;  // El tiempo (en "años"/segundos) en que se creó este nodo

    Nodo* padre;     // Puntero al nodo padre en el árbol
    Nodo* izquierda; // Puntero al hijo izquierdo
    Nodo* derecha;   // Puntero al hijo derecho

    // Constructor del nodo: inicializa todos los campos al crear un nuevo Nodo
    Nodo(string n, string t, string g, string e, Nodo* p)
    {
        nombre = n;      // Asigna el nombre
        tipo = t;        // Asigna el tipo
        genero = g;      // Asigna el género
        estado = e;      // Asigna el estado
        padre = p;       // Asigna el puntero al padre
        izquierda = NULL; // Inicializa el puntero del hijo izquierdo a nulo (sin hijo)
        derecha = NULL;  // Inicializa el puntero del hijo derecho a nulo (sin hijo)
        nacimiento = yearsElapsed(); // Guarda el tiempo actual como la edad de creación
    }

    // Función para obtener la edad actual del nodo en "años" (segundos)
    int edadActual() { 
        return yearsElapsed() - nacimiento; // Calcula la diferencia entre el tiempo actual y el tiempo de nacimiento
    }

    // Función que devuelve la cantidad de hijos directos del nodo
    int hijos() {
        int h = 0;           // InicializSa el contador de hijos a cero
        if (izquierda) h++;  // Si el puntero izquierdo no es NULL (hay hijo), incrementa el contador
        if (derecha) h++;    // Si el puntero derecho no es NULL (hay hijo), incrementa el contador
        return h;            // Devuelve la cantidad total de hijos (0, 1 o 2)
    }
};

// =========================
// Función para colorear nodos
// =========================
string colorNodo(Nodo* nodo) {
    if (nodo == NULL) return string("(NULL)"); // Si el nodo es nulo, retorna la cadena "(NULL)"
    if (nodo->tipo == "Roca" || nodo->nombre == "Asteroide")
        return string(AMARILLO) + nodo->nombre + RESET; // Si es Roca o Asteroide, color amarillo
    if (nodo->tipo == "Agua")
        return string(AZUL) + nodo->nombre + RESET;       // Si es Agua, color azul
    if (nodo->tipo == "Fuego")
        return string(ROJO) + nodo->nombre + RESET;       // Si es Fuego, color rojo
    return string(VERDE) + nodo->nombre + RESET;         // Para cualquier otro tipo, color verde
}

// --------------------------------------
// ÁRBOL GENEALÓGICO
// --------------------------------------
struct Arbol {
    Nodo* raiz;  // El nodo raíz del árbol (siempre "Asteroide")

    // Constructor del árbol: se ejecuta al crear un objeto Arbol
    Arbol() {
        raiz = new Nodo("Asteroide", "Roca", "None", "Vivo", NULL); // Crea el nodo raíz (sin padre)
        raiz->izquierda = new Nodo("Agua", "Agua", "None", "Muerto", raiz); // Crea el hijo izquierdo inicial
        raiz->derecha   = new Nodo("Fuego", "Fuego", "None", "Muerto", raiz); // Crea el hijo derecho inicial
    }

    // Función para buscar un nodo por su nombre utilizando un recorrido por niveles (BFS)
    Nodo* buscar(const string& nombre) {
        if (raiz == NULL) return NULL; // Si el árbol está vacío, retorna NULL
        queue<Nodo*> q;  // Declara una cola de punteros a Nodo para BFS (Breadth-First Search)
        q.push(raiz);    // Inserta el nodo raíz para comenzar el recorrido
        while (!q.empty()) {   // Repite mientras la cola no esté vacía
            Nodo* act = q.front();  // Obtiene el nodo al frente de la cola
            q.pop();             // Elimina el nodo del frente de la cola
            if (act->nombre == nombre) return act; // Si el nombre coincide, retorna el nodo encontrado
            if (act->izquierda) q.push(act->izquierda); // Si tiene hijo izquierdo, lo agrega a la cola
            if (act->derecha) q.push(act->derecha);      // Si tiene hijo derecho, lo agrega a la cola
        }
        return NULL; // Si el bucle termina sin encontrar el nodo, retorna NULL
    }

    // Función que devuelve una lista (vector) de todos los nodos que pueden tener al menos un hijo más (menos de 2 hijos)
    vector<Nodo*> padresDisponibles() {
        vector<Nodo*> lista; // Vector para almacenar los nodos disponibles
        queue<Nodo*> q;       // Cola para BFS
        q.push(raiz);         // Inicia el BFS desde la raíz
        while(!q.empty()) {
            Nodo* act = q.front(); q.pop(); // Saca el nodo actual de la cola
            if (act->hijos() < 2) lista.push_back(act); // Si tiene 0 o 1 hijo, es un padre disponible, se añade a la lista
            if (act->izquierda) q.push(act->izquierda); // Agrega hijo izquierdo a la cola para su posterior revisión
            if (act->derecha) q.push(act->derecha);      // Agrega hijo derecho a la cola
        }
        return lista; // Devuelve el vector con los padres disponibles
    }

    // Función que pide al usuario seleccionar el tipo ("Agua" o "Fuego") y lo retorna
    string elegirTipo() {
        int op; // Variable para almacenar la opción del menú
        cout << "\nTipo:\n1. Agua\n2. Fuego\nOpcion: ";
        cin >> op;           // Lee la opción ingresada por el usuario
        while (op < 1 || op > 2) { // Bucle de validación para asegurar que la opción es 1 o 2
            cout << "Opcion invalida. Intente de nuevo: ";
            cin >> op;
        }
        return (op == 1 ? "Agua" : "Fuego"); // Retorna "Agua" si es 1, o "Fuego" si es 2
    }

    // Función que pide al usuario seleccionar el género ("Hombre" o "Mujer") y lo retorna
    string elegirGenero() {
        int op;
        cout << "\nGenero:\n1. Hombre\n2. Mujer\nOpcion: ";
        cin >> op;
        while (op < 1 || op > 2) {
            cout << "Opcion invalida. Intente de nuevo: ";
            cin >> op;
        }
        return (op == 1 ? "Hombre" : "Mujer");
    }

    // Función que pide al usuario seleccionar el estado ("Vivo" o "Muerto") y lo retorna
    string elegirEstado() {
        int op;
        cout << "\nEstado:\n1. Vivo\n2. Muerto\nOpcion: ";
        cin >> op;
        while (op < 1 || op > 2) {
            cout << "Opcion invalida. Intente de nuevo: ";
            cin >> op;
        }
        return (op == 1 ? "Vivo" : "Muerto");
    }

    // Función principal para insertar un nuevo nodo en el árbol
    void insertar() {
        string nombre; // Variable para el nombre del nuevo personaje
        cout << "\n=== Nueva insercion ===\n";
        cout << "Nombre: ";
        cin >> nombre;   // Pide y lee el nombre

        if (buscar(nombre)) {   // Llama a buscar para verificar si el nombre ya existe
            cout << "ERROR: Ya existe un personaje con ese nombre.\n";
            return; // Termina la función si el nombre ya está en uso
        }

        string tipo = elegirTipo();       // Pide y obtiene el tipo del nuevo nodo
        string genero = elegirGenero(); // Pide y obtiene el género
        string estado = elegirEstado(); // Pide y obtiene el estado

        vector<Nodo*> disponibles = padresDisponibles(); // Obtiene la lista de nodos que pueden ser padres
        if ((int)disponibles.size() == 0) {             // Verifica si la lista está vacía
            cout << "No hay padres disponibles.\n";
            return; // Termina si no hay nodos con menos de 2 hijos
        }

        // Muestra la lista de padres disponibles al usuario
        cout << "\nSeleccione padre:\n";
        for (int i = 0; i < (int)disponibles.size(); i++) {
            cout << (i+1) << ". " << disponibles[i]->nombre  // Imprime la opción numerada y el nombre del padre
                 << " (hijos: " << disponibles[i]->hijos() << ")\n"; // Muestra la cantidad actual de hijos del potencial padre
        }

        int op; // Opción para seleccionar el padre
        cin >> op;
        if (op < 1 || op > (int)disponibles.size()) {  // Valida que la opción esté dentro del rango
            cout << "Opcion invalida.\n";
            return;
        }

        Nodo* padreSel = disponibles[op-1]; // Obtiene el puntero al nodo padre seleccionado (usando índice op-1)
        Nodo* nuevo = new Nodo(nombre, tipo, genero, estado, padreSel); // Crea el nuevo nodo en memoria dinámica

        if (padreSel->izquierda == NULL) padreSel->izquierda = nuevo; // Si el hijo izquierdo está libre, asigna el nuevo nodo como izquierdo
        else padreSel->derecha = nuevo; // Si no, asigna el nuevo nodo como hijo derecho

        cout << "Insertado correctamente bajo el padre: " << padreSel->nombre << "\n"; // Confirma la inserción
    }

    // Función para eliminar un nodo del árbol
    void eliminar() {
        string nombre;
        cout << "\nNombre del personaje a eliminar: ";
        cin >> nombre; // Pide el nombre a eliminar

        Nodo* objetivo = buscar(nombre); // Busca el nodo por nombre
        if (!objetivo) { // Si el resultado de buscar es NULL
            cout << "No existe ese personaje.\n";
            return; // Termina si no se encuentra
        }

        if (objetivo == raiz) { // Comprueba si el nodo es la raíz
            cout << "ERROR: No puedes eliminar la raiz (Asteroide).\n";
            return;
        }

        if (objetivo->hijos() > 0) { // Comprueba si el nodo tiene hijos
            cout << "ERROR: No se puede eliminar, tiene hijos.\n";
            return;
        }

        if (objetivo->edadActual() < 60) { // Comprueba si tiene al menos 60 "años"
            cout << "ERROR: Solo puede eliminarse si tiene mas de 60 anios.\n";
            return;
        }

        // Desconectamos nodo del padre
        if (objetivo->padre->izquierda == objetivo) // Si el objetivo es el hijo izquierdo de su padre
            objetivo->padre->izquierda = NULL; // El padre apunta a NULL en su izquierda
        else // Si es el hijo derecho
            objetivo->padre->derecha = NULL; // El padre apunta a NULL en su derecha

        delete objetivo; // Libera la memoria ocupada por el nodo
        cout << "Eliminado exitosamente.\n";
    }

    // Función para mostrar el árbol por niveles o generaciones (utiliza BFS)
    void mostrarGeneraciones() {
        cout << "\n=== ARBOL POR GENERACIONES ===\n";
        queue< pair<Nodo*, int> > q;  // Cola de pares: puntero a Nodo y su nivel de profundidad
        q.push(pair<Nodo*, int>(raiz, 0)); // Inserta la raíz con nivel 0
        int nivelActual = -1; // Variable para rastrear el nivel que se está imprimiendo
        while(!q.empty()) {
            pair<Nodo*, int> act = q.front(); q.pop(); // Saca el primer elemento de la cola
            Nodo* nodo = act.first; // Obtiene el puntero al nodo
            int nivel = act.second; // Obtiene el nivel del nodo
            if (nivel != nivelActual) {  // Comprueba si se ha cambiado a una nueva generación
                nivelActual = nivel; // Actualiza el nivel actual
                cout << "\n--- GENERACION " << nivel << " ---\n"; // Imprime el encabezado de la nueva generación
            }
            cout << "Nombre: " << nodo->nombre
                 << " | Tipo: " << nodo->tipo
                 << " | Genero: " << nodo->genero
                 << " | Estado: " << nodo->estado
                 << " | Padre: " << (nodo->padre ? nodo->padre->nombre : "Ninguno") // Muestra el nombre del padre o "Ninguno"
                 << " | Hijos: " << nodo->hijos()
                 << " | Edad: " << nodo->edadActual()
                 << "\n";
            if (nodo->izquierda) q.push(pair<Nodo*, int>(nodo->izquierda, nivel+1)); // Si hay hijo izquierdo, se agrega a la cola con nivel incrementado
            if (nodo->derecha)   q.push(pair<Nodo*, int>(nodo->derecha, nivel+1));    // Si hay hijo derecho, se agrega a la cola con nivel incrementado
        }
    }

    // Funciones de recorrido clásico del árbol (recursivos)
    void recorridoPreorden(Nodo* nodo) {  // Recorrido: Nodo - Izquierda - Derecha
        if (!nodo) return;              // Caso base de la recursión: si el nodo es nulo, termina
        cout << nodo->nombre << " ";    // Procesa/visita el nodo (imprime el nombre)
        recorridoPreorden(nodo->izquierda); // Llama recursivamente para el subárbol izquierdo
        recorridoPreorden(nodo->derecha);   // Llama recursivamente para el subárbol derecho
    }

    void recorridoInorden(Nodo* nodo) {   // Recorrido: Izquierda - Nodo - Derecha
        if (!nodo) return;
        recorridoInorden(nodo->izquierda);
        cout << nodo->nombre << " ";
        recorridoInorden(nodo->derecha);
    }

    void recorridoPostorden(Nodo* nodo) { // Recorrido: Izquierda - Derecha - Nodo
        if (!nodo) return;
        recorridoPostorden(nodo->izquierda);
        recorridoPostorden(nodo->derecha);
        cout << nodo->nombre << " ";
    }

    void preorden()  { recorridoPreorden(raiz); cout << "\n"; } // Función de envoltura: inicia el preorden desde la raíz
    void inorden()   { recorridoInorden(raiz);  cout << "\n"; } // Función de envoltura: inicia el inorden desde la raíz
    void postorden() { recorridoPostorden(raiz); cout << "\n"; } // Función de envoltura: inicia el postorden desde la raíz

    // ---------------------------
    // ÁRBOL VERTICAL CENTRADO
    // ---------------------------
    struct NodoPos {   // Estructura auxiliar para guardar la posición de impresión de un nodo/rama
        int x;         // Posición horizontal (columna)
        int y;         // Nivel vertical (fila)
        string texto;  // El texto (nombre coloreado o símbolo de rama) a imprimir
    };

    // Función recursiva para calcular las posiciones y textos de todos los elementos del árbol
    void buildTreeLines(Nodo* nodo, int x, int y, vector< vector<NodoPos> >& lines) {
        if (!nodo) return;   // Caso base: si el nodo es nulo, termina la recursión

        while ((int)lines.size() <= y)   // Asegura que el vector 'lines' tenga un vector para el nivel 'y'
            lines.push_back(vector<NodoPos>()); // Si no existe, agrega un nuevo nivel

        NodoPos np;          // Crea un objeto para la posición del nodo actual
        np.x = x;            // Asigna la posición horizontal
        np.y = y;            // Asigna el nivel vertical
        np.texto = colorNodo(nodo); // Obtiene y asigna el nombre del nodo con su color ANSI
        lines[y].push_back(np);      // Agrega este NodoPos a la lista de elementos en el nivel 'y'

        int gap = 6; // Define la separación horizontal base entre el padre y sus hijos

        // Procesamiento del Subárbol izquierdo
        if (nodo->izquierda) {
            while ((int)lines.size() <= y + 1) lines.push_back(vector<NodoPos>()); // Asegura espacio para la línea de conexión
            NodoPos branch;    // Crea un objeto para el símbolo de rama izquierda "/"
            branch.x = x - gap; // Posición de la rama izquierda (un poco a la izquierda del padre)
            branch.y = y + 1; // Nivel de la rama (debajo del padre)
            branch.texto = "/"; // El símbolo de rama
            lines[y + 1].push_back(branch);

            // Llamada recursiva para el hijo izquierdo, ajustando la posición horizontal
            buildTreeLines(nodo->izquierda, x - gap - (int)colorNodo(nodo->izquierda).length()/2, y + 2, lines); 
        }

        // Procesamiento del Subárbol derecho
        if (nodo->derecha) {
            while ((int)lines.size() <= y + 1) lines.push_back(vector<NodoPos>()); // Asegura espacio para la línea de conexión
            NodoPos branch;    // Crea un objeto para el símbolo de rama derecha "\"
            branch.x = x + gap; // Posición de la rama derecha (un poco a la derecha del padre)
            branch.y = y + 1;
            branch.texto = "\\";
            lines[y + 1].push_back(branch);

            // Llamada recursiva para el hijo derecho, ajustando la posición horizontal
            buildTreeLines(nodo->derecha, x + gap + (int)colorNodo(nodo).length()/2, y + 2, lines);
        }
    }

    // Función para mostrar el árbol como un diagrama vertical centrado y coloreado
    void mostrarArbolVertical() {
        cout << "\n=== ARBOL VERTICAL CENTRADO Y COLOREADO ===\n\n";

        vector< vector<NodoPos> > lines; // Vector de vectores: cada vector interno representa un nivel/línea de impresión

        int width = 80;          // Ancho de impresión (número de caracteres)
        int rootX = width / 2;   // Calcula la posición horizontal central para la raíz

        buildTreeLines(raiz, rootX, 0, lines); // Inicia la construcción de las posiciones de impresión

        // Bucle para iterar sobre cada nivel del árbol
        for (int i = 0; i < (int)lines.size(); i++) {
            string line(width * 2, ' '); // Inicializa una línea de impresión con el doble de ancho (para manejar caracteres ANSI)
            // Bucle para iterar sobre todos los elementos (nodos o ramas) en el nivel actual
            for (int j = 0; j < (int)lines[i].size(); j++) {
                int pos = lines[i][j].x; // Posición horizontal del elemento
                // Verifica que la posición esté dentro del rango de la línea
                if (pos >= 0 && pos < (int)line.length())
                    // Reemplaza los espacios en la línea con el texto (nombre o rama) del elemento
                    line.replace(pos, lines[i][j].texto.length(), lines[i][j].texto); 
            }
            cout << line << "\n"; // Muestra la línea completa del nivel actual
        }
        cout << "\n";
    }

};

// --------------------------------------
// MAIN
// --------------------------------------
int main() {
    Arbol arbol; // Crea una instancia del árbol genealógico
    int op;      // Variable para almacenar la opción del menú

    do { // Bucle principal del menú
        cout << "\n===== MENU =====\n"; // Muestra el encabezado del menú
        cout << "1. Insertar personaje\n";
        cout << "2. Eliminar personaje\n";
        cout << "3. Mostrar por generaciones\n";
        cout << "4. Recorrido Preorden\n";
        cout << "5. Recorrido Inorden\n";
        cout << "6. Recorrido Postorden\n";
        cout << "7. Mostrar arbol	\n";
        cout << "8. Salir\n";
        cout << "Opcion: ";
        cin >> op; // Lee la opción del usuario

        switch(op) { // Estructura de control para ejecutar la función según la opción
            case 1: arbol.insertar(); break; // Llama a la función para insertar
            case 2: arbol.eliminar(); break; // Llama a la función para eliminar
            case 3: arbol.mostrarGeneraciones(); break; // Muestra el árbol por niveles (BFS)
            case 4: arbol.preorden(); break; // Ejecuta el recorrido preorden
            case 5: arbol.inorden(); break; // Ejecuta el recorrido inorden
            case 6: arbol.postorden(); break; // Ejecuta el recorrido postorden
            case 7: arbol.mostrarArbolVertical(); break; // Muestra el diagrama vertical
        }

    } while(op != 8); // El bucle se repite mientras la opción no sea 8 (Salir)

    return 0; // Retorna 0, indicando que el programa terminó con éxito
}
