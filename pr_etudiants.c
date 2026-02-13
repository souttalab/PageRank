#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
int* compar(const void* i, const void* j, void* list){
    double* liste =list;
    const *j = j;
    const *i = i;

}
void qsort_r(void base, size_t nmemb, size_t size,
                  int (*compar)(const void , const void , void *),
                  void *arg);

void score(DIGRAPH *g)
{
    double *s = (double *)malloc(g->n * sizeof(double));
    double *s_k = (double *)malloc(g->n * sizeof(double));
    double epsilon = 1.0e-10; 
    double alpha = 0.9;       

    for (int i = 0; i < g->n; i++)
    {
        s[i] = 1.0 / g->n;
    }

    int iter = 0;
    while (1) 
    {
        iter++;
        double qabs = 0.0;
        
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
            s_k[i] += qabs * alpha + (1.0 - alpha) / g->n;
            
            for (int j = 0; j < g->nbSucc[i]; j++)
            {
                s_k[g->succ[i][j]] += (s[i] / g->nbSucc[i]) * alpha;
            }
        }

        double max_diff = 0.0;
        for (int i = 0; i < g->n; i++) {
            double diff = fabs(s_k[i] - s[i]);
            if (diff > max_diff) max_diff = diff;
        }

        double *temp = s;
        s = s_k;
        s_k = temp;

        
        if (max_diff < epsilon) break;
    }

    printf("Convergence atteinte après %d itérations.\n", iter);


    int *indices = (int *)malloc(g->n * sizeof(int));
    for(int i=0; i<g->n; i++) indices[i] = i;


    for (int i = 0; i < g->n - 1; i++)
    {
        int max_idx = i;
        for (int j = i + 1; j < g->n; j++)
        {
            if (s[indices[j]] > s[indices[max_idx]])
            {
                max_idx = j;
            }
        }

        int temp = indices[i];
        indices[i] = indices[max_idx];
        indices[max_idx] = temp;
    }

    printf("Top 5 pages:\n");
    int limit = (g->n < 5) ? g->n : 5; 
    for (int i = 0; i < limit; i++)
    {
        printf("Page %d : %f\n", indices[i], s[indices[i]]);
    }

    free(indices);
    free(s);
    free(s_k);
}
    

int main()
{
    FILE *fp = fopen("exemple2.txt", "r");
    DIGRAPH *g = readDigraph(fp);
    fclose(fp);
    printDigraph(g);
    score(g);
    return 0;
}
