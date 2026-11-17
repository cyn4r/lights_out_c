#include <stdio.h>
#include <stdlib.h> // Para rand() y srand()
#include <time.h>   // Para time()

// Macros
#define MAX_DIM 5
#define LIMIT_3X3 15  // Cantidad maxima de intentos por partida (15/25/35) -> Es arbitrario y puedo modificar la dific cuando quiera.
#define LIMIT_4X4 25
#define LIMIT_5X5 35

// Agrupo las estadisticas en un struct - Soluciono el tema de declarar 15 variables distintas en el main (5x c/tablero).
struct Estadisticas {
    int jugadas;
    int ganadas;
    int perdidas;
    int abandonadas;
    int inter_totales;
};

// Declaro prototipo funciones
void inicializar_tablero(int dim, int tab[MAX_DIM][MAX_DIM], int opcion_config);
void imprimir_tablero(int dim, int tab[MAX_DIM][MAX_DIM]);
int verificar_victoria(int dim, int tab[MAX_DIM][MAX_DIM]);
void alternar_luces(int dim, int f, int c, int tab[MAX_DIM][MAX_DIM]);
void jugar_partida(int dim, int limite, int tab[MAX_DIM][MAX_DIM], struct Estadisticas *est, int usadas[3]);
void mostrar_estadisticas(struct Estadisticas e3, struct Estadisticas e4, struct Estadisticas e5);

int main() {
    int tablero[MAX_DIM][MAX_DIM]; // defino el tamanio maximo del tablero desde el inicio aunque luego use solo una porcion.
    int opcion; //capturo la seleccion del usuario en el menu

    // creo las 3 fichas(variables tipo struct estadisticas por c/tablero) e inicializo cada contador individual en 0.
    struct Estadisticas est3 = {0, 0, 0, 0, 0};
    struct Estadisticas est4 = {0, 0, 0, 0, 0};
    struct Estadisticas est5 = {0, 0, 0, 0, 0};

    // Arreglos para llevar registro de matrices usadas (0 = no usada, 1 = usada)
    int usadas3[3] = {0, 0, 0};
    int usadas4[3] = {0, 0, 0};
    int usadas5[3] = {0, 0, 0};

    // Semilla para generar numeros aleatorios
    srand(time(NULL));

    do {
        printf("\n=== LIGHTS OUT ===\n");
        printf("1. Jugar en 3x3\n");
        printf("2. Jugar en 4x4\n");
        printf("3. Jugar en 5x5\n");
        printf("0. Salir y ver estadisticas\n");
        printf("Opcion: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:   // cada case invoca la funcion jugar partida.
                jugar_partida(3, LIMIT_3X3, tablero, &est3, usadas3); // &est3 es la direccion en memoria, ya que necesita modificar
                break;
            case 2:
                jugar_partida(4, LIMIT_4X4, tablero, &est4, usadas4);
                break;
            case 3:
                jugar_partida(5, LIMIT_5X5, tablero, &est5, usadas5);
                break;
            case 0:
                mostrar_estadisticas(est3, est4, est5); // invoca la funcion mostrar estadisticas, invoca por valor, solo muestra.
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while(opcion != 0);

    return 0;
}

// Carga la configuracion inicial validando cual matriz usar
void inicializar_tablero(int dim, int tab[MAX_DIM][MAX_DIM], int opcion_config) {
    int i, j;
    // Primero apagamos todo
    for(i = 0; i < MAX_DIM; i++) {
        for(j = 0; j < MAX_DIM; j++) {
            tab[i][j] = 0;
        }
    }

    if (dim == 3) {    // configuraciones iniciales del tablero en cada tamanio
        if (opcion_config == 0) {
            tab[0][1] = 1; tab[1][0] = 1; tab[1][2] = 1; tab[2][1] = 1;
        } else if (opcion_config == 1) {
            tab[0][0] = 1; tab[0][2] = 1; tab[1][1] = 1; tab[2][0] = 1; tab[2][2] = 1;
        } else {
            tab[0][0] = 1; tab[0][1] = 1; tab[1][0] = 1; tab[1][2] = 1; tab[2][1] = 1; tab[2][2] = 1;
        }
    } else if (dim == 4) {
        if (opcion_config == 0) {
            tab[0][0] = 1; tab[1][1] = 1; tab[2][2] = 1; tab[3][3] = 1;
        } else if (opcion_config == 1) {
            tab[0][3] = 1; tab[1][2] = 1; tab[2][1] = 1; tab[3][0] = 1;
        } else {
            tab[1][1] = 1; tab[1][2] = 1; tab[2][1] = 1; tab[2][2] = 1;
        }
    } else if (dim == 5) {
        if (opcion_config == 0) {
            tab[1][1] = 1; tab[2][2] = 1; tab[3][3] = 1; tab[4][4] = 1; tab[0][0] = 1;
        } else if (opcion_config == 1) {
            tab[0][2] = 1; tab[1][2] = 1; tab[2][0] = 1; tab[2][1] = 1; tab[2][3] = 1; tab[2][4] = 1; tab[3][2] = 1; tab[4][2] = 1;
        } else {
            tab[0][0] = 1; tab[0][4] = 1; tab[4][0] = 1; tab[4][4] = 1; tab[2][2] = 1;
        }
    }
}

// Muestra el estado del tablero por pantalla
void imprimir_tablero(int dim, int tab[MAX_DIM][MAX_DIM]) {
    int i, j;
    printf("\n  ");
    // Imprimir indices de columnas
    for(j = 0; j < dim; j++) printf("%d ", j);
    printf("\n");

    for(i = 0; i < dim; i++) {
        printf("%d ", i); // Indice de fila
        for(j = 0; j < dim; j++) {
            if(tab[i][j] == 1) {
                printf("O "); // Luz encendida
            } else {
                printf(". "); // Luz apagada
            }
        }
        printf("\n");
    }
}

// Retorna 1 si gano (todo apagado), 0 si aun quedan luces
int verificar_victoria(int dim, int tab[MAX_DIM][MAX_DIM]) {
    int i, j;
    for(i = 0; i < dim; i++) {
        for(j = 0; j < dim; j++) {
            if(tab[i][j] == 1) { // itera buscando un 1, cuando lo encuentra, corta y se sale.
                return 0; // Encontro una luz encendida, no gano todavia
            }
        }
    }
    return 1; // Todo es 0
}

// Logica principal de la cruz (alternar celda y vecinos) x_x
void alternar_luces(int dim, int f, int c, int tab[MAX_DIM][MAX_DIM]) {
    tab[f][c] = 1 - tab[f][c];
    if(f - 1 >= 0) tab[f-1][c] = 1 - tab[f-1][c];  // Los if evitan irme de los parametros de cada tablero.
    if(f + 1 < dim) tab[f+1][c] = 1 - tab[f+1][c];
    if(c - 1 >= 0) tab[f][c-1] = 1 - tab[f][c-1];
    if(c + 1 < dim) tab[f][c+1] = 1 - tab[f][c+1];
}

// Motor del juego para una partida
void jugar_partida(int dim, int limite, int tab[MAX_DIM][MAX_DIM], struct Estadisticas *est, int usadas[3]) {
    int f, c, i, j;
    int interacciones = 0;
    int jugando = 1;
    int config_idx;
    int todas_usadas = 1;
    int tab_original[MAX_DIM][MAX_DIM]; // Respaldo para poder resetear

    // Verifico si ya usamos las 3 configuraciones disponibles
    for(i = 0; i < 3; i++) {
        if(usadas[i] == 0) {
            todas_usadas = 0;
            break;
        }
    }

    // Si ya jugamos las 3, las volvemos a habilitar poniendo todo en 0
    if(todas_usadas == 1) {
        for(i = 0; i < 3; i++) {
            usadas[i] = 0;
        }
    }

    // Busca un tablero al azar que no haya salido (que tenga un 0)
    do {
        config_idx = rand() % 3;
    } while (usadas[config_idx] == 1);

    usadas[config_idx] = 1; // Lo marco como usado

    est->jugadas++;      //puntero al struct
    inicializar_tablero(dim, tab, config_idx);

    // Guarda una copia exacta por si el jugador decide reiniciar
    for(i = 0; i < dim; i++) {
        for(j = 0; j < dim; j++) {
            tab_original[i][j] = tab[i][j];
        }
    }

    while(jugando) {
        imprimir_tablero(dim, tab);
        printf("\nInteracciones: %d / %d\n", interacciones, limite);
        printf("Ingrese fila (-1 abandonar | -2 reiniciar): ");
        scanf("%d", &f);

        // Opcion de abandonar
        if (f == -1) {
            printf("\nPartida abandonada.\n");
            est->abandonadas++;
            jugando = 0;
            break;
        }

        // Opcion de reiniciar
        if (f == -2) {
            printf("\nPartida reiniciada a su estado original.\n");
            interacciones = 0;
            // Restauro el tablero usando la matriz de respaldo
            for(i = 0; i < dim; i++) {
                for(j = 0; j < dim; j++) {
                    tab[i][j] = tab_original[i][j];
                }
            }
            continue; // Vuelve al inicio del bucle while
        }

        printf("Ingrese columna: ");
        scanf("%d", &c);

        // Valido coordenadas
        if(f >= 0 && f < dim && c >= 0 && c < dim) {
            alternar_luces(dim, f, c, tab);
            interacciones++;
            est->inter_totales++;

            if(verificar_victoria(dim, tab)) {
                imprimir_tablero(dim, tab);
                printf("\n¡FELICITACIONES! Has apagado todas las luces.\n");
                est->ganadas++;
                jugando = 0;
            } else if (interacciones >= limite) {
                imprimir_tablero(dim, tab);
                printf("\n¡PERDISTE! Has alcanzado el limite de interacciones.\n");
                est->perdidas++;
                jugando = 0;
            }
        } else {
            printf("\n¡Coordenadas fuera de limites! Intenta de nuevo.\n");
        }
    }
}

// Imprime el resumen final
void mostrar_estadisticas(struct Estadisticas e3, struct Estadisticas e4, struct Estadisticas e5) {
    printf("\n=== ESTADISTICAS FINALES ===\n");

    printf("\nPartidas en 3x3\n");
    printf("%d Ganadas vs %d Perdidas\n", e3.ganadas, e3.perdidas);
    printf("%d Jugadas\n", e3.jugadas);
    if(e3.abandonadas > 0) printf("%d Abandonadas\n", e3.abandonadas);
    printf("%d Interacciones totales\n", e3.inter_totales);

    printf("\nPartidas en 4x4\n");
    printf("%d Ganadas vs %d Perdidas\n", e4.ganadas, e4.perdidas);
    printf("%d Jugadas\n", e4.jugadas);
    if(e4.abandonadas > 0) printf("%d Abandonadas\n", e4.abandonadas);
    printf("%d Interacciones totales\n", e4.inter_totales);

    printf("\nPartidas en 5x5\n");
    printf("%d Ganadas vs %d Perdidas\n", e5.ganadas, e5.perdidas);
    printf("%d Jugadas\n", e5.jugadas);
    if(e5.abandonadas > 0) printf("%d Abandonadas\n", e5.abandonadas);
    printf("%d Interacciones totales\n", e5.inter_totales);
    printf("\n");
}
