CC = gcc
CPP = g++

CFLAG = -O2 -c -D_XOPEN_SOURCE=600 -D_BSD_SOURCE
CFLAGS = $(CFLAG) -I.

LIBS = -L. -lsm4

CPPFLAGS += -Wall
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wno-cast-qual
CPPFLAGS += -Wno-deprecated
CPPFLAGS += -g -c
CPPFLAGS += -I.

OBJS =  aes_core.o	\
		bigint.o	\
		cryptlib.o	\
		curvep.o	\
		dbg_print.o	\
		eicl_mul.o	\
		mpzlib.o	\
		NNMLib.o	\
		sm2s.o		\
		sm3.o		\
		sm4.o		\
		srand.o		\
		tl_random.o		\
		native_sm2.o
		

TARGET = libsm2.a

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

%.o:%.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

all:$(OBJS)
	ar rc $(TARGET) $(OBJS);
	ranlib $(TARGET)

test:main.o
	$(CPP) -o test_app main.o $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -f main.o
