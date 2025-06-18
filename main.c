// Sistema de Gerenciamento de Eventos
// Desenvolvido com base no relatório técnico de Romulo Festa, Ahmad Moussa e Ali Ataya

// Importação de bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <locale.h> // Adicionado para suportar caracteres especiais

// ====== ESTRUTURAS DE DADOS ======

// Estrutura para representar um evento
typedef struct evento {
    int id;
    char nome[100];
    char descricao[500];
    char categoria[50];
    char data[11]; // Formato YYYY-MM-DD
    int capacidade;
    int vagasDisponiveis;
    struct evento* proximo;
} Evento;

// Estrutura para representar um participante
typedef struct participante {
    int id;
    char nome[100];
    char email[100];
    char telefone[20];
    struct participante* proximo;
} Participante;

// Estrutura para lista circular de participantes
typedef struct lista_participantes {
    Participante* inicio;
    int tamanho;
} ListaParticipantes;

// Enum para status de inscrição
typedef enum StatusInscricao { 
    CONFIRMADA, 
    PENDENTE, 
    CANCELADA 
} StatusInscricao;

// Estrutura para representar uma inscrição
typedef struct inscricao {
    Participante* participante;
    Evento* evento;
    char dataInscricao[11]; // Formato YYYY-MM-DD
    StatusInscricao status;
    struct inscricao* proxima;
} Inscricao;

// Estrutura para lista de inscrições
typedef struct lista_inscricoes {
    Inscricao* inicio;
    int tamanho;
} ListaInscricoes;

// ====== GERENCIAMENTO DE MEMÓRIA ======

// Função para alocação segura de memória
void* alocar_memoria(size_t tamanho) {
    void* ptr = malloc(tamanho);
    if (ptr == NULL) {
        fprintf(stderr, "Erro de alocação de memória!\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// Função para liberação segura de memória
void liberar_memoria(void* ptr) {
    if (ptr != NULL) {
        free(ptr);
    }
}

// ====== FUNÇÕES PARA MANIPULAÇÃO DE EVENTOS ======

// Função para criar um novo evento
Evento* evento_criar(int id, const char* nome, const char* descricao, 
                    const char* categoria, const char* data, int capacidade) {
    Evento* novo = (Evento*)alocar_memoria(sizeof(Evento));
    
    novo->id = id;
    strncpy(novo->nome, nome, sizeof(novo->nome)-1);
    strncpy(novo->descricao, descricao, sizeof(novo->descricao)-1);
    strncpy(novo->categoria, categoria, sizeof(novo->categoria)-1);
    strncpy(novo->data, data, sizeof(novo->data)-1);
    
    novo->capacidade = capacidade;
    novo->vagasDisponiveis = capacidade;
    novo->proximo = NULL;
    
    return novo;
}
// Função para destruir um evento
void evento_destruir(Evento* evento) {
    liberar_memoria(evento);
}

// Função para adicionar um evento à lista de eventos
Evento* eventos_adicionar(Evento* lista, Evento* novo) {
    if (lista == NULL) {
        return novo;
    }
    
    Evento* atual = lista;
    while (atual->proximo != NULL) {
        atual = atual->proximo;
    }
    
    atual->proximo = novo;
    return lista;
}

// Função para buscar um evento por ID
Evento* eventos_buscar_por_id(Evento* lista, int id) {
    Evento* atual = lista;
    while (atual != NULL) {
        if (atual->id == id) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// Função para remover um evento da lista
Evento* eventos_remover(Evento* lista, int id) {
    if (lista == NULL) {
        return NULL;
    }
    
    // Caso especial: remover o primeiro evento da lista
    if (lista->id == id) {
        Evento* temp = lista->proximo;
        evento_destruir(lista);
        return temp;
    }
    
    // Caso geral: remover evento do meio ou fim da lista
    Evento* atual = lista;
    while (atual->proximo != NULL) {
        if (atual->proximo->id == id) {
            Evento* temp = atual->proximo;
            atual->proximo = temp->proximo;
            evento_destruir(temp);
            return lista;
        }
        atual = atual->proximo;
    }
    
    return lista;
}

// Função para listar todos os eventos
void eventos_listar(Evento* lista) {
    if (lista == NULL) {
        printf("Nenhum evento cadastrado.\n");
        return;
    }
    
    printf("\n=== LISTA DE EVENTOS ===\n");
    Evento* atual = lista;
    while (atual != NULL) {
        printf("ID: %d\n", atual->id);
        printf("Nome: %s\n", atual->nome);
        printf("Categoria: %s\n", atual->categoria);
        printf("Data: %s\n", atual->data);
        printf("Vagas: %d/%d\n", atual->vagasDisponiveis, atual->capacidade);
        printf("--------------------\n");
        atual = atual->proximo;
    }
}

// ====== FUNÇÕES PARA MANIPULAÇÃO DE PARTICIPANTES ======

// Função para criar um novo participante
Participante* participante_criar(int id, const char* nome, const char* email, const char* telefone) {
    Participante* novo = (Participante*)alocar_memoria(sizeof(Participante));
    
    novo->id = id;
    strncpy(novo->nome, nome, sizeof(novo->nome)-1);
    strncpy(novo->email, email, sizeof(novo->email)-1);
    strncpy(novo->telefone, telefone, sizeof(novo->telefone)-1);
    
    novo->proximo = NULL;
    
    return novo;
}

// Função para destruir um participante
void participante_destruir(Participante* participante) {
    liberar_memoria(participante);
}

// Função para inicializar a lista circular de participantes
ListaParticipantes* lista_participantes_criar() {
    ListaParticipantes* lista = (ListaParticipantes*)alocar_memoria(sizeof(ListaParticipantes));
    lista->inicio = NULL;
    lista->tamanho = 0;
    return lista;
}

// Função para adicionar um participante à lista circular
void lista_participantes_adicionar(ListaParticipantes* lista, Participante* novo) {
    if (lista->inicio == NULL) {
        lista->inicio = novo;
        novo->proximo = novo;  // Aponta para si mesmo (circular)
    } else {
        Participante* ultimo = lista->inicio;
        // Encontrar o último elemento (que aponta para o primeiro)
        while (ultimo->proximo != lista->inicio) {
            ultimo = ultimo->proximo;
        }
        
        novo->proximo = lista->inicio;
        ultimo->proximo = novo;
    }
    
    lista->tamanho++;
}

// Função para buscar um participante por ID
Participante* lista_participantes_buscar_por_id(ListaParticipantes* lista, int id) {
    if (lista->inicio == NULL) {
        return NULL;
    }
    
    Participante* atual = lista->inicio;
    do {
        if (atual->id == id) {
            return atual;
        }
        atual = atual->proximo;
    } while (atual != lista->inicio);
    
    return NULL;
}

// Função para remover um participante da lista circular
void lista_participantes_remover(ListaParticipantes* lista, int id) {
    if (lista->inicio == NULL) {
        return;
    }
    
    // Caso especial: lista com apenas um elemento
    if (lista->inicio->id == id && lista->inicio->proximo == lista->inicio) {
        participante_destruir(lista->inicio);
        lista->inicio = NULL;
        lista->tamanho = 0;
        return;
    }
    
    // Caso especial: remover o primeiro elemento
    if (lista->inicio->id == id) {
        Participante* ultimo = lista->inicio;
        while (ultimo->proximo != lista->inicio) {
            ultimo = ultimo->proximo;
        }
        
        Participante* temp = lista->inicio;
        lista->inicio = lista->inicio->proximo;
        ultimo->proximo = lista->inicio;
        
        participante_destruir(temp);
        lista->tamanho--;
        return;
    }
    
    // Caso geral: remover do meio ou fim
    Participante* atual = lista->inicio;
    while (atual->proximo != lista->inicio) {
        if (atual->proximo->id == id) {
            Participante* temp = atual->proximo;
            atual->proximo = temp->proximo;
            participante_destruir(temp);
            lista->tamanho--;
            return;
        }
        atual = atual->proximo;
    }
}

// Função para listar todos os participantes
void lista_participantes_listar(ListaParticipantes* lista) {
    if (lista->inicio == NULL) {
        printf("Nenhum participante cadastrado.\n");
        return;
    }
    
    printf("\n=== LISTA DE PARTICIPANTES ===\n");
    Participante* atual = lista->inicio;
    do {
        printf("ID: %d\n", atual->id);
        printf("Nome: %s\n", atual->nome);
        printf("Email: %s\n", atual->email);
        printf("Telefone: %s\n", atual->telefone);
        printf("--------------------\n");
        atual = atual->proximo;
    } while (atual != lista->inicio);
}

// Função para destruir a lista de participantes
void lista_participantes_destruir(ListaParticipantes* lista) {
    if (lista->inicio == NULL) {
        liberar_memoria(lista);
        return;
    }
    
    Participante* atual = lista->inicio;
    Participante* proximo;
    
    do {
        proximo = atual->proximo;
        participante_destruir(atual);
        atual = proximo;
    } while (atual != lista->inicio);
    
    liberar_memoria(lista);
}

// ====== FUNÇÕES PARA MANIPULAÇÃO DE INSCRIÇÕES ======

// Função para criar uma lista de inscrições
ListaInscricoes* lista_inscricoes_criar() {
    ListaInscricoes* lista = (ListaInscricoes*)alocar_memoria(sizeof(ListaInscricoes));
    lista->inicio = NULL;
    lista->tamanho = 0;
    return lista;
}

// Função para criar uma nova inscrição
Inscricao* inscricao_criar(Participante* participante, Evento* evento, const char* dataInscricao, StatusInscricao status) {
    Inscricao* nova = (Inscricao*)alocar_memoria(sizeof(Inscricao));
    
    nova->participante = participante;
    nova->evento = evento;
    strncpy(nova->dataInscricao, dataInscricao, sizeof(nova->dataInscricao)-1);
    nova->status = status;
    nova->proxima = NULL;
    
    return nova;
}

// Função para destruir uma inscrição
void inscricao_destruir(Inscricao* inscricao) {
    liberar_memoria(inscricao);
}

// Função para verificar se um participante já está inscrito em um evento
bool inscricao_existe(ListaInscricoes* lista, int idParticipante, int idEvento) {
    Inscricao* atual = lista->inicio;
    
    while (atual != NULL) {
        if (atual->participante->id == idParticipante && 
            atual->evento->id == idEvento && 
            atual->status != CANCELADA) {
            return true;
        }
        atual = atual->proxima;
    }
    
    return false;
}

// Função para adicionar uma inscrição à lista
void lista_inscricoes_adicionar(ListaInscricoes* lista, Inscricao* nova) {
    if (lista->inicio == NULL) {
        lista->inicio = nova;
    } else {
        Inscricao* atual = lista->inicio;
        while (atual->proxima != NULL) {
            atual = atual->proxima;
        }
        atual->proxima = nova;
    }
    
    lista->tamanho++;
}

// Função para cancelar uma inscrição
bool lista_inscricoes_cancelar(ListaInscricoes* lista, int idParticipante, int idEvento) {
    Inscricao* atual = lista->inicio;
    
    while (atual != NULL) {
        if (atual->participante->id == idParticipante && 
            atual->evento->id == idEvento && 
            atual->status == CONFIRMADA) {
            
            atual->status = CANCELADA;
            atual->evento->vagasDisponiveis++;
            
            return true;
        }
        atual = atual->proxima;
    }
    
    return false;
}

// Função para listar inscrições por evento
void lista_inscricoes_por_evento(ListaInscricoes* lista, int idEvento) {
    Inscricao* atual = lista->inicio;
    bool encontrado = false;
    
    printf("\n=== INSCRIÇÕES DO EVENTO ID %d ===\n", idEvento);
    
    while (atual != NULL) {
        if (atual->evento->id == idEvento && atual->status == CONFIRMADA) {
            printf("Participante: %s (ID: %d)\n", 
                   atual->participante->nome, 
                   atual->participante->id);
            printf("Data de inscrição: %s\n", atual->dataInscricao);
            printf("--------------------\n");
            encontrado = true;
        }
        atual = atual->proxima;
    }
    
    if (!encontrado) {
        printf("Nenhuma inscrição encontrada para este evento.\n");
    }
}

// Função para listar inscrições por participante
void lista_inscricoes_por_participante(ListaInscricoes* lista, int idParticipante) {
    Inscricao* atual = lista->inicio;
    bool encontrado = false;
    
    printf("\n=== INSCRIÇÕES DO PARTICIPANTE ID %d ===\n", idParticipante);
    
    while (atual != NULL) {
        if (atual->participante->id == idParticipante && atual->status == CONFIRMADA) {
            printf("Evento: %s (ID: %d)\n", 
                   atual->evento->nome, 
                   atual->evento->id);
            printf("Data: %s\n", atual->evento->data);
            printf("Data de inscrição: %s\n", atual->dataInscricao);
            printf("--------------------\n");
            encontrado = true;
        }
        atual = atual->proxima;
    }
    
    if (!encontrado) {
        printf("Nenhuma inscrição encontrada para este participante.\n");
    }
}

// Função para destruir a lista de inscrições
void lista_inscricoes_destruir(ListaInscricoes* lista) {
    Inscricao* atual = lista->inicio;
    Inscricao* proxima;
    
    while (atual != NULL) {
        proxima = atual->proxima;
        inscricao_destruir(atual);
        atual = proxima;
    }
    
    liberar_memoria(lista);
}

// ====== ALGORITMOS DE ORDENAÇÃO ======

// Função para trocar dois eventos (utilizada no QuickSort)
void trocar_eventos(Evento* a, Evento* b) {
    Evento temp = *a;
    *a = *b;
    *b = temp;
    
    // Correção dos ponteiros proximo após a troca
    temp.proximo = a->proximo;
    a->proximo = b->proximo;
    b->proximo = temp.proximo;
}

// Função de particionamento para o QuickSort (ordenação por data)
int particionar_por_data(Evento eventos[], int baixo, int alto) {
    char* pivo = eventos[alto].data;
    int i = (baixo - 1);
    
    for (int j = baixo; j <= alto - 1; j++) {
        if (strcmp(eventos[j].data, pivo) <= 0) {
            i++;
            trocar_eventos(&eventos[i], &eventos[j]);
        }
    }
    trocar_eventos(&eventos[i + 1], &eventos[alto]);
    return (i + 1);
}

// Implementação do QuickSort para ordenar eventos por data
void quicksort_eventos_por_data(Evento eventos[], int baixo, int alto) {
    if (baixo < alto) {
        int pi = particionar_por_data(eventos, baixo, alto);
        
        quicksort_eventos_por_data(eventos, baixo, pi - 1);
        quicksort_eventos_por_data(eventos, pi + 1, alto);
    }
}

// Função para ordenar eventos por data
void ordenar_eventos_por_data(Evento* lista, int tamanho) {
    if (lista == NULL || tamanho <= 1) {
        return;
    }
    
    // Copiar eventos para um array para facilitar a ordenação
    Evento* eventos_array = (Evento*)alocar_memoria(tamanho * sizeof(Evento));
    
    Evento* atual = lista;
    int i = 0;
    while (atual != NULL && i < tamanho) {
        eventos_array[i] = *atual;
        atual = atual->proximo;
        i++;
    }
    
    // Ordenar o array usando QuickSort
    quicksort_eventos_por_data(eventos_array, 0, tamanho - 1);
    
    // Reconstruir a lista encadeada com os eventos ordenados
    atual = lista;
    for (i = 0; i < tamanho; i++) {
        *atual = eventos_array[i];
        atual = atual->proximo;
    }
    
    liberar_memoria(eventos_array);
}

// ====== FUNÇÕES DE UTILIDADE ======

// Função para obter a data atual no formato YYYY-MM-DD
void obter_data_atual(char* data_str) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    
    strftime(data_str, 11, "%Y-%m-%d", tm_info);
}

// Função para contar eventos na lista
int contar_eventos(Evento* lista) {
    int contador = 0;
    Evento* atual = lista;
    
    while (atual != NULL) {
        contador++;
        atual = atual->proximo;
    }
    
    return contador;
}

// ====== INTERFACE DO USUÁRIO ======

// Função para exibir o menu principal
void exibir_menu_principal() {
    printf("\n===== SISTEMA DE GERENCIAMENTO DE EVENTOS =====\n");
    printf("1. Gerenciar Eventos\n");
    printf("2. Gerenciar Participantes\n");
    printf("3. Gerenciar Inscrições\n");
    printf("4. Relatórios\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

// Função para exibir o menu de eventos
void exibir_menu_eventos() {
    printf("\n===== GERENCIAMENTO DE EVENTOS =====\n");
    printf("1. Adicionar Evento\n");
    printf("2. Listar Eventos\n");
    printf("3. Buscar Evento por ID\n");
    printf("4. Editar Evento\n");
    printf("5. Remover Evento\n");
    printf("6. Ordenar Eventos por Data\n");
    printf("0. Voltar\n");
    printf("Escolha uma opção: ");
}

// Função para exibir o menu de participantes
void exibir_menu_participantes() {
    printf("\n===== GERENCIAMENTO DE PARTICIPANTES =====\n");
    printf("1. Adicionar Participante\n");
    printf("2. Listar Participantes\n");
    printf("3. Buscar Participante por ID\n");
    printf("4. Editar Participante\n");
    printf("5. Remover Participante\n");
    printf("0. Voltar\n");
    printf("Escolha uma opção: ");
}

// Função para exibir o menu de inscrições
void exibir_menu_inscricoes() {
    printf("\n===== GERENCIAMENTO DE INSCRIÇÕES =====\n");
    printf("1. Nova Inscrição\n");
    printf("2. Listar Inscrições por Evento\n");
    printf("3. Listar Inscrições por Participante\n");
    printf("4. Cancelar Inscrição\n");
    printf("0. Voltar\n");
    printf("Escolha uma opção: ");
}

// Função para exibir o menu de relatórios
void exibir_menu_relatorios() {
    printf("\n===== RELATÓRIOS =====\n");
    printf("1. Eventos por Categoria\n");
    printf("2. Eventos com Vagas Disponíveis\n");
    printf("3. Participantes por Evento\n");
    printf("4. Estatísticas do Sistema\n");
    printf("0. Voltar\n");
    printf("Escolha uma opção: ");
}

// ====== FUNÇÕES PARA GERENCIAMENTO DE EVENTOS ====== 

// Função para processar a adição de um evento
void adicionar_evento(Evento** lista_eventos, int* proximoIdEvento) {
    char nome[100], descricao[500], categoria[50], data[11];
    int capacidade;
    
    printf("\n== Adicionar Novo Evento ==\n");
    
    printf("Nome do evento: ");
    scanf(" %[^\n]", nome);
    
    printf("Descrição: ");
    scanf(" %[^\n]", descricao);
    
    printf("Categoria: ");
    scanf(" %[^\n]", categoria);
    
    printf("Data (YYYY-MM-DD): ");
    scanf(" %10s", data);
    
    printf("Capacidade: ");
    scanf("%d", &capacidade);
    
    Evento* novo = evento_criar(*proximoIdEvento, nome, descricao, categoria, data, capacidade);
    *lista_eventos = eventos_adicionar(*lista_eventos, novo);
    
    (*proximoIdEvento)++;
    
    printf("\nEvento adicionado com sucesso! ID: %d\n", novo->id);
}

// Função para buscar e exibir um evento por ID
void buscar_evento(Evento* lista_eventos) {
    int id;
    
    printf("\n== Buscar Evento por ID ==\n");
    printf("ID do evento: ");
    scanf("%d", &id);
    
    Evento* evento = eventos_buscar_por_id(lista_eventos, id);
    
    if (evento != NULL) {
        printf("\n=== EVENTO ENCONTRADO ===\n");
        printf("ID: %d\n", evento->id);
        printf("Nome: %s\n", evento->nome);
        printf("Descrição: %s\n", evento->descricao);
        printf("Categoria: %s\n", evento->categoria);
        printf("Data: %s\n", evento->data);
        printf("Capacidade: %d\n", evento->capacidade);
        printf("Vagas disponíveis: %d\n", evento->vagasDisponiveis);
    } else {
        printf("\nEvento não encontrado!\n");
    }
}

// Função para editar um evento
void editar_evento(Evento* lista_eventos) {
    int id;
    
    printf("\n== Editar Evento ==\n");
    printf("ID do evento a ser editado: ");
    scanf("%d", &id);
    
    Evento* evento = eventos_buscar_por_id(lista_eventos, id);
    
    if (evento != NULL) {
        printf("\n=== EDITANDO EVENTO ===\n");
        printf("Nome atual: %s\n", evento->nome);
        printf("Novo nome (ou pressione Enter para manter): ");
        char nome[100];
        getchar(); // Limpar o buffer
        fgets(nome, sizeof(nome), stdin);
        if (nome[0] != '\n') {
            nome[strcspn(nome, "\n")] = 0; // Remover o \n do final
            strncpy(evento->nome, nome, sizeof(evento->nome)-1);
        }
        
        printf("Descrição atual: %s\n", evento->descricao);
        printf("Nova descrição (ou pressione Enter para manter): ");
        char descricao[500];
        fgets(descricao, sizeof(descricao), stdin);
        if (descricao[0] != '\n') {
            descricao[strcspn(descricao, "\n")] = 0;
            strncpy(evento->descricao, descricao, sizeof(evento->descricao)-1);
        }
        
        printf("Categoria atual: %s\n", evento->categoria);
        printf("Nova categoria (ou pressione Enter para manter): ");
        char categoria[50];
        fgets(categoria, sizeof(categoria), stdin);
        if (categoria[0] != '\n') {
            categoria[strcspn(categoria, "\n")] = 0;
            strncpy(evento->categoria, categoria, sizeof(evento->categoria)-1);
        }
        
        printf("Data atual: %s\n", evento->data);
        printf("Nova data (YYYY-MM-DD) (ou pressione Enter para manter): ");
        char data[11];
        fgets(data, sizeof(data), stdin);
        if (data[0] != '\n') {
            data[strcspn(data, "\n")] = 0;
            strncpy(evento->data, data, sizeof(evento->data)-1);
        }
        
        printf("Capacidade atual: %d\n", evento->capacidade);
        printf("Nova capacidade (ou digite 0 para manter): ");
        int capacidade;
        scanf("%d", &capacidade);
        if (capacidade > 0) {
            int diferenca = capacidade - evento->capacidade;
            evento->capacidade = capacidade;
            evento->vagasDisponiveis += diferenca;
            if (evento->vagasDisponiveis < 0) {
                evento->vagasDisponiveis = 0;
            }
        }
        
        printf("\nEvento atualizado com sucesso!\n");
    } else {
        printf("\nEvento não encontrado!\n");
    }
}

// Função para remover um evento
void remover_evento(Evento** lista_eventos) {
    int id;
    
    printf("\n== Remover Evento ==\n");
    printf("ID do evento a ser removido: ");
    scanf("%d", &id);
    
    Evento* evento = eventos_buscar_por_id(*lista_eventos, id);
    
    if (evento != NULL) {
        *lista_eventos = eventos_remover(*lista_eventos, id);
        printf("\nEvento removido com sucesso!\n");
    } else {
        printf("\nEvento não encontrado!\n");
    }
}

// ====== FUNÇÕES PARA GERENCIAMENTO DE PARTICIPANTES ====== 

// Função para adicionar um participante
void adicionar_participante(ListaParticipantes* lista_participantes, int* proximoIdParticipante) {
    char nome[100], email[100], telefone[20];
    
    printf("\n== Adicionar Novo Participante ==\n");
    
    printf("Nome: ");
    scanf(" %[^\n]", nome);
    
    printf("Email: ");
    scanf(" %[^\n]", email);
    
    printf("Telefone: ");
    scanf(" %[^\n]", telefone);
    
    Participante* novo = participante_criar(*proximoIdParticipante, nome, email, telefone);
    lista_participantes_adicionar(lista_participantes, novo);
    
    (*proximoIdParticipante)++;
    
    printf("\nParticipante adicionado com sucesso! ID: %d\n", novo->id);
}

// Função para buscar e exibir um participante por ID
void buscar_participante(ListaParticipantes* lista_participantes) {
    int id;
    
    printf("\n== Buscar Participante por ID ==\n");
    printf("ID do participante: ");
    scanf("%d", &id);
    
    Participante* participante = lista_participantes_buscar_por_id(lista_participantes, id);
    
    if (participante != NULL) {
        printf("\n=== PARTICIPANTE ENCONTRADO ===\n");
        printf("ID: %d\n", participante->id);
        printf("Nome: %s\n", participante->nome);
        printf("Email: %s\n", participante->email);
        printf("Telefone: %s\n", participante->telefone);
    } else {
        printf("\nParticipante não encontrado!\n");
    }
}

// Função para editar um participante
void editar_participante(ListaParticipantes* lista_participantes) {
    int id;
    
    printf("\n== Editar Participante ==\n");
    printf("ID do participante a ser editado: ");
    scanf("%d", &id);
    
    Participante* participante = lista_participantes_buscar_por_id(lista_participantes, id);
    
    if (participante != NULL) {
        printf("\n=== EDITANDO PARTICIPANTE ===\n");
        printf("Nome atual: %s\n", participante->nome);
        printf("Novo nome (ou pressione Enter para manter): ");
        char nome[100];
        getchar(); // Limpar o buffer
        fgets(nome, sizeof(nome), stdin);
   if (nome[0] != '\n') {
            nome[strcspn(nome, "\n")] = 0; // Remover o \n do final
            strncpy(participante->nome, nome, sizeof(participante->nome)-1);
        }
        
        printf("Email atual: %s\n", participante->email);
        printf("Novo email (ou pressione Enter para manter): ");
        char email[100];
        fgets(email, sizeof(email), stdin);
        if (email[0] != '\n') {
            email[strcspn(email, "\n")] = 0;
            strncpy(participante->email, email, sizeof(participante->email)-1);
        }
        
        printf("Telefone atual: %s\n", participante->telefone);
        printf("Novo telefone (ou pressione Enter para manter): ");
        char telefone[20];
        fgets(telefone, sizeof(telefone), stdin);
        if (telefone[0] != '\n') {
            telefone[strcspn(telefone, "\n")] = 0;
            strncpy(participante->telefone, telefone, sizeof(participante->telefone)-1);
        }
        
        printf("\nParticipante atualizado com sucesso!\n");
    } else {
        printf("\nParticipante não encontrado!\n");
    }
}

// Função para remover um participante
void remover_participante(ListaParticipantes* lista_participantes) {
    int id;
    
    printf("\n== Remover Participante ==\n");
    printf("ID do participante a ser removido: ");
    scanf("%d", &id);
    
    Participante* participante = lista_participantes_buscar_por_id(lista_participantes, id);
    
    if (participante != NULL) {
        lista_participantes_remover(lista_participantes, id);
        printf("\nParticipante removido com sucesso!\n");
    } else {
        printf("\nParticipante não encontrado!\n");
    }
}

// ====== FUNÇÕES PARA GERENCIAMENTO DE INSCRIÇÕES ======

// Função para processar uma nova inscrição
void nova_inscricao(Evento* lista_eventos, ListaParticipantes* lista_participantes, 
                    ListaInscricoes* lista_inscricoes) {
    int idEvento, idParticipante;
    
    printf("\n== Nova Inscrição ==\n");
    printf("ID do evento: ");
    scanf("%d", &idEvento);
    
    Evento* evento = eventos_buscar_por_id(lista_eventos, idEvento);
    if (evento == NULL) {
        printf("\nEvento não encontrado!\n");
        return;
    }
    
    if (evento->vagasDisponiveis <= 0) {
        printf("\nNão há vagas disponíveis para este evento!\n");
        return;
    }
    
    printf("ID do participante: ");
    scanf("%d", &idParticipante);
    
    Participante* participante = lista_participantes_buscar_por_id(lista_participantes, idParticipante);
    if (participante == NULL) {
        printf("\nParticipante não encontrado!\n");
        return;
    }
    
    // Verificar se o participante já está inscrito
    if (inscricao_existe(lista_inscricoes, idParticipante, idEvento)) {
        printf("\nEste participante já está inscrito neste evento!\n");
        return;
    }
    
    // Obter data atual
    char dataAtual[11];
    obter_data_atual(dataAtual);
    
    // Criar e adicionar a inscrição
    Inscricao* nova = inscricao_criar(participante, evento, dataAtual, CONFIRMADA);
    lista_inscricoes_adicionar(lista_inscricoes, nova);
    
    // Atualizar vagas disponíveis
    evento->vagasDisponiveis--;
    
    printf("\nInscrição realizada com sucesso!\n");
}

// Função para cancelar uma inscrição
void cancelar_inscricao(ListaInscricoes* lista_inscricoes) {
    int idEvento, idParticipante;
    
    printf("\n== Cancelar Inscrição ==\n");
    printf("ID do evento: ");
    scanf("%d", &idEvento);
    
    printf("ID do participante: ");
    scanf("%d", &idParticipante);
    
    if (lista_inscricoes_cancelar(lista_inscricoes, idParticipante, idEvento)) {
        printf("\nInscrição cancelada com sucesso!\n");
    } else {
        printf("\nInscrição não encontrada ou já cancelada!\n");
    }
}

// ====== FUNÇÕES PARA RELATÓRIOS ======

// Função para gerar relatório de eventos por categoria
void relatorio_eventos_por_categoria(Evento* lista_eventos) {
    char categoria[50];
    bool encontrado = false;
    
    printf("\n== Relatório: Eventos por Categoria ==\n");
    printf("Informe a categoria: ");
    scanf(" %[^\n]", categoria);
    
    printf("\n=== EVENTOS DA CATEGORIA: %s ===\n", categoria);
    
    Evento* atual = lista_eventos;
    while (atual != NULL) {
        if (strcmp(atual->categoria, categoria) == 0) {
            printf("ID: %d\n", atual->id);
            printf("Nome: %s\n", atual->nome);
            printf("Data: %s\n", atual->data);
            printf("Vagas: %d/%d\n", atual->vagasDisponiveis, atual->capacidade);
            printf("--------------------\n");
            encontrado = true;
        }
        atual = atual->proximo;
    }
    
    if (!encontrado) {
        printf("Nenhum evento encontrado nesta categoria.\n");
    }
}

// Função para gerar relatório de eventos com vagas disponíveis
void relatorio_eventos_com_vagas(Evento* lista_eventos) {
    bool encontrado = false;
    
    printf("\n== Relatório: Eventos com Vagas Disponíveis ==\n");
    
    Evento* atual = lista_eventos;
    while (atual != NULL) {
        if (atual->vagasDisponiveis > 0) {
            printf("ID: %d\n", atual->id);
            printf("Nome: %s\n", atual->nome);
            printf("Categoria: %s\n", atual->categoria);
            printf("Data: %s\n", atual->data);
            printf("Vagas disponíveis: %d\n", atual->vagasDisponiveis);
            printf("--------------------\n");
            encontrado = true;
        }
        atual = atual->proximo;
    }
    
    if (!encontrado) {
        printf("Nenhum evento com vagas disponíveis.\n");
    }
}

// Função para gerar relatório de participantes por evento
void relatorio_participantes_por_evento(ListaInscricoes* lista_inscricoes) {
    int idEvento;
    
    printf("\n== Relatório: Participantes por Evento ==\n");
    printf("ID do evento: ");
    scanf("%d", &idEvento);
    
    lista_inscricoes_por_evento(lista_inscricoes, idEvento);
}

// Função para gerar estatísticas do sistema
void relatorio_estatisticas(Evento* lista_eventos, ListaParticipantes* lista_participantes, 
                           ListaInscricoes* lista_inscricoes) {
    int total_eventos = contar_eventos(lista_eventos);
    
    printf("\n== Relatório: Estatísticas do Sistema ==\n");
    printf("Total de eventos: %d\n", total_eventos);
    printf("Total de participantes: %d\n", lista_participantes->tamanho);
    printf("Total de inscrições: %d\n", lista_inscricoes->tamanho);
    
    // Calcular média de inscrições por evento
    if (total_eventos > 0) {
        float media = (float)lista_inscricoes->tamanho / total_eventos;
        printf("Média de inscrições por evento: %.2f\n", media);
    }
    
    // Encontrar evento com mais inscrições
    if (total_eventos > 0) {
        Evento* atual = lista_eventos;
        int max_inscricoes = 0;
        int id_max = -1;
        char nome_max[100] = "";
        
        while (atual != NULL) {
            int inscricoes = atual->capacidade - atual->vagasDisponiveis;
            if (inscricoes > max_inscricoes) {
                max_inscricoes = inscricoes;
                id_max = atual->id;
                strcpy(nome_max, atual->nome);
            }
            atual = atual->proximo;
        }
        
        if (id_max != -1) {
            printf("Evento com mais inscrições: %s (ID: %d) - %d inscrições\n", 
                   nome_max, id_max, max_inscricoes);
        }
    }
}

// ====== FUNÇÕES DE PERSISTÊNCIA EM CSV ======

// Salvar eventos em CSV
void salvar_eventos_csv(Evento* lista, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) return;
    Evento* atual = lista;
    fprintf(f, "id,nome,descricao,categoria,data,capacidade,vagasDisponiveis\n");
    while (atual) {
        fprintf(f, "%d,\"%s\",\"%s\",\"%s\",%s,%d,%d\n",
            atual->id, atual->nome, atual->descricao, atual->categoria,
            atual->data, atual->capacidade, atual->vagasDisponiveis);
        atual = atual->proximo;
    }
    fclose(f);
}

// Carregar eventos de CSV
Evento* carregar_eventos_csv(const char* filename, int* proximoIdEvento) {
    FILE* f = fopen(filename, "r");
    if (!f) return NULL;
    Evento* lista = NULL;
    char linha[1024];
    fgets(linha, sizeof(linha), f); // header
    int id, capacidade, vagas;
    char nome[100], descricao[500], categoria[50], data[11];
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%d,\"%99[^\"]\",\"%499[^\"]\",\"%49[^\"]\",%10[^,],%d,%d",
            &id, nome, descricao, categoria, data, &capacidade, &vagas);
        Evento* novo = evento_criar(id, nome, descricao, categoria, data, capacidade);
        novo->vagasDisponiveis = vagas;
        lista = eventos_adicionar(lista, novo);
        if (id >= *proximoIdEvento) *proximoIdEvento = id + 1;
    }
    fclose(f);
    return lista;
}

// Salvar participantes em CSV
void salvar_participantes_csv(ListaParticipantes* lista, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) return;
    Participante* atual = lista->inicio;
    fprintf(f, "id,nome,email,telefone\n");
    if (atual) {
        int first = 1;
        do {
            if (!first && atual == lista->inicio) break;
            fprintf(f, "%d,\"%s\",\"%s\",\"%s\"\n",
                atual->id, atual->nome, atual->email, atual->telefone);
            atual = atual->proximo;
            first = 0;
        } while (atual != lista->inicio);
    }
    fclose(f);
}

// Carregar participantes de CSV
void carregar_participantes_csv(ListaParticipantes* lista, const char* filename, int* proximoIdParticipante) {
    FILE* f = fopen(filename, "r");
    if (!f) return;
    char linha[512];
    fgets(linha, sizeof(linha), f); // header
    int id;
    char nome[100], email[100], telefone[20];
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%d,\"%99[^\"]\",\"%99[^\"]\",\"%19[^\"]\"",
            &id, nome, email, telefone);
        Participante* novo = participante_criar(id, nome, email, telefone);
        lista_participantes_adicionar(lista, novo);
        if (id >= *proximoIdParticipante) *proximoIdParticipante = id + 1;
    }
    fclose(f);
}

// Salvar inscrições em CSV
void salvar_inscricoes_csv(ListaInscricoes* lista, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) return;
    fprintf(f, "idParticipante,idEvento,dataInscricao,status\n");
    Inscricao* atual = lista->inicio;
    while (atual) {
        fprintf(f, "%d,%d,%s,%d\n",
            atual->participante->id, atual->evento->id,
            atual->dataInscricao, atual->status);
        atual = atual->proxima;
    }
    fclose(f);
}

// Carregar inscrições de CSV
void carregar_inscricoes_csv(ListaInscricoes* lista, Evento* lista_eventos, ListaParticipantes* lista_participantes, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return;
    char linha[256];
    fgets(linha, sizeof(linha), f); // header
    int idParticipante, idEvento, status;
    char dataInscricao[11];
    while (fgets(linha, sizeof(linha), f)) {
        sscanf(linha, "%d,%d,%10[^,],%d", &idParticipante, &idEvento, dataInscricao, &status);
        Participante* p = lista_participantes_buscar_por_id(lista_participantes, idParticipante);
        Evento* e = eventos_buscar_por_id(lista_eventos, idEvento);
        if (p && e) {
            Inscricao* nova = inscricao_criar(p, e, dataInscricao, (StatusInscricao)status);
            lista_inscricoes_adicionar(lista, nova);
        }
    }
    fclose(f);
}

// ====== FUNÇÃO PRINCIPAL ======

int main() {
    // Troque para o locale UTF-8 do Windows se disponível
    setlocale(LC_ALL, ".UTF8");
    // Inicialização de estruturas
    Evento* lista_eventos = NULL;
    ListaParticipantes* lista_participantes = lista_participantes_criar();
    ListaInscricoes* lista_inscricoes = lista_inscricoes_criar();

    // Contadores para IDs
    int proximoIdEvento = 1;
    int proximoIdParticipante = 1;

    // Carregar dados dos arquivos CSV
    lista_eventos = carregar_eventos_csv("eventos.csv", &proximoIdEvento);
    carregar_participantes_csv(lista_participantes, "participantes.csv", &proximoIdParticipante);
    carregar_inscricoes_csv(lista_inscricoes, lista_eventos, lista_participantes, "inscricoes.csv");

    int opcao, subOpcao;
    
    do {
        exibir_menu_principal();
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1: // Menu de Eventos
                do {
                    exibir_menu_eventos();
                    scanf("%d", &subOpcao);
                    
                    switch (subOpcao) {
                        case 1:
                            adicionar_evento(&lista_eventos, &proximoIdEvento);
                            break;
                        case 2:
                            eventos_listar(lista_eventos);
                            break;
                        case 3:
                            buscar_evento(lista_eventos);
                            break;
                        case 4:
                            editar_evento(lista_eventos);
                            break;
                        case 5:
                            remover_evento(&lista_eventos);
                            break;
                        case 6:
                            {
                                int totalEventos = contar_eventos(lista_eventos);
                                if (totalEventos > 0) {
                                    ordenar_eventos_por_data(lista_eventos, totalEventos);
                                    printf("\nEventos ordenados por data!\n");
                                    eventos_listar(lista_eventos);
                                } else {
                                    printf("\nNenhum evento para ordenar.\n");
                                }
                            }
                            break;
                        case 0:
                            break;
                        default:
                            printf("\nOpção inválida!\n");
                    }
                } while (subOpcao != 0);
                break;
                
            case 2: // Menu de Participantes
                do {
                    exibir_menu_participantes();
                    scanf("%d", &subOpcao);
                    
                    switch (subOpcao) {
                        case 1:
                            adicionar_participante(lista_participantes, &proximoIdParticipante);
                            break;
                        case 2:
                            lista_participantes_listar(lista_participantes);
                            break;
                        case 3:
                            buscar_participante(lista_participantes);
                            break;
                        case 4:
                            editar_participante(lista_participantes);
                            break;
                        case 5:
                            remover_participante(lista_participantes);
                            break;
                        case 0:
                            break;
                        default:
                            printf("\nOpção inválida!\n");
                    }
                } while (subOpcao != 0);
                break;
                
            case 3: // Menu de Inscrições
                do {
                    exibir_menu_inscricoes();
                    scanf("%d", &subOpcao);
                    
                    switch (subOpcao) {
                        case 1:
                            nova_inscricao(lista_eventos, lista_participantes, lista_inscricoes);
                            break;
                        case 2:
                            {
                                int idEvento;
                                printf("\nInforme o ID do evento: ");
                                scanf("%d", &idEvento);
                                lista_inscricoes_por_evento(lista_inscricoes, idEvento);
                            }
                            break;
                        case 3:
                            {
                                int idParticipante;
                                printf("\nInforme o ID do participante: ");
                                scanf("%d", &idParticipante);
                                lista_inscricoes_por_participante(lista_inscricoes, idParticipante);
                            }
                            break;
                        case 4:
                            cancelar_inscricao(lista_inscricoes);
                            break;
                        case 0:
                            break;
                        default:
                            printf("\nOpção inválida!\n");
                    }
                } while (subOpcao != 0);
                break;
                
            case 4: // Menu de Relatórios
                do {
                    exibir_menu_relatorios();
                    scanf("%d", &subOpcao);
                    
                    switch (subOpcao) {
                        case 1:
                            relatorio_eventos_por_categoria(lista_eventos);
                            break;
                        case 2:
                            relatorio_eventos_com_vagas(lista_eventos);
                            break;
                        case 3:
                            relatorio_participantes_por_evento(lista_inscricoes);
                            break;
                        case 4:
                            relatorio_estatisticas(lista_eventos, lista_participantes, lista_inscricoes);
                            break;
                        case 0:
                            break;
                        default:
                            printf("\nOpção inválida!\n");
                    }
                } while (subOpcao != 0);
                break;
                
            case 0: // Sair
                printf("\nSaindo do sistema. Obrigado!\n");
                break;
                
            default:
                printf("\nOpção inválida!\n");
        }
        

        // Limpar o buffer de entrada
        while (getchar() != '\n'); // Limpar o buffer de entrada
        // Limpar a tela
        system("clear || cls"); // Limpar a tela (funciona em sistemas Unix e Windows)
        printf("\n=== VOLTANDO AO MENU PRINCIPAL ===\n");
        // Exibir a lista de eventos após cada operação
        printf("\n=== LISTA DE EVENTOS ===\n");
        printf("Total de eventos: %d\n", contar_eventos(lista_eventos));
        ordenar_eventos_por_data(lista_eventos, contar_eventos(lista_eventos));
        eventos_listar(lista_eventos);
        printf("\n=== LISTA DE PARTICIPANTES ===\n");
        printf("Total de participantes: %d\n", lista_participantes->tamanho);
        lista_participantes_listar(lista_participantes);
        printf("\n=== LISTA DE INSCRIÇÕES ===\n");
        printf("Total de inscrições: %d\n", lista_inscricoes->tamanho);
        if (lista_inscricoes->tamanho > 0) {
            Inscricao* inscricao_atual = lista_inscricoes->inicio;
            while (inscricao_atual != NULL) {
                printf("Participante: %s (ID: %d) - Evento: %s (ID: %d) - Status: %s\n", 
                       inscricao_atual->participante->nome, 
                       inscricao_atual->participante->id, 
                       inscricao_atual->evento->nome, 
                       inscricao_atual->evento->id, 
                       inscricao_atual->status == CONFIRMADA ? "Confirmada" : "Cancelada");
                inscricao_atual = inscricao_atual->proxima;
            }
        } else {
            printf("Nenhuma inscrição registrada.\n");
        }
        printf("\n=== FIM DA LISTA ===\n");


    } while (opcao != 0);

    // Salvar dados nos arquivos CSV antes de sair
    salvar_eventos_csv(lista_eventos, "eventos.csv");
    salvar_participantes_csv(lista_participantes, "participantes.csv");
    salvar_inscricoes_csv(lista_inscricoes, "inscricoes.csv");

    // Liberação de memória
    Evento* evento_atual = lista_eventos;
    while (evento_atual != NULL) {
        Evento* temp = evento_atual;
        evento_atual = evento_atual->proximo;
        evento_destruir(temp);
    }
    
    lista_participantes_destruir(lista_participantes);
    lista_inscricoes_destruir(lista_inscricoes);
    
    return 0;
}