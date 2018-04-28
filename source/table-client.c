/*
	Programa cliente para manipular tabela de hash remota.
	Os comandos introduzido no programa não deverão exceder
	80 carateres.

	Uso: table-client <ip servidor>:<porta servidor>
	Exemplo de uso: ./table_client 10.101.148.144:54321
*/
/*
 Grupo 02
 FC47873 - João Manuel Gil Mendes Ferreira
 FC47845 - João Pedro Pereira Lobo
 FC47806 - Paulo Alexandre Canelas dos Santos
*/
#include "network_client-private.h"

char **create_tokens(char *frase, int limiteEspacos) {

	char *buffer = (char *) malloc(sizeof(char) * 81);
	char **result = (char **) malloc(sizeof(char *) * (limiteEspacos + 1));

	int i;
	int indice = 0;
	int contador = 0;

	for (i = 0; i < limiteEspacos; i++) {

		while (*(frase + contador) != ' ' && contador < strlen(frase)) {

			*(buffer + indice) = *(frase + contador);
			contador++;
			indice++;
		}

		*(buffer + indice) = '\0';
		result[i] = strdup(buffer);
		indice = 0;
		contador++;
	}

	result[i] = strdup(frase + contador);

    free(buffer);
	return result;
}

void free_tokens(char **teste, int limiteEspacos) {
    int i;

    for (i = 0; i < limiteEspacos; i++)
        free(teste[i]);

    free(teste);
}

int main(int argc, char **argv){

	signal(SIGPIPE, SIG_IGN);

    struct server_t *server;
	char input[81];
	struct message_t *msg_out, *msg_resposta;

	/* Testar os argumentos de entrada */
	if (argc < 2) {

		perror("Valores de entrada invalidos");
		return -1;
	}

	// PERGUNTAR SE EH NECESSARIO VERIFICAR SE O PARAMETRO DE ENTRADA PODE SER INVALIDO ????????????????
    //TEMOS DE VER ver se eh numero e se ta em range


	/* Usar network_connect para estabelcer ligação ao servidor */
	server = network_connect(argv[1]);

	if (server == NULL) {

		perror("Impossibilidade na criacao do servidor");
		return -1;
	}

	/* Fazer ciclo até que o utilizador resolva fazer "quit" */
	strcpy(input, "inicio");

 	while (strcmp(input, "quit") != 0) {

		printf(">>> "); // Mostrar a prompt para inserção de comando

		/* Receber o comando introduzido pelo utilizador
		   Sugestão: usar fgets de stdio.h
		   Quando pressionamos enter para finalizar a entrada no
		   comando fgets, o carater \n é incluido antes do \0.
		   Convém retirar o \n substituindo-o por \0.
		*/
		fgets(input, BUFFER_SIZE, stdin);
		input[strlen(input) - 1] = '\0';

		/* Verificar se o comando foi "quit". Em caso afirmativo
		   não há mais nada a fazer a não ser terminar decentemente.
		 */

		if (strcmp(input, "quit") != 0) {

            msg_out = (struct message_t *) malloc(sizeof(struct message_t));

            if (msg_out != NULL) {
                // SE O STRDUP DER MAL -> VER DEPOIS
                char **comando = create_tokens(input, 1);

                // Verificar qual o comando;
                // Preparar msg_out;
                if(strcmp(comando[0], "put") == 0) {

                    char **dados = create_tokens(comando[1], 2);
                    msg_out->opcode = OC_PUT;
                    msg_out->c_type = CT_ENTRY;
                    msg_out->table_num = atoi(dados[0]);

                    struct entry_t *e = (struct entry_t *) malloc(sizeof(struct entry_t));

                    if (e != NULL) {
                        entry_initialize(e);
                        e->key = strdup(dados[1]);
                        e->value = data_create2(strlen(dados[2]), dados[2]);
                        msg_out->content.entry = e;
                    }

                    free_tokens(dados, 3);
                }

                else if (strcmp(comando[0], "get") == 0) {

                    char **dados = create_tokens(comando[1], 1);

                    msg_out->opcode = OC_GET;
                    msg_out->c_type = CT_KEY;
                    msg_out->table_num = atoi(dados[0]);
                    msg_out->content.key = strdup(dados[1]);
                    free_tokens(dados, 2);
	            }

                else if (strcmp(comando[0], "update") == 0) {

                    char **dados = create_tokens(comando[1], 2);

                    msg_out->opcode = OC_UPDATE;
                    msg_out->c_type = CT_ENTRY;
                    msg_out->table_num = atoi(dados[0]);

                    struct entry_t *e = (struct entry_t *) malloc(sizeof(struct entry_t));

                    if (e != NULL) {
                        entry_initialize(e);
                        e->key = strdup(dados[1]);
                        e->value = data_create2(strlen(dados[2]), dados[2]);
                        msg_out->content.entry = e;
                    }

                    free_tokens(dados, 3);
	            }

                else if (strcmp(comando[0], "size") == 0) {

                    msg_out->opcode = OC_SIZE;
					msg_out->c_type = 0;
                    msg_out->table_num = atoi(comando[1]);
	            }

                else if (strcmp(comando[0], "collisions") == 0){
                    msg_out->opcode = OC_COLLS;
					msg_out->c_type = 0;
                    msg_out->table_num = atoi(comando[1]);
                }

                else {
					perror("O comando inserido eh invalido");
					free(msg_out);
					continue;
								}
				free_tokens(comando, 2);
			}

            /*	Usar network_send_receive para enviar msg_out para
                o server e receber msg_resposta.
            */
			msg_resposta = network_send_receive(server, msg_out);

			print_message(msg_resposta);

            free_message(msg_resposta);
            free_message(msg_out);
		}
	}

	signal(SIGPIPE, SIG_DFL);
  	return network_close(server);
}
