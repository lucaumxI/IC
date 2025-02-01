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
dataInicial =  input("Digite a data inicial da simulacao no formato: YYYY-MM-DD: ")   
pos = df.index.get_loc(dataInicial)    #pega a posição na tabela do indice
dataFinal = int(input("Digite quantos dias serao simulados a partir da data inicial: "))   

for m in range(dataFinal):
    taxas_retorno = {}  # Dicionário para armazenar vetores de taxa de retorno
    valores_esperados = []  # Lista para armazenar os valores esperados
    n = len(df.columns) #define quantidade de ações
    matriz_covariancia = np.zeros((n, n))   #cria a matriz

    for coluna in df.columns:   #itera entre as colunas do dataset
        vetorPreco = df[coluna].iloc[max(pos - 100, 0):pos].dropna().tolist()   #pega os 100 ultimos valores ou menos caso não haja data
        if len(vetorPreco) == 0:    #verifica se há dados sobre esta ação
            taxas_retorno[coluna] = ["NONE"]    #caso não há salva os dados sobre com NONE para usar como flag no SA para indicar que está ação não é elegivel
            valores_esperados.append("NONE")
        else:
            vetorTaxaRetorno = taxaRetorno(vetorPreco) #Calcula o vetorTaxa
            taxas_retorno[coluna] = vetorTaxaRetorno    # Passa o vetorTaxa[i] para a posição i do dicionário
            valores_esperados.append(valorEsperado(vetorTaxaRetorno))   #da um append na lista de valorEsperado com o valor esperado[i]
 
    # Preencher a matriz de covariância
    for i in range(n):
        for j in range(i, n):  # Itera apenas metade da matriz pois é uma matriz simétrica 
            vetorX = taxas_retorno[df.columns[i]]
            vetorY = taxas_retorno[df.columns[j]]
            valorEsperadoX = valores_esperados[i]
            valorEsperadoY = valores_esperados[j]

            if valorEsperadoX != "NONE" and valorEsperadoY != "NONE":       #verifica se é elegivel. Embora não haja tanta diferença aqui ser ou não, só é necessária a flag em um dos vetores mas economiza processamento
                matriz_covariancia[i][j] = covariancia(vetorX, vetorY, valorEsperadoX, valorEsperadoY)
                matriz_covariancia[j][i] = matriz_covariancia[i][j]  # Simetria
            else:
                matriz_covariancia[i][j] = "NONE"
                matriz_covariancia[j][i] = matriz_covariancia[i][j]  # Simetria

    valores_linha = df.iloc[pos].tolist()   #paga os precos das acoes em cada dia

    #prefixos dos arquivos .txt. Minha ideia e salvar cada dia da simulacao com um numero de 0 a m para poder iterar entre os dias com um for no SA
    strCovariancia = "entradas/covariancia/"
    strPreco = "entradas/preco/"
    strValorEsperado = "entradas/valor_esperado/"

    
    with open(strPreco + str(m) + ".txt", "w") as arquivo:  
        arquivo.write("\n".join(map(str, valores_linha)))

    with open(strValorEsperado + str(m) + ".txt", "w") as arquivo:
        arquivo.write("\n".join(map(str, valores_esperados)))
    
    np.savetxt(strCovariancia + str(m) + ".txt", matriz_covariancia, fmt="%s", delimiter=" ") 
    pos = pos+1

