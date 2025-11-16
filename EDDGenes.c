#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_GEN_MAXIMO 10

typedef struct ListaEnteros {
    int* posiciones;
    int cantidad;
    int capacidad;
} ListaEnteros;

typedef struct NodoTrie {
    struct NodoTrie* hijos[4];
    ListaEnteros* ocurrencias;
    int esHoja;
} NodoTrie;

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

char indice_a_base(int indice)
{
    char bases[] = { 'A', 'C', 'G', 'T' };
    if (indice >= 0 && indice < 4)
        return bases[indice];
    return '?';
}

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

void liberar_lista(ListaEnteros* lista)
{
    if (!lista) return;
    free(lista->posiciones);
    free(lista);
}

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

NodoTrie* inicializar_trie(int tam_gen)
{
    NodoTrie* raiz = crear_nodo(0, tam_gen);
    return raiz;
}

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

void recorrer_trie(NodoTrie* nodo, int nivel, int nivel_max, char* buffer, char* max_gen, char* min_gen, int* max_count, int* min_count)
{
    int i;
    if (!nodo) return;

    if (nivel == nivel_max)
    {
        int cantidad = nodo->ocurrencias->cantidad;
        if (cantidad > 0)
        {
            if (cantidad > *max_count)
            {
                *max_count = cantidad;
                strncpy(max_gen, buffer, nivel_max);
                max_gen[nivel_max] = '\0';
            }
            if ((cantidad < *min_count) || (*min_count == -1))
            {
                *min_count = cantidad;
                strncpy(min_gen, buffer, nivel_max);
                min_gen[nivel_max] = '\0';
            }
        }
        return;
    }

    for (i = 0; i < 4; i++)
    {
        buffer[nivel] = indice_a_base(i);
        recorrer_trie(nodo->hijos[i], nivel + 1, nivel_max, buffer, max_gen, min_gen, max_count, min_count);
    }
}

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
            printf("%s : %d\n", buffer, cantidad);
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
    NodoTrie* trie = NULL;
    int tam_gen = 0;
    char comando[32];
    char nombre_archivo[128];
    char gen[TAM_GEN_MAXIMO + 1];
    char secuencia[4096];
    FILE* archivo;
    int largo_secuencia;
    int i;
    ListaEnteros* resultado;
    char buffer[TAM_GEN_MAXIMO + 1];
    char gen_max[TAM_GEN_MAXIMO + 1];
    char gen_min[TAM_GEN_MAXIMO + 1];
    int max_cantidad = -1;
    int min_cantidad = -1;

    mostrar_help();

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
        else if (strcmp(comando, "max") == 0)
        {
            max_cantidad = -1;
            min_cantidad = -1;
            recorrer_trie(trie, 0, tam_gen, buffer, gen_max, gen_min, &max_cantidad, &min_cantidad);
            if (max_cantidad > 0)
                printf("Gen mas repetido: %s con %d ocurrencias\n", gen_max, max_cantidad);
            else
                printf("-1\n");
        }
        else if (strcmp(comando, "min") == 0)
        {
            max_cantidad = -1;
            min_cantidad = -1;
            recorrer_trie(trie, 0, tam_gen, buffer, gen_max, gen_min, &max_cantidad, &min_cantidad);
            if (min_cantidad >= 0)
                printf("Gen menos repetido: %s con %d ocurrencias\n", gen_min, min_cantidad);
            else
                printf("-1\n");
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