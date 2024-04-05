import math

caminho_do_arquivo = "database/734 cidades Uruguay (79114)/xy.txt"

# Lista para armazenar as tuplas
lista_de_tuplas = []

# Abrir o arquivo em modo de leitura
with open(caminho_do_arquivo, "r") as arquivo:
    # Ler cada linha do arquivo
    for linha in arquivo:
        # Dividir a linha em números
        numeros_da_linha = linha.split()
        # Converter os números para inteiros e criar uma tupla
        tupla = tuple(map(float, numeros_da_linha))
        # Adicionar a tupla à lista de tuplas
        lista_de_tuplas.append(tupla)


matriz = [[0] * 48 for _ in range(48)]

for i in range (48):
    for j in range (48):
        matriz[i][j] = math.ceil(math.sqrt((lista_de_tuplas[i][0] - lista_de_tuplas[j][0])**2 + (lista_de_tuplas[i][1] - lista_de_tuplas[j][1])**2))

with open('database/734 cidades Uruguay (79114)/distance.txt', "w") as arquivo:
    for linha in matriz:
        linha_formatada = " ".join(str(numero).rjust(5) for numero in linha)  # Formata a linha com números justificados
        arquivo.write(linha_formatada + "\n")  # Escreve a linha no arquivo, com uma quebra de linha no final