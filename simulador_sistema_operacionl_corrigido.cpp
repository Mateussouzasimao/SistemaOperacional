#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib>

enum class EstadoProcesso { NOVO, PRONTO, EXECUTANDO, BLOQUEADO, TERMINADO };
enum class TipoRecurso { CPU, DISCO, IMPRESSORA };

struct Processo {
    int id;
    EstadoProcesso estado;
    std::vector<TipoRecurso> recursosNecessarios;
    int usoCPU;
    int usoMemoria;

    Processo(int _id) : id(_id), estado(EstadoProcesso::NOVO), usoCPU(0), usoMemoria(0) {}
    Processo() : id(-1), estado(EstadoProcesso::NOVO), usoCPU(0), usoMemoria(0) {}
};

class GerenciadorRecursos {
private:
    std::vector<int> recursosDisponiveis;

public:
    GerenciadorRecursos(int numRecursos) : recursosDisponiveis(numRecursos, 100) {}

    bool alocarRecursos(const std::vector<TipoRecurso>& recursos, int usoCPU) {
        for (TipoRecurso recurso : recursos) {
            if (recursosDisponiveis[static_cast<int>(recurso)] < 30 || usoCPU > 70) {
                return false; // Recursos insuficientes
            }
        }

        for (TipoRecurso recurso : recursos) {
            recursosDisponiveis[static_cast<int>(recurso)] -= 30;
        }

        return true; // Recursos alocados com sucesso
    }

    void liberarRecursos(const std::vector<TipoRecurso>& recursos) {
        for (TipoRecurso recurso : recursos) {
            recursosDisponiveis[static_cast<int>(recurso)] += 30;
        }
    }
};

class GerenciadorProcessos {
private:
    std::queue<Processo> filaNovos;
    std::queue<Processo> filaProntos;
    Processo processoExecutando;
    std::queue<Processo> filaBloqueados;
    GerenciadorRecursos gerenciadorRecursos;
    std::map<int, std::string> aplicativos;

public:
    GerenciadorProcessos(int numRecursos) : gerenciadorRecursos(numRecursos) {
        aplicativos = {
            {1, "Editor de texto"},
            {2, "Editor de planilhas"},
            {3, "Editor de Apresentações"},
            {4, "Navegador"},
            {5, "Jogo"}
        };
    }

    void criarProcesso(int id) {
        if (aplicativos.find(id) != aplicativos.end()) {
            Processo novoProcesso(id);
            novoProcesso.recursosNecessarios = {TipoRecurso::CPU, TipoRecurso::DISCO};
            novoProcesso.usoCPU = 30;
            novoProcesso.usoMemoria = rand() % 101;

            if (!gerenciadorRecursos.alocarRecursos(novoProcesso.recursosNecessarios, novoProcesso.usoCPU)) {
                std::cout << "Limite de recursos ou CPU atingido. Não é possível abrir mais aplicativos.\n";
                return;
            }

            novoProcesso.estado = EstadoProcesso::PRONTO; // Definir estado como PRONTO
            filaNovos.push(novoProcesso);
            std::cout << "Aplicativo " << aplicativos[id] << " aberto.\n";
        } else {
            std::cout << "Aplicativo não encontrado.\n";
        }
    }

    void fecharProcesso() {
        if (processoExecutando.estado == EstadoProcesso::EXECUTANDO) {
            int idFechado = processoExecutando.id;
            gerenciadorRecursos.liberarRecursos(processoExecutando.recursosNecessarios);
            std::cout << "Aplicativo " << aplicativos[idFechado] << " encerrado.\n";
            processoExecutando.estado = EstadoProcesso::TERMINADO;
            processoExecutando = Processo();
        } else {
            std::cout << "Nenhum aplicativo está em execução.\n";
        }
    }

    void escalonar() {
        if (!filaNovos.empty()) {
            Processo proximoProcesso = filaNovos.front();
            if (gerenciadorRecursos.alocarRecursos(proximoProcesso.recursosNecessarios, proximoProcesso.usoCPU)) {
                processoExecutando = proximoProcesso;
                filaNovos.pop();
                processoExecutando.estado = EstadoProcesso::EXECUTANDO;
                std::cout << "Executando aplicativo " << aplicativos[processoExecutando.id] << "\n";
            } else {
                filaBloqueados.push(proximoProcesso); // Adiciona à fila de bloqueados se recursos não estão disponíveis
                std::cout << "Recursos não disponíveis para executar próximo aplicativo. Adicionado à fila de bloqueados.\n";
            }
        } else {
            std::cout << "Nenhum aplicativo na fila de novos.\n";
        }
    }

    const std::map<int, std::string>& getAplicativos() const {
        return aplicativos;
    }
};

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    const int numRecursos = 3;
    GerenciadorProcessos gerenciador(numRecursos);

    while (true) {
        std::cout << "Escolha uma ação:\n";
        std::cout << "1. Abrir aplicativo\n";
        std::cout << "2. Fechar aplicativo\n";
        std::cout << "3. Sair\n";

        int escolha;
        std::cin >> escolha;

        switch (escolha) {
            case 1: {
                int id;
                std::cout << "Escolha o ID do aplicativo:\n";
                for (const auto& app : gerenciador.getAplicativos()) {
                    std::cout << app.first << ". " << app.second << "\n";
                }
                std::cin >> id;

                if (gerenciador.getAplicativos().find(id) != gerenciador.getAplicativos().end()) {
                    gerenciador.criarProcesso(id);
                    gerenciador.escalonar(); // Escalonar após abrir
                } else {
                    std::cout << "ID de aplicativo inválido.\n";
                }
                break;
            }
            case 2:
                gerenciador.fecharProcesso();
                gerenciador.escalonar(); // Escalonar após fechar
                break;
            case 3:
                return 0;
            default:
                std::cout << "Opção inválida.\n";
                break;
        }
    }

    return 0;
}
