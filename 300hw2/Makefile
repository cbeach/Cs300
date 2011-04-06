
OBJECTS :=\
	main.o\
	mailsender.o\
	parser.o

SOURCE :=\
	main.cc\
	mailsender.cc\
	parser.cc
	



CC := g++
HEADERFILES := -I./
LDFLAGS := -o
CPFLAGS := -c -g
NAME := sender

INCLUDES :=

all: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(NAME) $(INCLUDES)

$(OBJECTS): $(SOURCE)
	$(CC) $(HEADERFILES) $(SOURCE) $(CPFLAGS)

clean:
	rm -f *.o
	
