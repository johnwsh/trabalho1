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
        bool estaNaMochila;
        item(int _valor, int _peso){
            valor = _valor;
            peso = _peso;
            estaNaMochila = 0;
        }
};

class estado {
    public:
        vector<item> items;
        int backpackCurrent;
        int backpackMax;

        estado(int _backpackMax){
            backpackCurrent = 0;
            backpackMax = _backpackMax;
        }
};

item genRandomItem(default_random_engine &generator, int minValue, int maxValue, int minWeight, int maxWeight){
    uniform_int_distribution<int> valueRandomDistValue(minValue, maxValue);
    uniform_int_distribution<int> valueRandomDistWeight(minWeight, maxWeight);

    item _item(valueRandomDistValue(generator), valueRandomDistWeight(generator));
    return _item;
}

//Criar uma função de temperatura
float funcaoTemperatura(int x, int iter){
    return max(1.0, (double)(iter/2 - x));
}

int funcaoCusto(estado _estado){
    int custo = 0;
    for (int i = 0; i< _estado.items.size(); i++){
        if (_estado.items[i].estaNaMochila){
            custo += _estado.items[i].valor;
        }
    }
    return custo;
}

estado getVizinhoAleatorio(estado &_estado, default_random_engine generator){
    estado vizinho(_estado.backpackMax);
    vizinho = _estado;
    

    bool flagSelected = 0;
    uniform_int_distribution<int> dist(0,vizinho.items.size()-1);

    while (!flagSelected){
        int i = dist(generator);

        item& itemDiff = vizinho.items[i];
        if (itemDiff.estaNaMochila){
            vizinho.backpackCurrent -= itemDiff.peso;
            itemDiff.estaNaMochila = 0;
            flagSelected = 1;
        }
        else {
            if (itemDiff.peso + vizinho.backpackCurrent <= vizinho.backpackMax){
                vizinho.backpackCurrent += itemDiff.peso;
                itemDiff.estaNaMochila = 1;
                flagSelected = 1;
            }
        }
    }

    return vizinho;
}

int main(){
    int ITERACOES = 1000;
    int MAXVALUEITEM = 25;
    int MAXWEIGHTITEM = 30;
    int NUMITEMS = 200;
    int BACKPACKMAX = 500;
    default_random_engine generator;
    generator.seed(time(0));
    ofstream outFile;
    outFile.open("result.csv");
    outFile << "iteracao,lucro\n";

    //inicializar o problema e escolher um estado inicial aleatório
    estado estadoAtual(BACKPACKMAX);

    for (int i = 0; i < NUMITEMS; i++){
        item it = genRandomItem(generator, 1, MAXVALUEITEM, 1, MAXWEIGHTITEM);

        if (it.peso + estadoAtual.backpackCurrent <= estadoAtual.backpackMax){
            it.estaNaMochila = 1;
            estadoAtual.backpackCurrent += it.peso;
        }
        else{
            it.estaNaMochila = 0;
        }
        estadoAtual.items.push_back(it);
    }

    //entrar em tempera simulada
    for (int i = 0; i< ITERACOES; i++){
        //escolher vizinho aleatório
        estado vizinho = getVizinhoAleatorio(estadoAtual,generator);
        //decide se é melhor ou não 
        int custoAtual = funcaoCusto(estadoAtual);
        cout << "Iteracao: " << i << " Lucro atual: " << custoAtual << " Peso atual: " << estadoAtual.backpackCurrent << endl;
        outFile << i << "," << custoAtual <<"\n";
        int custoVizinho = funcaoCusto(vizinho);
        cout << "Iteracao: " << i << " Lucro vizinho: " << custoVizinho << " Peso vizinho: " << vizinho.backpackCurrent << endl;
        int diff = custoAtual - custoVizinho;
        //se for pega 
        if (diff <= 0){
            estadoAtual = vizinho;
        }
        //se não, talvez pega com base em temperatura
        else{
            int T = funcaoTemperatura(i, ITERACOES);
            double prob = (1/(exp((double)diff/T))) * 100;

            uniform_int_distribution<int> dist(1,100);
            int numAleatorio = dist(generator);

            if (numAleatorio < prob){
                estadoAtual = vizinho;
            }
        }
    }
    return 0;
}