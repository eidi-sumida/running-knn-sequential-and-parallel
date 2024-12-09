import matplotlib.pyplot as plt
import re

# Função para ler o arquivo e extrair os dados
def ler_dados(arquivo):
    sequencial = {}
    paralelo = {}
    with open(arquivo, 'r') as f:
        lines = f.readlines()

    # Variáveis de controle para distinguir os blocos de dados
    modo = None

    # Processando as linhas do arquivo
    for line in lines:
        line = line.strip()
        
        if "Execução Sequencial" in line:
            modo = 'sequencial'
        elif "Execução em Paralelo" in line:
            modo = 'paralelo'
        elif line:
            # Usando regex para extrair os valores de cada linha
            match = re.match(r"(\d+):\s*([\d\.]+)", line)
            if match:
                quantidade = int(match.group(1))
                tempo = float(match.group(2))
                
                if modo == 'sequencial':
                    sequencial[quantidade] = tempo
                elif modo == 'paralelo':
                    paralelo[quantidade] = tempo

    return sequencial, paralelo

# Função para gerar o gráfico
def gerar_grafico(sequencial, paralelo, y_log=False):
    # Ordenar as chaves (quantidade de itens) para o gráfico
    x = sorted(sequencial.keys())
    y_sequencial = [sequencial[i] for i in x]
    y_paralelo = [paralelo[i] for i in x]

    # Plotando o gráfico
    plt.figure(figsize=(10, 6))
    plt.plot(x, y_sequencial, label='Sequencial', marker='o', color='blue', linewidth=2)
    plt.plot(x, y_paralelo, label='Paralelo', marker='x', color='orange', linewidth=2)

    # Configurações do gráfico
    plt.title('Tempo de Execução: Sequencial vs Paralelo', fontsize=14, fontweight='bold')
    plt.xlabel('Quantidade de Itens', fontsize=12, fontweight='bold')
    plt.ylabel('Tempo (segundos)', fontsize=12, fontweight='bold')

    # Definindo o eixo X e y em escala logarítmica
    filename = "graficos/grafico_comparacao.png"
    if y_log:
        plt.xscale('log')
        filename = "graficos/grafico_comparacao_eixo_logs.png"

    # Ajustando os ticks em negrito
    plt.tick_params(axis='x', labelsize=10, width=2)
    plt.tick_params(axis='y', labelsize=10, width=2)

    # Adicionando a legenda
    plt.legend()

    # Salvar o gráfico como imagem PNG
    plt.savefig(filename, bbox_inches='tight')


# Caminho do arquivo
arquivo = 'resultados/tempos_execucao_2.txt'

# Ler os dados
sequencial, paralelo = ler_dados(arquivo)

gerar_grafico(sequencial, paralelo, y_log=False)
gerar_grafico(sequencial, paralelo, y_log=True)
