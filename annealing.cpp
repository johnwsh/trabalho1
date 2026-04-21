#include <vector>
#include <random>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std;

#define ITERACOES 500
#define NUMTESTES 1
#define MUDANCAS 2

#define BACKPACKMAX 500
#define NUMITEMS 100
#define MAXVALUEITEM 25
#define MINVALUEITEM 1
#define MINWEIGHTITEM 1
#define MAXWEIGHTITEM 30

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

item genRandomItem(mt19937 &generator){
    uniform_int_distribution<int> valueRandomDistValue(MINVALUEITEM, MAXVALUEITEM);
    uniform_int_distribution<int> valueRandomDistWeight(MINWEIGHTITEM, MAXWEIGHTITEM);

    item _item(valueRandomDistValue(generator), valueRandomDistWeight(generator));
    return _item;
}

//Criar uma função de temperatura
float funcaoTemperatura(int x){
    //return max(0.001f, (float)(ITERACOES - x) / ITERACOES);
    float k = 12.0f / ITERACOES;
    return std::max(0.001f, exp(-k * x));
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

float funcaoDiff(int valorAtual, int valorVizinho){
    float diff = valorAtual - valorVizinho;

    float diffNormalizado = (diff - MINVALUEITEM) / ((MAXVALUEITEM*MUDANCAS) - MINVALUEITEM);
    return max(0.1f, (1 - diffNormalizado));
}

estado getVizinhoAleatorio(estado &_estado, mt19937 &generator){
    estado vizinho(_estado.backpackMax);
    vizinho = _estado;
    
    int numSelected = 0;
    uniform_int_distribution<int> dist(0,vizinho.items.size()-1);

    while (numSelected < MUDANCAS){
        int i = dist(generator);

        item& itemDiff = vizinho.items[i];
        if (itemDiff.estaNaMochila){
            vizinho.backpackCurrent -= itemDiff.peso;
            itemDiff.estaNaMochila = 0;
            numSelected++;
        }
        else {
            if (itemDiff.peso + vizinho.backpackCurrent <= vizinho.backpackMax){
                vizinho.backpackCurrent += itemDiff.peso;
                itemDiff.estaNaMochila = 1;
                numSelected++;
            }
        }
    }
    return vizinho;
}

int main(){
    mt19937 generator(time(0));
    ofstream outFile;
    ofstream outFileProb;
    outFile.open("result.csv");
    outFile << "iteracao,lucro\n";
    outFileProb.open("prob.csv");
    outFileProb << "iteracao,prob\n";

    float ValorTotal = 0;

    //inicializar o problema e escolher um estado inicial aleatório
    estado estadoAtual(BACKPACKMAX);

    for (int i = 0; i < NUMITEMS; i++){
        item it = genRandomItem(generator);

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
    for (int teste = 0; teste< NUMTESTES; teste++){
        for (int i = 0; i< ITERACOES; i++){
            //escolher vizinho aleatório
            estado vizinho = getVizinhoAleatorio(estadoAtual,generator);
            //decide se é melhor ou não 
            int custoAtual = funcaoCusto(estadoAtual);
            outFile << i << "," << custoAtual <<"\n";
            int custoVizinho = funcaoCusto(vizinho);
            int diff = custoAtual - custoVizinho;
            //se for pega 
            if (diff <= 0){
                estadoAtual = vizinho;
            }
            //se não, talvez pega com base em temperatura
            else{
                double T = funcaoTemperatura(i);
                double E = funcaoDiff(custoAtual, custoVizinho);
                double prob = (1/(exp((double)E/T)));

                //cout << "T: " << T << " E: " << E << " prob: " << prob << endl;
                outFileProb << i << "," << prob << "\n";

                uniform_real_distribution<double> dist(0.0, 1.0);
                double numAleatorio = dist(generator);

                if (numAleatorio < prob){
                    estadoAtual = vizinho;
                }
            }
        }
        ValorTotal += funcaoCusto(estadoAtual);
        //cout << "Lucro atual: " << funcaoCusto(estadoAtual) << " Peso atual: " << estadoAtual.backpackCurrent << endl;
    }
    cout << "Media do lucro: " << ValorTotal/NUMTESTES << endl;
    cout << "Peso final: " << estadoAtual.backpackCurrent << endl;
    return 0;
}