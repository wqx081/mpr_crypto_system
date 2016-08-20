App=libmpr_sm4.so

CFLAGS += -I./
CFLAGS += -I./sm4
CFLAGS += -fPIC -c -o

CXXFLAGS+=-I./
CXXFLAGS+=-I./sm4

CXXFLAGS+=-fpermissive
CXXFLAGS+= -fPIC -c -o


#		

BASE_DIR=./aes



C_SOURCES := MPRSymmCrypt.c	\
	./aes/sm4.c	\
	./aes/cryptlib.c	\
	./aes/aes_core.c

C_OBJECTS := MPRSymmCrypt.o	\
	./aes/sm4.o	\
	./aes/cryptlib.o	\
	./aes/aes_core.o

all:$(App)

$(App): $(CPP_OBJECTS) $(C_OBJECTS)
	@echo "  LINK  $(App)"
	@$(CC) -shared -o $(App) $(CPP_OBJECTS) $(C_OBJECTS)

.SUFFIXES: .cpp .c .o

%.o: %.cpp
	@echo "  CXX  $<"
	@$(CXX) $(CXXFLAGS) $@ $<

%.o: %.c
	@echo "  CC  $<"
	@$(CC)  $(CFLAGS) $@ $<

clean:
	rm -fr $(CPP_OBJECTS) $(C_OBJECTS) $(App)
