#include <iostream>
#include <vector>

using namespace std;

// Número de processos e tipos de recursos
const int num_processos = 5;
const int num_recursos = 3;

// Vetor de recursos disponíveis
vector<int> disponivel = {10, 5, 7};

// Matrizes de alocação e necessidade
vector<vector<int>> alocacao = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
};

vector<vector<int>> necessidade = {
    {7, 4, 3},
    {3, 2, 2},
    {9, 0, 0},
    {0, 1, 0},
    {4, 3, 3}
};

bool estaSeguro(int processo) {
    for (int i = 0; i < num_recursos; ++i) {
        if (necessidade[processo][i] > disponivel[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    vector<int> sequencia_segura;
    int completos[num_processos] = {0};

    for (int passo = 0; passo < num_processos; ++passo) {
        bool encontrado = false;
        for (int processo = 0; processo < num_processos; ++processo) {
            if (!completos[processo] && estaSeguro(processo)) {
                completos[processo] = true;
                sequencia_segura.push_back(processo);

                for (int i = 0; i < num_recursos; ++i) {
                    disponivel[i] += alocacao[processo][i];
                }

                encontrado = true;
                break;
            }
        }
        if (!encontrado) {
            cout << "O sistema nao esta em um estado seguro.\n";
            return 0;
        }
    }

    cout << "O sistema esta em um estado seguro. Sequencia segura: ";
    for (int processo : sequencia_segura) {
        cout << "P" << processo << " ";
    }
    cout << endl;

    return 0;
}
