CC=g++
CFLAGS=-std=c++20 -Wall -Wextra -Werror -pedantic -O3 #-DSTATS
SRC_FILES=main.cpp args.cpp io.cpp model.cpp rle.cpp huffman.cpp compress.cpp
HEADER_FILES=args.h io.h model.h rle.h huffman.h compress.h
OBJECT_FILES=main.o args.o io.o model.o rle.o huffman.o compress.o
BIN=huff_codec
PACK=xnejed09.zip

.PHONY: all pack clean clean-pack

all: $(BIN)

$(BIN): $(HEADER_FILES) $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) -o $@

$(OBJECT_FILES): %.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

pack: $(PACK)

$(PACK): $(SRC_FILES) $(HEADER_FILES) Makefile KKO_project_doc.pdf
	zip -r $@ $^

clean:
	rm -f $(OBJECT_FILES) $(BIN)

clean-pack:
	rm -f $(PACK)
