
# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# If you are running on SunOS 5.x machines, uncomment the next line
GCCDIR = /auto/home-scf-22/csci402/testgnu/decstation-ultrix/bin/
LDFLAGS = -T newscript -N
ASFLAGS = -mips1
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)


CC = $(GCCDIR)gcc
AS = $(GCCDIR)as
LD = $(GCCDIR)ld
CPP = $(GCCDIR)cpp

INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

all: halt matmult sort test0 testfiles locktest orderinglocktest forktest exectest exectest2 testlockcondition broadcasttest createdestroytest signaltest waittest monitortest

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

testfiles.o: testfiles.c
	$(CC) $(CFLAGS) -c testfiles.c
testfiles: testfiles.o start.o
	$(LD) $(LDFLAGS) start.o testfiles.o -o testfiles.coff
	../bin/coff2noff testfiles.coff testfiles


orderinglocktest.o: orderinglocktest.c
	$(CC) $(CFLAGS) -c orderinglocktest.c
orderinglocktest: orderinglocktest.o start.o
	$(LD) $(LDFLAGS) start.o orderinglocktest.o -o orderinglocktest.coff
	../bin/coff2noff orderinglocktest.coff orderinglocktest

forktest.o: forktest.c
	$(CC) $(CFLAGS) -c forktest.c
forktest: forktest.o start.o
	$(LD) $(LDFLAGS) start.o forktest.o -o forktest.coff
	../bin/coff2noff forktest.coff forktest


locktest.o: locktest.c
	$(CC) $(CFLAGS) -c locktest.c
locktest: locktest.o start.o
	$(LD) $(LDFLAGS) start.o locktest.o -o locktest.coff
	../bin/coff2noff locktest.coff locktest

test0.o: test0.c
	$(CC) $(CFLAGS) -c test0.c
test0: test0.o start.o
	$(LD) $(LDFLAGS) start.o test0.o -o test0.coff
	../bin/coff2noff test0.coff test0


exectest.o: exectest.c
	$(CC) $(CFLAGS) -c exectest.c
exectest: exectest.o start.o
	$(LD) $(LDFLAGS) start.o exectest.o -o exectest.coff
	../bin/coff2noff exectest.coff exectest

exectest2.o: exectest2.c
	$(CC) $(CFLAGS) -c exectest2.c
exectest2: exectest2.o start.o
	$(LD) $(LDFLAGS) start.o exectest2.o -o exectest2.coff
	../bin/coff2noff exectest2.coff exectest2



testlockcondition.o: testlockcondition.c
	$(CC) $(CFLAGS) -c testlockcondition.c
testlockcondition: testlockcondition.o start.o
	$(LD) $(LDFLAGS) start.o testlockcondition.o -o testlockcondition.coff
	../bin/coff2noff testlockcondition.coff testlockcondition

broadcasttest.o: broadcasttest.c
	$(CC) $(CFLAGS) -c broadcasttest.c
broadcasttest: broadcasttest.o start.o
	$(LD) $(LDFLAGS) start.o broadcasttest.o -o broadcasttest.coff
	../bin/coff2noff broadcasttest.coff broadcasttest

createdestroytest.o: createdestroytest.c
	$(CC) $(CFLAGS) -c createdestroytest.c
createdestroytest: createdestroytest.o start.o
	$(LD) $(LDFLAGS) start.o createdestroytest.o -o createdestroytest.coff
	../bin/coff2noff createdestroytest.coff createdestroytest

signaltest.o: signaltest.c
	$(CC) $(CFLAGS) -c signaltest.c
signaltest: signaltest.o start.o
	$(LD) $(LDFLAGS) start.o signaltest.o -o signaltest.coff
	../bin/coff2noff signaltest.coff signaltest

waittest.o: waittest.c
	$(CC) $(CFLAGS) -c waittest.c
waittest: waittest.o start.o
	$(LD) $(LDFLAGS) start.o waittest.o -o waittest.coff
	../bin/coff2noff waittest.coff waittest

monitortest.o: monitortest.c
	$(CC) $(CFLAGS) -c monitortest.c
monitortest: monitortest.o start.o
	$(LD) $(LDFLAGS) start.o monitortest.o -o monitortest.coff
	../bin/coff2noff monitortest.coff monitortest



clean:
	rm -f *.o *.coff
