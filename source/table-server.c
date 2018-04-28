/*
   Programa que implementa um servidor de uma tabela hash com chainning.
   Uso: table-server <port> <table1_size> [<table2_size> ...]
   Exemplo de uso: ./table_server 54321 10 15 20 25
*/
/*
 Grupo 02
 FC47873 - João Manuel Gil Mendes Ferreira
 FC47845 - João Pedro Pereira Lobo
 FC47806 - Paulo Alexandre Canelas dos Santos
*/
#include <error.h>

#include "inet.h"
#include "table-private.h"
#include "network_client-private.h"

int totalTabelas;
/* Função para preparar uma socket de receção de pedidos de ligação.
*/
int make_server_socket(short port){

    int socket_fd;
    struct sockaddr_in server;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {

        perror("Erro ao criar socket");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0){

        perror("Erro ao fazer bind");
        close(socket_fd);
        return -1;
    }

    if (listen(socket_fd, 0) < 0){

        perror("Erro ao executar listen");
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}


/* Função que recebe uma tabela e uma mensagem de pedido e:
	- aplica a operação na mensagem de pedido na tabela;
	- devolve uma mensagem de resposta com oresultado.
*/
struct message_t *process_message(struct message_t *msg_pedido, struct table_t *tabela){

    struct message_t *msg_resposta = NULL;

	/* Verificar parâmetros de entrada */

    if (msg_pedido != NULL && tabela != NULL){

        int numTab = msg_pedido->table_num - 1;
        int result = -1;
        msg_resposta = (struct message_t *) malloc(sizeof(struct message_t));

        /* Verificar opcode e c_type na mensagem de pedido */
        /* Aplicar operação na tabela */
        /* Preparar mensagem de resposta */
        msg_resposta->table_num = msg_pedido->table_num;

        switch(msg_pedido->opcode){

            case(OC_SIZE):
              if(numTab < totalTabelas){
                result = table_size(&tabela[numTab]);
                msg_resposta->opcode = OC_SIZE+1;
                msg_resposta->c_type = CT_RESULT;
                msg_resposta->content.result = result;
              }
              else{
                msg_resposta->c_type = CT_RESULT;
                msg_resposta->opcode = OC_RT_ERROR;
                msg_resposta->content.result = -1;
              }
            break;

            case(OC_UPDATE):

                if(numTab < totalTabelas){
                  result = table_update(&tabela[numTab], msg_pedido->content.entry->key, msg_pedido->content.entry->value);

                  msg_resposta->opcode = OC_UPDATE+1;
                  msg_resposta->c_type = CT_RESULT;
                  msg_resposta->content.result = result;
                }
                else{
                  msg_resposta->c_type = CT_RESULT;
                  msg_resposta->opcode = OC_RT_ERROR;
                  msg_resposta->content.result = -1;
                }
            break;

            case(OC_GET):

                if(numTab < totalTabelas){
                  if (strcmp(msg_pedido->content.key, "*") == 0) {

                      char **res = table_get_keys(&tabela[numTab]);
                      msg_resposta->opcode = OC_GET+1;
                      msg_resposta->c_type = CT_KEYS;
                      msg_resposta->content.keys = res;
                  }

                  else {

                      struct data_t *res = table_get(&tabela[numTab], msg_pedido->content.key);
                      if (res == NULL) {
                          msg_resposta->content.data = (struct data_t*) malloc(sizeof(struct data_t));
                          msg_resposta->content.data->datasize = 0;
                          msg_resposta->content.data->data = NULL;
                      }
                      else {
                          msg_resposta->content.data = data_dup(res);
                          free(res);
                      }


                      msg_resposta->opcode = OC_GET+1;
                      msg_resposta->c_type = CT_VALUE;
                  }
                }
                else{
                  msg_resposta->c_type = CT_RESULT;
                  msg_resposta->opcode = OC_RT_ERROR;
                  msg_resposta->content.result = -1;
                }
            break;

            case(OC_PUT):
                result = table_put(&tabela[numTab], msg_pedido->content.entry->key, msg_pedido->content.entry->value);
                msg_resposta->opcode = OC_PUT+1;
                msg_resposta->c_type = CT_RESULT;
                msg_resposta->content.result = result;
            break;

            case(OC_COLLS):
              if(numTab < totalTabelas){
                result = collisions(&tabela[numTab]);
                msg_resposta->opcode = OC_COLLS+1;
                msg_resposta->c_type = CT_RESULT;
                msg_resposta->content.result = result;
              }
              else{
                msg_resposta->c_type = CT_RESULT;
                msg_resposta->opcode = OC_RT_ERROR;
                msg_resposta->content.result = -1;
              }
            break;

            default:
                perror("Operacao nao definida");
        }
    }

	return msg_resposta;
}


/* Função "inversa" da função network_send_receive usada no table-client.
   Neste caso a função implementa um ciclo receive/send:

	Recebe um pedido;
	Aplica o pedido na tabela;
	Envia a resposta.
*/
int network_receive_send(int sockfd, struct table_t *tables){

	/* Verificar parâmetros de entrada */
    if (sockfd > 0 && tables != NULL) {

        char *message_resposta, *message_pedido;
        int message_size, msg_size, result;
        struct message_t *msg_pedido, *msg_resposta;

        /* Com a função read_all, receber num inteiro o tamanho da
           mensagem de pedido que será recebida de seguida.*/
        result = read_all(sockfd, (char *) &msg_size, _INT);
        msg_size= ntohl(msg_size);
        /* Verificar se a receção teve sucesso */
        if(result < 0)
            return -1;

        /* Alocar memória para receber o número de bytes da
            mensagem de pedido. */
        message_pedido = (char *) malloc(sizeof(char) * msg_size);

        if(message_pedido == NULL)
            return -1;

        /* Com a função read_all, receber a mensagem de resposta. */
        result = read_all(sockfd, message_pedido, msg_size);

        /* Verificar se a receção teve sucesso */
        if(result < 0) {

            free(message_pedido);
            return -1;
        }

        /* Desserializar a mensagem do pedido */
        msg_pedido = buffer_to_message(message_pedido, msg_size);

        /* Verificar se a desserialização teve sucesso */
        if(msg_pedido == NULL) {

            free(message_pedido);
            return -1;
        }

        /* Processar a mensagem */
        msg_resposta = process_message(msg_pedido, tables);
        if(msg_resposta == NULL){
            free(message_pedido);
            return -1;
        }
        /* Serializar a mensagem recebida */
        message_size = message_to_buffer(msg_resposta, &message_resposta);

        /* Verificar se a serialização teve sucesso */
        if(message_size < 0) {

            free(message_pedido);
            free_message(msg_resposta);
            return -1;

        }

        /* Enviar ao cliente o tamanho da mensagem que será enviada
        logo de seguida
        */
        msg_size = htonl(message_size);
        result = write_all(sockfd, (char *) &msg_size, _INT);

        /* Verificar se o envio teve sucesso */
        if(result < 0) {

            free(message_pedido);
            free(message_resposta);
            free_message(msg_resposta);
            return -1;
        }

        /* Enviar a mensagem que foi previamente serializada */
        result = write_all(sockfd, message_resposta, message_size);

        /* Verificar se o envio teve sucesso */
        if(result < 0) {

            free(message_resposta);
            free(message_pedido);
            free_message(msg_resposta);
            return -1;
        }


        print_message(msg_resposta);

        /* Libertar memória */
        free(message_pedido);
        free(message_resposta);
        free_message(msg_pedido);
        free_message(msg_resposta);
    }

    return 0;
}



int main(int argc, char **argv) {

  	signal(SIGPIPE, SIG_IGN);

	int listening_socket, connsock, result;
	struct sockaddr_in client;
	socklen_t size_client;
	struct table_t *tables;

	if (argc < 3) {
	   printf("Uso: ./server <porta TCP> <port> <table1_size> [<table2_size> ...]\n");
	   printf("Exemplo de uso: ./table-server 54321 10 15 20 25\n");
        return -1;
	}

	if ((listening_socket = make_server_socket(atoi(argv[1]))) < 0)
        return -1;

	/*********************************************************/
	/* Criar as tabelas de acordo com linha de comandos dada :*/
	/*********************************************************/

    int initialArgs = 2;
    int i = 0;
    totalTabelas = (argc - initialArgs);
    tables = (struct table_t *) malloc(sizeof(struct table_t) * totalTabelas);

    if (tables != NULL) {
        while (argc > initialArgs) {
            tables[i] = *(table_create(atoi(argv[initialArgs])));
            initialArgs++;
            i++;
        }
    }

	while ((connsock = accept(listening_socket, (struct sockaddr *) &client, &size_client)) != -1) {

        printf(" * Client is connected!\n");

		while (1) {

			/* Fazer ciclo de pedido e resposta */
			result = network_receive_send(connsock, tables);

            if(result < 0)
                break;

			/* Ciclo feito com sucesso ? Houve erro?
			   Cliente desligou? */
        }
        printf(" * Client disconnected!\n");
    }

    signal(SIGPIPE, SIG_DFL);
}
