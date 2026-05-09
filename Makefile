CC      = gcc
CFLAGS  = -Wall -Wextra -g

SRC    = src
BUILD  = build
BIN    = bin

TARGET = $(BIN)/yuasm

all: $(TARGET)

#bison
$(BUILD)/parser.tab.c $(BUILD)/parser.tab.h: $(SRC)/parser.y
	bison -d $(SRC)/parser.y -o $(BUILD)/parser.tab.c

#flex
$(BUILD)/lex.yy.c: $(SRC)/lexer.l $(BUILD)/parser.tab.h
	flex -o $(BUILD)/lex.yy.c $(SRC)/lexer.l

$(TARGET): $(BUILD)/parser.tab.c $(BUILD)/lex.yy.c $(SRC)/ast.c $(SRC)/unroll.c $(SRC)/main.c
	$(CC) $(CFLAGS) -I$(SRC) -I$(BUILD) -o $(TARGET) $(BUILD)/parser.tab.c $(BUILD)/lex.yy.c $(SRC)/ast.c $(SRC)/unroll.c $(SRC)/main.c

clean:
	rm -f $(TARGET) $(BUILD)/parser.tab.c $(BUILD)/parser.tab.h $(BUILD)/lex.yy.c $(BUILD)/*.o

clear:
	rm -f $(TARGET) $(BUILD)/parser.tab.c $(BUILD)/parser.tab.h $(BUILD)/lex.yy.c $(BUILD)/*.o

run: $(TARGET)
	./$(TARGET) $(FILE)

.PHONY: all clean run
