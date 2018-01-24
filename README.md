# Sistemas-Distribuidos
Projeto em  4 fases de Sistemas Distribuidos em C, feito em grupo com **@João Lobo** e **@João Ferreira**.

## 1ª Fase
Criação das estruturas data.c, entry.c e table.c

A estrutura **data.c** é uma estrutura de dados genérica.

A estrutura **entry.c** representa uma entrada de uma tabela contendo uma chave, uma data e uma referência para uma próxima entrada.

              Entry 1                              Entry 2
       +-------------------+                +-------------------+ 
       |        key        |                |        key        |
       +-------------------+                +-------------------+ 
       |                   |                |                   |
       |       data        |        +---->  |       data        |        +----> ...
       |                   |        |       |                   |        |
       +-------------------+        |       +-------------------+        |
       |       next -------+--------+       |       next -------+--------+
       +-------------------+                +-------------------+


A estrutura **table.c** corresponde a um array de entradas de dados genéricos. Cada entrada é colocada numa posição especifica na tabela através de uma função de hash. No entanto, se a posição já estiver preenchida por uma entry1, a entry2 correspondente é colocada no final da tabela e o apontador next da entry1 é atualizada para a posição da entry2 na tabela. 


## 2ª Fase
Criação da estrutura message.c utilizada para o encapsulmaneto de envio de dados de um cliente para o servidor.

Implementação do cliente e do servidor e o meio de comunicação entre ambos, network_client.
Para que a comunicação funciona é necessário que ambos o cliente e o utilizador se encontrem na mesma rede local.


## 3ª Fase
Reformatação do código usando o RPC *(Remote Proccedure Calls)*.


## 4ª Fase
Implementação de um servidor secundário que é atualizado sempre que ocorra uma atualização de um valor no servidor primário.

Qualquer cliente contata o servidor primário para realizar atualizações. Caso este não se encontre ativo, então o servidor secundário passa a comportar-se como primário. Quando o antigo servidor primário voltar ao ativo, verifica que foi abaixo e passa a ser primário.

**Operação de Leitura de um Cliente**
```
Cliente             _________+...................+____________________________
                              \                 /
                      pedido   \               /  devolve valor
Servidor Primário   ............\_____________/...............................


Servidor Secundário ..........................................................
```

**Operação de Escrita de um Cliente**
```
Cliente             _________+........................................+____________________________
                              \                                      /
                      escrita  \                                    /  OK / NOK
Servidor Primário   ............\__+...................+___________/...............................
                                    \                 /
                           atualiza  \               /  OK / NOK
Servidor Secundário ..................\_____________/..............................................
```

Quando um servidor secundário volta ao ativo, realiza um pedido de atualização ao servidor primário. Enquanto ocorre a atualização todas as operações por parte de um cliente são bloqueadas, e posteriormente aplicadas
