#include "envoy/common/exception.h"

#include "common/buffer/buffer_impl.h"

#include "extensions/filters/network/thrift_proxy/buffer_helper.h"

#include "test/extensions/filters/network/thrift_proxy/utility.h"
#include "test/test_common/printers.h"
#include "test/test_common/utility.h"

#include "gtest/gtest.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace ThriftProxy {

TEST(BufferWrapperTest, ImplementedFunctions) {
  Buffer::OwnedImpl buffer;
  addString(buffer, "abcdefghij");

  BufferWrapper wrapper(buffer);
  {
    char s[4] = {0};
    wrapper.copyOut(0, 3, s);
    EXPECT_EQ("abc", std::string(s));
    EXPECT_EQ(10, wrapper.length());
    EXPECT_EQ(0, wrapper.position());
  }

  {
    char s[6] = {0};
    wrapper.copyOut(5, 5, s);
    EXPECT_EQ("fghij", std::string(s));
    EXPECT_EQ(10, wrapper.length());
    EXPECT_EQ(0, wrapper.position());
  }

  {
    std::string s(static_cast<char*>(wrapper.linearize(5)), 5);
    EXPECT_EQ("abcde", s);
    EXPECT_EQ(0, wrapper.position());
  }

  wrapper.drain(2);

  {
    char s[4] = {0};
    wrapper.copyOut(4, 3, s);
    EXPECT_EQ("ghi", std::string(s));
    EXPECT_EQ(8, wrapper.length());
    EXPECT_EQ(2, wrapper.position());
  }

  {
    std::string s(static_cast<char*>(wrapper.linearize(8)), 8);
    EXPECT_EQ("cdefghij", s);
    EXPECT_EQ(2, wrapper.position());
  }
}

TEST(BufferHelperTest, PeekI8) {
  {
    Buffer::OwnedImpl buffer;
    addSeq(buffer, {0, 1, 0xFE});
    EXPECT_EQ(BufferHelper::peekI8(buffer), 0);
    EXPECT_EQ(BufferHelper::peekI8(buffer, 0), 0);
    EXPECT_EQ(BufferHelper::peekI8(buffer, 1), 1);
    EXPECT_EQ(BufferHelper::peekI8(buffer, 2), -2);
    EXPECT_EQ(buffer.length(), 3);
  }

  {
    Buffer::OwnedImpl buffer;
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekI8(buffer, 0), EnvoyException, "buffer underflow");
  }

  {
    Buffer::OwnedImpl buffer;
    addInt8(buffer, 0);
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekI8(buffer, 1), EnvoyException, "buffer underflow");
  }
}

TEST(BufferHelperTest, PeekI16) {
  {
    Buffer::OwnedImpl buffer;
    addSeq(buffer, {0, 1, 2, 3, 0xFF, 0xFF});
    EXPECT_EQ(BufferHelper::peekI16(buffer), 1);
    EXPECT_EQ(BufferHelper::peekI16(buffer, 0), 1);
    EXPECT_EQ(BufferHelper::peekI16(buffer, 1), 0x0102);
    EXPECT_EQ(BufferHelper::peekI16(buffer, 2), 0x0203);
    EXPECT_EQ(BufferHelper::peekI16(buffer, 4), -1);
    EXPECT_EQ(buffer.length(), 6);
  }

  {
    Buffer::OwnedImpl buffer;
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekI16(buffer, 0), EnvoyException, "buffer underflow");
  }

  {
    Buffer::OwnedImpl buffer;
    addRepeated(buffer, 2, 0);
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekI16(buffer, 1), EnvoyException, "buffer underflow");
  }
}

TEST(BufferHelperTest, PeekI32) {
  {
    Buffer::OwnedImpl buffer;
    addSeq(buffer, {0, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0xFF});
    EXPECT_EQ(BufferHelper::peekI32(buffer), 0x00010203);
    EXPECT_EQ(BufferHelper::peekI32(buffer, 0), 0x00010203);
    EXPECT_EQ(BufferHelper::peekI32(buffer, 1), 0x010203FF);
    EXPECT_EQ(BufferHelper::peekI32(buffer, 2), 0x0203FFFF);
    EXPECT_EQ(BufferHelper::peekI32(buffer, 4), -1);
    EXPECT_EQ(buffer.length(), 8);
  }
  {
    Buffer::OwnedImpl buffer;
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekI32(buffer, 0), EnvoyException, "buffer underflow");
  }

  {
    Buffer::OwnedImpl buffer;
    addRepeated(buffer, 4, 0);
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekI32(buffer, 1), EnvoyException, "buffer underflow");
  }
}

TEST(BufferHelperTest, PeekI64) {
  {
    Buffer::OwnedImpl buffer;
    addSeq(buffer, {0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
    EXPECT_EQ(BufferHelper::peekI64(buffer), 0x0001020304050607);
    EXPECT_EQ(BufferHelper::peekI64(buffer, 0), 0x0001020304050607);
    EXPECT_EQ(BufferHelper::peekI64(buffer, 1), 0x01020304050607FF);
    EXPECT_EQ(BufferHelper::peekI64(buffer, 2), 0x020304050607FFFF);
    EXPECT_EQ(BufferHelper::peekI64(buffer, 8), -1);
    EXPECT_EQ(buffer.length(), 16);
  }

  {
    Buffer::OwnedImpl buffer;
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekI64(buffer, 0), EnvoyException, "buffer underflow");
  }

  {
    Buffer::OwnedImpl buffer;
    addRepeated(buffer, 8, 0);
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekI64(buffer, 1), EnvoyException, "buffer underflow");
  }
}

TEST(BufferHelperTest, PeekU16) {
  {
    Buffer::OwnedImpl buffer;
    addSeq(buffer, {0, 1, 2, 3, 0xFF, 0xFF});
    EXPECT_EQ(BufferHelper::peekU16(buffer), 1);
    EXPECT_EQ(BufferHelper::peekU16(buffer, 0), 1);
    EXPECT_EQ(BufferHelper::peekU16(buffer, 1), 0x0102);
    EXPECT_EQ(BufferHelper::peekU16(buffer, 2), 0x0203);
    EXPECT_EQ(BufferHelper::peekU16(buffer, 4), 0xFFFF);
    EXPECT_EQ(buffer.length(), 6);
  }
  {
    Buffer::OwnedImpl buffer;
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekU16(buffer, 0), EnvoyException, "buffer underflow");
  }

  {
    Buffer::OwnedImpl buffer;
    addRepeated(buffer, 2, 0);
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekU16(buffer, 1), EnvoyException, "buffer underflow");
  }
}

TEST(BufferHelperTest, PeekU32) {
  {
    Buffer::OwnedImpl buffer;
    addSeq(buffer, {0, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0xFF});
    EXPECT_EQ(BufferHelper::peekU32(buffer), 0x00010203);
    EXPECT_EQ(BufferHelper::peekU32(buffer, 0), 0x00010203);
    EXPECT_EQ(BufferHelper::peekU32(buffer, 1), 0x010203FF);
    EXPECT_EQ(BufferHelper::peekU32(buffer, 2), 0x0203FFFF);
    EXPECT_EQ(BufferHelper::peekU32(buffer, 4), 0xFFFFFFFF);
    EXPECT_EQ(buffer.length(), 8);
  }
  {
    Buffer::OwnedImpl buffer;
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekU32(buffer, 0), EnvoyException, "buffer underflow");
  }

  {
    Buffer::OwnedImpl buffer;
    addRepeated(buffer, 4, 0);
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekU32(buffer, 1), EnvoyException, "buffer underflow");
  }
}

TEST(BufferHelperTest, PeekU64) {
  {
    Buffer::OwnedImpl buffer;
    addSeq(buffer, {0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
    EXPECT_EQ(BufferHelper::peekU64(buffer), 0x0001020304050607);
    EXPECT_EQ(BufferHelper::peekU64(buffer, 0), 0x0001020304050607);
    EXPECT_EQ(BufferHelper::peekU64(buffer, 1), 0x01020304050607FF);
    EXPECT_EQ(BufferHelper::peekU64(buffer, 2), 0x020304050607FFFF);
    EXPECT_EQ(BufferHelper::peekU64(buffer, 8), 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(buffer.length(), 16);
  }
  {
    Buffer::OwnedImpl buffer;
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekU64(buffer, 0), EnvoyException, "buffer underflow");
  }

  {
    Buffer::OwnedImpl buffer;
    addRepeated(buffer, 8, 0);
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekU64(buffer, 1), EnvoyException, "buffer underflow");
  }
}

TEST(BufferHelperTest, DrainI8) {
  Buffer::OwnedImpl buffer;
  addSeq(buffer, {0, 1, 0xFE});
  EXPECT_EQ(BufferHelper::drainI8(buffer), 0);
  EXPECT_EQ(BufferHelper::drainI8(buffer), 1);
  EXPECT_EQ(BufferHelper::drainI8(buffer), -2);
  EXPECT_EQ(buffer.length(), 0);
}

TEST(BufferHelperTest, DrainI16) {
  Buffer::OwnedImpl buffer;
  addSeq(buffer, {0, 1, 2, 3, 0xFF, 0xFF});
  EXPECT_EQ(BufferHelper::drainI16(buffer), 1);
  EXPECT_EQ(BufferHelper::drainI16(buffer), 0x0203);
  EXPECT_EQ(BufferHelper::drainI16(buffer), -1);
  EXPECT_EQ(buffer.length(), 0);
}

TEST(BufferHelperTest, DrainI32) {
  Buffer::OwnedImpl buffer;
  addSeq(buffer, {0, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0xFF});
  EXPECT_EQ(BufferHelper::drainI32(buffer), 0x00010203);
  EXPECT_EQ(BufferHelper::drainI32(buffer), -1);
  EXPECT_EQ(buffer.length(), 0);
}

TEST(BufferHelperTest, DrainI64) {
  Buffer::OwnedImpl buffer;
  addSeq(buffer, {0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
  EXPECT_EQ(BufferHelper::drainI64(buffer), 0x0001020304050607);
  EXPECT_EQ(BufferHelper::drainI64(buffer), -1);
  EXPECT_EQ(buffer.length(), 0);
}

TEST(BufferHelperTest, DrainU32) {
  Buffer::OwnedImpl buffer;
  addSeq(buffer, {0, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0xFF});
  EXPECT_EQ(BufferHelper::drainU32(buffer), 0x00010203);
  EXPECT_EQ(BufferHelper::drainU32(buffer), 0xFFFFFFFF);
  EXPECT_EQ(buffer.length(), 0);
}

TEST(BufferHelperTest, DrainU64) {
  Buffer::OwnedImpl buffer;
  addSeq(buffer, {0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
  EXPECT_EQ(BufferHelper::drainU64(buffer), 0x0001020304050607);
  EXPECT_EQ(BufferHelper::drainU64(buffer), 0xFFFFFFFFFFFFFFFF);
  EXPECT_EQ(buffer.length(), 0);
}

TEST(BufferHelperTest, DrainDouble) {
  Buffer::OwnedImpl buffer;

  // c.f. https://en.wikipedia.org/wiki/Double-precision_floating-point_format
  // 01000000 00001000 00000000 0000000 00000000 00000000 00000000 000000000 = 3
  addSeq(buffer, {0x40, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  // 11111111 11101111 11111111 1111111 11111111 11111111 11111111 111111111 = -DBL_MAX
  addSeq(buffer, {0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});

  EXPECT_EQ(BufferHelper::drainDouble(buffer), 3.0);
  EXPECT_EQ(BufferHelper::drainDouble(buffer), -DBL_MAX);
  EXPECT_EQ(buffer.length(), 0);
}

TEST(BufferHelperTest, PeekVarInt32) {
  {
    Buffer::OwnedImpl buffer;
    addInt8(buffer, 0);
    addInt8(buffer, 0x7F);
    addSeq(buffer, {0xFF, 0x01});                   // 0xFF
    addSeq(buffer, {0xFF, 0xFF, 0x03});             // 0xFFFF
    addSeq(buffer, {0xFF, 0xFF, 0xFF, 0x07});       // 0xFFFFFF
    addSeq(buffer, {0xFF, 0xFF, 0xFF, 0xFF, 0x07}); // 0x7FFFFFFF
    addSeq(buffer, {0xFF, 0xFF, 0xFF, 0xFF, 0x0F}); // 0xFFFFFFFF

    int size = 0;
    EXPECT_EQ(BufferHelper::peekVarIntI32(buffer, 0, size), 0);
    EXPECT_EQ(size, 1);

    EXPECT_EQ(BufferHelper::peekVarIntI32(buffer, 1, size), 0x7F);
    EXPECT_EQ(size, 1);

    EXPECT_EQ(BufferHelper::peekVarIntI32(buffer, 2, size), 0xFF);
    EXPECT_EQ(size, 2);

    EXPECT_EQ(BufferHelper::peekVarIntI32(buffer, 4, size), 0xFFFF);
    EXPECT_EQ(size, 3);

    EXPECT_EQ(BufferHelper::peekVarIntI32(buffer, 7, size), 0xFFFFFF);
    EXPECT_EQ(size, 4);

    EXPECT_EQ(BufferHelper::peekVarIntI32(buffer, 11, size), 0x7FFFFFFF);
    EXPECT_EQ(size, 5);

    EXPECT_EQ(BufferHelper::peekVarIntI32(buffer, 16, size), 0xFFFFFFFF);
    EXPECT_EQ(size, 5);
  }

  {
    Buffer::OwnedImpl buffer;
    int size = 0;
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekVarIntI32(buffer, 0, size), EnvoyException,
                              "buffer underflow");
  }

  {
    Buffer::OwnedImpl buffer;
    int size = 0;
    addInt8(buffer, 0);
    EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekVarIntI32(buffer, 1, size), EnvoyException,
                              "buffer underflow");
  }
}

TEST(BufferHelperTest, PeekVarInt32BufferUnderflow) {
  Buffer::OwnedImpl buffer;
  int size = 0;

  for (int i = 1; i < 5; i++) {
    addInt8(buffer, 0x80);
    EXPECT_EQ(BufferHelper::peekVarIntI32(buffer, 0, size), 0);
    EXPECT_EQ(size, -i);
  }

  addInt8(buffer, 0x80);
  EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekVarIntI32(buffer, 0, size), EnvoyException,
                            "invalid compact protocol varint i32");
}

TEST(BufferHelperTest, PeekZigZagI32) {
  Buffer::OwnedImpl buffer;
  addInt8(buffer, 0);                             // zigzag(0) = 0
  addInt8(buffer, 1);                             // zigzag(1) = -1
  addInt8(buffer, 2);                             // zigzag(2) = 1
  addSeq(buffer, {0xFE, 0x01});                   // zigzag(0xFE) = 127
  addSeq(buffer, {0xFF, 0x01});                   // zigzag(0xFF) = -128
  addSeq(buffer, {0xFF, 0xFF, 0x03});             // zigzag(0xFFFF) = -32768
  addSeq(buffer, {0xFF, 0xFF, 0xFF, 0x07});       // zigzag(0xFFFFFF) = -8388608
  addSeq(buffer, {0xFE, 0xFF, 0xFF, 0xFF, 0x07}); // zigzag(0x7FFFFFFE) = 0x3FFFFFFF
  addSeq(buffer, {0xFE, 0xFF, 0xFF, 0xFF, 0x0F}); // zigzag(0xFFFFFFFE) = 0x7FFFFFFF
  addSeq(buffer, {0xFF, 0xFF, 0xFF, 0xFF, 0x0F}); // zigzag(0xFFFFFFFF) = 0x80000000

  int size = 0;
  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 0, size), 0);
  EXPECT_EQ(size, 1);

  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 1, size), -1);
  EXPECT_EQ(size, 1);

  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 2, size), 1);
  EXPECT_EQ(size, 1);

  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 3, size), 127);
  EXPECT_EQ(size, 2);

  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 5, size), -128);
  EXPECT_EQ(size, 2);

  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 7, size), -32768);
  EXPECT_EQ(size, 3);

  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 10, size), -8388608);
  EXPECT_EQ(size, 4);

  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 14, size), 0x3FFFFFFF);
  EXPECT_EQ(size, 5);

  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 19, size), 0x7FFFFFFF);
  EXPECT_EQ(size, 5);

  EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 24, size), 0x80000000);
  EXPECT_EQ(size, 5);
}

TEST(BufferHelperTest, PeekZigZagI32BufferUnderflow) {
  Buffer::OwnedImpl buffer;
  int size = 0;

  for (int i = 1; i < 5; i++) {
    addInt8(buffer, 0x80);
    EXPECT_EQ(BufferHelper::peekZigZagI32(buffer, 0, size), 0);
    EXPECT_EQ(size, -i);
  }

  addInt8(buffer, 0x80);
  EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekZigZagI32(buffer, 0, size), EnvoyException,
                            "invalid compact protocol zig-zag i32");
}

TEST(BufferHelperTest, PeekZigZagI64) {
  Buffer::OwnedImpl buffer;
  addInt8(buffer, 0);                             // zigzag(0) = 0
  addInt8(buffer, 1);                             // zigzag(1) = -1
  addInt8(buffer, 2);                             // zigzag(2) = 1
  addSeq(buffer, {0xFF, 0xFF, 0x03});             // zigzag(0xFFFF) = -32768
  addSeq(buffer, {0xFE, 0xFF, 0xFF, 0xFF, 0x0F}); // zigzag(0xFFFF FFFE) = 0x7FFF FFFF

  // zigzag(0xFFFF FFFF FFFE) = 0x7FFF FFFF FFFF
  addSeq(buffer, {0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F});

  // zigzag(0x7FFF FFFF FFFF FFFE) = 0x3FFF FFFF FFFF FFFF
  addSeq(buffer, {0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F});

  // zigzag(0xFFFF FFFF FFFF FFFF) = 0x8000 0000 0000 0000 (-2^63)
  addSeq(buffer, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01});

  int size = 0;
  EXPECT_EQ(BufferHelper::peekZigZagI64(buffer, 0, size), 0);
  EXPECT_EQ(size, 1);

  EXPECT_EQ(BufferHelper::peekZigZagI64(buffer, 1, size), -1);
  EXPECT_EQ(size, 1);

  EXPECT_EQ(BufferHelper::peekZigZagI64(buffer, 2, size), 1);
  EXPECT_EQ(size, 1);

  EXPECT_EQ(BufferHelper::peekZigZagI64(buffer, 3, size), -32768);
  EXPECT_EQ(size, 3);

  EXPECT_EQ(BufferHelper::peekZigZagI64(buffer, 6, size), 0x7FFFFFFF);
  EXPECT_EQ(size, 5);

  EXPECT_EQ(BufferHelper::peekZigZagI64(buffer, 11, size), 0x7FFFFFFFFFFF);
  EXPECT_EQ(size, 7);

  EXPECT_EQ(BufferHelper::peekZigZagI64(buffer, 18, size), 0x3FFFFFFFFFFFFFFF);
  EXPECT_EQ(size, 9);

  EXPECT_EQ(BufferHelper::peekZigZagI64(buffer, 27, size), 0x8000000000000000);
  EXPECT_EQ(size, 10);
}

TEST(BufferHelperTest, PeekZigZagI64BufferUnderflow) {
  Buffer::OwnedImpl buffer;
  int size = 0;

  for (int i = 1; i < 10; i++) {
    addInt8(buffer, 0x80);
    EXPECT_EQ(BufferHelper::peekZigZagI64(buffer, 0, size), 0);
    EXPECT_EQ(size, -i);
  }

  addInt8(buffer, 0x80);
  EXPECT_THROW_WITH_MESSAGE(BufferHelper::peekZigZagI64(buffer, 0, size), EnvoyException,
                            "invalid compact protocol zig-zag i64");
}

} // namespace ThriftProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
