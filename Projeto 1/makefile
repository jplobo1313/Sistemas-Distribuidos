CC = gcc

BIN_DIR = binary
INC_DIR = include
OBJ_DIR = object
SRC_DIR = source

CFLAGS = -Wall -I./$(INC_DIR)

OBJ_DATA = $(OBJ_DIR)/data.o $(OBJ_DIR)/test_data.o
OBJ_ENTRY = $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/test_entry.o
OBJ_TABLE = $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/table.o $(OBJ_DIR)/test_table.o
OBJ_ALL = $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/table.o $(OBJ_DIR)/test_data.o $(OBJ_DIR)/test_entry.o $(OBJ_DIR)/test_table.o

$(BIN_DIR)/out: $(OBJ_ALL)
	$(CC) $(CFLAGS) $(OBJ_DATA) -o $(BIN_DIR)/test_data
	$(CC) $(CFLAGS) $(OBJ_ENTRY) -o $(BIN_DIR)/test_entry
	$(CC) $(CFLAGS) $(OBJ_TABLE) -o $(BIN_DIR)/test_table

$(OBJ_DIR)/test_data.o: $(SRC_DIR)/test_data.c $(INC_DIR)/data.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/test_data.c -o $(OBJ_DIR)/test_data.o

$(OBJ_DIR)/test_entry.o: $(SRC_DIR)/test_entry.c $(INC_DIR)/data.h $(INC_DIR)/entry.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/test_entry.c -o $(OBJ_DIR)/test_entry.o

$(OBJ_DIR)/test_table.o: $(SRC_DIR)/test_table.c $(INC_DIR)/data.h $(INC_DIR)/entry.h $(INC_DIR)/table.h $(INC_DIR)/table-private.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/test_table.c -o $(OBJ_DIR)/test_table.o

$(OBJ_DIR)/data.o: $(SRC_DIR)/data.c $(INC_DIR)/data.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/data.c -o $(OBJ_DIR)/data.o

$(OBJ_DIR)/entry.o: $(SRC_DIR)/entry.c $(INC_DIR)/data.h $(INC_DIR)/entry.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/entry.c -o $(OBJ_DIR)/entry.o

$(OBJ_DIR)/table.o: $(SRC_DIR)/table.c $(INC_DIR)/data.h $(INC_DIR)/entry.h $(INC_DIR)/table.h $(INC_DIR)/table-private.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/table.c -o $(OBJ_DIR)/table.o

valgrind:
	valgrind --leak-check=yes ./$(BIN_DIR)/test_table

clean:
	rm -f $(OBJ_DIR)/*
	rm -f $(BIN_DIR)/*
