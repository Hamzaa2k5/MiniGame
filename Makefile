CC = gcc
CFLAGS = -Wall -Werror -std=c99

GAME = traffic

SOURCE_FILES = traffic.c 

OBJECT_FILES = $(SOURCE_FILES:.c=.o)

all: $(GAME)

$(GAME): $(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $(GAME) $(OBJECT_FILES)

# Compile each source file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECT_FILES) $(GAME)

# Run
run:
	./$(GAME)

# Edit the source code
edit:
	$(EDITOR) $(SOURCE_FILES)
