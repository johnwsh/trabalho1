import pandas as pd
import glob
import matplotlib.pyplot as plt

def main():
    arquivos_csv = glob.glob("result_genetics_*.csv")

    lista_dfs = []

    for arquivo in arquivos_csv:
        df = pd.read_csv(arquivo)
        lista_dfs.append(df)

    df_completo = pd.concat(lista_dfs, ignore_index=True)

    df_media = df_completo.groupby('iteracao')['lucro'].mean().reset_index()

    plt.figure(figsize=(10, 6))
    
    plt.plot(df_media['iteracao'], df_media['lucro'], color='blue', linewidth=2, label='Lucro Médio')
    
    plt.title('Curva de Evolução do Algoritmo Genético (Média de 100 Execuções)')
    plt.xlabel('Geração (Iteração)')
    plt.ylabel('Lucro Médio')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    
    plt.savefig('grafico_evolucao_media.png', dpi=300)
    plt.show()

if __name__ == "__main__":
    main()