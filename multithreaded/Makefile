# make file for router application

CC = gcc
TARGETS = router client

all: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -o $@ $< -lpthread

clean:
	rm $(TARGETS)