/*
 Grupo 02
 FC47873 - João Manuel Gil Mendes Ferreira
 FC47845 - João Pedro Pereira Lobo
 FC47806 - Paulo Alexandre Canelas dos Santos
*/
#ifndef _TABLE_PRIVATE
#define _TABLE_PRIVATE

#include "data.h"
#include "entry.h"
#include "table.h"
#include "string.h"

struct table_t {
	struct entry_t *entradas;
	int size;
	int tableSize;
	int lastInsert;
};

int hash(char *key, int tableSize);

#endif
