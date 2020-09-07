#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    int pid; 
    int average; 
} ChildProcess; 

void print_help();
void printGraph(int n);

int maxAverage = 0;

int main(int argc, char * const * argv) {
    char * input = NULL;
    int processAmount = 0;
    int index;
    int opt;
    int help = 0;

    pid_t pid;
    int estado;

    ChildProcess * childs;
    
    opterr = 0;
    
    while ((opt = getopt (argc, argv, "n:h")) != -1)
        switch (opt) {
            case 'n':
                input = optarg;
                if (isdigit(*input))
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
    
    for (index = optind; index < argc; index++)
        printf ("El argumento no es una opción válida %s\n", argv[index]);
    
    if (help == 1)
        print_help();
    
    if (processAmount == 0)
        return 1; 

    childs = (ChildProcess *)malloc(sizeof(ChildProcess) * processAmount);

    for (int i=0; i<processAmount; ++i) {
        pid = fork();

        if (pid == 1) {
            printf("Error creando proceso hijo. %d procesos creados.\n", i);
        }
        else if (pid == 0) {
            int average = (getpid() + getppid())/2;
            printf("Soy el proceso hijo con PID = %d y mi promedio es = %d\n", getpid(), average);
            exit(average);
        }
        else {
            if (waitpid(pid, &estado, 0) != -1){
                if (WIFEXITED(estado)) {
                    ChildProcess * child = childs + i;
                    child->pid = pid;
                    child->average =  WEXITSTATUS(estado);

                    if (WEXITSTATUS(estado) > maxAverage)
                        maxAverage = WEXITSTATUS(estado);
                }
            }
        }
    }

    ChildProcess * aux = childs; 
    ChildProcess * last = childs + processAmount;

    printf("\n%-10s %-10s %-10s\n", "PID Hijo", "Promedio", "Histograma");
    for (; aux<last; ++aux) {
        printf("%-10d %-10d ", aux->pid, aux->average);
        printGraph(aux->average);
    }

    free(childs);

    return 0;
}

void print_help()
{
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