#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <psapi.h>

typedef struct {
    int x;
    int y;
} Cidade;

// Função para gerar um número aleatório dentro de um intervalo
int gerarNumeroAleatorio(int intervalo) {
    return rand() % intervalo;
}

// Função para gerar coordenadas aleatórias para uma cidade
Cidade gerarCoordenadas(int intervalo) {
    Cidade cidade;
    cidade.x = gerarNumeroAleatorio(intervalo);
    cidade.y = gerarNumeroAleatorio(intervalo);
    return cidade;
}

// Função para gerar cidades com coordenadas únicas
Cidade* gerarCidades(int quantidade, int intervalo, int *qtdGerada) {
    Cidade *cidades = (Cidade*) malloc(quantidade * sizeof(Cidade));
    int geradas = 0;
    while (geradas < quantidade) {
        Cidade novaCidade = gerarCoordenadas(intervalo);
        
        // Verifica se já existe uma cidade com as mesmas coordenadas
        int unico = 1;
        for (int i = 0; i < geradas; i++) {
            if (cidades[i].x == novaCidade.x && cidades[i].y == novaCidade.y) {
                unico = 0;
                break;
            }
        }

        if (unico) {
            cidades[geradas] = novaCidade;
            geradas++;
        }
    }
    *qtdGerada = geradas;
    return cidades;
}

// Função para calcular a distancia euclidiana entre duas cidades
double calcularDistancia(Cidade a, Cidade b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

// Função para calcular a distancia total de um caminho
double calcularDistanciaTotal(Cidade *cidades, int *caminho, int n) {
    double distanciaTotal = 0.0;
    for (int i = 0; i < n-1; i++) {
        distanciaTotal += calcularDistancia(cidades[caminho[i]], cidades[caminho[i+1]]);
    }
    // Adiciona a distancia de volta para a cidade inicial
    distanciaTotal += calcularDistancia(cidades[caminho[n-1]], cidades[caminho[0]]);
    return distanciaTotal;
}

// Função para obter o uso de memoria atual do processo
SIZE_T obterUsoMemoria() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

// Função para permutar as cidades (usada na força bruta), mostrando caminhos e distancias
void permutar(int *caminho, int l, int r, Cidade *cidades, double *menorDistancia, int *melhorCaminho, int n, FILE *arquivoCaminhos) {
    if (l == r) {
        // Calcula o tempo de execução para cada permutação
        clock_t start = clock();

        double distancia = calcularDistanciaTotal(cidades, caminho, n);
        clock_t end = clock();
        double tempoExecucao = ((double)(end - start)) / CLOCKS_PER_SEC;

        // Exibe e salva o caminho e a distancia no formato solicitado
        printf("Caminho testado (Forca Bruta): ");
        for (int i = 0; i < n; i++) {
            printf("%d -> ", caminho[i] + 1);
        }
        printf("%d | Distancia: %.2f Km | Tempo: %.6f s\n", caminho[0] + 1, distancia, tempoExecucao);

        // Grava no arquivo no formato solicitado
        for (int i = 0; i < n; i++) {
            fprintf(arquivoCaminhos, "%d -> ", caminho[i] + 1);
        }
        fprintf(arquivoCaminhos, "%d, %.2f, %.6f\n", caminho[0] + 1, distancia, tempoExecucao);

        // Atualiza o melhor caminho se a distancia for menor
        if (distancia < *menorDistancia) {
            *menorDistancia = distancia;
            for (int i = 0; i < n; i++) {
                melhorCaminho[i] = caminho[i];
            }
        }
    } else {
        for (int i = l; i <= r; i++) {
            int temp = caminho[l];
            caminho[l] = caminho[i];
            caminho[i] = temp;
            
            permutar(caminho, l+1, r, cidades, menorDistancia, melhorCaminho, n, arquivoCaminhos);
            
            temp = caminho[l];
            caminho[l] = caminho[i];
            caminho[i] = temp;
        }
    }
}

// Algoritmo de força bruta para encontrar o melhor caminho
void algoritmoForcaBruta(Cidade *cidades, int n) {
    int *caminho = (int*) malloc(n * sizeof(int));
    int *melhorCaminho = (int*) malloc(n * sizeof(int));
    double menorDistancia = __DBL_MAX__;
    
    for (int i = 0; i < n; i++) {
        caminho[i] = i;
    }

    // Abrir arquivo para salvar todos os caminhos testados
    FILE *arquivoCaminhos = fopen("caminhos.txt", "w");
    if (arquivoCaminhos == NULL) {
        printf("Erro ao criar o arquivo de caminhos.\n");
        free(caminho);
        free(melhorCaminho);
        return;
    }

    // Obtém a memoria usada antes da execução
    SIZE_T memoriaAntes = obterUsoMemoria();

    // Obtém o tempo total de execução
    clock_t inicioExecucao = clock();

    // Permutar e testar todos os caminhos
    permutar(caminho, 0, n-1, cidades, &menorDistancia, melhorCaminho, n, arquivoCaminhos);
    
    clock_t fimExecucao = clock();
    double tempoExecucaoTotal = ((double)(fimExecucao - inicioExecucao)) / CLOCKS_PER_SEC;

    // Obtém a memoria usada após a execução
    SIZE_T memoriaDepois = obterUsoMemoria();

    printf("\nMelhor caminho encontrado (Forca Bruta): ");
    for (int i = 0; i < n; i++) {
        printf("%d -> ", melhorCaminho[i] + 1);
    }
    printf("%d\n", melhorCaminho[0] + 1);
    printf("Distancia total: %.2f Km\n", menorDistancia);
    printf("Tempo total de execucao: %.6f segundos\n", tempoExecucaoTotal);

    // Convertendo o uso de memória para KB e exibindo
    printf("Uso de memoria antes da execucao: %.2f KB\n", memoriaAntes / 1024.0);
    printf("Uso de memoria apos a execucao: %.2f KB\n", memoriaDepois / 1024.0);
    printf("Memoria usada durante a execucao: %.2f KB\n", (memoriaDepois - memoriaAntes) / 1024.0);
    
    // Fechar arquivo de caminhos
    fclose(arquivoCaminhos);

    // Salvar o caminho mínimo e as distâncias entre pontos no arquivo caminhominimo.txt
    FILE *arquivoCaminhoMinimo = fopen("caminhominimo.txt", "w");
    if (arquivoCaminhoMinimo == NULL) {
        printf("Erro ao criar o arquivo do caminho minimo.\n");
        free(caminho);
        free(melhorCaminho);
        return;
    }

    fprintf(arquivoCaminhoMinimo, "Melhor caminho (Forca Bruta):\n");
    for (int i = 0; i < n - 1; i++) {
        double distancia = calcularDistancia(cidades[melhorCaminho[i]], cidades[melhorCaminho[i+1]]);
        fprintf(arquivoCaminhoMinimo, "%d -> %d: %.2f Km\n", melhorCaminho[i] + 1, melhorCaminho[i+1] + 1, distancia);
    }
    // Distância de volta à cidade inicial
    double distanciaFinal = calcularDistancia(cidades[melhorCaminho[n-1]], cidades[melhorCaminho[0]]);
    fprintf(arquivoCaminhoMinimo, "%d -> %d: %.2f Km\n", melhorCaminho[n-1] + 1, melhorCaminho[0] + 1, distanciaFinal);

    fprintf(arquivoCaminhoMinimo, "Distancia total: %.2f Km\n", menorDistancia);

    fclose(arquivoCaminhoMinimo);

    free(caminho);
    free(melhorCaminho);
}

// Função para carregar as cidades de um arquivo existente
Cidade* carregarCidadesDeArquivo(int *quantidade) {
    FILE *arquivo = fopen("cidades.txt", "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo.\n");
        return NULL;
    }

    Cidade* cidades = NULL;
    int qtd = 0, x, y;
    char buffer[100];
    
    while (fgets(buffer, sizeof(buffer), arquivo)) {
        sscanf(buffer, "Cidade %*d: (%d, %d)", &x, &y);
        cidades = (Cidade*) realloc(cidades, (qtd + 1) * sizeof(Cidade));
        cidades[qtd].x = x;
        cidades[qtd].y = y;
        qtd++;
    }

    *quantidade = qtd;
    fclose(arquivo);
    return cidades;
}

int main() {
    srand(time(NULL));
    
    int escolha, quantidade, intervalo = 100;
    Cidade* cidades = NULL;

    printf("Deseja gerar um novo conjunto de cidades (1) ou carregar de um arquivo existente (2)? ");
    scanf("%d", &escolha);

    if (escolha == 1) {
        printf("Quantas cidades deseja gerar? (max: 8)");
        scanf("%d", &quantidade);

        cidades = gerarCidades(quantidade, intervalo, &quantidade);

        // Armazenar as cidades em um arquivo
        FILE *arquivo = fopen("cidades.txt", "w");
        for (int i = 0; i < quantidade; i++) {
            fprintf(arquivo, "Cidade %d: (%d, %d)\n", i+1, cidades[i].x, cidades[i].y);
        }
        fclose(arquivo);
    } else if (escolha == 2) {
        cidades = carregarCidadesDeArquivo(&quantidade);
        if (cidades == NULL) {
            return 1;
        }
    } else {
        printf("Escolha invalida.\n");
        return 1;
    }

    // Executar algoritmo de força bruta e mostrar todos os caminhos testados
    algoritmoForcaBruta(cidades, quantidade);

    free(cidades);
    return 0;
}
