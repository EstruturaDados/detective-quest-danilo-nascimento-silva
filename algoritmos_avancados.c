#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAMANHO_HASH 30 // Tamanho da tabela hash para armazenar as associações

/* ==========================================================================
   STRUCTS DE DADOS
   ========================================================================== */

/**
 * Estrutura que representa um cômodo da Mansão (Árvore Binária comum).
 */
typedef struct Sala {
    char nome[50];          // Nome da sala/cômodo
    char pista[100];        // Conteúdo da pista contida neste local
    struct Sala* esquerda;  // Ponteiro para o próximo cômodo à esquerda
    struct Sala* direita;   // Ponteiro para o próximo cômodo à direita
} Sala;

/**
 * Estrutura que representa um nó do inventário de pistas (Árvore BST).
 */
typedef struct PistaNode {
    char conteudo[100];           // Texto descritivo da pista coletada
    struct PistaNode* esquerda;   // Ponteiro para pistas alfabeticamente menores
    struct PistaNode* direita;    // Ponteiro para pistas alfabeticamente maiores
} PistaNode;

/**
 * Estrutura que representa uma casinha da nossa Tabela Hash.
 */
typedef struct {
    char chave_pista[100];     // A pista funciona como a CHAVE de busca
    char valor_suspeito[50];   // O suspeito é o VALOR associado
    int ocupado;               // Controla o estado: 0 para vazio, 1 para ocupado
} ElementoHash;

// Declaração global da Tabela Hash para simplificar o acesso pelas funções
ElementoHash tabela_hash[TAMANHO_HASH];

/* ==========================================================================
   PROTÓTIPOS DAS FUNÇÕES (DOCUMENTAÇÃO OBRIGATÓRIA)
   ========================================================================== */
Sala* criarSala(char* nome, char* pista);
void explorarSalas(Sala* raiz);
PistaNode* inserirPista(PistaNode* raiz, const char* pista);
void inserirNaHash(const char* pista, const char* suspeito);
char* encontrarSuspeito(const char* pista);
void verificarSuspeitoFinal(PistaNode* inventarioPistasBST);

// Funções de suporte adicionais (Exibição e Limpeza)
void exibirPistas(PistaNode* raiz);
int contarPistasDoSuspeito(PistaNode* raiz, const char* suspeitoAcusado);
void liberarMansao(Sala* raiz);
void liberarPistasBST(PistaNode* raiz);

/* ==========================================================================
   FUNÇÃO PRINCIPAL (MAIN)
   ========================================================================== */
int main(){
    int i;
    
    // Inicializa todos os estados da tabela hash como vazios (0)
    for (i = 0; i < TAMANHO_HASH; i++) {
        tabela_hash[i].ocupado = 0;
    }

    // ----------------------------------------------------------------------
    // CONFIGURAÇÃO DA TABELA HASH: Vincula cada pista estática a um culpado
    // ----------------------------------------------------------------------
    inserirNaHash("Chave velha caída sob o tapete", "Mordomo");
    inserirNaHash("Faca de cortar carne limpa demais", "Cozinheiro");
    inserirNaHash("Pegadas profundas perto da cerca", "Jardineiro");
    inserirNaHash("Quadro ligeiramente torto na parede", "Mordomo");
    inserirNaHash("Bilhete rasgado escondido no colchao", "Esposa");
    inserirNaHash("Calice com vestigios de po branco", "Cozinheiro");
    inserirNaHash("Xicara de cha ainda morna sobre a mesa", "Esposa");
    inserirNaHash("Cofre entreaberto atras da estante", "Mordomo");

    // ----------------------------------------------------------------------
    // CONFIGURAÇÃO DO MAPA DA MANSÃO: Montagem manual da Árvore Binária
    // ----------------------------------------------------------------------
    Sala* raiz = criarSala("Hall de entrada", "Chave velha caída sob o tapete");
    
    raiz->esquerda = criarSala("Cozinha", "Faca de cortar carne limpa demais");
    raiz->esquerda->esquerda = criarSala("Quintal", "Pegadas profundas perto da cerca");
    raiz->esquerda->direita = criarSala("Corredor", "Quadro ligeiramente torto na parede");
    raiz->esquerda->direita->esquerda = criarSala("Quarto", "Bilhete rasgado escondido no colchao");
    
    raiz->direita = criarSala("Sala de Estar", "Calice com vestigios de po branco");
    raiz->direita->esquerda = criarSala("Cafe", "Xicara de cha ainda morna sobre a mesa");
    raiz->direita->direita = criarSala("Escritorio", "Cofre entreaberto atras da estante");

    // Executa a gameplay
    explorarSalas(raiz);

    // Limpeza final da memória RAM
    liberarMansao(raiz);
    
    return 0;
}

/* ==========================================================================
   IMPLEMENTAÇÃO COMPLETA DAS FUNÇÕES
   ========================================================================== */

/**
 * criarSala() - Aloca dinamicamente na memória uma nova estrutura do tipo Sala,
 * preenchendo o nome do cômodo e copiando a string da pista caso ela exista.
 */
Sala* criarSala(char* nome, char* pista) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));
    
    if (novaSala != NULL) {
        strcpy(novaSala->nome, nome);
        
        if (pista != NULL) {
            strcpy(novaSala->pista, pista);
        } else {
            novaSala->pista[0] = '\0'; 
        }
        
        novaSala->esquerda = NULL;
        novaSala->direita = NULL;
    }
    return novaSala;
}

/**
 * inserirPista() - Adiciona uma nova string de pista de forma ordenada dentro
 * da árvore de busca (BST), usando a função strcmp para decidir esquerda ou direita.
 */
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL) {
        PistaNode* novoNo = (PistaNode*) malloc(sizeof(PistaNode));
        if (novoNo != NULL) {
            strcpy(novoNo->conteudo, pista);
            novoNo->esquerda = NULL;
            novoNo->direita = NULL;
        }
        return novoNo;
    }

    // Se a nova pista for alfabeticamente menor que a atual, vai para a esquerda
    if (strcmp(pista, raiz->conteudo) < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } 
    // Se for maior, vai para a direita
    else if (strcmp(pista, raiz->conteudo) > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    
    return raiz;
}

/**
 * inserirNaHash() - Transforma a string da pista em um índice numérico e armazena
 * a relação pista/suspeito na tabela. Usa sondagem linear simples se houver colisão.
 */
void inserirNaHash(const char* pista, const char* suspeito) {
    int soma = 0;
    int i;
    
    // Função Hash Simples: Soma os caracteres ASCII da string da pista
    for (i = 0; pista[i] != '\0'; i++) {
        soma = soma + pista[i];
    }
    
    // Restringe o índice para caber dentro do tamanho do vetor da nossa tabela
    int indice = soma % TAMANHO_HASH;
    
    // Sondagem Linear: Procura a próxima vaga caso ocorra colisão de índices
    while (tabela_hash[indice].ocupado == 1) {
        indice = (indice + 1) % TAMANHO_HASH;
    }
    
    // Grava as informações na vaga encontrada
    strcpy(tabela_hash[indice].chave_pista, pista);
    strcpy(tabela_hash[indice].valor_suspeito, suspeito);
    tabela_hash[indice].ocupado = 1; // Ativa a flag de ocupação
}

/**
 * encontrarSuspeito() - Realiza uma busca na tabela hash baseando-se na chave da pista.
 * Retorna o ponteiro para o nome do suspeito vinculado àquela pista específica.
 */
char* encontrarSuspeito(const char* pista) {
    int soma = 0;
    int i;
    
    for (i = 0; pista[i] != '\0'; i++) {
        soma = soma + pista[i];
    }
    
    int indice = soma % TAMANHO_HASH;
    int ponto_de_partida = indice;
    
    // Varre as vagas sequencialmente a partir do índice calculado
    do {
        if (tabela_hash[indice].ocupado == 1) {
            // Se o texto da pista guardada for idêntico ao buscado, encontramos!
            if (strcmp(tabela_hash[indice].chave_pista, pista) == 0) {
                return tabela_hash[indice].valor_suspeito;
            }
        } else {
            // Se bater numa posição com estado 0 (nunca usada), o item não existe
            return NULL; 
        }
        
        indice = (indice + 1) % TAMANHO_HASH;
    } while (indice != ponto_de_partida); // Evita loop infinito se a tabela encher
    
    return NULL;
}

/**
 * explorarSalas() - Rastreia a navegação em tempo real do detetive pelos nós,
 * processa os comandos textuais do usuário e alimenta a BST de inventário.
 */
void explorarSalas(Sala* raiz) {
    if (raiz == NULL) return;

    Sala* mov = raiz;
    PistaNode* inventarioPistasBST = NULL; 
    char menu;

    printf("\n==============================================================\n");
    printf("====================== DETECTIVE QUEST =======================");
    printf("\n==============================================================\n");

    do {
        // Se a sala atual contiver uma pista não coletada, recolhe automaticamente
        if (mov->pista[0] != '\0') {
            printf("\n🔍 [INVESTIGACAO]: Voce vasculhou o local e encontrou uma pista!\n");
            printf(">> PISTA COLETADA: \"%s\"\n", mov->pista);
            
            // Insere na Árvore de Busca (BST)
            inventarioPistasBST = inserirPista(inventarioPistasBST, mov->pista);
            
            // Consome a pista limpando a string para evitar re-coletas futuras
            mov->pista[0] = '\0';
        }

        printf("\n==============================================================\n");
        printf("============================ MENU ============================\n");
        printf("# Você está atualmente no(a): [%s]\n\n", mov->nome);
        
        // Remoção completa dos operadores ternários complexos por estruturas condicionais simples
        if (mov->esquerda != NULL) {
            printf("- Digite [e] para ir à Esquerda (%s)\n", mov->esquerda->nome);
        } else {
            printf("- Digite [e] para ir à Esquerda (Porta Trancada)\n");
        }
        
        if (mov->direita != NULL) {
            printf("- Digite [d] para ir à Direita (%s)\n", mov->direita->nome);
        } else {
            printf("- Digite [d] para ir à Direita (Porta Trancada)\n");
        }
        
        printf("- Digite [s] para Finalizar a investigacao e ir ao Julgamento\n");
        printf("==============================================================\n");

        printf(">>>>>: ");
        scanf(" %c", &menu);
        printf("\n");

        switch (menu) {
            case 'e':
            case 'E':
                if (mov->esquerda == NULL) {
                    printf("\n[!] Nao existe comodo nessa direcao!\n");
                } else {
                    mov = mov->esquerda;
                }
                break;
            
            case 'd':
            case 'D':
                if (mov->direita == NULL) {
                    printf("\n[!] Nao existe comodo nessa direcao!\n");
                } else {
                    mov = mov->direita;
                }
                break;
            
            case 's':
            case 'S':
                printf("\n------------------- ENCERRANDO EXPLORACAO -------------------\n");
                menu = 's'; 
                break;
            
            default:
                printf("\n[!] Entrada invalida. Escolha e, d ou s.\n");
                break;
        }

    } while (menu != 's');

    // Apresenta o inventário organizado alfabeticamente pela árvore BST
    printf("\n==============================================================\n");
    printf("============== RELATORIO FINAL: PISTAS DO CASO ==============");
    printf("\n==============================================================\n");
    exibirPistas(inventarioPistasBST);
    printf("==============================================================\n");

    // Dispara a fase do veredito final
    verificarSuspeitoFinal(inventarioPistasBST);

    // Desaloca a árvore de pistas
    liberarPistasBST(inventarioPistasBST);
}

/**
 * verificarSuspeitoFinal() - Abre a seção de acusação formal do suspeito,
 * cruza as pistas coletadas com a tabela hash e calcula o desfecho do mistério.
 */
void verificarSuspeitoFinal(PistaNode* inventarioPistasBST) {
    char acusado[50];
    
    printf("\n==============================================================\n");
    printf("====================== O JULGAMENTO FINAL ====================\n");
    printf("==============================================================\n");
    printf("Chegou o momento crucial. Quem e o assassino?\n");
    printf("Opcoes: Mordomo, Cozinheiro, Jardineiro, Esposa\n");
    printf("Digite exatamente o nome do culpado: ");
    
    scanf("%s", acusado);
    
    // Consulta a árvore de pistas coletadas cruzando com as chaves da tabela hash
    int totalPistasCertas = contarPistasDoSuspeito(inventarioPistasBST, acusado);
    
    printf("\nAnalisando a consistencia das provas contra [%s]...\n", acusado);
    printf("Das pistas que voce coletou, %d apontam diretamente para ele(a).\n", totalPistasCertas);
    
    if (totalPistasCertas >= 2) {
        printf("\n==============================================================\n");
        printf("🎉 EXCELENTE TRABALHO, DETETIVE! VOCE VENCEU!\n");
        printf("As evidencias contra [%s] sao inquestionaveis.\n", acusado);
        printf("Diante das provas irrefutaveis, o criminoso confessou. Caso Solucionado!\n");
        printf("==============================================================\n");
    } else {
        printf("\n==============================================================\n");
        printf("❌ CASO ARQUIVADO! PROVAS INSUFICIENTES.\n");
        printf("Voce nao coletou indicios bastantes (minimo de 2) contra [%s].\n", acusado);
        printf("Sem provas materiais solidas, o suspeito foi liberado pelo juiz.\n");
        printf("==============================================================\n");
    }
}

/**
 * Função recursiva simples que navega pela BST de pistas coletadas,
 * joga cada pista dentro da tabela hash para saber qual suspeito ela aponta
 * e soma o contador se coincidir com o suspeito acusado pelo usuário.
 */
int contarPistasDoSuspeito(PistaNode* raiz, const char* suspeitoAcusado) {
    if (raiz == NULL) {
        return 0;
    }
    
    int pontos = 0;
    
    // Procura na tabela hash quem é o dono dessa pista
    char* suspeitoDaPista = encontrarSuspeito(raiz->conteudo);
    
    if (suspeitoDaPista != NULL) {
        if (strcmp(suspeitoDaPista, suspeitoAcusado) == 0) {
            pontos = 1; // Essa pista valida a acusação do detetive
        }
    }
    
    // Soma de forma simples o ponto atual com os resultados das ramificações
    int soma_esquerda = contarPistasDoSuspeito(raiz->esquerda, suspeitoAcusado);
    int soma_direita = contarPistasDoSuspeito(raiz->direita, suspeitoAcusado);
    
    return pontos + soma_esquerda + soma_direita;
}

void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("• %s\n", raiz->conteudo);
        exibirPistas(raiz->direita);
    }
}

void liberarMansao(Sala* raiz) {
    if (raiz != NULL) {
        liberarMansao(raiz->esquerda);
        liberarMansao(raiz->direita);
        free(raiz);
    }
}

void liberarPistasBST(PistaNode* raiz) {
    if (raiz != NULL) {
        liberarPistasBST(raiz->esquerda);
        liberarPistasBST(raiz->direita);
        free(raiz);
    }
}
