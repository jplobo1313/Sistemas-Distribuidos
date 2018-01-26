/*
 Grupo 02
 FC47873 - João Manuel Gil Mendes Ferreira
 FC47845 - João Pedro Pereira Lobo
 FC47806 - Paulo Alexandre Canelas dos Santos
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "entry.h"

/* Função que inicializa os membros de uma entrada na tabela com
   o valor NULL.
 */
void entry_initialize(struct entry_t *entry) {

  if (entry != NULL) {

      entry->key = NULL;
      entry->value = NULL;
      entry->next = NULL;
	}
}


/* Função que duplica um par {chave, valor}.
 */
struct entry_t *entry_dup(struct entry_t *entry) {

	struct entry_t *e = NULL;

	if (entry != NULL) {

        e = (struct entry_t *) malloc(sizeof(struct entry_t));

        if (e != NULL) {

            e->key = strdup(entry->key);

            if (e->key != NULL) {
                if ((e->value = data_dup(entry->value)) == NULL) {

                    free(e->key);
                    e->key = NULL;
                    free(e);
                    e = NULL;
                }

                else {
                    e->next = entry->next;
                }
            }

            else {
                free(e);
                e = NULL;
            }
        }
	}

	return e;
}
