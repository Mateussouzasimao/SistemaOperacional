#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib>

// Enumeração para os possíveis estados de um processo
enum class EstadoProcesso { NOVO, PRONTO, EXECUTANDO, BLOQUEADO, TERMINADO };
// Enumeração para os tipos de recursos do sistema
enum class TipoRecurso { CPU, DISCO, IMPRESSORA };

// Definição da estrutura de dados para um processo
struct Processo {
    int id; // Identificador único do processo
    EstadoProcesso estado; // Estado atual do processo
    std::vector<TipoRecurso> recursosNecessarios; // Tipos de recursos necessários para o processo
    int usoCPU; // Uso de CPU do processo (em percentual)
    int usoMemoria; // Uso de memória do processo (em MB)

    Processo(int _id) : id(_id), estado(EstadoProcesso::NOVO), usoCPU(0), usoMemoria(0) {}
    Processo() : id(-1), estado(EstadoProcesso::NOVO), usoCPU(0), usoMemoria(0) {}
};

// Classe que gerencia os recursos do sistema
class GerenciadorRecursos {
private:
    std::vector<int> recursosDisponiveis; // Armazena a quantidade disponível de cada recurso

public:
    // Construtor que inicializa o gerenciador de recursos com 100 unidades de cada recurso
    GerenciadorRecursos(int numRecursos) : recursosDisponiveis(numRecursos, 100) {}

    // Aloca recursos para um processo e retorna true se possível, ou false se não houver recursos suficientes
    bool alocarRecursos(const std::vector<TipoRecurso>& recursos) {
        // Verifica se há pelo menos 30 unidades disponíveis de cada recurso
        for (TipoRecurso recurso : recursos) {
            if (recursosDisponiveis[static_cast<int>(recurso)] < 30) {
                return false; // Recursos insuficientes
            }
        }

        // Subtrai 30 unidades de cada recurso alocado
        for (TipoRecurso recurso : recursos) {
            recursosDisponiveis[static_cast<int>(recurso)] -= 30;
        }

        return true; // Recursos alocados com sucesso
    }

    // Libera recursos de um processo
    void liberarRecursos(const std::vector<TipoRecurso>& recursos) {
        // Adiciona 30 unidades de cada recurso liberado
        for (TipoRecurso recurso : recursos) {
            recursosDisponiveis[static_cast<int>(recurso)] += 30;
        }
    }
};

// Classe que gerencia os processos e seu escalonamento
class GerenciadorProcessos {
private:
    std::queue<Processo> filaNovos; // Fila de processos novos (ainda não escalonados)
    std::queue<Processo> filaProntos; // Fila de processos prontos para execução
    Processo processoExecutando; // Processo atualmente em execução
    std::queue<Processo> filaBloqueados; // Fila de processos bloqueados (esperando recursos)
    GerenciadorRecursos gerenciadorRecursos; // Instância do gerenciador de recursos
    std::map<int, std::string> aplicativos; // Mapeamento de IDs de aplicativos para seus nomes

public:
    // Construtor que inicializa o gerenciador de processos com o número de recursos disponíveis
    GerenciadorProcessos(int numRecursos) : gerenciadorRecursos(numRecursos) {
        // Inicialização dos nomes dos aplicativos disponíveis
        aplicativos = {
            {1, "Editor de texto"},
            {2, "Editor de planilhas"},
            {3, "Editor de Apresentações"},
            {4, "Navegador"},
            {5, "Jogo"}
        };
    }

    // Método para criar um novo processo (abrir um aplicativo) com base no ID fornecido
    void criarProcesso(int id) {
        if (aplicativos.find(id) != aplicativos.end()) {
            Processo novoProcesso(id); // Cria uma nova instância de Processo
            novoProcesso.recursosNecessarios = {TipoRecurso::CPU, TipoRecurso::DISCO}; // Define recursos necessários
            novoProcesso.usoCPU = 30; // Uso de CPU fixo em 30%
            novoProcesso.usoMemoria = rand() % 101; // Uso de memória entre 0MB e 100MB

            // Verifica se há recursos disponíveis para alocar
            if (!gerenciadorRecursos.alocarRecursos(novoProcesso.recursosNecessarios)) {
                std::cout << "Limite de recursos atingido. Não é possível abrir mais aplicativos.\n";
                return;
            }

            filaNovos.push(novoProcesso); // Adiciona o novo processo à fila de novos
            std::cout << "Aplicativo " << aplicativos[id] << " aberto.\n";
        } else {
            std::cout << "Aplicativo não encontrado.\n";
        }
    }

    // Método para fechar um processo (encerrar um aplicativo) que está em execução
    void fecharProcesso() {
        if (processoExecutando.estado == EstadoProcesso::EXECUTANDO) {
            int idFechado = processoExecutando.id; // Guarda o ID antes de liberar recursos
            gerenciadorRecursos.liberarRecursos(processoExecutando.recursosNecessarios);
            std::cout << "Aplicativo " << aplicativos[idFechado] << " encerrado.\n";
            processoExecutando.estado = EstadoProcesso::TERMINADO; // Define o estado como TERMINADO
            processoExecutando = Processo(); // Limpa o processo em execução
        } else {
            std::cout << "Nenhum aplicativo está em execução.\n";
        }
    }

    // Método para escalonar um processo da fila de novos para a fila de prontos (execução)
    void escalonar() {
        if (!filaNovos.empty()) {
            Processo proximoProcesso = filaNovos.front(); // Pega o próximo processo da fila de novos
            if (gerenciadorRecursos.alocarRecursos(proximoProcesso.recursosNecessarios)) {
                processoExecutando = proximoProcesso; // Define o processo como em execução
                filaNovos.pop(); // Remove o processo da fila de novos
                processoExecutando.estado = EstadoProcesso::EXECUTANDO; // Define o estado como EXECUTANDO
                std::cout << "Executando aplicativo " << aplicativos[processoExecutando.id] << "\n";
            } else {
                std::cout << "Recursos não disponíveis para executar próximo aplicativo.\n";
            }
        } else {
            std::cout << "Nenhum aplicativo na fila de novos.\n";
        }
    }

    // Métodos bloquear() e desbloquear() não foram implementados neste exemplo

    // Retorna a lista de aplicativos disponíveis
    const std::map<int, std::string>& getAplicativos() const {
        return aplicativos;
    }
};

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // Inicialização da semente para números aleatórios

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
