/*
 Grupo 02
 FC47873 - João Manuel Gil Mendes Ferreira
 FC47845 - João Pedro Pereira Lobo
 FC47806 - Paulo Alexandre Canelas dos Santos
*/
#include <stdio.h>
#include <stdlib.h>
#include "table-private.h"

/* Função para criar/inicializar uma nova tabela hash, com n
 * linhas(n = módulo da função hash)
 */
struct table_t *table_create(int n) {

    struct table_t *result = NULL;

    if (n > 0) {

        result = (struct table_t *) malloc(sizeof(struct table_t));

        if (result != NULL) {

            result->entradas = (struct entry_t *) malloc(sizeof(struct entry_t) * n);

            if (result->entradas == NULL) {

                free(result);
                result = NULL;
            }

            else {

                result->size = 0;
                result->tableSize = n;
                result->lastInsert = n - 1;

                int i;

                for (i = 0; i < n; i++)
                    entry_initialize(&(result->entradas[i]));
            }
        }
    }

    return result;
}

/* Libertar toda a memória ocupada por uma tabela.
 */
void table_destroy(struct table_t *table) {

    if (table != NULL) {

        int i;

        for (i = 0; i < table->tableSize; i++)
            if (table->entradas[i].key != NULL) {

                free(table->entradas[i].key);
                // Se nao for preciso - erase
                table->entradas[i].key = NULL;

                data_destroy(table->entradas[i].value);
            }

        free(table->entradas);
        free(table);
    }
}

/* Função para adicionar um par chave-valor na tabela.
 * Os dados de entrada desta função deverão ser copiados.
 * Devolve 0 (ok) ou -1 (out of memory, outros erros)
 */
int table_put(struct table_t *table, char *key, struct data_t *value){

    int result = -1;

    if( table != NULL && key != NULL && value != NULL && table->size < table->tableSize) {

        int hashcode = hash(key, table->tableSize);

        struct entry_t *curr = &(table->entradas[hashcode]);

        // Ja existe alguma coisa
        if (curr->key != NULL) {
            
            // Procura de um noh que tenha o proximo livre ou que ja tenha a chave na table
            while (curr->next != NULL && strcmp(curr->key, key) != 0) {
                curr = curr->next;
            }

            if (curr->next == NULL && strcmp(curr->key, key) != 0) {

                // Pesquisa de baixo para cima
                while (table->entradas[table->lastInsert].key != NULL)
                    table->lastInsert--;

                // Chegamos a uma posicao onde lastInsert estah numa posicao vazia
                table->entradas[table->lastInsert].key = strdup(key);
                table->entradas[table->lastInsert].value = data_dup(value);

                if (table->entradas[table->lastInsert].value == NULL) {

                    free(table->entradas[table->lastInsert].key);
                }

                else if (table->entradas[table->lastInsert].key == NULL) {

                    free(table->entradas[table->lastInsert].value);
                }

                else {

                    table->entradas[table->lastInsert].next = NULL;
                    curr->next = &(table->entradas[table->lastInsert]);

                    table->lastInsert--;
                    table->size++;
                    result = 0;
                }
            }
        }
        
        // Nao existe nada, metemos la para dentro
        else {
            table->entradas[hashcode].key = strdup(key);
            table->entradas[hashcode].value = data_dup(value);
            table->entradas[hashcode].next = NULL;
            
            // Verificar se a duplicacao do value deu erro
            if (table->entradas[hashcode].value == NULL) {

                free(table->entradas[hashcode].key);
            }

            // Verificar se a duplicacao da key deu erro
            else if (table->entradas[hashcode].key == NULL) {

                free(table->entradas[hashcode].value);
            }

            // Se tiver corrido tudo bem
            else {

                if (table->lastInsert == hashcode)
                    table->lastInsert--;

                table->size++;
                result = 0;
            }
        }
    }

    return result;
}

/* Função para substituir na tabela, o valor associado à chave key.
 * Os dados de entrada desta função deverão ser copiados.
 * Devolve 0 (OK) ou -1 (out of memory, outros erros)
 */
int table_update(struct table_t *table, char *key, struct data_t *value) {
   
    int result = -1;

    if (table != NULL && key != NULL && value != NULL) {

        int hashcode = hash(key, table->tableSize);

        struct entry_t *curr = &(table->entradas[hashcode]);

        while (curr != NULL && strcmp(curr->key, key) != 0)
            curr = curr->next;

        if (curr != NULL) {
            
          data_destroy(curr->value);
          curr->value = data_dup(value);
          result = curr->value == NULL ? -1 : 0;
        }
    }
   
    return result;
}

/* Função para obter da tabela o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser libertados
 * no contexto da função que chamou table_get.
 * Devolve NULL em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key) {

    struct data_t *result = NULL;

    if (table != NULL && key != NULL) {

        int hashcode = hash(key, table->tableSize);

        struct entry_t *curr = &(table->entradas[hashcode]);

        while (curr != NULL && curr->key != NULL && strcmp(curr->key, key) != 0)
            curr = curr->next;

        if (curr != NULL && curr->key != NULL)
            result = data_dup(curr->value);
    }

    return result;
}

/* Devolve o número de elementos na tabela.
 */
int table_size(struct table_t *table) {

    return table == NULL ? -1 : table->size;
}

/* Devolve um array de char * com a cópia de todas as keys da tabela,
 * e um último elemento a NULL.
 */
 char **table_get_keys(struct table_t *table) {

    char **result = NULL;

    if (table != NULL) {

        result = (char **) malloc((table->size + sizeof(char)) * sizeof(char *));

        if (result != NULL) {

            int i;
            int count = 0;

            for (i = 0; i < table->tableSize; i++)
                if (table->entradas[i].key != NULL) {
                    result[count] = strdup(table->entradas[i].key);
                    // Caso uma duplicacao deh erro, vamos apagar tudo o que fizemos
                    if(result[count] == NULL) {

                        count--;

                        while(count >= 0) {

                            free(result[count]);
                            count--;
                        }

                        free(result);
                        result = NULL;
                        break;
                    }

                    count++;
                }

            result[table->size] = NULL;
        }
    }

    return result;
}

/* Liberta a memória alocada por table_get_keys().
 */
void table_free_keys(char **keys) {

    int i = 0;

    for( i = 0; keys[i] != NULL; i++)
        free(keys[i]);

    free(keys);
}

int hash(char *key, int tableSize) {

    int length = strlen(key);
    int sum = 0;

    if (length <= 4) {
        int i;

        for (i = 0; i < length; i++) {
            sum += *(key+i);
        }
    }
   
    else
        sum = *(key) + *(key + 1) + *(key + length - 2) + *(key+length - 1);

    return sum % tableSize;
}
