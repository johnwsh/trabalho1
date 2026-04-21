import pandas as pd
import glob
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np

def main():
    plt.figure(figsize=(12, 8))
    
    cores = cm.viridis(np.linspace(0, 1, 20))
    
    cor_index = 0
    for pop in range(10, 90, 5):
        caminho = f"result_genetic_pop{pop}/result_genetics_*.csv"
        arquivos_csv = glob.glob(caminho)
        
        if not arquivos_csv:
            continue
            
        lista_dfs = []
        for arquivo in arquivos_csv:
            df = pd.read_csv(arquivo)
            lista_dfs.append(df)

        df_completo = pd.concat(lista_dfs, ignore_index=True)
        
        df_media = df_completo.groupby('iteracao')['lucro'].mean().reset_index()

        plt.plot(df_media['iteracao'], df_media['lucro'], 
                 color=cores[cor_index], linewidth=1.5, label=f'Pop {pop}')
        
        cor_index += 1
        print(f"Processada a população {pop}...")

    plt.title('Comparativo de Convergência por Tamanho da População')
    plt.xlabel('Geração (Iteração)')
    plt.ylabel('Lucro Médio (100 Execuções)')
    plt.grid(True, linestyle='--', alpha=0.7)
    
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0.)
    plt.tight_layout() 
    
    plt.savefig('grafico_comparativo_populacoes.png', dpi=300)
    print("Gráfico comparativo salvo com sucesso!")

if __name__ == "__main__":
    main()