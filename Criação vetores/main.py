import pandas as pd
import numpy as np

def taxaRetorno(vetorPreco):
    vetorTaxaRetorno = [0] * len(vetorPreco)  # Inicializa com 0
    for i in range(1, len(vetorPreco)):
        vetorTaxaRetorno[i] = (vetorPreco[i] - vetorPreco[i - 1]) / vetorPreco[i - 1]   # Calcula a taxa de variação do preço da ação
    return vetorTaxaRetorno

def valorEsperado(vetorTaxaRetorno):
    return sum(vetorTaxaRetorno) / len(vetorTaxaRetorno)

def covariancia(vetorTaxaRetornoX, vetorTaxaRetornoY, valorEsperadoX, valorEsperadoY):
    soma = 0
    n = min(len(vetorTaxaRetornoX), len(vetorTaxaRetornoY))
    for i in range(n):
            soma += (vetorTaxaRetornoX[i] - valorEsperadoX) * (vetorTaxaRetornoY[i] - valorEsperadoY)   # Calcula a covariancia entre X e Y e retorna um float
    return soma / n



# Le a tabela, coloca a coluna Date como um indice de data
df = pd.read_excel("dados.xlsx", index_col="Date", parse_dates=True, engine="openpyxl")
data = "2022-01-03"     #define a data inicial da simulação
pos = df.index.get_loc(data)    #pega a posição na tabela do indice


taxas_retorno = {}  # Dicionário para armazenar vetores de taxa de retorno
valores_esperados = []  # Lista para armazenar os valores esperados

for coluna in df.columns:   #itera entre as colunas do dataset
    vetorPreco = df[coluna].iloc[max(pos - 100, 0):pos].dropna().tolist()   #pega os 100 ultimos valores ou menos caso não haja data
    vetorTaxaRetorno = taxaRetorno(vetorPreco) #Calcula o vetorTaxa
    taxas_retorno[coluna] = vetorTaxaRetorno    # Passa o vetorTaxa[i] para a posição i do dicionário
    valores_esperados.append(valorEsperado(vetorTaxaRetorno))   #da um append na lista de valorEsperado com o valor esperado[i]

n = len(df.columns) #define quantidade de ações
matriz_covariancia = np.zeros((n, n))

# Preencher a matriz de covariância
for i in range(n):
    for j in range(i, n):  # Itera apenas metade da matriz pois é uma matriz simétrica 
        vetorX = taxas_retorno[df.columns[i]]
        vetorY = taxas_retorno[df.columns[j]]
        valorEsperadoX = valores_esperados[i]
        valorEsperadoY = valores_esperados[j]

        matriz_covariancia[i][j] = covariancia(vetorX, vetorY, valorEsperadoX, valorEsperadoY)
        matriz_covariancia[j][i] = matriz_covariancia[i][j]  # Simetria

