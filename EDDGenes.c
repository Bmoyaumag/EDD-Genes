#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_GEN_MAXIMO 10


//listas globales para guardar todos los genes max y min
char max_genes[256][TAM_GEN_MAXIMO + 1];
int max_count_genes = 0;

char min_genes[256][TAM_GEN_MAXIMO + 1];
int min_count_genes = 0;

typedef struct ListaEnteros 
{
    int* posiciones;
    int cantidad;
    int capacidad;
} ListaEnteros;

typedef struct NodoTrie 
{
    struct NodoTrie* hijos[4];
    ListaEnteros* ocurrencias;
    int esHoja;
} NodoTrie;
// Función que convierte un carácter base (A,C,G,T) en un índice numérico (0-3).
// Retorna -1 si el carácter no es una base válida.
int base_a_indice(char base)
{
    int indice;
    char bases[] = { 'A', 'C', 'G', 'T' };
    for (indice = 0; indice < 4; indice++)
    {
        if (base == bases[indice])
        {
            return indice;
        }
    }
    return -1;
}
// Función inversa que convierte un índice numérico (0-3) en un carácter base.
// Retorna '?' si el índice no es válido.
char indice_a_base(int indice)
{
    char bases[] = { 'A', 'C', 'G', 'T' };
    if (indice >= 0 && indice < 4)
        return bases[indice];
    return '?';
}
// Crea un nodo del árbol trie para genes al nivel dado.
// Nivel máximo determina la altura total del árbol.
// Inicializa listas de posiciones solo en nodos hoja.
NodoTrie* crear_nodo(int nivel, int nivel_max)
{
    int i;
    NodoTrie* nodo = (NodoTrie*)malloc(sizeof(NodoTrie));
    nodo->esHoja = (nivel == nivel_max) ? 1 : 0;
    nodo->ocurrencias = (nodo->esHoja) ? (ListaEnteros*)malloc(sizeof(ListaEnteros)) : NULL;
    if (nodo->esHoja)
    {
        nodo->ocurrencias->posiciones = NULL;
        nodo->ocurrencias->cantidad = 0;
        nodo->ocurrencias->capacidad = 0;
    }
    for (i = 0; i < 4; i++)
    {
        nodo->hijos[i] = NULL;
        if (nivel < nivel_max)
        {
            nodo->hijos[i] = crear_nodo(nivel + 1, nivel_max);
        }
    }
    return nodo;
}
// Agrega la posición de un gen a la lista dinámica del nodo hoja.
// Maneja realocación dinámica para ampliar la lista.
void agregar_posicion(ListaEnteros* lista, int posicion)
{
    int* nuevas_posiciones;
    if (lista->cantidad == lista->capacidad)
    {
        lista->capacidad = (lista->capacidad == 0) ? 8 : lista->capacidad * 2;
        nuevas_posiciones = (int*)realloc(lista->posiciones, lista->capacidad * sizeof(int));
        if (!nuevas_posiciones) return;
        lista->posiciones = nuevas_posiciones;
    }
    lista->posiciones[lista->cantidad] = posicion;
    lista->cantidad++;
}
// Libera la memoria utilizada por una lista dinámica de posiciones.
void liberar_lista(ListaEnteros* lista)
{
    if (!lista) return;
    free(lista->posiciones);
    free(lista);
}
// Libera recursivamente la memoria de todo el árbol trie incluyendo listas.
void liberar_trie(NodoTrie* nodo, int nivel, int nivel_max)
{
    int i;
    if (!nodo) return;
    if (nivel == nivel_max)
    {
        liberar_lista(nodo->ocurrencias);
    }
    for (i = 0; i < 4; i++)
    {
        liberar_trie(nodo->hijos[i], nivel + 1, nivel_max);
    }
    free(nodo);
}
// Inicializa la raíz del trie para genes de tamaño tam_gen.
NodoTrie* inicializar_trie(int tam_gen)
{
    NodoTrie* raiz = crear_nodo(0, tam_gen);
    return raiz;
}
// Inserta una ocurrencia de un gen en el trie con su posición dentro de la secuencia.
void insertar_gen(NodoTrie* raiz, const char* gen, int tam_gen, int posicion)
{
    int i, indice;
    NodoTrie* actual = raiz;
    for (i = 0; i < tam_gen; i++)
    {
        indice = base_a_indice(gen[i]);
        if (indice == -1) return;
        actual = actual->hijos[indice];
    }
    agregar_posicion(actual->ocurrencias, posicion);
}
// Procesa toda la secuencia y registra todas las posiciones de genes posibles en el trie.
void procesar_secuencia(NodoTrie* raiz, const char* secuencia, int largo_secuencia, int tam_gen)
{
    int i;
    char gen[TAM_GEN_MAXIMO + 1];
    for (i = 0; i <= largo_secuencia - tam_gen; i++)
    {
        strncpy(gen, secuencia + i, tam_gen);
        gen[tam_gen] = '\0';
        insertar_gen(raiz, gen, tam_gen, i);
    }
}
// Busca un gen específico en el trie y retorna la lista de posiciones donde aparece.
// Retorna NULL si el gen no es válido o no existe.
ListaEnteros* buscar_gen(NodoTrie* raiz, const char* gen, int tam_gen)
{
    int i, indice;
    NodoTrie* actual = raiz;
    for (i = 0; i < tam_gen; i++)
    {
        indice = base_a_indice(gen[i]);
        if (indice == -1) return NULL;
        actual = actual->hijos[indice];
        if (!actual) return NULL;
    }
    return actual->ocurrencias;
}
// Valida que un gen tenga el largo correcto y solo contenga bases válidas (A,C,G,T).
int es_gen_valido(const char* gen, int tam_gen)
{
    int i;
    if ((int)strlen(gen) != tam_gen) return 0;
    for (i = 0; i < tam_gen; i++)
    {
        if (base_a_indice(gen[i]) == -1)
            return 0;
    }
    return 1;
}
// Muestra la ayuda con los comandos disponibles para el usuario.
void mostrar_help()
{
    printf("\nComandos disponibles:\n");
    printf("iniciar <n>  - Crear árbol para genes de tamaño n\n");
    printf("leer <archivo> - Cargar y procesar secuencia desde archivo\n");
    printf("buscar <GEN>  - Buscar posiciones de un gen\n");
    printf("max          - Mostrar gen mas repetido\n");
    printf("min          - Mostrar gen menos repetido\n");
    printf("all          - Mostrar todos los genes y sus repeticiones\n");
    printf("help         - Mostrar esta ayuda\n");
    printf("salir        - Salir del programa\n\n");
}
// Función recursiva para recorrer el trie y encontrar genes con máximo y mínimo número de repeticiones.
// Guarda los genes y cantidades máximas y mínimas encontradas.

//agrega lsita
void recorrer_trie(NodoTrie* nodo, int nivel, int nivel_max, char* buffer,
                   int* max_count, int* min_count)
{
    int i;

    if (!nodo) return;

    if (nivel == nivel_max)
    {
        int cantidad = nodo->ocurrencias->cantidad;

        if (cantidad > 0)
        {
            buffer[nivel] = '\0';

            // --- MAX ---
            if (cantidad > *max_count)
            {
                *max_count = cantidad;
                max_count_genes = 0;              // reset lista
                strcpy(max_genes[max_count_genes++], buffer);
            }
            else if (cantidad == *max_count)
            {
                strcpy(max_genes[max_count_genes++], buffer);
            }

            // --- MIN ---
            if (*min_count == -1 || cantidad < *min_count)
            {
                *min_count = cantidad;
                min_count_genes = 0;
                strcpy(min_genes[min_count_genes++], buffer);
            }
            else if (cantidad == *min_count)
            {
                strcpy(min_genes[min_count_genes++], buffer);
            }
        }
        return;
    }

    for (i = 0; i < 4; i++)
    {
        buffer[nivel] = indice_a_base(i);
        recorrer_trie(nodo->hijos[i], nivel + 1, nivel_max, buffer, max_count, min_count);
    }
}

// Función recursiva para imprimir todos los genes almacenados en el trie junto a la cantidad de repeticiones.
void imprimir_todos_gener(NodoTrie* nodo, int nivel, int nivel_max, char* buffer)
{
    int i;
    if (!nodo) return;

    if (nivel == nivel_max)
    {
        int cantidad = nodo->ocurrencias->cantidad;
        if (cantidad > 0)
        {
            buffer[nivel] = '\0';

            // Imprimir el gen
            printf("%s ", buffer);

            // Imprimir todas sus posiciones
            for (int j = 0; j < cantidad; j++)
            {
                printf("%d ", nodo->ocurrencias->posiciones[j]);
            }

            printf("\n");
        }
        return;
    }

    for (i = 0; i < 4; i++)
    {
        buffer[nivel] = indice_a_base(i);
        imprimir_todos_gener(nodo->hijos[i], nivel + 1, nivel_max, buffer);
    }
}





int main()
{
    NodoTrie* trie = NULL; // Estructura trie para almacenar genes
    int tam_gen = 0; // Tamaño de los genes (altura del trie)
    char comando[32]; // Comando ingresado por el usuario
    char nombre_archivo[128]; // Nombre o ruta del archivo con secuencia de ADN
    char gen[TAM_GEN_MAXIMO + 1]; // Variable para almacenar gen ingresado por usuario
    char secuencia[4096]; // Buffer para cargar la secuencia desde archivo
    FILE* archivo; // Puntero a archivo para lectura
    int largo_secuencia; // Longitud de la secuencia leída
    int i;
    ListaEnteros* resultado; // Puntero a lista de posiciones devuelto por buscar_gen
    char buffer[TAM_GEN_MAXIMO + 1]; // Buffer para operaciones en funciones recursivas
    char gen_max[TAM_GEN_MAXIMO + 1]; // Buffer para almacenar gen más repetido
    char gen_min[TAM_GEN_MAXIMO + 1]; // Buffer para almacenar gen menos repetido
    int max_cantidad = -1; // Cantidad máxima de repeticiones encontradas
    int min_cantidad = -1; // Cantidad mínima de repeticiones encontradas

    mostrar_help();
    // Bucle principal de interacción con el usuario
    while (1)
    {
        printf("bio> ");
        scanf("%s", comando);

        if (strcmp(comando, "iniciar") == 0)
        {
            scanf("%d", &tam_gen);
            trie = inicializar_trie(tam_gen);
            printf("Arbol creado con altura %d\n", tam_gen);
        }
        else if (strcmp(comando, "leer") == 0)
        {
            scanf("%s", nombre_archivo);
            archivo = fopen(nombre_archivo, "r");
            if (!archivo)
            {
                printf("Error al abrir archivo\n");
                continue;
            }
            fgets(secuencia, sizeof(secuencia), archivo);
            fclose(archivo);
            largo_secuencia = strlen(secuencia);
            if (secuencia[largo_secuencia - 1] == '\n')
                secuencia[--largo_secuencia] = '\0';
            procesar_secuencia(trie, secuencia, largo_secuencia, tam_gen);
            printf("Secuencia leída del archivo\n");
        }
        else if (strcmp(comando, "buscar") == 0)
        {
            scanf("%s", gen);
            if (!es_gen_valido(gen, tam_gen))
            {
                printf("-1\n");
                continue;
            }
            resultado = buscar_gen(trie, gen, tam_gen);
            if (!resultado || resultado->cantidad == 0)
            {
                printf("-1\n");
                continue;
            }
            for (i = 0; i < resultado->cantidad; i++)
            {
                printf("%d ", resultado->posiciones[i]);
            }
            printf("\n");
        }
        //cambio
        else if (strcmp(comando, "max") == 0)
        {
            max_cantidad = -1;
            min_cantidad = -1;
            max_count_genes = 0;

            recorrer_trie(trie, 0, tam_gen, buffer, &max_cantidad, &min_cantidad);

            if (max_cantidad <= 0)
            {
                printf("-1\n");
                continue;
            }

            for (i = 0; i < max_count_genes; i++) 
            {
                printf("%s ", max_genes[i]);

                ListaEnteros* pos = buscar_gen(trie, max_genes[i], tam_gen);
                if (pos) 
                {
                    for (int j = 0; j < pos->cantidad; j++)
                        printf("%d ", pos->posiciones[j]);
                }

                printf("\n");
            }
        }
        else if (strcmp(comando, "min") == 0)
        {
            max_cantidad = -1;
            min_cantidad = -1;
            min_count_genes = 0;

            recorrer_trie(trie, 0, tam_gen, buffer, &max_cantidad, &min_cantidad);

            if (min_cantidad < 0)
            {
                printf("-1\n");
                continue;
            }

            for (i = 0; i < min_count_genes; i++)
            {
                printf("%s ", min_genes[i]);

                ListaEnteros* pos = buscar_gen(trie, min_genes[i], tam_gen);
                if (pos) 
                {
                    for (int j = 0; j < pos->cantidad; j++)
                        printf("%d ", pos->posiciones[j]);
                }

                printf("\n");
            }
        }
        else if (strcmp(comando, "all") == 0)
        {
            imprimir_todos_gener(trie, 0, tam_gen, buffer);
        }
        else if (strcmp(comando, "help") == 0)
        {
            mostrar_help();
        }
        else if (strcmp(comando, "salir") == 0)
        {
            printf("Limpiando memoria y saliendo\n");
            liberar_trie(trie, 0, tam_gen);
            break;
        }
        else
        {
            printf("Comando no reconocido\n");
        }
    }

    return 0;

}
