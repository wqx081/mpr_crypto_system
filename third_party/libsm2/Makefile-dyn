App=libmpr_sm2.so

CFLAGS += -I./
CFLAGS += -I./sm2
CFLAGS += -fPIC -c -o

CXXFLAGS+=-I./
CXXFLAGS+=-I./sm2

CXXFLAGS+=-fpermissive
CXXFLAGS+= -fPIC -c -o


#OBJS =  aes_core.o	\
#		bigint.o	\
#		cryptlib.o	\
#		curvep.o	\
#		dbg_print.o	\
#		eicl_mul.o	\
#		mpzlib.o	\
#		NNMLib.o	\
#		sm2s.o		\
#		sm3.o		\
#		sm4.o		\
#		srand.o		\
#		tl_random.o		\
#		native_sm2.o
#		

BASE_DIR=./sm2

CPP_SOURCES := $(BASE_DIR)/aes_core.cpp	\
	$(BASE_DIR)/bigint.cpp	\
	$(BASE_DIR)/cryptlib.cpp	\
	$(BASE_DIR)/curvep.cpp	\
	$(BASE_DIR)/dbg_print.cpp	\
	$(BASE_DIR)/eicl_mul.cpp	\
	$(BASE_DIR)/mpzlib.cpp	\
	$(BASE_DIR)/NNMLib.cpp	\
	$(BASE_DIR)/sm2s.cpp	\
	$(BASE_DIR)/sm3.cpp	\
	$(BASE_DIR)/sm4.cpp	\
	$(BASE_DIR)/srand.cpp	\
	$(BASE_DIR)/tl_random.cpp	\
	$(BASE_DIR)/native_sm2.cpp	
	
#CPP_OBJECTS := MPRAsymmCrypt_s.o	\
#	aes_core.o	\
#	bigint.o	\
#	cryptlib.o	\
#	curvep.o	\
#	dbg_print.o	\
#	eicl_mul.o	\
#	mpzlib.o	\
#	NNMLib.o	\
#	sm2s.o	\
#	sm3.o	\
#	sm4.o	\
#	srand.o	\
#	tl_random.o	\
#	native_sm2.o	
#

CPP_OBJECTS := $(CPP_SOURCES:.cpp=.o)

C_SOURCES := MPRAsymmCrypt_s.c
C_OBJECTS := MPRAsymmCrypt_s.o

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
