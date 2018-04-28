/*
 Grupo 02
 FC47873 - João Manuel Gil Mendes Ferreira
 FC47845 - João Pedro Pereira Lobo
 FC47806 - Paulo Alexandre Canelas dos Santos
*/
#include <stdlib.h>
#include <string.h>

#include "data.h"

struct data_t *data_create(int size) {

    struct data_t *d = NULL;

    if(size > 0){

        d = (struct data_t *) malloc(sizeof(struct data_t));

        if(d != NULL) {

            d->datasize = size;
            d->data = malloc(size);

            if(d->data == NULL) {

                free(d);
                d = NULL;
            }
        }
    }

    return d;
}

struct data_t *data_create2(int size, void * data) {

    struct data_t *d2 = NULL;

    if (data != NULL) {

        d2 = data_create(size);

        if (d2 != NULL)
            memcpy(d2->data, data, size);
	}

    return d2;
}

void data_destroy(struct data_t *data) {

    if (data!=NULL) {

        free(data->data);
        free(data);
    }
}

struct data_t *data_dup(struct data_t *data) {

    struct data_t *d = NULL;

    if (data != NULL) {

        d = data_create2(data->datasize, data->data);
    }

    return d;
}
