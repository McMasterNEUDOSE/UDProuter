# make file for router application

CC = gcc
TARGETS = router client

all: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -o $@ $<

clean:
	rm $(TARGETS)