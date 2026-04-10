#include <vector>
#include <random>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>

using namespace std;

class item {
        
    public:
        int valor;
        int peso;

        item(int _valor, int _peso){
            valor = _valor;
            peso = _peso;
        }
};

class estado {
    public:
        vector<item>* itemPointer;
        vector<bool> itemsPertenc;
        int backpackCurrent;
        int backpackMax;

        estado(int _backpackMax, vector<item>* _itemPointer){
            backpackCurrent = 0;
            backpackMax = _backpackMax;
            itemPointer = _itemPointer;
            itemsPertenc.resize(_itemPointer->size());
        }
};

void fixState(estado &state, default_random_engine &generator){
    //recebemos um estado quebrado, ou seja, com peso estourado, e removemos itens aleatórios até corrigir
    bool flagFixed = 0;
    uniform_int_distribution<int> indexDistribution(0, state.itemsPertenc.size() - 1);

    if (state.backpackCurrent <= state.backpackMax) {
        return; 
    }

    while (!flagFixed){
        int index = indexDistribution(generator);
        if (state.itemsPertenc[index]){
            state.itemsPertenc[index] = 0;
            state.backpackCurrent -= (*state.itemPointer)[index].peso;
            if (state.backpackCurrent <= state.backpackMax){
                flagFixed = 1;
            }
        } 
    }
}

item genRandomItem(default_random_engine &generator, int minValue, int maxValue, int minWeight, int maxWeight){
    uniform_int_distribution<int> valueRandomDistValue(minValue, maxValue);
    uniform_int_distribution<int> valueRandomDistWeight(minWeight, maxWeight);

    item _item(valueRandomDistValue(generator), valueRandomDistWeight(generator));
    return _item;
}

int funcaoCusto(estado &_estado){
    int custo = 0;

    for (int i = 0; i < _estado.itemsPertenc.size(); i++){
        if (_estado.itemsPertenc[i]){
            custo += (*_estado.itemPointer)[i].valor;
        }
    }

    return custo;
}

estado crossover(estado &pai, estado &mae, default_random_engine &generator){
    estado filho = pai;
    filho.backpackCurrent = 0;
    uniform_int_distribution<int> distCoinFlip(0,1);

    for (int i = 0; i < filho.itemsPertenc.size(); i++){
        if (distCoinFlip(generator)){
            filho.itemsPertenc[i] = pai.itemsPertenc[i];
        }
        else{
            filho.itemsPertenc[i] = mae.itemsPertenc[i];
        }
        if (filho.itemsPertenc[i]){
            filho.backpackCurrent += (*filho.itemPointer)[i].peso;
        }
    }
    fixState(filho, generator);

    return filho;
}

void mutacao (estado &filho, default_random_engine &generator){
    uniform_int_distribution<int> indexDistribution(0,filho.itemsPertenc.size() -1);
    uniform_int_distribution<int> probMutationDistribution(0,99); //1% de prob

    if (probMutationDistribution(generator) == 0){
        int indexMutation = indexDistribution(generator);
        filho.itemsPertenc[indexMutation] = !filho.itemsPertenc[indexMutation];
        if (filho.itemsPertenc[indexMutation]){
            filho.backpackCurrent += (*filho.itemPointer)[indexMutation].peso;
        }
        else{
            filho.backpackCurrent -= (*filho.itemPointer)[indexMutation].peso;
        }
        fixState(filho, generator);
    }
}

int torneio (vector<estado> populacao, default_random_engine &generator){
    uniform_int_distribution<int> indexDistribution(0, populacao.size() - 1);
    int numParticipantes = populacao.size() / 4;
    int index = indexDistribution(generator);
    int indexPais = index;
    int maiorValor = funcaoCusto(populacao[index]);

    for (int i = 0; i < numParticipantes - 1; i++){
        index = indexDistribution(generator);
        int valorAtual = funcaoCusto(populacao[index]);
        if(valorAtual > maiorValor){
            maiorValor = valorAtual;
            indexPais = index;
        }
    }
    return indexPais;
}

int main(){
    
    //PARÂMETROS PARA O PROBLEMA
    int MAXVALUEITEM = 25;
    int MAXWEIGHTITEM = 30;
    int NUMITEMS = 200;
    int BACKPACKMAX = 500;
    //==========================
    int NUMPOPULACAO;
    int ITERACOES = 1000;
    int ITERLARGE = 100;

    for(NUMPOPULACAO = 5; NUMPOPULACAO <= 100; NUMPOPULACAO += 5){
        for (int k = 0; k< ITERLARGE; k++){
            default_random_engine generator;
            generator.seed(time(0));

            ofstream outFile;
            outFile.open("result_genetic_pop" + to_string(NUMPOPULACAO) + "/result_genetics_" + to_string(k) + ".csv");
            outFile << "iteracao,lucro\n";

            vector<item> items;
            
            for (int i = 0; i< NUMITEMS; i++){
                item it = genRandomItem(generator, 1, MAXVALUEITEM, 1, MAXWEIGHTITEM);
                items.push_back(it);
            }
            
            uniform_int_distribution<int> distCoinFlip(0,1);
            vector<estado> populacao;

            for (int i = 0; i< NUMPOPULACAO; i++){
                estado estadoAleatorio(BACKPACKMAX, &items);
                for (int j = 0; j < estadoAleatorio.itemsPertenc.size(); j ++){
                    if (distCoinFlip(generator)){
                        estadoAleatorio.itemsPertenc[j] = 1;
                        estadoAleatorio.backpackCurrent += (*estadoAleatorio.itemPointer)[j].peso;
                    }
                    else {
                        estadoAleatorio.itemsPertenc[j] = 0;
                    }
                }
                
                fixState(estadoAleatorio, generator);
                populacao.push_back(estadoAleatorio);
            }

            vector<estado> filhos;
            //loop função genética
            for (int i = 0; i < ITERACOES; i ++){

                int maiorLucro = 0;
                int pesoMaiorLucro;
                for (int k = 0; k < populacao.size(); k++){
                    int lucroAtual = funcaoCusto(populacao[k]);
                    if (lucroAtual > maiorLucro){
                        maiorLucro = lucroAtual;
                        pesoMaiorLucro = populacao[k].backpackCurrent;
                    }
                }
                cout << "Iteracao: " << i << " | Maior Lucro: " << maiorLucro << " | Peso: " << pesoMaiorLucro <<endl;
                outFile << i << "," << maiorLucro << "\n";

                filhos.clear();
                for (int j = 0; j < NUMPOPULACAO; j ++){
                    int indexPai = torneio(populacao, generator);
                    int indexMae;
                    bool flagMae = 0;
                    while (!flagMae){
                        indexMae = torneio(populacao, generator);
                        if (indexMae != indexPai){
                            flagMae = 1;
                        }
                    }
                    estado filho = crossover(populacao[indexPai], populacao[indexMae], generator);
                    mutacao(filho, generator);
                    filhos.push_back(filho);
                }
                populacao = filhos;
            }

            outFile.close();
        }
    }
    return 0;
}