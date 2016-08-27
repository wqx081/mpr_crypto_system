CXXFLAGS += -I./
CXXFLAGS += -I./third_party/boringssl/include
CXXFLAGS += -std=c++11 -Wall -g -c -o

LIB_FILES :=-lglog -lgflags -L/usr/local/lib -lgtest -lgtest_main -lpthread -lz -lminizip \
	./third_party/boringssl/build/crypto/libcrypto.a \
	./third_party/boringssl/build/ssl/libssl.a \
	./third_party/boringssl/build/decrepit/libdecrepit.a \
	-L./third_party/libsm2/ -lmpr_sm2  \
	\
	-lnspr4 \
	-ldl \
	-lmysqlclient

CPP_SOURCES := \
	./base/ascii_ctype.cc \
	./base/numbers.cc \
	./base/string_piece.cc \
	./base/string_util.cc \
	./base/string_printf.cc \
	./base/string_encode.cc \
	./base/status.cc \
	./base/pickle.cc \
	./base/time.cc \
	./base/file_path.cc \
	./base/file.cc \
	./base/file_util.cc \
	./base/file_enumerator.cc \
	./base/scoped_file.cc \
	./base/ref_counted.cc \
	./base/test/opaque_ref_counted.cc \
	./base/once.cc \
	./base/location.cc \
	\
	./zip/zip_internal.cc \
	./zip/zip_reader.cc \
	./zip/zip.cc \
	\
	\
	./crypto/openssl_util.cc \
	./crypto/symmetric_key.cc \
	./crypto/symmetric_encryptor.cc \
	./crypto/symmetric_crypt.cc \
	./crypto/sm2_util.cc \
	./crypto/sm2_access_point.cc \
	./crypto/asymmetric_padding.cc \
	./crypto/asymmetric_key.cc \
	\
	./crypto/memory_input_stream.cc \
	./crypto/memory_output_stream.cc \
	\
	\
	./threading/time_util.cc \
	./threading/mutex.cc \
	./threading/monitor.cc \
	./threading/thread_factory.cc \
	./threading/thread_manager.cc \
	\
	\
#	./db/common/connection_info.cc \
	./db/common/connection_pool.cc \
	./db/common/connection_manager.cc \
	./db/backend/db_result.cc \
	./db/backend/db_statement.cc \
	./db/backend/db_connection.cc \
	./db/backend/connector_interface.cc \
	\
	./db/drivers/mysql/mysql_connector.cc \
	./db/drivers/mysql/mysql_direct_result.cc \
	./db/drivers/mysql/mysql_direct_statement.cc \
	./db/drivers/mysql/mysql_prepared_result.cc \
	./db/drivers/mysql/mysql_prepared_statement.cc \
	./db/drivers/mysql/mysql_connection.cc \
	\
	./db/frontend/result.cc \
	./db/frontend/statement.cc \
	./db/frontend/session.cc \
	./db/frontend/transaction.cc \


CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./base/file_path_unittest \
	./base/once_unittest \
	./base/ref_counted_unittest \
	./base/pickle_unittest \
	./base/time_unittest \
	./base/numbers_unittest \
	./zip/zip_reader_unittest \
	./zip/zip_unittest \
	\
	./crypto/symmetric_key_unittest \
	./crypto/symmetric_encryptor_unittest \
	./crypto/sm2_unittest \
	./crypto/sm2_util_unittest \
	./crypto/asymmetric_padding_unittest \
	./crypto/sm2_access_point_unittest \
	\
	./crypto/memory_input_stream_unittest \
	./crypto/memory_output_stream_unittest \
	\
	./threading/thread_factory_unittest \
	./threading/thread_manager_unittest \
	./threading/mutex_unittest \
	\
	\
#	./db/drivers/mysql/mysql_connection_unittest \
	./db/frontend/frontend_unittest \
	

all: $(CPP_OBJECTS) $(TESTS)
.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/file_path_unittest: ./base/file_path_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./base/file_path_unittest.o: ./base/file_path_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./base/time_unittest: ./base/time_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./base/time_unittest.o: ./base/time_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./base/once_unittest: ./base/once_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./base/once_unittest.o: ./base/once_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./base/pickle_unittest: ./base/pickle_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./base/pickle_unittest.o: ./base/pickle_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./base/numbers_unittest: ./base/numbers_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./base/numbers_unittest.o: ./base/numbers_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./zip/zip_reader_unittest: ./zip/zip_reader_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./zip/zip_reader_unittest.o: ./zip/zip_reader_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./zip/zip_unittest: ./zip/zip_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./zip/zip_unittest.o: ./zip/zip_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./crypto/sm2_unittest: ./crypto/sm2_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./crypto/sm2_unittest.o: ./crypto/sm2_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./crypto/sm2_access_point_unittest: ./crypto/sm2_access_point_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./crypto/sm2_access_point_unittest.o: ./crypto/sm2_access_point_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./crypto/sm2_util_unittest: ./crypto/sm2_util_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./crypto/sm2_util_unittest.o: ./crypto/sm2_util_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./crypto/asymmetric_padding_unittest: ./crypto/asymmetric_padding_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./crypto/asymmetric_padding_unittest.o: ./crypto/asymmetric_padding_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./crypto/symmetric_key_unittest: ./crypto/symmetric_key_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./crypto/symmetric_key_unittest.o: ./crypto/symmetric_key_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./crypto/symmetric_encryptor_unittest: ./crypto/symmetric_encryptor_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./crypto/symmetric_encryptor_unittest.o: ./crypto/symmetric_encryptor_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./crypto/memory_input_stream_unittest: ./crypto/memory_input_stream_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./crypto/memory_input_stream_unittest.o: ./crypto/memory_input_stream_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./crypto/memory_output_stream_unittest: ./crypto/memory_output_stream_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./crypto/memory_output_stream_unittest.o: ./crypto/memory_output_stream_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<















./threading/thread_factory_unittest: ./threading/thread_factory_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./threading/thread_factory_unittest.o: ./threading/thread_factory_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./threading/thread_manager_unittest: ./threading/thread_manager_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./threading/thread_manager_unittest.o: ./threading/thread_manager_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./threading/mutex_unittest: ./threading/mutex_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./threading/mutex_unittest.o: ./threading/mutex_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./base/ref_counted_unittest: ./base/ref_counted_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./base/ref_counted_unittest.o: ./base/ref_counted_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<


./db/drivers/mysql/mysql_connection_unittest: ./db/drivers/mysql/mysql_connection_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./db/drivers/mysql/mysql_connection_unittest.o: ./db/drivers/mysql/mysql_connection_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

./db/frontend/frontend_unittest: ./db/frontend/frontend_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./db/frontend/frontend_unittest.o: ./db/frontend/frontend_unittest.cc
	@$(CXX) -Wno-unused-variable $(CXXFLAGS) $@ $<

db_clean:
	`find ./db -name "*.o" | xarg rm -fr`

clean:
	rm -fr base/*.o
	rm -fr threading/*.o
	rm -fr zip/*.o
	rm -fr crypto/*.o
	@rm -fr $(TESTS)
	@rm -fr $(CPP_OBJECTS)
