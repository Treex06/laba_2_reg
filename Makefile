CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TARGET = sed_simplified
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/sed_operations.o

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

test: $(TARGET)
		echo "Test input:" > test.txt
		echo "line 1 old_text_example" >> test.txt
	echo "line 2 old_text_example" >> test.txt
	echo "regex_example" >> test.txt
	echo "line 4" >> test.txt

		./$(TARGET) test.txt 's/old_text_example/new_text/'
		cat test.txt
		./$(TARGET) test.txt '/regex_example/d'
		cat test.txt
    	./$(TARGET) test.txt 's/^/prefix/'
    	cat test.txt
    	./$(TARGET) test.txt 's/$/suffix/'
    	cat test.txt
