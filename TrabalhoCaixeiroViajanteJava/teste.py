import matplotlib.pyplot as plt
import numpy as np

# Função para ler o arquivo cidades.txt
def ler_cidades(arquivo_cidades):
    cidades = {}
    with open(arquivo_cidades, 'r') as f:
        for linha in f:
            if linha.strip():
                nome, coordenadas = linha.split(':')
                cidade_id = int(nome.split()[-1])
                x, y = map(int, coordenadas.strip().strip('()').split(','))
                cidades[cidade_id] = (x, y)
    return cidades

# Função para ler o arquivo caminhominimo.txt
def ler_caminho_minimo(arquivo_caminho):
    caminho = []
    distancia_total = 0
    with open(arquivo_caminho, 'r') as f:
        for linha in f:
            if '->' in linha:
                partes = linha.split(':')
                cidades_conectadas = partes[0].strip().split(' -> ')
                # Substituir a vírgula por um ponto para converter em float
                peso = float(partes[1].split()[0].replace(',', '.'))
                caminho.append((int(cidades_conectadas[0]), int(cidades_conectadas[1]), peso))
            elif 'Distancia total' in linha:
                # Substituir a vírgula por um ponto para converter em float
                distancia_total = float(linha.split(':')[-1].split()[0].replace(',', '.'))
    return caminho, distancia_total


# Função para plotar o gráfico
def plotar_cidades_e_caminho(cidades, caminho, distancia_total):
    plt.figure(figsize=(8, 8))

    # Pegar as coordenadas das cidades
    x_vals = [cidades[cidade][0] for cidade in cidades]
    y_vals = [cidades[cidade][1] for cidade in cidades]
    
    # Plotar os pontos das cidades
    for cidade, (x, y) in cidades.items():
        plt.scatter(x, y, label=f'Cidade {cidade}', zorder=5)
        plt.text(x + 1, y + 1, f'{cidade}', fontsize=12)

    # Desenhar as conexões do caminho mínimo
    for (cidade1, cidade2, peso) in caminho:
        x1, y1 = cidades[cidade1]
        x2, y2 = cidades[cidade2]
        plt.plot([x1, x2], [y1, y2], 'k-', lw=2, zorder=3)  # Linha conectando as cidades
        plt.text((x1 + x2) / 2, (y1 + y2) / 2, f'{peso:.2f} Km', color='red', fontsize=10, zorder=10)

    # Exibir a distância total no gráfico
    plt.title(f'Melhor Caminho - Força Bruta\nDistância Total: {distancia_total:.2f} Km', fontsize=14)
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.grid(True)
    plt.legend()
    plt.show()

# Leitura dos arquivos
arquivo_cidades = 'cidades.txt'
arquivo_caminho = 'caminhominimo.txt'

# Carregar os dados
cidades = ler_cidades(arquivo_cidades)
caminho_minimo, distancia_total = ler_caminho_minimo(arquivo_caminho)

# Plotar os dados
plotar_cidades_e_caminho(cidades, caminho_minimo, distancia_total)
