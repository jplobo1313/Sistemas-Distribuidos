#include "network_client-private.h"

#include <stdlib.h>
/*
 Grupo 02
 FC47873 - João Manuel Gil Mendes Ferreira
 FC47845 - João Pedro Pereira Lobo
 FC47806 - Paulo Alexandre Canelas dos Santos
*/
int write_all(int sock, char *buf, int len){

	int bufsize = len;

	while (len > 0) {

		int res = write(sock, buf, len);

        if (res == 0)
            return res;

		if (res < 0) {
			if (errno == EINTR)
                continue;
			perror("write failed:");
			return res;
		}

		buf += res;
		len -= res;
	}

	return bufsize;
}


int read_all(int sock, char *buf, int len){

	int bufsize = len;

	while(len > 0) {

		int res = read(sock, buf, len);

        if (res == 0)
            return res;

		if(res < 0) {
			if(errno == EINTR) continue;
			perror("read failed:");
			return res;
		}

		buf += res;
		len -= res;
	}

	return bufsize;
}

void print_message(struct message_t *msg) {
    int i;

    printf("\n----- MESSAGE -----\n");

    printf("Tabela número: %d\n", msg->table_num);
    printf("opcode: %d, c_type: %d\n", msg->opcode, msg->c_type);
    switch(msg->c_type) {
        case CT_ENTRY:{
            printf("key: %s\n", msg->content.entry->key);
            printf("datasize: %d\n", msg->content.entry->value->datasize);
        }break;
        case CT_KEY:{
            printf("key: %s\n", msg->content.key);
        }break;
        case CT_KEYS:{
            for(i = 0; msg->content.keys[i] != NULL; i++) {
                printf("key[%d]: %s\n", i, msg->content.keys[i]);
            }
        }break;
        case CT_VALUE:{
            printf("datasize: %d\n", msg->content.data->datasize);
        }break;
        case CT_RESULT:{
            printf("result: %d\n", msg->content.result);
        }break;
        case OC_RT_ERROR:{
            printf("result: %d\n", msg->content.result);
        };
    }
    printf("-------------------\n");
}

struct server_t *network_connect(const char *address_port) {

	struct server_t *server = malloc(sizeof(struct server_t));

	/* Verificar parâmetro da função e alocação de memória */
	if (server != NULL) {

        if (address_port == NULL) {

            free(server);
            return NULL;
		}

		if ((server->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

			perror("Erro ao criar socket TCP");
			free(server);
			return NULL;
		}

		char *dup = strdup(address_port);

		char *token = strtok(dup, ":");
		char *ip_adress = strdup(token);

		token = strtok(NULL, ":");
		int port = atoi(token);

		if (port == 0) {
			perror("Numero de porto invalido");
			free(dup);
			free(ip_adress);
			network_close(server);
			return NULL;
		}

        // Preencher estrutura struct sockaddr_in com dados do endereço do servidor.
		server->server.sin_family = AF_INET;
		server->server.sin_port = htons(port);

		// Criar a socket
		if (inet_pton(AF_INET, ip_adress, &(server->server.sin_addr)) < 1) {

			perror("Erro ao converter IP\n");
			network_close(server);
			return NULL;
		}

        free(dup);
		free(ip_adress);

        // Estabelecer ligacao
		if (connect(server->sockfd, (struct sockaddr *) &(server->server), sizeof(server->server)) < 0) {
			perror("Erro ao conectar-se ao servidor");
			network_close(server);
			return NULL;
		}
	}

	/* Se a ligação não foi estabelecida, retornar NULL */

	return server;
}

struct message_t *network_send_receive(struct server_t *server, struct message_t *msg) {

	char *message_out, *message_in;
	int message_size, msg_size, result;
	struct message_t *msg_resposta;

	/* Verificar parâmetros de entrada */
	if (server != NULL && msg != NULL) {

		/* Serializar a mensagem recebida */

        message_size = message_to_buffer(msg, &message_out);

		/* Verificar se a serialização teve sucesso */
		if(message_size > 0){

			/* Enviar ao servidor o tamanho da mensagem que será enviada
			   logo de seguida
			*/
			msg_size = htonl(message_size);
		 	result = write_all(server->sockfd, (char *) &msg_size, _INT);

			/* Verificar se o envio teve sucesso */
			if (result < 0){
				free(message_out);
				return NULL;
			}


			/* Enviar a mensagem que foi previamente serializada */
			result = write_all(server->sockfd, message_out, message_size);

			/* Verificar se o envio teve sucesso */
			if (result < 0){
				free(message_out);
				return NULL;
			}


			/* De seguida vamos receber a resposta do servidor:
			Com a função read_all, receber num inteiro o tamanho da
			mensagem de resposta.
			*/
			result = read_all(server->sockfd, (char *) &msg_size, _INT);
			msg_size = ntohl(msg_size);
			if (result < 0){
				free(message_out);
				return NULL;
			}


			/* Alocar memória para receber o número de bytes da
			mensagem de resposta.
			Com a função read_all, receber a mensagem de resposta.
			*/
			message_in = (char *) malloc(sizeof(char) * msg_size);

			if (message_in == NULL){
				free(message_out);
				return NULL;
			}


			result = read_all(server->sockfd, message_in, msg_size);

			if (result < 0){
				free(message_out);
				free(message_in);
				return NULL;
			}


			/* Desserializar a mensagem de resposta */
			msg_resposta = buffer_to_message(message_in, msg_size);
			/* Verificar se a desserialização teve sucesso */
			if (msg_resposta == NULL){
				free(message_in);
				free(message_out);
				return NULL;
			}


			/* Libertar memória */
			free(message_in);
			free(message_out);
		}
	}

	return msg_resposta;
}

int network_close(struct server_t *server) {

	int result = -1;

    /* Verificar parâmetros de entrada */
	if (server != NULL) {

		/* Terminar ligação ao servidor */
		result = close(server->sockfd);

		free(server);
	}

	return result;
}
