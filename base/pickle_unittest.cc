#include "base/pickle.h"
#include <gtest/gtest.h>

namespace base {

namespace {

const bool testbool1 = false;
const bool testbool2 = true;
const int testint = 2093847192;
const long testlong = 1093847192;
const uint16_t testuint16 = 32123;
const uint32_t testuint32 = 1593847192;
const int64_t testint64 = -0x7E8CA9253104BDFCLL;
const uint64_t testuint64 = 0xCE8CA9253104BDF7ULL;
const float testfloat = 3.1415926935f;
const double testdouble = 2.71828182845904523;
const std::string teststring("Hello world");  // note non-aligned string length
const std::wstring testwstring(L"Hello, world");
const char testrawstring[] = "Hello new world";
const char testdata[] = "AAA\0BBB\0";
const int testdatalen = arraysize(testdata) - 1;


void VerifyResult(const Pickle& pickle) {
  PickleIterator iter(pickle);
  
  bool outbool;
  EXPECT_TRUE(iter.ReadBool(&outbool));
  EXPECT_FALSE(outbool);
  EXPECT_TRUE(iter.ReadBool(&outbool));
  EXPECT_TRUE(outbool);
  
  int outint;
  EXPECT_TRUE(iter.ReadInt(&outint));
  EXPECT_EQ(testint, outint);
  
  long outlong;
  EXPECT_TRUE(iter.ReadLong(&outlong));
  EXPECT_EQ(testlong, outlong);
  
  uint16_t outuint16;
  EXPECT_TRUE(iter.ReadUInt16(&outuint16));
  EXPECT_EQ(testuint16, outuint16);
  
  uint32_t outuint32;
  EXPECT_TRUE(iter.ReadUInt32(&outuint32));
  EXPECT_EQ(testuint32, outuint32);
  
  int64_t outint64;
  EXPECT_TRUE(iter.ReadInt64(&outint64));
  EXPECT_EQ(testint64, outint64);
  
  uint64_t outuint64;
  EXPECT_TRUE(iter.ReadUInt64(&outuint64));
  EXPECT_EQ(testuint64, outuint64);
  
  float outfloat;
  EXPECT_TRUE(iter.ReadFloat(&outfloat));
  EXPECT_EQ(testfloat, outfloat);
  
  double outdouble;
  EXPECT_TRUE(iter.ReadDouble(&outdouble));
  EXPECT_EQ(testdouble, outdouble);
  
  std::string outstring;
  EXPECT_TRUE(iter.ReadString(&outstring));
  EXPECT_EQ(teststring, outstring);
  
  StringPiece outstringpiece;
  EXPECT_TRUE(iter.ReadStringPiece(&outstringpiece));
  EXPECT_EQ(testrawstring, outstringpiece);
  
  const char* outdata;
  int outdatalen;
  EXPECT_TRUE(iter.ReadData(&outdata, &outdatalen));
  EXPECT_EQ(testdatalen, outdatalen);
  EXPECT_EQ(memcmp(testdata, outdata, outdatalen), 0);
 
  EXPECT_FALSE(iter.ReadInt(&outint));
}
  
}  // namespace

TEST(PickleTest, EncodeDecode) {
  Pickle pickle;
  
  EXPECT_TRUE(pickle.WriteBool(testbool1));
  EXPECT_TRUE(pickle.WriteBool(testbool2));
  EXPECT_TRUE(pickle.WriteInt(testint));
  EXPECT_TRUE(pickle.WriteLong(testlong));
  EXPECT_TRUE(pickle.WriteUInt16(testuint16));
  EXPECT_TRUE(pickle.WriteUInt32(testuint32));
  EXPECT_TRUE(pickle.WriteInt64(testint64));
  EXPECT_TRUE(pickle.WriteUInt64(testuint64));
  EXPECT_TRUE(pickle.WriteFloat(testfloat));
  EXPECT_TRUE(pickle.WriteDouble(testdouble));
  EXPECT_TRUE(pickle.WriteString(teststring));
  EXPECT_TRUE(pickle.WriteString(testrawstring));
  EXPECT_TRUE(pickle.WriteData(testdata, testdatalen));
  VerifyResult(pickle);
  
  Pickle pickle2(pickle);
  VerifyResult(pickle2);
  
  Pickle pickle3;
  pickle3 = pickle;
  VerifyResult(pickle3);
}

// Tests that reading/writing a long works correctly when the source process
// is 64-bit.  We rely on having both 32- and 64-bit trybots to validate both
// arms of the conditional in this test.

TEST(PickleTest, LongFrom64Bit) {
  Pickle pickle;
  EXPECT_TRUE(pickle.WriteInt64(testint64));
  
  PickleIterator iter(pickle);
  long outlong;
  if (sizeof(long) < sizeof(int64_t)) {
  } else {
    EXPECT_TRUE(iter.ReadLong(&outlong));
    EXPECT_EQ(testint64, outlong);
  }
}

  TEST(PickleTest, SmallBuffer) {
    std::unique_ptr<char[]> buffer(new char[1]);
  
    Pickle pickle(buffer.get(), 1);
  
    PickleIterator iter(pickle);
    int data;
    EXPECT_FALSE(iter.ReadInt(&data));
  }

  TEST(PickleTest, BigSize) {
    int buffer[] = { 0x56035200, 25, 40, 50 };
  
    Pickle pickle(reinterpret_cast<char*>(buffer), sizeof(buffer));
  
    PickleIterator iter(pickle);
    int data;
    EXPECT_FALSE(iter.ReadInt(&data));
  }
  
  TEST(PickleTest, UnalignedSize) {
    int buffer[] = { 10, 25, 40, 50 };
  
    Pickle pickle(reinterpret_cast<char*>(buffer), sizeof(buffer));
  
    PickleIterator iter(pickle);
    int data;
    EXPECT_FALSE(iter.ReadInt(&data));
  }
  
  TEST(PickleTest, ZeroLenStr) {
    Pickle pickle;
    EXPECT_TRUE(pickle.WriteString(std::string()));
  
    PickleIterator iter(pickle);
    std::string outstr;
    EXPECT_TRUE(iter.ReadString(&outstr));
    EXPECT_EQ("", outstr);
  }
  
  TEST(PickleTest, BadLenStr) {
    Pickle pickle;
    EXPECT_TRUE(pickle.WriteInt(-2));
  
    PickleIterator iter(pickle);
    std::string outstr;
    EXPECT_FALSE(iter.ReadString(&outstr));
  }


#if 0
  TEST(PickleTest, PeekNext) {
    struct CustomHeader : base::Pickle::Header {
      int cookies[10];
    };
  
    Pickle pickle(sizeof(CustomHeader));
  
    EXPECT_TRUE(pickle.WriteString("Goooooooooooogle"));
  
    const char* pickle_data = static_cast<const char*>(pickle.data());
  
    size_t pickle_size;
  
    EXPECT_FALSE(Pickle::PeekNext(
        sizeof(CustomHeader),
        pickle_data,
        pickle_data + sizeof(CustomHeader) - 1,
        &pickle_size));
  
    EXPECT_TRUE(Pickle::PeekNext(
        sizeof(CustomHeader),
        pickle_data,
        pickle_data + sizeof(CustomHeader),
        &pickle_size));
    EXPECT_EQ(pickle_size, pickle.size());
  
    EXPECT_TRUE(Pickle::PeekNext(
        sizeof(CustomHeader),
        pickle_data,
        pickle_data + sizeof(CustomHeader) + 1,
        &pickle_size));
    EXPECT_EQ(pickle_size, pickle.size());
  
    EXPECT_TRUE(Pickle::PeekNext(
        sizeof(CustomHeader),
        pickle_data,
        pickle_data + pickle.size(),
        &pickle_size));
    EXPECT_EQ(pickle_size, pickle.size());
  }
#endif

//TODO

} // namespace base
