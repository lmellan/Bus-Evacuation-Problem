#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;
using namespace chrono;

struct Viaje {
    int punto;
    int refugio;
};

struct Bus {
    vector<Viaje> viajes;
    int tiempo_total = 0;
    int estacion;
    int ultima_pos = -1; // último refugio visitado
};

// se obtiene el tiempo que le toma al bus más lento en completar todos sus viajes
int calcular_makespan(const vector<Bus>& buses) {
    int max_tiempo = 0;
    for (const auto& b : buses) max_tiempo = max(max_tiempo, b.tiempo_total);
    return max_tiempo;
}

string limpiar_espacios(string s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
    return s;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: ./BEP <archivo_instancia.txt>" << endl;
        return 1;
    }

    ifstream in(argv[1]);
    if (!in) {
        cerr << "No se pudo abrir el archivo de instancia." << endl;
        return 1;
    }

    // Lectura de la instancia
    auto start = high_resolution_clock::now();

    string linea;
    int B, C;
    getline(in, linea);
    linea = limpiar_espacios(linea);
    stringstream ss1(linea);
    ss1 >> B;
    ss1.ignore();
    ss1 >> C;

    int Y;
    getline(in, linea);
    linea = limpiar_espacios(linea);
    stringstream ss2(linea);
    ss2 >> Y;
    ss2.ignore();
    vector<int> buses_por_estacion(Y);
    for (int i = 0; i < Y; i++) ss2 >> buses_por_estacion[i];

    int P, total_personas;
    getline(in, linea);
    linea = limpiar_espacios(linea);
    stringstream ss3(linea);
    ss3 >> P;
    ss3.ignore();
    ss3 >> total_personas;
    ss3.ignore();
    vector<int> q_p(P);
    for (int i = 0; i < P; i++) ss3 >> q_p[i];

    int R, capacidad_total;
    getline(in, linea);
    linea = limpiar_espacios(linea);
    stringstream ss4(linea);
    ss4 >> R;
    ss4.ignore();
    ss4 >> capacidad_total;
    ss4.ignore();
    vector<int> u_r(R);
    for (int i = 0; i < R; i++) ss4 >> u_r[i];

    getline(in, linea); // salto
    vector<vector<int>> d_ini(Y, vector<int>(P));
    for (int y = 0; y < Y; y++) {
        getline(in, linea);
        linea = limpiar_espacios(linea);
        size_t pos = linea.find(":");
        if (pos != string::npos) linea = linea.substr(pos + 1);
        linea = limpiar_espacios(linea);
        stringstream ss(linea);
        for (int p = 0; p < P; p++) ss >> d_ini[y][p];
    }

    getline(in, linea); // salto
    vector<vector<int>> d_pr(P, vector<int>(R));
    for (int p = 0; p < P; p++) {
        getline(in, linea);
        linea = limpiar_espacios(linea);
        size_t pos = linea.find(":");
        if (pos != string::npos) linea = linea.substr(pos + 1);
        linea = limpiar_espacios(linea);
        stringstream ss(linea);
        for (int r = 0; r < R; r++) ss >> d_pr[p][r];
    }

    vector<Bus> buses;
    for (int y = 0; y < Y; y++) {
        for (int i = 0; i < buses_por_estacion[y]; i++) {
            buses.push_back(Bus{{}, 0, y, -1});
        }
    }

    vector<Bus> solucion_greedy = buses;
    vector<int> q_p_greedy = q_p;
    vector<int> u_r_greedy = u_r;

    //Paso 1: Se aplica el algoritmo Greedy para construir la solución inicial
    while (true) {
        // se verifica si quedan personas por evacuar
        int total_restantes = 0;
        for (int p : q_p_greedy) total_restantes += p;
        if (total_restantes == 0) break;

        // se busca el punto con más personas restantes
        int p_max = -1, max_personas = -1;
        for (int i = 0; i < P; i++) {
            if (q_p_greedy[i] > max_personas) {
                max_personas = q_p_greedy[i];
                p_max = i;
            }
        }
        if (p_max == -1) break;  // se debe eliminar

        // se busca el mejor refugio disponible (más cercano con capacidad)
        int mejor_refugio = -1, mejor_dist = numeric_limits<int>::max();
        int personas_a_evacuarse = 0;
        for (int r = 0; r < R; r++) {
            int posibles = min({C, q_p_greedy[p_max], u_r_greedy[r]});
            if (posibles > 0 && d_pr[p_max][r] < mejor_dist) {
                mejor_refugio = r;
                mejor_dist = d_pr[p_max][r];
                personas_a_evacuarse = posibles;
            }
        }
        if (mejor_refugio == -1 || personas_a_evacuarse == 0) {
            q_p_greedy[p_max] = 0;
            continue;
        }

        // se selecciona el bus que puede hacer el viaje en el menor tiempo adicional
        int mejor_bus = -1, menor_tiempo = numeric_limits<int>::max();
        for (int b = 0; b < (int)solucion_greedy.size(); b++) {
            int tiempo = solucion_greedy[b].tiempo_total;
            int extra = 0;
            if (solucion_greedy[b].viajes.empty()) {
                extra += d_ini[solucion_greedy[b].estacion][p_max];
            } else {
                // aquí sería ideal usar una matriz d_rp, pero como no existe, asumimos un costo simétrico
                extra += d_pr[p_max][solucion_greedy[b].ultima_pos];
            }
            extra += d_pr[p_max][mejor_refugio];
            if (tiempo + extra < menor_tiempo) {
                mejor_bus = b;
                menor_tiempo = tiempo + extra;
            }
        }

        // se actualizan los valores considerando que se evacuaron personas 
        q_p_greedy[p_max] -= personas_a_evacuarse;
        u_r_greedy[mejor_refugio] -= personas_a_evacuarse;

        Bus& bus = solucion_greedy[mejor_bus];
        bus.viajes.push_back({p_max, mejor_refugio});

        if (bus.viajes.size() == 1) {
            bus.tiempo_total += d_ini[bus.estacion][p_max];
        } else {
            // Usamos d_pr simétricamente como si fuera distancia refugio → punto
            bus.tiempo_total += d_pr[p_max][bus.ultima_pos];
        }

        bus.tiempo_total += d_pr[p_max][mejor_refugio];
        bus.ultima_pos = mejor_refugio;
    }

    int makespan_greedy = calcular_makespan(solucion_greedy);
    cout << "\n\n--- Solución Greedy ---" << endl;
    for (int b = 0; b < (int)solucion_greedy.size(); b++) {
        cout << "Bus " << b + 1 << ": ";
        for (auto& v : solucion_greedy[b].viajes)
            cout << "(" << v.punto + 1 << "," << v.refugio + 1 << ") ";
        cout << "| Tiempo: " << solucion_greedy[b].tiempo_total << endl;
    }
    cout << "Makespan Greedy: " << makespan_greedy << endl;

// Paso 2: Se aplica HC con mejor mejora, intercambiando viajes entre el bus más lento y el más rápido
// Paso 2: Se aplica HC con mejor mejora, moviendo el viaje del bus más lento al más rápido
vector<Bus> mejor_solucion = solucion_greedy;
bool mejora = true;
while (mejora) {
    mejora = false;

    // se identifican los índices del bus más lento (mayor tiempo) y el más rápido (menor tiempo)
    int idx_lento = -1, max_t = -1;
    int idx_rapido = -1, min_t = numeric_limits<int>::max();
    for (int i = 0; i < (int)mejor_solucion.size(); i++) {
        if (mejor_solucion[i].tiempo_total > max_t) {
            max_t = mejor_solucion[i].tiempo_total;
            idx_lento = i;
        }
        if (mejor_solucion[i].tiempo_total < min_t) {
            min_t = mejor_solucion[i].tiempo_total;
            idx_rapido = i;
        }
    }

    // si se encuentran buses válidos distintos, se intentan movimientos
    if (idx_lento != -1 && idx_rapido != -1 && idx_lento != idx_rapido) {
        int mejor_makespan = calcular_makespan(mejor_solucion);
        vector<Bus> mejor_vecino = mejor_solucion;

        // se prueban todos los viajes del bus más lento
        for (int i = 0; i < (int)mejor_solucion[idx_lento].viajes.size(); i++) {
            Bus copia_lento = mejor_solucion[idx_lento];
            Bus copia_rapido = mejor_solucion[idx_rapido];

            Viaje movido = copia_lento.viajes[i];
            copia_lento.viajes.erase(copia_lento.viajes.begin() + i);
            copia_rapido.viajes.push_back(movido);

            auto calc_tiempo = [&](const Bus& b) {
                if (b.viajes.empty()) return 0;
                int total = d_ini[b.estacion][b.viajes[0].punto];
                total += d_pr[b.viajes[0].punto][b.viajes[0].refugio];
                for (int j = 1; j < (int)b.viajes.size(); j++) {
                    total += d_pr[b.viajes[j].punto][b.viajes[j - 1].refugio];
                    total += d_pr[b.viajes[j].punto][b.viajes[j].refugio];
                }
                return total;
            };

            copia_lento.tiempo_total = calc_tiempo(copia_lento);
            copia_rapido.tiempo_total = calc_tiempo(copia_rapido);

            vector<Bus> vecino = mejor_solucion;
            vecino[idx_lento] = copia_lento;
            vecino[idx_rapido] = copia_rapido;

            int nuevo_makespan = calcular_makespan(vecino);
            if (nuevo_makespan < mejor_makespan) {
                mejor_makespan = nuevo_makespan;
                mejor_vecino = vecino;
                mejora = true;
            }
        }

        // aplicar el mejor movimiento encontrado
        if (mejora) {
            mejor_solucion = mejor_vecino;
        }
    }
}


int final_makespan = calcular_makespan(mejor_solucion);


    cout << "\n--- Solución Hill Climbing ---" << endl;
    for (int b = 0; b < (int)mejor_solucion.size(); b++) {
        cout << "Bus " << b + 1 << ": ";
        for (auto& v : mejor_solucion[b].viajes)
            cout << "(" << v.punto + 1 << "," << v.refugio + 1 << ") ";
        cout << "| Tiempo: " << mejor_solucion[b].tiempo_total << endl;
    }
    cout << "Makespan Final: " << final_makespan << endl;

    // se muestra el tiempo total de la ejecución del algoritmo
    auto end = high_resolution_clock::now();
    duration<double, milli> duration_ms = end - start;
    cout << "\nTiempo de ejecución (ms): " << duration_ms.count() << endl;

    return 0;
}
