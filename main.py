import pandas as pd
import glob
import matplotlib.pyplot as plt

def main():
    arquivos_csv = glob.glob("result_genetic_pop75/result_genetics_*.csv")

    lista_dfs = []

    for arquivo in arquivos_csv:
        df = pd.read_csv(arquivo)
        lista_dfs.append(df)

    df_completo = pd.concat(lista_dfs, ignore_index=True)

    df_stats = df_completo.groupby('iteracao')['lucro'].agg(['mean', 'max', 'min']).reset_index()

    plt.figure(figsize=(10, 6))
    
    plt.plot(df_stats['iteracao'], df_stats['mean'], color='blue', linewidth=2, label='Lucro Médio')
    
    plt.plot(df_stats['iteracao'], df_stats['max'], color='green', linewidth=1, linestyle='--', alpha=0.6, label='Lucro Máximo')
    plt.plot(df_stats['iteracao'], df_stats['min'], color='red', linewidth=1, linestyle='--', alpha=0.6, label='Lucro Mínimo')
    
    plt.fill_between(df_stats['iteracao'], df_stats['min'], df_stats['max'], color='blue', alpha=0.15)
    
    plt.title('Curva de Evolução do Algoritmo Genético (População 75)')
    plt.xlabel('Geração (Iteração)')
    plt.ylabel('Lucro')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    
    plt.savefig('grafico_evolucao_max_min75.png', dpi=300)

if __name__ == "__main__":
    main()