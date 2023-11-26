TARGETS = serverM serverS serverL serverH client
CC = g++ 
CFLAGS= -Wall -g  # error flag
all: $(TARGETS) 
clean:
	rm -f $(TARGETS) 
%: %.cpp
	$(CC) -o $@ $<