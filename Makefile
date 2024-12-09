# Variáveis
CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -fopenmp # Para suporte a OpenMP no programa paralelo
ARGS = dados_xtrain.txt dados_xtest_10.txt 10 dados_xtest_30.txt 30 dados_xtest_50.txt 50 dados_xtest_100.txt 100 dados_xtest_1000.txt 1000 dados_xtest_100000.txt 100000 dados_xtest_1000000.txt 1000000 dados_xtest_10000000.txt 10000000

# Alvo padrão
all: main_sequential main_parallel

# Compilar main_sequential
main_sequential: main_sequential.c
	$(CC) $(CFLAGS) -o main_sequential main_sequential.c 

# Compilar main_parallel
main_parallel: main_parallel.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o main_parallel main_parallel.c 

# Executar ambos os programas
run: main_sequential main_parallel
	@echo "Executando o programa sequencial:"
	./main_sequential $(ARGS)
	@echo "Executando o programa paralelo:"
	./main_parallel $(ARGS)
	@echo "Gerando gráfico de comparação:"
	python3 create_plot.py
	@echo "Os resultados estão na pasta resultados/ e graficos/"

# Limpar arquivos gerados
clean:
	rm -f main_sequential main_parallel
	find resultados/ -type f ! -name '.gitkeep' -delete
	find graficos/ -type f ! -name '.gitkeep' -delete
