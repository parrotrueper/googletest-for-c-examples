#include <cstdint>
#include <gtest/gtest.h>

#include "getBytes.h"

struct GetBytesTests : public ::testing::Test {
};

/* getBytes_u16getLoByteBe, getBytes_u16getHiByteBe */
TEST_F(GetBytesTests, U16HighLowBytes)
{
    uint16_t v = 0x1234U;
    EXPECT_EQ(getBytes_u16getHiByteBe(v), static_cast<uint8_t>(0x12));
    EXPECT_EQ(getBytes_u16getLoByteBe(v), static_cast<uint8_t>(0x34));

    v = 0xABCDU;
    EXPECT_EQ(getBytes_u16getHiByteBe(v), static_cast<uint8_t>(0xAB));
    EXPECT_EQ(getBytes_u16getLoByteBe(v), static_cast<uint8_t>(0xCD));
}
/* getBytes_s16getLoByteBe, getBytes_s16getHiByteBe */
TEST_F(GetBytesTests, S16HighLowBytes)
{
    int16_t v = 0x1234;
    EXPECT_EQ(getBytes_s16getHiByteBe(v), static_cast<uint8_t>(0x12));
    EXPECT_EQ(getBytes_s16getLoByteBe(v), static_cast<uint8_t>(0x34));

    v = -1; // 0xFFFF
    EXPECT_EQ(getBytes_s16getHiByteBe(v), static_cast<uint8_t>(0xFF));
    EXPECT_EQ(getBytes_s16getLoByteBe(v), static_cast<uint8_t>(0xFF));
}
/* getBytes_bytesToU16 */
TEST_F(GetBytesTests, BytesToU16BigEndian)
{
    const uint8_t be[2] = {0x12, 0x34};
    EXPECT_EQ(getBytes_bytesToU16(be), static_cast<uint16_t>(0x1234));

    const uint8_t be2[2] = {0x00, 0x01};
    EXPECT_EQ(getBytes_bytesToU16(be2), static_cast<uint16_t>(0x0001));
}
/* getBytes_bytesToU16Le*/
TEST_F(GetBytesTests, BytesToU16LittleEndian)
{
    const uint8_t le[2] = {0x34, 0x12};
    EXPECT_EQ(getBytes_bytesToU16Le(le), static_cast<uint16_t>(0x1234));

    const uint8_t le2[2] = {0x01, 0x00};
    EXPECT_EQ(getBytes_bytesToU16Le(le2), static_cast<uint16_t>(0x0001));
}
/* getBytes_bytesToS16Le */
TEST_F(GetBytesTests, BytesToS16LittleEndian)
{
    const uint8_t le[2] = {0x34, 0x12};
    EXPECT_EQ(getBytes_bytesToS16Le(le), static_cast<int16_t>(0x1234));

    const uint8_t le2[2] = {0x01, 0x00};
    EXPECT_EQ(getBytes_bytesToS16Le(le2), static_cast<int16_t>(0x0001));
}
/* getBytes_bytesToS16Be*/
TEST_F(GetBytesTests, BytesToS16BigEndianSigned)
{
    const uint8_t pos[2] = {0x00, 0x7F};
    EXPECT_EQ(getBytes_bytesToS16Be(pos), static_cast<int16_t>(0x007F));

    const uint8_t neg1[2] = {0xFF, 0xFF}; // -1
    EXPECT_EQ(getBytes_bytesToS16Be(neg1), static_cast<int16_t>(-1));

    const uint8_t neg2[2] = {0xFF, 0xFE}; // -2
    EXPECT_EQ(getBytes_bytesToS16Be(neg2), static_cast<int16_t>(-2));
}
/* getBytes_s32getByte0, getBytes_s32getByte1, getBytes_s32getByte2, getBytes_s32getByte3 */
TEST_F(GetBytesTests, I32ByteExtraction)
{
    int32_t v = 0xA1B2C3D4;
    EXPECT_EQ(getBytes_s32getByte0(v), static_cast<uint8_t>(0xD4));
    EXPECT_EQ(getBytes_s32getByte1(v), static_cast<uint8_t>(0xC3));
    EXPECT_EQ(getBytes_s32getByte2(v), static_cast<uint8_t>(0xB2));
    EXPECT_EQ(getBytes_s32getByte3(v), static_cast<uint8_t>(0xA1));
}

TEST_F(GetBytesTests, I32NegativeValueBytes)
{
    int32_t v = -1; // 0xFFFFFFFF
    EXPECT_EQ(getBytes_s32getByte0(v), static_cast<uint8_t>(0xFF));
    EXPECT_EQ(getBytes_s32getByte1(v), static_cast<uint8_t>(0xFF));
    EXPECT_EQ(getBytes_s32getByte2(v), static_cast<uint8_t>(0xFF));
    EXPECT_EQ(getBytes_s32getByte3(v), static_cast<uint8_t>(0xFF));
}
/* getBytes_u32ToBytesLe */
TEST_F(GetBytesTests, U32ToBytesLittleEndian)
{
    uint8_t payload[4] = {0};
    getBytes_u32ToBytesLe(0xA1B2C3D4U, payload, sizeof(payload));
    EXPECT_EQ(payload[0], static_cast<uint8_t>(0xD4));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0xC3));
    EXPECT_EQ(payload[2], static_cast<uint8_t>(0xB2));
    EXPECT_EQ(payload[3], static_cast<uint8_t>(0xA1));
}

/* getBytes_s32ToBytesLe */
TEST_F(GetBytesTests, S32ToBytesLittleEndian)
{
    int32_t value      = static_cast<int32_t>(0xA1B2C3D4U);
    uint8_t payload[4] = {0};

    getBytes_s32ToBytesLe(value, payload, sizeof(payload));

    EXPECT_EQ(payload[0], static_cast<uint8_t>(0xD4));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0xC3));
    EXPECT_EQ(payload[2], static_cast<uint8_t>(0xB2));
    EXPECT_EQ(payload[3], static_cast<uint8_t>(0xA1));
}

TEST_F(GetBytesTests, S32ToBytesLittleEndianNegative)
{
    int32_t value      = -1; // 0xFFFFFFFF
    uint8_t payload[4] = {0};

    getBytes_s32ToBytesLe(value, payload, sizeof(payload));

    EXPECT_EQ(payload[0], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[2], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[3], static_cast<uint8_t>(0xFF));
}
/* getBytes_u16ToBytesLe */
TEST_F(GetBytesTests, U16ToBytesLittleEndian)
{
    uint8_t payload[2] = {0};
    getBytes_u16ToBytesLe(0x1234U, payload, sizeof(payload));
    EXPECT_EQ(payload[0], static_cast<uint8_t>(0x34));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0x12));
}
TEST_F(GetBytesTests, S16ToBytesLittleEndian)
{
    uint8_t payload[2] = {0};
    getBytes_s16ToBytesLe(0x1234, payload, sizeof(payload));
    EXPECT_EQ(payload[0], static_cast<uint8_t>(0x34));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0x12));
}
/* getBytes_u16getLsbLe, getBytes_u16getMsbLe */
TEST_F(GetBytesTests, U16LsbMsbLittleEndian)
{
    EXPECT_EQ(getBytes_u16getLsbLe(0x1234U), static_cast<uint8_t>(0x34));
    EXPECT_EQ(getBytes_u16getMsbLe(0x1234U), static_cast<uint8_t>(0x12));
    EXPECT_EQ(getBytes_u16getLsbLe(0xABCDU), static_cast<uint8_t>(0xCD));
    EXPECT_EQ(getBytes_u16getMsbLe(0xABCDU), static_cast<uint8_t>(0xAB));
}
/* getBytes_s16getLsbLe, getBytes_s16getMsbLe */
TEST_F(GetBytesTests, S16LsbMsbLittleEndian)
{
    EXPECT_EQ(getBytes_s16getLsbLe(0x1234), static_cast<uint8_t>(0x34));
    EXPECT_EQ(getBytes_s16getMsbLe(0x1234), static_cast<uint8_t>(0x12));
    EXPECT_EQ(getBytes_s16getLsbLe(-1), static_cast<uint8_t>(0xFF));
    EXPECT_EQ(getBytes_s16getMsbLe(-1), static_cast<uint8_t>(0xFF));
}
/* getBytes_encodeS16Le */
TEST_F(GetBytesTests, encodeS16ToBytesLittleEndian)
{
    int16_t value      = 0x1234;
    uint8_t payload[2] = {0};

    getBytes_encodeS16Le(value, &payload[0], &payload[1]);

    EXPECT_EQ(payload[0], static_cast<uint8_t>(0x34));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0x12));
}

TEST_F(GetBytesTests, encodeS16ToBytesLittleEndianNegative)
{
    int16_t value      = -1; // 0xFFFF
    uint8_t payload[2] = {0};

    getBytes_encodeS16Le(value, &payload[0], &payload[1]);

    EXPECT_EQ(payload[0], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0xFF));
}

TEST_F(GetBytesTests, encodeS16ToBytesLittleEndianNullptr)
{
    int16_t value = 0x1234;

    /* Should not crash or write to memory when passed nullptrs. */
    getBytes_encodeS16Le(value, nullptr, nullptr);
}

TEST_F(GetBytesTests, encodeS16ToBytesLittleEndianNullLowByte)
{
    int16_t value     = 0x1234;
    uint8_t high_byte = 0;

    /* Should not crash or write to memory when passed nullptr for low byte. */
    getBytes_encodeS16Le(value, nullptr, &high_byte);

    EXPECT_EQ(high_byte, static_cast<uint8_t>(0x12));
}

TEST_F(GetBytesTests, encodeS16ToBytesLittleEndianNullHighByte)
{
    int16_t value    = 0x1234;
    uint8_t low_byte = 0;

    /* Should not crash or write to memory when passed nullptr for high byte. */
    getBytes_encodeS16Le(value, &low_byte, nullptr);

    EXPECT_EQ(low_byte, static_cast<uint8_t>(0x34));
}
TEST_F(GetBytesTests, encodeS16ToBytesLittleEndianNullHighByteNegative)
{
    int16_t value    = -1; // 0xFFFF
    uint8_t low_byte = 0;

    /* Should not crash or write to memory when passed nullptr for high byte. */
    getBytes_encodeS16Le(value, &low_byte, nullptr);

    EXPECT_EQ(low_byte, static_cast<uint8_t>(0xFF));
}
/* getBytes_decodeU16Le */
TEST_F(GetBytesTests, DecodeU16Le)
{
    uint8_t low_byte  = 0x34;
    uint8_t high_byte = 0x12;
    uint16_t value    = 0;

    getBytes_decodeU16Le(low_byte, high_byte, &value);

    EXPECT_EQ(value, static_cast<uint16_t>(0x1234));
}

/* getBytes_decodeS16Le */
TEST_F(GetBytesTests, DecodeS16Le)
{
    uint8_t low_byte  = 0x34;
    uint8_t high_byte = 0x12;
    int16_t value     = 0;

    getBytes_decodeS16Le(low_byte, high_byte, &value);

    EXPECT_EQ(value, static_cast<int16_t>(0x1234));
}
/*getBytes_bytesToU32Le*/
TEST_F(GetBytesTests, BytesToU32LittleEndian)
{
    const uint8_t le[4] = {0x78, 0x56, 0x34, 0x12};
    EXPECT_EQ(getBytes_bytesToU32Le(le), static_cast<uint32_t>(0x12345678U));
}

/* getBytes_bytesToS32Le */
TEST_F(GetBytesTests, BytesToS32LittleEndian)
{
    const uint8_t le[4] = {0x78, 0x56, 0x34, 0x12};
    EXPECT_EQ(getBytes_bytesToS32Le(le), static_cast<int32_t>(0x12345678));
}

/*getBytes_encodeU16Le*/
TEST_F(GetBytesTests, EncodeU16Le)
{
    uint16_t value    = 0x1234U;
    uint8_t low_byte  = 0;
    uint8_t high_byte = 0;

    getBytes_encodeU16Le(value, &low_byte, &high_byte);

    EXPECT_EQ(low_byte, static_cast<uint8_t>(0x34));
    EXPECT_EQ(high_byte, static_cast<uint8_t>(0x12));
}

/* getBytes_u64ToBytesLe*/
TEST_F(GetBytesTests, U64ToBytesLittleEndian)
{
    uint64_t value     = 0xFEDCBA9876543210ULL;
    uint8_t payload[8] = {0};

    getBytes_u64ToBytesLe(value, payload, sizeof(payload));

    EXPECT_EQ(payload[0], static_cast<uint8_t>(0x10));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0x32));
    EXPECT_EQ(payload[2], static_cast<uint8_t>(0x54));
    EXPECT_EQ(payload[3], static_cast<uint8_t>(0x76));
    EXPECT_EQ(payload[4], static_cast<uint8_t>(0x98));
    EXPECT_EQ(payload[5], static_cast<uint8_t>(0xBA));
    EXPECT_EQ(payload[6], static_cast<uint8_t>(0xDC));
    EXPECT_EQ(payload[7], static_cast<uint8_t>(0xFE));
}

/* getBytes_s64ToBytesLe*/
TEST_F(GetBytesTests, S64ToBytesLittleEndian)
{
    int64_t value      = 0x0123456789ABCDEFLL;
    uint8_t payload[8] = {0};

    getBytes_s64ToBytesLe(value, payload, sizeof(payload));

    EXPECT_EQ(payload[0], static_cast<uint8_t>(0xEF));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0xCD));
    EXPECT_EQ(payload[2], static_cast<uint8_t>(0xAB));
    EXPECT_EQ(payload[3], static_cast<uint8_t>(0x89));
    EXPECT_EQ(payload[4], static_cast<uint8_t>(0x67));
    EXPECT_EQ(payload[5], static_cast<uint8_t>(0x45));
    EXPECT_EQ(payload[6], static_cast<uint8_t>(0x23));
    EXPECT_EQ(payload[7], static_cast<uint8_t>(0x01));
}
TEST_F(GetBytesTests, S64ToBytesLittleEndianNegative)
{
    int64_t value      = -1; // 0xFFFFFFFFFFFFFFFF
    uint8_t payload[8] = {0};

    getBytes_s64ToBytesLe(value, payload, sizeof(payload));

    EXPECT_EQ(payload[0], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[1], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[2], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[3], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[4], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[5], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[6], static_cast<uint8_t>(0xFF));
    EXPECT_EQ(payload[7], static_cast<uint8_t>(0xFF));
}
