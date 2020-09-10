/*  Andrés Barragán Salas
    A01026567
    Actividad 4: Procesos */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Estrucura que guarda la información obtenida en cada proceso hijo */
typedef struct {
    int pid; 
    int average; 
} ChildProcess; 

/* Declaración de los métodos */
void print_help();
void printGraph(int n);
int isInt(char * input);

/* Se guarda el promedio más alto obtenido */
int maxAverage = 0;

/* Ejecución principal */
int main(int argc, char * const * argv) {
    char * input = NULL;
    int processAmount = 0;
    int index;
    int opt;
    int help = 0;

    pid_t pid;
    int estado;
    int tuberia[2];

    pipe(tuberia);

    // Arreglo con la información de los procesos hijos
    ChildProcess * childs;
    
    opterr = 0;
    
    //  Se obtienenn los argumentos al ejecutar el programa
    while ((opt = getopt (argc, argv, "n:h")) != -1)
        switch (opt) {
            case 'n':
                input = optarg;
                if (isInt(input))
                    processAmount = atoi(input);
                else
                    printf("Opción -%c requiere un numero entero como argumento\n", opt);
                break;
            case 'h':
                help = 1;
                break;
            case '?':
                if (optopt == 'n')
                    fprintf (stderr, "Opción -%c requiere un numero entero como argumento\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Opción desconocida '-%c'.\n", optopt);
                else
                    fprintf (stderr, "Opción desconocida '\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }
    
    // Señalizar todos los opciones ingresadas que no sol válidas
    for (index = optind; index < argc; index++)
        printf ("El argumento no es una opción válida %s\n", argv[index]);
    
    // Se imprime el menú de ayuda con la opción "-h"
    if (help == 1)
        print_help();
    
    // Si no se encontro un numero de procesos hijos a obtener salir del programa
    if (processAmount == 0)
        return 1; 

    // Crear un arreglo dinámico en el numero de procesos hijos a crear
    childs = (ChildProcess *)malloc(sizeof(ChildProcess) * processAmount);

    // Crear un proceso hijo utilizando un ciclo
    for (int i=0; i<processAmount; ++i) {
        pid = fork();

        if (pid == 1) {
            // Señalizar un error al crear un proceeso hijo, dejar de crear processos 
            printf("Error creando proceso hijo. %d procesos creados.\n", i);
            processAmount = i;
            break;
        }
        else if (pid == 0) {
            // Obtener el promedio e imprimirlo
            int average = (getpid() + getppid())/2;
            printf("Soy el proceso hijo con PID = %d y mi promedio es = %d\n", getpid(), average);
            // Escribir el valor en la tubería
            close (tuberia[0]);
            write(tuberia[1], &average, sizeof(int));
            exit(0);
        }
        else {
            // Esperar a que termine el proceso hijo
            if (waitpid(pid, &estado, 0) != -1){
                if (WIFEXITED(estado)) {
                    // Leer el promedio obtenido por el hijo
                    int childAverage; 
                    read(tuberia[0], &childAverage, sizeof(int));
                    // Guardar la información obtenida en una estructura del arreglo
                    ChildProcess * child = childs + i;
                    child->pid = pid;
                    child->average = childAverage;
                    // Actualizar el promedio máximo de ser necesario
                    if (childAverage > maxAverage)
                        maxAverage = childAverage;
                }
            }
        }
    }

    // Iterar sobre la información obtenida por los procesos hijos e imprimirla
    ChildProcess * aux = childs; 
    ChildProcess * last = childs + processAmount;
    printf("\n%-10s %-10s %-10s\n", "PID Hijo", "Promedio", "Histograma");
    for (; aux<last; ++aux) {
        printf("%-10d %-10d ", aux->pid, aux->average);
        printGraph(aux->average);
    }

    // Liberar memoria
    free(childs);

    return 0;
}

/*  Verifica si un string recibido como argumento es un número entero o no 
    @param input: string con el argumento recibido
*/
int isInt(char * input) {
    size_t len = strlen(input);
    
    char * current = input; 
    char * last = input + len;
    for (; current<last; current++)
        if (!isdigit(*current))
            return 0;
    
    return 1;
}

/* Imprime un menú para conocer los argumentos que puede utilizar el programa al ser ejecutado */
void print_help() {
    printf("\nUse: ./a.out [-n integer] [-h]\n");
    printf("\nOpciones:\n");
    printf("-n : Crear la cantidad dada de procesos y calcular promedio entre sus PIDs\n-h : Ayuda\n\n");
}

/*  Metodo utilizado para imprimir la cantidad de asteríscos (*) adecuada al mostrar el histograma de los directorios recorridos.
    @param n: tamaño del grupo que se desea mostrar gráficamente.
*/
void printGraph(int n) {
    // Se obtiene una relación porcentual para imprimir la cantidad correcta de asteríscos
    int size = n * 10 / maxAverage;
    // Imprimir los asteríscos
    for (int i=0; i<size; ++i)
        printf("*");

    printf("\n");
}