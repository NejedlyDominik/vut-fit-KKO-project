CC=g++
CFLAGS=-std=c++17 -Wall -Wextra -Werror -pedantic -O3
SRC_FILES=main.cpp args.cpp io.cpp model.cpp rle.cpp
HEADER_FILES=args.h io.h model.h rle.h
OBJECT_FILES=main.o args.o io.o model.o rle.o
BIN=huff_codec
PACK=xnejed09.zip

.PHONY: all pack clean clean-pack

all: $(BIN)

$(BIN): $(HEADER_FILES) $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) -o $@

$(OBJECT_FILES): %.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

pack: $(PACK)

$(PACK): $(SRC_FILES) $(HEADER_FILES) Makefile
	zip -r $@ $^

clean:
	rm -f $(OBJECT_FILES) $(BIN)

clean-pack:
	rm -f $(PACK)
