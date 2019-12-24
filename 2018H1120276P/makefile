CC              = gcc
CFLAGS          = -Wall #-ansi
LFLAGS          = -Wall #-ansi

SENDEROBJS		= sender.o gbn.o
RECEIVEROBJS	= receiver.o gbn.o
BOTHOBJS		= receiver.o sender.o gbn.o
ALLEXEC			= sender receiver

.c.o:
	gcc $(CFLAGS) -c $<
all: $(ALLEXEC)

sender: $(SENDEROBJS)
	$(CC) $(LFLAGS) -o $@ $(SENDEROBJS)

receiver: $(RECEIVEROBJS)
	$(CC) $(LFLAGS) -o $@ $(RECEIVEROBJS)

both: $(BOTHOBJS)
	$(CC) $(LFLAGS) -o $@ $(BOTHOBJS)

clean:
	rm -f *.o $(ALLEXEC)
	rm -f output.txt

realclean: clean
	rm -rf proj1.tar.gz

