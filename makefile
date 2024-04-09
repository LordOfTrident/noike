BIN = bin
OUT = noike

SRC  = $(wildcard src/*.c) $(wildcard src/**/*.c)
DEPS = $(wildcard src/*.h) $(wildcard src/**/*.h)
OBJ  = $(addsuffix .o,$(subst src/,$(BIN)/,$(basename $(SRC))))
DIRS = $(subst src/,$(BIN)/,$(sort $(dir $(wildcard src/*/))))

CSTD   = c99
CLIBS  = -lm -lSDL2 -lSDL2_image -lSDL2_mixer
CFLAGS = -O2 -std=$(CSTD) -Wall -Wextra -Werror -pedantic -Wno-deprecated-declarations -g \
         -D_POSIX_C_SOURCE=200809L

build: $(OUT)

$(OUT): $(DIRS) $(BIN) $(OBJ) $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ) $(CLIBS)

$(BIN)/%/:
	mkdir -p $@

$(BIN)/%.o: src/%.c $(DEPS)
	$(CC) -c $< $(CFLAGS) -o $@

$(BIN):
	mkdir -p $(BIN)

clean: $(BIN)
	rm -r $(BIN)/*
	rm $(OUT)

all:
	@echo build, clean
