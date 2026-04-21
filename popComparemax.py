import pandas as pd
import glob
import matplotlib.pyplot as plt
import seaborn as sns

def main():
    dados = []

    print("Lendo os arquivos e extraindo os valores máximos...")
    
    for pop in range(5, 90, 5):
        caminho = f"result_genetic_pop{pop}/result_genetics_*.csv"
        arquivos_csv = glob.glob(caminho)
        
        if not arquivos_csv:
            continue
            
        for arquivo in arquivos_csv:
            df = pd.read_csv(arquivo)
            lucro_maximo = df['lucro'].max() 
            
            dados.append({
                'Tamanho da População': pop,
                'Lucro Máximo': lucro_maximo
            })

    df_box = pd.DataFrame(dados)

    if df_box.empty:
        print("Nenhum dado encontrado! Verifique se as pastas estão no mesmo diretório do script.")
        return

    print("Gerando o gráfico Boxplot...")
    
    plt.figure(figsize=(14, 8))
    sns.set_theme(style="whitegrid")
    
    ax = sns.boxplot(data=df_box, x='Tamanho da População', y='Lucro Máximo', palette='viridis')
    
    sns.stripplot(data=df_box, x='Tamanho da População', y='Lucro Máximo', color='black', alpha=0.15, size=3, ax=ax)
    
    plt.title('Distribuição do Melhor Lucro Encontrado por Tamanho de População\n(100 Execuções por População)', fontsize=14, pad=15)
    plt.xlabel('Tamanho da População', fontsize=12)
    plt.ylabel('Maior Lucro Atingido', fontsize=12)
    
    plt.tight_layout()
    plt.savefig('grafico_boxplot_populacoes.png', dpi=300)
    

if __name__ == "__main__":
    main()