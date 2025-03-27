CC = gcc

CFLAGS = -g -std=gnu99 -Wall -pedantic -Wextra -flto

CPP_FLAGS = -I. -I./../include

BUILD_DIR = build/$(shell uname -s)-$(shell uname -m)
LIB_DIR = lib/$(shell uname -s)-$(shell uname -m)
BIN_DIR = bin/$(shell uname -s)-$(shell uname -m)

TESTS = list_test

all: $(TESTS)

clean:
	rm -rf $(TESTS) build/ lib/ bin/

$(BUILD_DIR) :
	mkdir -p $(BUILD_DIR)

$(LIB_DIR) :
	mkdir -p $(LIB_DIR)

$(BIN_DIR) :
	mkdir -p $(BIN_DIR)


$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CPP_FLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/list_test: $(BUILD_DIR)/list_test.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(BUILD_DIR)/list_test.o -o $(BIN_DIR)/list_test

list_test: $(BIN_DIR)/list_test
	ln -sf $(BIN_DIR)/list_test list_test

