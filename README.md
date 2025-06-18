# Sistema de Gerenciamento de Eventos

## Descrição

Sistema completo de gerenciamento de eventos desenvolvido em linguagem C, que permite o cadastro e controle de eventos, participantes e inscrições. O sistema utiliza estruturas de dados dinâmicas e algoritmos de ordenação para fornecer uma solução robusta e eficiente.

## Funcionalidades Principais

### Gerenciamento de Eventos

- Cadastro de novos eventos com informações completas
- Listagem de todos os eventos cadastrados
- Busca de eventos por ID
- Edição de informações dos eventos
- Remoção de eventos do sistema
- Ordenação automática por data usando algoritmo QuickSort

### Gerenciamento de Participantes

- Cadastro de novos participantes
- Listagem de participantes (estrutura circular)
- Busca de participantes por ID
- Edição de dados dos participantes
- Remoção de participantes do sistema

### Gerenciamento de Inscrições

- Criação de novas inscrições
- Verificação automática de disponibilidade de vagas
- Prevenção de inscrições duplicadas
- Cancelamento de inscrições
- Listagem de inscrições por evento ou participante

### Sistema de Relatórios

- Eventos por categoria
- Eventos com vagas disponíveis
- Participantes por evento
- Estatísticas gerais do sistema

### Persistência de Dados

- Salvamento automático em arquivos CSV
- Carregamento de dados na inicialização
- Backup automático das informações

## Arquitetura do Sistema

### Estruturas de Dados Utilizadas

#### 1. Estrutura Evento

```c
typedef struct evento {
    int id;
    char nome[100];
    char descricao[500];
    char categoria[50];
    char data[11];
    int capacidade;
    int vagasDisponiveis;
    struct evento* proximo;
} Evento;
```

#### 2. Estrutura Participante

```c
typedef struct participante {
    int id;
    char nome[100];
    char email[100];
    char telefone[20];
    struct participante* proximo;
} Participante;
```

#### 3. Estrutura Inscrição

```c
typedef struct inscricao {
    Participante* participante;
    Evento* evento;
    char dataInscricao[11];
    StatusInscricao status;
    struct inscricao* proxima;
} Inscricao;
```

### Tipos de Listas Implementadas

1. Lista Ligada Simples (Eventos)
2. Lista Circular (Participantes)
3. Lista Ligada Simples (Inscrições)

## Compilação e Execução

### Pré-requisitos

- Compilador GCC ou similar
- Sistema operacional: Windows, Linux ou macOS

### Compilação

```bash
gcc -o sistema_eventos main.c
```

### Execução

```bash
./sistema_eventos
```

## Arquivos do Sistema

### Arquivos de Código

- `main.c` - Código fonte principal do sistema

### Arquivos de Dados (CSV)

- `eventos.csv`
- `participantes.csv`
- `inscricoes.csv`

### Estrutura dos Arquivos CSV

#### eventos.csv

```
id,nome,descricao,categoria,data,capacidade,vagasDisponiveis
1,"Workshop de C","Curso básico de programação","Tecnologia",2024-12-15,30,25
```

#### participantes.csv

```
id,nome,email,telefone
1,"João Silva","joao@email.com","(11)99999-9999"
```

#### inscricoes.csv

```
idParticipante,idEvento,dataInscricao,status
1,1,2024-11-20,0
```

## Como Usar o Sistema

### Menu Principal

```
===== SISTEMA DE GERENCIAMENTO DE EVENTOS =====
1. Gerenciar Eventos
2. Gerenciar Participantes
3. Gerenciar Inscrições
4. Relatórios
0. Sair
```

### Fluxo de Uso Recomendado

1. Cadastrar Eventos
2. Cadastrar Participantes
3. Criar Inscrições
4. Gerar Relatórios

### Exemplos de Operações

#### Cadastrar um Evento

1. Menu Principal → Opção 1 (Gerenciar Eventos)
2. Menu Eventos → Opção 1 (Adicionar Evento)
3. Preencher dados solicitados
4. Evento criado com ID único

#### Fazer uma Inscrição

1. Menu Principal → Opção 3 (Gerenciar Inscrições)
2. Menu Inscrições → Opção 1 (Nova Inscrição)
3. Informar ID do evento e ID do participante
4. Sistema verifica disponibilidade automaticamente

## Recursos Técnicos Avançados

### Gerenciamento de Memória

- Alocação Segura
- Liberação Controlada
- Destruição de Estruturas

### Algoritmos Implementados

#### QuickSort para Ordenação

```c
void quicksort_eventos_por_data(Evento eventos[], int baixo, int alto)
```

- Complexidade: O(n log n) no caso médio
- Uso: Ordenação automática de eventos por data

### Validações e Controles

1. Verificação de Duplicatas
2. Controle de Vagas
3. Validação de IDs
4. Estados de Inscrição

### Recursos de Interface

- Menus Hierárquicos
- Limpeza de Tela
- Mensagens Informativas
- Listas Automáticas

## Relatórios Disponíveis

### 1. Eventos por Categoria

Lista todos os eventos de uma categoria específica

### 2. Eventos com Vagas

Mostra apenas eventos com vagas disponíveis

### 3. Participantes por Evento

Lista de inscritos em um evento específico

### 4. Estatísticas Gerais

Dashboard com métricas importantes

## Tratamento de Erros

### Validações Implementadas

- Verificação de alocação de memória
- Validação de IDs inexistentes
- Controle de capacidade de eventos
- Prevenção de inscrições duplicadas
- Verificação de arquivos CSV

### Mensagens de Erro Claras

- Participante/Evento não encontrado
- Sem vagas disponíveis
- Inscrição já existe
- Dados inválidos

## Detalhes de Implementação

### Características das Estruturas de Dados

#### Lista Ligada de Eventos

- Vantagem: Crescimento dinâmico
- Operações: O(1) inserção, O(n) busca

#### Lista Circular de Participantes

- Vantagem: Navegação contínua
- Operações: O(n) busca, O(1) inserção

#### Sistema de Status de Inscrições

```c
typedef enum StatusInscricao {
    CONFIRMADA,
    PENDENTE,
    CANCELADA
} StatusInscricao;
```

### Persistência de Dados

#### Formato CSV

- Compatível com Excel
- Legível e portátil

#### Estratégia de Salvamento

- Automático ao sair
- Seguro com verificação de abertura
- Carregamento automático

## Possíveis Melhorias Futuras

### Funcionalidades Avançadas

- Interface gráfica (GUI)
- Banco de dados SQL
- Sistema de notificações
- Relatórios em PDF
- API REST
- Sistema de autenticação

### Otimizações Técnicas

- Árvores balanceadas
- Cache de consultas
- Compressão de dados
- Backup automático
- Log de operações

## Créditos

Sistema desenvolvido com base no relatório técnico de:

- Romulo Festa
- Ahmad Moussa
- Ali Ataya

## Suporte

Para dúvidas ou sugestões, consulte a documentação do código ou entre em contato com a equipe de desenvolvimento.

**Versão**: 1.0\
**Data**: 2024\
**Linguagem**: C (C99 Standard)\
**Licença**: Open Source


