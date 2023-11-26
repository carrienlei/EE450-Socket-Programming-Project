TARGETS = serverM serverS serverL serverH client
CC = g++ 
CFLAGS= -Wall -g  # error flags
all: $(TARGETS) 
clean:
	rm -f $(TARGETS) 
%: %.cpp
	$(CC) -o $@ $<
