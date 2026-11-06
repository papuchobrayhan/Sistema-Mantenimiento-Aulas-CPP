/*
  ========================================================================
  proyecto: sistema de gestion de solicitudes de mantenimiento de aulas
  curso: estructura de datos - universidad continental
  docente: mag. yesenia concha ramos
  integrantes: jose daniel araoz lima, george brayhan murillo sinsaya
  ========================================================================
*/

#include <iostream>
#include <string>
#include <fstream> //libreria para persistencia de datos en archivos txt

using namespace std;

// =========================================================================
// 1. estructuras de datos (nodos)
// =========================================================================

//nodo para la lista enlazada simple (catalogo de aulas)
struct NodoAula {
    string codigo;//codigo del aula (ejm: "a-101")
    string pabellon;//pabellon o bloque
    int piso;//piso del aula
    int solicitudesActivas;//solicitudes pendientes en cola
    NodoAula* siguiente;//puntero al siguiente nodo
};

//nodo para la cola enlazada (solicitudes - fifo)
struct NodoSolicitud {
    int id;//identificador automatico
    string codigoAula;//aula asociada
    string descripcion;//detalle de la falla
    string reportante;//nombre del solicitante
    string estado;//"pendiente" o "en proceso"
    NodoSolicitud* siguiente;//puntero a la siguiente solicitud
};

//nodo para la pila enlazada (bitacora - lifo)
struct NodoIntervencion {
    int numero;//correlativo de intervencion
    string codigoAula;//aula intervenida
    string trabajo;//trabajo realizado
    string tecnico;//nombre del tecnico
    NodoIntervencion* siguiente;//puntero a la intervencion anterior
};

// =========================================================================
// punteros globales
// =========================================================================
NodoAula* inicioLista = NULL;//inicio de la lista de aulas

NodoSolicitud* frenteCola = NULL;//frente de la cola
NodoSolicitud* finCola = NULL;//final de la cola
int contadorIdSolicitud = 0;//contador de ids

NodoIntervencion* topePila = NULL;//tope de la pila
int contadorIntervencion = 0;//contador de intervenciones

// =========================================================================
// 2. funciones para la lista enlazada simple (aulas)
// =========================================================================

//busqueda lineal de aula por codigo
NodoAula* buscarAula(string codigo) {
    NodoAula* temp = inicioLista;
    while (temp != NULL) {
        if (temp->codigo == codigo) {
            return temp;
        }
        temp = temp->siguiente;
    }
    return NULL;
}

//registrar un aula en la lista
void registrarAula() {
    string codigo, pabellon;
    int piso;

    cout << "\n--- REGISTRAR NUEVA AULA ---\n";
    cout << "Ingrese codigo de aula: ";
    cin >> codigo;

    if (buscarAula(codigo) != NULL) {//validar que no este duplicado
        cout << "El aula '" << codigo << "' ya esta registrada.\n";
        return;
    }

    cout << "Ingrese pabellon: ";
    cin >> pabellon;
    cout << "Ingrese piso: ";
    cin >> piso;

    NodoAula* nuevoNodo = new NodoAula();
    nuevoNodo->codigo = codigo;
    nuevoNodo->pabellon = pabellon;
    nuevoNodo->piso = piso;
    nuevoNodo->solicitudesActivas = 0;
    nuevoNodo->siguiente = NULL;

    if (inicioLista == NULL) {
        inicioLista = nuevoNodo;
    } else {
        NodoAula* temp = inicioLista;
        while (temp->siguiente != NULL) {
            temp = temp->siguiente;
        }
        temp->siguiente = nuevoNodo;
    }

    cout << "Aula registrada correctamente.\n";
}

//mostrar catalogo de aulas
void mostrarAulas() {
    if (inicioLista == NULL) {
        cout << "\nNo hay aulas registradas.\n";
        return;
    }

    cout << "\n--- CATALOGO DE AULAS REGISTRADAS ---\n";
    NodoAula* temp = inicioLista;
    while (temp != NULL) {
        cout << "Codigo: " << temp->codigo 
             << " | Pabellon: " << temp->pabellon 
             << " | Piso: " << temp->piso 
             << " | Solicitudes Activas: " << temp->solicitudesActivas << endl;
        temp = temp->siguiente;
    }
}

//modificar datos de un aula existente
void modificarAula() {
    string codigo;
    cout << "\n--- MODIFICAR AULA ---\n";
    cout << "Ingrese el codigo del aula: ";
    cin >> codigo;

    NodoAula* aula = buscarAula(codigo);
    if (aula == NULL) {
        cout << "El aula no existe.\n";
        return;
    }

    cout << "Ingrese nuevo pabellon: ";
    cin >> aula->pabellon;
    cout << "Ingrese nuevo piso: ";
    cin >> aula->piso;

    cout << "Datos del aula actualizados.\n";
}

//eliminar un aula de la lista
void eliminarAula() {
    string codigo;
    cout << "\n--- ELIMINAR AULA ---\n";
    cout << "Ingrese el codigo del aula: ";
    cin >> codigo;

    NodoAula* temp = inicioLista;
    NodoAula* anterior = NULL;

    while (temp != NULL && temp->codigo != codigo) {
        anterior = temp;
        temp = temp->siguiente;
    }

    if (temp == NULL) {
        cout << "El aula no fue encontrada.\n";
        return;
    }

    if (temp->solicitudesActivas > 0) {//validacion para no eliminar con solicitudes pendientes
        cout << "No se puede eliminar el aula '" << codigo 
             << "' porque tiene " << temp->solicitudesActivas << " solicitud(es) en cola.\n";
        return;
    }

    if (anterior == NULL) {
        inicioLista = temp->siguiente;
    } else {
        anterior->siguiente = temp->siguiente;
    }

    delete temp;
    cout << "Aula eliminada correctamente.\n";
}

// =========================================================================
// 3. funciones para la cola enlazada (solicitudes)
// =========================================================================

//registrar nueva solicitud en la cola (encolar)
void registrarSolicitud() {
    string codigoAula, descripcion, reportante;

    cout << "\n--- REGISTRAR SOLICITUD DE MANTENIMIENTO ---\n";
    cout << "Ingrese codigo del aula: ";
    cin >> codigoAula;

    NodoAula* aula = buscarAula(codigoAula);
    if (aula == NULL) {
        cout << "El aula '" << codigoAula << "' no existe en el catalogo.\n";
        return;
    }

    cin.ignore();
    cout << "Ingrese descripcion de la falla: ";
    getline(cin, descripcion);
    cout << "Ingrese nombre del reportante: ";
    getline(cin, reportante);

    contadorIdSolicitud++;
    NodoSolicitud* nuevoNodo = new NodoSolicitud();
    nuevoNodo->id = contadorIdSolicitud;
    nuevoNodo->codigoAula = codigoAula;
    nuevoNodo->descripcion = descripcion;
    nuevoNodo->reportante = reportante;
    nuevoNodo->estado = "Pendiente";
    nuevoNodo->siguiente = NULL;

    if (frenteCola == NULL) {
        frenteCola = nuevoNodo;
        finCola = nuevoNodo;
    } else {
        finCola->siguiente = nuevoNodo;
        finCola = nuevoNodo;
    }

    aula->solicitudesActivas++;
    cout << "Solicitud registrada con ID #" << nuevoNodo->id << endl;
}

//mostrar todas las solicitudes en cola
void mostrarCola() {
    if (frenteCola == NULL) {
        cout << "\nLa cola de solicitudes esta vacia.\n";
        return;
    }

    cout << "\n--- SOLICITUDES EN COLA (FIFO) ---\n";
    NodoSolicitud* temp = frenteCola;
    while (temp != NULL) {
        cout << "ID: #" << temp->id 
             << " | Aula: " << temp->codigoAula 
             << " | Estado: " << temp->estado 
             << " | Reportante: " << temp->reportante 
             << " | Falla: " << temp->descripcion << endl;
        temp = temp->siguiente;
    }
}

//consultar primer elemento de la cola
void consultarFrenteCola() {
    if (frenteCola == NULL) {
        cout << "\nLa cola esta vacia.\n";
        return;
    }

    cout << "\n--- SOLICITUD AL FRENTE DE LA COLA ---\n";
    cout << "ID: #" << frenteCola->id << endl;
    cout << "Aula: " << frenteCola->codigoAula << endl;
    cout << "Estado: " << frenteCola->estado << endl;
    cout << "Reportante: " << frenteCola->reportante << endl;
    cout << "Descripcion: " << frenteCola->descripcion << endl;
}

//cambiar estado a en proceso
void iniciarAtencion() {
    if (frenteCola == NULL) {
        cout << "\nNo hay solicitudes en la cola.\n";
        return;
    }

    if (frenteCola->estado == "En proceso") {
        cout << "\nLa solicitud #" << frenteCola->id << " ya esta en proceso.\n";
        return;
    }

    frenteCola->estado = "En proceso";
    cout << "\nAtencion iniciada para la solicitud #" << frenteCola->id 
         << " del aula " << frenteCola->codigoAula << endl;
}

//desencolar solicitud y apilar en bitacora
void finalizarAtencion() {
    if (frenteCola == NULL) {
        cout << "\nNo hay solicitudes en la cola.\n";
        return;
    }

    if (frenteCola->estado != "En proceso") {
        cout << "\nDebe iniciar la atencion primero (cambiar a 'En proceso').\n";
        return;
    }

    string trabajo, tecnico;
    cin.ignore();
    cout << "\n--- FINALIZAR ATENCION DE SOLICITUD #" << frenteCola->id << " ---\n";
    cout << "Ingrese trabajo realizado: ";
    getline(cin, trabajo);
    cout << "Ingrese nombre del tecnico: ";
    getline(cin, tecnico);

    NodoSolicitud* atendida = frenteCola;//1. guardar referencia del nodo atendido
    string aulaAtendida = atendida->codigoAula;

    frenteCola = frenteCola->siguiente;//2. desencolar de la cola
    if (frenteCola == NULL) {
        finCola = NULL;
    }

    contadorIntervencion++;
    NodoIntervencion* nuevoInter = new NodoIntervencion();//3. crear nodo y apilar en la pila
    nuevoInter->numero = contadorIntervencion;
    nuevoInter->codigoAula = aulaAtendida;
    nuevoInter->trabajo = trabajo;
    nuevoInter->tecnico = tecnico;

    nuevoInter->siguiente = topePila;
    topePila = nuevoInter;

    NodoAula* aula = buscarAula(aulaAtendida);//4. decrementar contador de solicitudes activas
    if (aula != NULL) {
        aula->solicitudesActivas--;
    }

    delete atendida;//5. liberar memoria del nodo atendido

    cout << "Atencion finalizada e intervencion #" << contadorIntervencion << " registrada en la bitacora.\n";
}

// =========================================================================
// 4. funciones para la pila enlazada (bitacora)
// =========================================================================

//mostrar elementos de la pila (de reciente a antiguo)
void mostrarBitacora() {
    if (topePila == NULL) {
        cout << "\nLa bitacora de intervenciones esta vacia.\n";
        return;
    }

    cout << "\n--- BITACORA DE INTERVENCIONES (PILA - LIFO) ---\n";
    NodoIntervencion* temp = topePila;
    while (temp != NULL) {
        cout << "Nro Intervencion: #" << temp->numero 
             << " | Aula: " << temp->codigoAula 
             << " | Tecnico: " << temp->tecnico 
             << " | Trabajo: " << temp->trabajo << endl;
        temp = temp->siguiente;
    }
}

//consultar el tope de la pila
void consultarUltimaIntervencion() {
    if (topePila == NULL) {
        cout << "\nNo hay intervenciones en la bitacora.\n";
        return;
    }

    cout << "\n--- ULTIMA INTERVENCION (TOPE) ---\n";
    cout << "Nro Intervencion: #" << topePila->numero << endl;
    cout << "Aula: " << topePila->codigoAula << endl;
    cout << "Tecnico: " << topePila->tecnico << endl;
    cout << "Trabajo: " << topePila->trabajo << endl;
}

//desapilar elemento del tope de la pila
void anularUltimaIntervencion() {
    if (topePila == NULL) {
        cout << "\nNo hay intervenciones para anular.\n";
        return;
    }

    NodoIntervencion* temp = topePila;
    topePila = topePila->siguiente;

    cout << "\nIntervencion #" << temp->numero << " del aula " << temp->codigoAula << " anulada.\n";
    delete temp;
}

// =========================================================================
// 5. modulo de persistencia de datos (guardar y cargar en archivos txt)
// =========================================================================

//guardar todas las estructuras dinámicas en archivos de texto
void guardarDatos() {
    // 1. Guardar Aulas
    ofstream arqAulas("aulas.txt");
    if (arqAulas.is_open()) {
        NodoAula* temp = inicioLista;
        while (temp != NULL) {
            arqAulas << temp->codigo << "\n" 
                     << temp->pabellon << "\n" 
                     << temp->piso << "\n" 
                     << temp->solicitudesActivas << "\n";
            temp = temp->siguiente;
        }
        arqAulas.close();
    }

    // 2. Guardar Solicitudes de la Cola
    ofstream arqSolicitudes("solicitudes.txt");
    if (arqSolicitudes.is_open()) {
        arqSolicitudes << contadorIdSolicitud << "\n"; //guardar el correlativo actual
        NodoSolicitud* temp = frenteCola;
        while (temp != NULL) {
            arqSolicitudes << temp->id << "\n" 
                           << temp->codigoAula << "\n" 
                           << temp->descripcion << "\n" 
                           << temp->reportante << "\n" 
                           << temp->estado << "\n";
            temp = temp->siguiente;
        }
        arqSolicitudes.close();
    }

    // 3. Guardar Intervenciones de la Pila
    ofstream arqIntervenciones("intervenciones.txt");
    if (arqIntervenciones.is_open()) {
        arqIntervenciones << contadorIntervencion << "\n"; //guardar el correlativo actual
        NodoIntervencion* temp = topePila;
        while (temp != NULL) {
            arqIntervenciones << temp->numero << "\n" 
                              << temp->codigoAula << "\n" 
                              << temp->trabajo << "\n" 
                              << temp->tecnico << "\n";
            temp = temp->siguiente;
        }
        arqIntervenciones.close();
    }
}

//cargar todas las estructuras dinámicas desde archivos de texto al iniciar
void cargarDatos() {
    // 1. Cargar Aulas
    ifstream arqAulas("aulas.txt");
    if (arqAulas.is_open()) {
        string codigo, pabellon;
        int piso, solicitudes;
        while (arqAulas >> codigo >> pabellon >> piso >> solicitudes) {
            NodoAula* nuevoNodo = new NodoAula();
            nuevoNodo->codigo = codigo;
            nuevoNodo->pabellon = pabellon;
            nuevoNodo->piso = piso;
            nuevoNodo->solicitudesActivas = solicitudes;
            nuevoNodo->siguiente = NULL;

            if (inicioLista == NULL) {
                inicioLista = nuevoNodo;
            } else {
                NodoAula* temp = inicioLista;
                while (temp->siguiente != NULL) {
                    temp = temp->siguiente;
                }
                temp->siguiente = nuevoNodo;
            }
        }
        arqAulas.close();
    }

    // 2. Cargar Solicitudes de la Cola
    ifstream arqSolicitudes("solicitudes.txt");
    if (arqSolicitudes.is_open()) {
        if (arqSolicitudes >> contadorIdSolicitud) {
            arqSolicitudes.ignore();
            int id;
            string codigoAula, descripcion, reportante, estado;
            while (arqSolicitudes >> id) {
                arqSolicitudes.ignore();
                getline(arqSolicitudes, codigoAula);
                getline(arqSolicitudes, descripcion);
                getline(arqSolicitudes, reportante);
                getline(arqSolicitudes, estado);

                NodoSolicitud* nuevoNodo = new NodoSolicitud();
                nuevoNodo->id = id;
                nuevoNodo->codigoAula = codigoAula;
                nuevoNodo->descripcion = descripcion;
                nuevoNodo->reportante = reportante;
                nuevoNodo->estado = estado;
                nuevoNodo->siguiente = NULL;

                if (frenteCola == NULL) {
                    frenteCola = nuevoNodo;
                    finCola = nuevoNodo;
                } else {
                    finCola->siguiente = nuevoNodo;
                    finCola = nuevoNodo;
                }
            }
        }
        arqSolicitudes.close();
    }

    // 3. Cargar Intervenciones de la Pila
    ifstream arqIntervenciones("intervenciones.txt");
    if (arqIntervenciones.is_open()) {
        if (arqIntervenciones >> contadorIntervencion) {
            arqIntervenciones.ignore();
            int numero;
            string codigoAula, trabajo, tecnico;
            NodoIntervencion* ultimo = NULL;

            while (arqIntervenciones >> numero) {
                arqIntervenciones.ignore();
                getline(arqIntervenciones, codigoAula);
                getline(arqIntervenciones, trabajo);
                getline(arqIntervenciones, tecnico);

                NodoIntervencion* nuevoNodo = new NodoIntervencion();
                nuevoNodo->numero = numero;
                nuevoNodo->codigoAula = codigoAula;
                nuevoNodo->trabajo = trabajo;
                nuevoNodo->tecnico = tecnico;
                nuevoNodo->siguiente = NULL;

                if (topePila == NULL) {
                    topePila = nuevoNodo;
                    ultimo = nuevoNodo;
                } else {
                    ultimo->siguiente = nuevoNodo;
                    ultimo = nuevoNodo;
                }
            }
        }
        arqIntervenciones.close();
    }
}

//liberar memoria de todas las estructuras antes de salir
void liberarMemoria() {
    while (inicioLista != NULL) {
        NodoAula* temp = inicioLista;
        inicioLista = inicioLista->siguiente;
        delete temp;
    }
    while (frenteCola != NULL) {
        NodoSolicitud* temp = frenteCola;
        frenteCola = frenteCola->siguiente;
        delete temp;
    }
    while (topePila != NULL) {
        NodoIntervencion* temp = topePila;
        topePila = topePila->siguiente;
        delete temp;
    }
}

// =========================================================================
// 6. menu interactivo principal
// =========================================================================

int main() {
    cargarDatos(); //restaurar los registros guardados previamente

    int opcion;
    do {
        cout << "\n=========================================\n";
        cout << "   SISTEMA DE MANTENIMIENTO DE AULAS\n";
        cout << "=========================================\n";
        cout << "1. Registrar Aula\n";
        cout << "2. Mostrar Catalogo de Aulas\n";
        cout << "3. Modificar Aula\n";
        cout << "4. Eliminar Aula\n";
        cout << "-----------------------------------------\n";
        cout << "5. Registrar Solicitud (Encolar)\n";
        cout << "6. Mostrar Solicitudes en Cola\n";
        cout << "7. Consultar Siguiente Solicitud\n";
        cout << "8. Iniciar Atencion (En proceso)\n";
        cout << "9. Finalizar Atencion (Apilar Intervencion)\n";
        cout << "-----------------------------------------\n";
        cout << "10. Mostrar Bitacora de Intervenciones\n";
        cout << "11. Consultar Ultima Intervencion\n";
        cout << "12. Anular Ultima Intervencion\n";
        cout << "-----------------------------------------\n";
        cout << "0. Salir\n";
        cout << "=========================================\n";
        cout << "Ingrese una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: registrarAula(); break;
            case 2: mostrarAulas(); break;
            case 3: modificarAula(); break;
            case 4: eliminarAula(); break;
            case 5: registrarSolicitud(); break;
            case 6: mostrarCola(); break;
            case 7: consultarFrenteCola(); break;
            case 8: iniciarAtencion(); break;
            case 9: finalizarAtencion(); break;
            case 10: mostrarBitacora(); break;
            case 11: consultarUltimaIntervencion(); break;
            case 12: anularUltimaIntervencion(); break;
            case 0: 
                guardarDatos();   //guardar cambios en los archivos .txt
                liberarMemoria();  //liberar la memoria dinámica
                cout << "\nDatos guardados correctamente. Memoria liberada. Saliendo del programa...\n"; 
                break;
            default:
                cout << "\nOpcion invalida. Intente de nuevo.\n";
        }
    } while (opcion != 0);

    return 0;
}
