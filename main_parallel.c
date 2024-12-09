#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>

#define SEQUENCE_SIZE 3
#define K 3
#define NUM_THREADS 10

#define TRAIN_SIZE 500
#define TRAIN_X_SIZE (TRAIN_SIZE - SEQUENCE_SIZE) * SEQUENCE_SIZE
#define TRAIN_Y_SIZE TRAIN_SIZE - SEQUENCE_SIZE


void lerArquivo(char *nomeArquivo, float *vetor)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }
    int tamanho = 0;
    while (fscanf(arquivo, "%f", &vetor[tamanho]) == 1)
    {
        tamanho++;
    }
    fclose(arquivo);
}

void salvarArquivo(const char *nomeArquivo, float *resultados, int teste_size)
{
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo para escrita: %s\n", nomeArquivo);
        return;
    }
    for (int i = 0; i < teste_size; i++)
    {
        fprintf(arquivo, "%.3f\n", resultados[i]);
    }
    fclose(arquivo);
}

void criarVetoresXY(float *original, int size, float *x, float *y)
{
    int indice_x = 0;
    int indice_y = 0;
    for (int i = 0; i < size - SEQUENCE_SIZE; i++)
    {
        for (int j = 0; j < SEQUENCE_SIZE; j++)
        {
            x[indice_x] = original[i + j];
            indice_x++;
        }
    }
    for (int i = SEQUENCE_SIZE; i < size; i++)
    {
        y[indice_y] = original[i];
        indice_y++;
    }
}

float calcularDistancia(float *seq1, float *seq2)
{
    float distancia = 0;
    for (int i = 0; i < SEQUENCE_SIZE; i++)
    {
        distancia += fabs(seq1[i] - seq2[i]);
    }
    return distancia;
}

float encontrarKVizinhos(float *x_treino, float *y_treino, float *sequencia_teste)
{

    int indices[K];
    float distancias[K];

    for (int i = 0; i < K; i++)
    {
        indices[i] = -1;
        distancias[i] = FLT_MAX;
    }

    for (int j = 0; j < TRAIN_X_SIZE; j += SEQUENCE_SIZE)
    {

        float sequencia_treino[SEQUENCE_SIZE];

        for (int k = 0; k < SEQUENCE_SIZE; k++)
        {
            sequencia_treino[k] = x_treino[j + k];
        }

        float distancia = calcularDistancia(sequencia_treino, sequencia_teste);

        for (int k = 0; k < K; k++)
        {
            if (distancia < distancias[k])
            {
                for (int l = K - 1; l > k; l--)
                {
                    indices[l] = indices[l - 1];
                    distancias[l] = distancias[l - 1];
                }
                indices[k] = j / SEQUENCE_SIZE;
                distancias[k] = distancia;
                break;
            }
        }
    }
    float soma = 0;
    for (int j = 0; j < K; j++)
    {
        soma += y_treino[indices[j]];
    }
    float media = soma / K;

    return media;
}

int main(int argc, char *argv[])
{
    float treino[TRAIN_SIZE];
    float x_treino[TRAIN_X_SIZE], y_treino[TRAIN_Y_SIZE];
    char path[256];

    FILE *log_tempo = fopen("resultados/tempos_execucao.txt", "a");
    fprintf(log_tempo, "\n\nExecução em Paralelo (OpenMP):\n\n");
    FILE *log_tempo_2 = fopen("resultados/tempos_execucao_2.txt", "a");
    fprintf(log_tempo_2, "\n\nExecução em Paralelo (OpenMP):\n\n");

    snprintf(path, sizeof(path), "train_test_txt/%s", argv[1]);
    lerArquivo(path, treino);
    criarVetoresXY(treino, TRAIN_SIZE, x_treino, y_treino);

    for (int argv_idx = 2; argv_idx < argc; argv_idx += 2)
    {
        
        double tempo_inicio = omp_get_wtime();

        int TEST_SIZE = atoi(argv[argv_idx+1]);
        int TEST_X_SIZE = (TEST_SIZE - SEQUENCE_SIZE) * SEQUENCE_SIZE;
        int TEST_Y_SIZE = TEST_SIZE - SEQUENCE_SIZE;

        float *teste = (float *)malloc(TEST_SIZE * sizeof(float));
        float *x_teste = (float *)malloc(TEST_X_SIZE * sizeof(float));
        float *y_teste = (float *)malloc(TEST_Y_SIZE * sizeof(float));
        float *resultados = (float *)malloc(TEST_Y_SIZE * sizeof(float));

        snprintf(path, sizeof(path), "train_test_txt/%s", argv[argv_idx]);
        lerArquivo(path, teste);
        criarVetoresXY(teste, TEST_SIZE, x_teste, y_teste);

        omp_set_num_threads(NUM_THREADS);
        #pragma omp parallel for
        for (int i = 0; i < TEST_X_SIZE - SEQUENCE_SIZE + 1; i += SEQUENCE_SIZE)
        {
            float sequencia_teste[SEQUENCE_SIZE];
            for (int j = 0; j < SEQUENCE_SIZE; j++)
            {
                sequencia_teste[j] = x_teste[i + j];
            }

            resultados[(int)(i / SEQUENCE_SIZE)] = encontrarKVizinhos(x_treino, y_teste, sequencia_teste);
        }

        double tempo_fim = omp_get_wtime();
        printf("Tempo de execução: %.4f segundos\n", tempo_fim - tempo_inicio);

        char nome_arquivo_resultado[256];
        sprintf(nome_arquivo_resultado, "resultados/%s", argv[argv_idx]);
        salvarArquivo(nome_arquivo_resultado, resultados, TEST_Y_SIZE);

        fprintf(log_tempo, "%s: %.4f segundos\n", nome_arquivo_resultado, tempo_fim - tempo_inicio);
        fprintf(log_tempo_2, "%d: %.4f\n", TEST_SIZE, tempo_fim - tempo_inicio);

        free(teste);
        free(x_teste);
        free(y_teste);
        free(resultados);
    }

    fclose(log_tempo);
    return 0;
}