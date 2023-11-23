// Fecha creación: 23/11/2023
// Fecha modificación: 23/11/2023
// Número de ejericio: 1
// Problema planteado: Tabla de pocisiones de la LIgaBoliviana

#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

struct structEquiposLPFB {
    char nombreEquipo[50];
    char colorCamiseta[50];
    char departamento[30];
    int anioCreacion;
    int partidosJugados;
    int partidosGanados;
    int partidosEmpatados;
    int partidosPerdidos;
    int golesFavor;
    int golesContra;
    int diferenciaGoles; // Se calculará como golesFavor - golesContra
    int puntos;
};

struct structResultadosLPFB {
    char nombreEquipoLocal[50];
    char nombreEquipoVisitante[50];
    int golesEquipoLocal;
    int golesEquipoVisitante;
    char fechaPartido[10];
};

bool existeEquipo(const char* nombreEquipo, structEquiposLPFB &equipo);
void ingresarDatosEquipo();
void ingresarResultadoPartido();
void mostrarTablaPosiciones();
void actualizarEstadisticasEquipo(const char* nombreEquipo, int golesFavor, int golesContra, bool equipoGanador, bool empate);
void ordenarPorPuntos(structEquiposLPFB* equipos, int numEquipos);

int main() {
    setlocale(LC_ALL, "");
    int opcion;
    do {
        cout << "Menú de opciones:\n";
        cout << "1. Ingreso de datos de los equipos de la LPFB.\n";
        cout << "2. Ingreso de los resultados de los partidos.\n";
        cout << "3. Reporte de la tabla de posiciones.\n";
        cout << "4. Salir.\n";
        cout << "Ingrese su opción: ";
        cin >> opcion;

        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            cout << "Opción no válida. Por favor intente de nuevo.\n";
            continue;
        }

        switch (opcion) {
            case 1:
                ingresarDatosEquipo();
                break;
            case 2:
                ingresarResultadoPartido();
                break;
            case 3:
                mostrarTablaPosiciones();
                break;
            case 4:
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                cout << "Opción no válida. Por favor intente de nuevo.\n";
        }
    } while (opcion != 4);

    return 0;
}


bool existeEquipo(const char* nombreEquipo, structEquiposLPFB &equipoOut) {

    ifstream archivo("EquiposLPFB.bin", ios::binary);
    structEquiposLPFB equipo={};
    if (!archivo) {
        return false;
    }
    while (archivo.read((char*)&equipo, sizeof(structEquiposLPFB))) {
        if (strcmp(equipo.nombreEquipo, nombreEquipo) == 0) {
            archivo.close();
            equipoOut = equipo; // Copia la información del equipo encontrado
            return true;
        }
    }
    archivo.close();
    return false;
}

void ingresarDatosEquipo() {
    structEquiposLPFB equipo = {};
    cout << "Ingrese el nombre del equipo: ";
    cin.ignore();
    cin.getline(equipo.nombreEquipo, 50);

    structEquiposLPFB equipoExistente;
    if (existeEquipo(equipo.nombreEquipo, equipoExistente)) {
        cout << "El equipo ya existe.\n";
        return;
    }

    cout << "Ingrese el color de la camiseta: ";
    cin.getline(equipo.colorCamiseta, 50);
    cout << "Ingrese el departamento: ";
    cin.getline(equipo.departamento, 30);
    cout << "Ingrese el año de creación: ";
    cin >> equipo.anioCreacion;
    cin.ignore();
    equipo.partidosJugados = 0;
    equipo.partidosGanados = 0;
    equipo.partidosEmpatados = 0;
    equipo.partidosPerdidos = 0;
    equipo.golesFavor = 0;
    equipo.golesContra = 0;
    equipo.diferenciaGoles = 0;
    equipo.puntos = 0;

    cin.ignore();
    ofstream archivo("EquiposLPFB.bin", ios::binary | ios::app);
    archivo.write((char*)&equipo, sizeof(structEquiposLPFB));
    archivo.close();

    cout << "Equipo ingresado con éxito.\n";
}
void ingresarResultadoPartido() {
    char continuar;
    do {
        structResultadosLPFB resultado;
        cout << "Ingrese el nombre del equipo local: ";
        cin >> ws; // Esto consume cualquier espacio en blanco que sobra, incluyendo newlines.
        cin.getline(resultado.nombreEquipoLocal, 50);

        structEquiposLPFB equipoLocal, equipoVisitante;
        if (!existeEquipo(resultado.nombreEquipoLocal, equipoLocal)) {
            cout << "El equipo local no existe.\n";
            return;
        }

        cout << "Ingrese el nombre del equipo visitante: ";
        cin.getline(resultado.nombreEquipoVisitante, 50);
        if (!existeEquipo(resultado.nombreEquipoVisitante, equipoVisitante)) {
            cout << "El equipo visitante no existe.\n";
            return;
        }

        cout << "Ingrese los goles del equipo local: ";
        cin >> resultado.golesEquipoLocal;
        cin.ignore(); // Limpia la entrada después de leer un número

        cout << "Ingrese los goles del equipo visitante: ";
        cin >> resultado.golesEquipoVisitante;
        cin.ignore();

        // Guardar'a el resultado del partido en el archivo
        ofstream archivoResultados("ResultadosLPFB.bin", ios::binary | ios::app);
        if (!archivoResultados) {
            cout << "No se pudo abrir el archivo de resultados para escritura.\n";
            return;
        }
        archivoResultados.write((char*)&resultado, sizeof(structResultadosLPFB));
        archivoResultados.close();

        // Determinar'a el resultado del partido y actualiza
        bool localGanador = resultado.golesEquipoLocal > resultado.golesEquipoVisitante;
        bool empate = resultado.golesEquipoLocal == resultado.golesEquipoVisitante;
        bool visitanteGanador = resultado.golesEquipoLocal < resultado.golesEquipoVisitante;

        // para el local
        actualizarEstadisticasEquipo(resultado.nombreEquipoLocal, resultado.golesEquipoLocal, resultado.golesEquipoVisitante, localGanador, empate);

        // para el visitante
        actualizarEstadisticasEquipo(resultado.nombreEquipoVisitante, resultado.golesEquipoVisitante, resultado.golesEquipoLocal, visitanteGanador, empate);
        }while (continuar == 'S' || continuar == 's'); // Asegúrate de que esta línea está presente
}

void actualizarEstadisticasEquipo(const char* nombreEquipo, int golesFavor, int golesContra, bool equipoGanador, bool empate) {
    fstream archivoEquipos("EquiposLPFB.bin", ios::in | ios::out | ios::binary);
    if (!archivoEquipos) {
        cout << "No se pudo abrir el archivo de equipos para actualizar las estadísticas.\n";
        return;
    }

    structEquiposLPFB equipo;
    while (archivoEquipos.read((char*)&equipo, sizeof(structEquiposLPFB))) {
        if (strcmp(equipo.nombreEquipo, nombreEquipo) == 0) {
            equipo.partidosJugados++;
            equipo.golesFavor += golesFavor;
            equipo.golesContra += golesContra;
            equipo.diferenciaGoles = equipo.golesFavor - equipo.golesContra;

            if (equipoGanador) {
                equipo.partidosGanados++;
                equipo.puntos += 3;
            } else if (empate) {
                equipo.partidosEmpatados++;
                equipo.puntos += 1;
            } else {
                equipo.partidosPerdidos++;
            }

            // calcula las posiciones
            long pos = archivoEquipos.tellg();
            archivoEquipos.seekp(pos - sizeof(structEquiposLPFB));
            archivoEquipos.write((char*)&equipo, sizeof(structEquiposLPFB));// Escribe lo actualizado
            break;
        }
    }

    archivoEquipos.close();
}



void ordenarPorPuntos(structEquiposLPFB* equipos, int numEquipos) {
    bool intercambiado;
    do {
        intercambiado = false;
        for (int i = 0; i < numEquipos - 1; i++) {
            if (equipos[i].puntos < equipos[i + 1].puntos) {
                structEquiposLPFB temp = equipos[i];
                equipos[i] = equipos[i + 1];
                equipos[i + 1] = temp;
                intercambiado = true;
            }
        }
    } while (intercambiado);
}

void mostrarTablaPosiciones() {
    ifstream archivo("EquiposLPFB.bin", ios::binary);
    if (!archivo) {
        cout << "No se pudo abrir el archivo o todavía no hay equipos registrados.\n";
        return;
    }

    archivo.seekg(0, ios::end);
    int numEquipos = archivo.tellg() / sizeof(structEquiposLPFB);
    archivo.seekg(0, ios::beg);

    structEquiposLPFB* equipos = new structEquiposLPFB[numEquipos];
    for (int i = 0; i < numEquipos; ++i) {
        equipos[i] = {};
        archivo.read((char*)&equipos[i], sizeof(structEquiposLPFB));
    }
    archivo.close();

    ordenarPorPuntos(equipos, numEquipos);

    cout << "Club                  PJ   G   E   P   GF  GC  DG  Pts\n";
    for (int i = 0; i < numEquipos; ++i) {
        cout << equipos[i].nombreEquipo;
        cout << string(22 - strlen(equipos[i].nombreEquipo), ' '); // Rellena con espacios hasta 22 caracteres
        cout << equipos[i].partidosJugados << "   ";
        cout << equipos[i].partidosGanados << "   ";
        cout << equipos[i].partidosEmpatados << "   ";
        cout << equipos[i].partidosPerdidos << "   ";
        cout << equipos[i].golesFavor << "   ";
        cout << equipos[i].golesContra << "   ";
        cout << equipos[i].diferenciaGoles << "   ";
        cout << equipos[i].puntos << endl;
    }

    delete[] equipos;
}
