#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

typedef struct
{
    int n;       // number of vertices in the graph
    int *nbSucc; // for each 0<=i<n, nbSucc[i] = number of successors of i
    int **succ;  // for each 0<=i<n and each 0<=j<nbSucc[i], succ[i][j]=jth successor of i
} DIGRAPH;

DIGRAPH *readDigraph(FILE *fp)
{
    // return the DIGRAPH contained in file fp
    DIGRAPH *g = (DIGRAPH *)malloc(sizeof(DIGRAPH));
    if (fscanf(fp, "%d\n", &(g->n)) != 1 || g->n <= 0)
    {
        printf("erreur de lecture du fichier\n");
        exit(1);
    }
    g->nbSucc = (int *)malloc(g->n * sizeof(int));
    g->succ = (int **)malloc(g->n * sizeof(int *));
    int succ[g->n];
    for (int i = 0; i < g->n; i++)
    {
        g->nbSucc[i] = 0;
        while (1)
        {
            if (fscanf(fp, "%d", &(succ[g->nbSucc[i]])) != 1 || succ[g->nbSucc[i]] >= g->n)
            {
                printf("erreur de lecture du fichier\n");
                exit(1);
            }
            if (succ[g->nbSucc[i]] < 0)
                break;
            g->nbSucc[i]++;
        };
        g->succ[i] = (int *)malloc(g->nbSucc[i] * sizeof(int));
        memcpy(g->succ[i], succ, g->nbSucc[i] * sizeof(int));
    }
    return g;
}

void printDigraph(DIGRAPH *g)
{
    // For each vertex of g, display the list of its successors
    for (int i = 0; i < g->n; i++)
    {
        printf("Vertex %d has %d successors: ", i, g->nbSucc[i]);
        for (int j = 0; j < g->nbSucc[i]; j++)
            printf("%d ", g->succ[i][j]);
        printf("\n");
    }
}

/*void score(DIGRAPH *g, int k)
{
    double *s = (double *)malloc(g->n * sizeof(double));
    double *s_k = (double *)malloc(g->n * sizeof(double));

    for (int i = 0; i < g->n; i++)
    {
        s[i] = 1.0 / g->n;
    }
    

    for (int iter = 0; iter < k; iter++)
    {
        double qabs = 0.0;
        double alpha=0.9;
        for (int i = 0; i < g->n; i++)
        {
            if (g->nbSucc[i] == 0)
            {
                qabs += s[i] / g->n;
            }
            s_k[i] = 0.0;
        }

        for (int i = 0; i < g->n; i++)
        {
            s_k[i] += qabs*alpha + (1.0 - alpha) / g->n;
            for (int j = 0; j < g->nbSucc[i]; j++)
            {
                s_k[g->succ[i][j]] += (s[i] / g->nbSucc[i]) * alpha;
            }
        }
        double *temp = s;
        s = s_k;
        s_k = temp;
    }

    printf("s=(");
    for (int i = 0; i < g->n; i++)
    {
        printf("%f", s[i]);
        printf(", ");
    }
    printf(")\n");
    free(s);
    free(s_k);
}*/


void qsort_r(void *base, size_t nmemb, size_t size,
             int (*compar)(const void *, const void *, void *),
             void *arg);

int compare(const void *a, const void *b, void *arg)
{
    int index_a = *(const int *)a;
    int index_b = *(const int *)b;
    double *scores = (double *)arg; 

    if (scores[index_a] < scores[index_b]) return 1;
    if (scores[index_a] > scores[index_b]) return -1;
    return 0;
}

void score(DIGRAPH *g)
{
    // --- Initialisations ---
    double *s = (double *)malloc(g->n * sizeof(double));
    double *s_k = (double *)malloc(g->n * sizeof(double));
    double epsilon = 1.0e-10;
    double alpha = 0.9; // Modifiez selon l'exercice (0.9 pour genetic.txt)

    for (int i = 0; i < g->n; i++)
        s[i] = 1.0 / g->n;

    // --- Boucle de convergence ---
    int iter = 0;
    while (1)
    {
        iter++;
        double qabs = 0.0;
        
        // Calcul contribution absorbants
        for (int i = 0; i < g->n; i++)
        {
            if (g->nbSucc[i] == 0)
                qabs += s[i] / g->n;
            s_k[i] = 0.0;
        }

        // Distribution des scores
        for (int i = 0; i < g->n; i++)
        {
            s_k[i] += qabs * alpha + (1.0 - alpha) / g->n;
            for (int j = 0; j < g->nbSucc[i]; j++)
            {
                s_k[g->succ[i][j]] += (s[i] / g->nbSucc[i]) * alpha;
            }
        }

        // Test d'arrêt
        double max_diff = 0.0;
        for (int i = 0; i < g->n; i++)
        {
            double diff = fabs(s_k[i] - s[i]);
            if (diff > max_diff) max_diff = diff;
        }

        // Swap
        double *temp = s;
        s = s_k;
        s_k = temp;

        if (max_diff < epsilon) break;
    }
    
    printf("Convergence en %d itérations.\n", iter);

    // --- TRI AVEC QSORT_R ---
    
    // 1. On crée un tableau qui contient juste les numéros de pages [0, 1, 2...]
    int *indices = (int *)malloc(g->n * sizeof(int));
    for (int i = 0; i < g->n; i++)
    {
        indices[i] = i;
    }

    // 2. On trie ce tableau d'indices en utilisant 's' comme référence
    // Syntaxe GNU (Linux/INSA) : qsort_r(base, nmemb, size, compar, arg)
    qsort_r(indices, g->n, sizeof(int), compare, s);

    // 3. Affichage
    printf("Top 5 pages:\n");
    int limit = (g->n < 5) ? g->n : 5;
    for (int i = 0; i < limit; i++)
    {
        // indices[i] est le numéro de la page triée
        // s[indices[i]] est son score
        printf("Page %d : %.12f\n", indices[i], s[indices[i]]);
    }

    free(indices);
    free(s);
    free(s_k);
}


int main()
{
    FILE *fp = fopen("genetic.txt", "r");
    DIGRAPH *g = readDigraph(fp);
    fclose(fp);
    printDigraph(g);
    score(g);
    return 0;
}
