#include <stddef.h>

#include "getBytes.h"

/*******************************************************************************
 *                         Private Functions
 ******************************************************************************/
static inline uint8_t getBytes_shiftAndMask64(uint64_t value, uint8_t shift)
{
    return (uint8_t)((value >> shift) & (uint64_t)(0xFF));
}

/*******************************************************************************
 *                      Exposed Interfaces
 *****************************************************************************/
/**
 * \brief helper gets low byte from uint16_t
 * \param value - input uint16_t value
 * \return low byte
 * \note used when serialising values as high-byte then low-byte
 */
uint8_t getBytes_u16getLoByteBe(uint16_t value)
{
    return (uint8_t)(value & 0x00FFU);
}
/**
 * \brief helper gets high byte from uint16_t
 * \param value - input uint16_t value
 * \return high byte
 * \note used when serialising values as high-byte then low-byte
 */
uint8_t getBytes_u16getHiByteBe(uint16_t value)
{
    return (uint8_t)((value >> 8U) & 0x00FFU);
}
/**
 * \brief helper gets low byte from int16_t
 * \param value - input int16_t value
 * \return low byte
 * \note used when serialising values as high-byte then low-byte
 */
uint8_t getBytes_s16getLoByteBe(int16_t value)
{
    uint16_t uvalue = (uint16_t)value;

    return (uint8_t)(uvalue & 0x00FFU);
}
/**
 * \brief helper gets high byte from int16_t
 * \param value - input int16_t value
 * \return high byte
 * \note used when serialising values as high-byte then low-byte
 */
uint8_t getBytes_s16getHiByteBe(int16_t value)
{
    uint16_t uvalue = (uint16_t)value;

    return (uint8_t)((uvalue >> 8U) & 0x00FFU);
}

/**
 * \brief convert 2 bytes in payload to uint16_t
 * \param payload - pointer to 2 byte payload
 * \return converted uint16_t value
 * \note caller is responsible for ensuring payload is not null and has at least
 * 2 bytes
 * \note this assumes payload is in big-endian format (high byte first)
 */
uint16_t getBytes_bytesToU16(const uint8_t* payload)
{
    return (uint16_t)(((uint16_t)payload[0] << 8U) | (uint16_t)payload[1]);
}
/**
 * \brief convert 2 bytes in payload to uint16_t
 * \param payload - pointer to 2 byte payload
 * \return converted uint16_t value
 * \note caller is responsible for ensuring payload is not null and has at least
 * 2 bytes
 * \note this assumes payload is in little-endian format (low byte first)
 */
uint16_t getBytes_bytesToU16Le(const uint8_t* payload)
{
    return (uint16_t)(((uint16_t)payload[1] << 8U) | (uint16_t)payload[0]);
}

/**
 * \brief convert 2 bytes in payload to int16_t
 * \param payload - pointer to 2 byte payload
 * \return converted int16_t value
 * \note caller is responsible for ensuring payload is not null and has at least
 * 2 bytes
 * \note this assumes payload is in little-endian format (low byte first)
 */
int16_t getBytes_bytesToS16Le(const uint8_t* payload)
{
    return (int16_t)(getBytes_bytesToU16Le(payload));
}
/**
 * \brief convert 2 bytes in payload to int16_t
 * \param payload - pointer to 2 byte payload
 * \return converted int16_t value
 * \note caller is responsible for ensuring payload is not null and has at least
 * 2 bytes
 * \note this assumes payload is in big-endian format (high byte first)
 */
int16_t getBytes_bytesToS16Be(const uint8_t* payload)
{
    uint16_t combined = ((uint16_t)payload[0] << 8U) | (uint16_t)payload[1];

    return (int16_t)combined;
}

/**
 * \brief get byte 0 from int32_t (least significant byte)
 * \param value - input int32_t value
 * \return byte 0 (least significant byte)
 */
uint8_t getBytes_s32getByte0(int32_t value)
{
    uint32_t uvalue = (uint32_t)value;

    return (uint8_t)(uvalue & (uint32_t)(0xFF));
}

/**
 * \brief get byte 1 from int32_t
 * \param value - input int32_t value
 * \return byte 1
 */
uint8_t getBytes_s32getByte1(int32_t value)
{
    uint32_t uvalue = (uint32_t)value;

    return (uint8_t)((uvalue >> 8U) & (uint32_t)(0xFF));
}
/**
 * \brief get byte 2 from int32_t
 * \param value - input int32_t value
 * \return byte 2
 */
uint8_t getBytes_s32getByte2(int32_t value)
{
    uint32_t uvalue = (uint32_t)value;

    return (uint8_t)((uvalue >> 16U) & (uint32_t)(0xFF));
}
/**
 * \brief get byte 3 from int32_t (most significant byte)
 * \param value - input int32_t value
 * \return byte 3 (most significant byte)
 */
uint8_t getBytes_s32getByte3(int32_t value)
{
    uint32_t uvalue = (uint32_t)value;

    return (uint8_t)((uvalue >> 24U) & (uint32_t)(0xFF));
}

/**
 * \brief convert u32_t value to 4 bytes and store in payload
 * \param value - input uint32_t value
 * \param payload - pointer to buffer where bytes should be stored
 * \param payload_size - size of the payload buffer (must have space for at
 * least 4 bytes)
 * \note Little-endian format (low byte first)
 */
void getBytes_u32ToBytesLe(uint32_t value, uint8_t* payload,
                           uint16_t payload_size)
{
    if ( (payload != NULL) && (payload_size >= 4U) ) {
        payload[0] = (uint8_t)(value & (uint32_t)(0xFF));
        payload[1] = (uint8_t)((value >> 8U) & (uint32_t)(0xFF));
        payload[2] = (uint8_t)((value >> 16U) & (uint32_t)(0xFF));
        payload[3] = (uint8_t)((value >> 24U) & (uint32_t)(0xFF));
    }
}

/**
 * \brief convert u16_t value to 2 bytes and store in payload
 * \param value - input uint16_t value
 * \param payload - pointer to buffer where bytes should be stored
 * \param payload_size - size of the payload buffer (must have space for at
 * least 2 bytes)
 * \note Little-endian format (low byte first)
 */
void getBytes_u16ToBytesLe(uint16_t value, uint8_t* payload,
                           uint16_t payload_size)
{
    if ( (payload != NULL) && (payload_size >= 2U) ) {
        payload[0] = (uint8_t)(value & (uint16_t)(0xFF));
        payload[1] = (uint8_t)((value >> 8U) & (uint16_t)(0xFF));
    }
}
/**
 * \brief convert s16_t value to 2 bytes and store in payload
 * \param value - input int16_t value
 * \param payload - pointer to buffer where bytes should be stored
 * \param payload_size - size of the payload buffer (must have space for at
 * least 2 bytes)
 * \note Little-endian format (low byte first)
 */
void getBytes_s16ToBytesLe(int16_t value, uint8_t* payload,
                           uint16_t payload_size)
{
    if ( (payload != NULL) && (payload_size >= 2U) ) {
        uint16_t uvalue = (uint16_t)value;
        payload[0]      = (uint8_t)(uvalue & (uint16_t)(0xFF));
        payload[1]      = (uint8_t)((uvalue >> 8U) & (uint16_t)(0xFF));
    }
}
/**
 * \brief get the LSB of a uint16_t value in little-endian format (low byte first)
 * \param value - input uint16_t value
 * \return low byte
 */
uint8_t getBytes_u16getLsbLe(uint16_t value)
{
    return (uint8_t)(value & (uint16_t)(0xFF));
}
/**
 * \brief get the MSB of a uint16_t value in little-endian format (low byte first)
 * \param value - input uint16_t value
 * \return high byte
 */
uint8_t getBytes_u16getMsbLe(uint16_t value)
{
    return (uint8_t)((value >> 8U) & (uint16_t)(0xFF));
}
/**
 * \brief convert i32_t value to 4 bytes and store in payload
 * \param value - input int32_t value
 * \param payload - pointer to buffer where bytes should be stored
 * \param payload_size - size of the payload buffer (must have space for at
 * least 4 bytes)
 * \note Little-endian format (low byte first)
 */
void getBytes_s32ToBytesLe(int32_t value, uint8_t* payload,
                           uint16_t payload_size)
{
    if ( (payload != NULL) && (payload_size >= 4U) ) {
        uint32_t uvalue = (uint32_t)value;

        payload[0] = (uint8_t)(uvalue & (uint32_t)(0xFF));
        payload[1] = (uint8_t)((uvalue >> 8U) & (uint32_t)(0xFF));
        payload[2] = (uint8_t)((uvalue >> 16U) & (uint32_t)(0xFF));
        payload[3] = (uint8_t)((uvalue >> 24U) & (uint32_t)(0xFF));
    }
}

/**
 * \brief convert 4 bytes in payload to uint32_t
 * \param payload - pointer to 4 byte payload
 * \return converted uint32_t value
 * \note caller is responsible for ensuring payload is not null and has at least
 * 4 bytes
 * \note this assumes payload is in little-endian format (low byte first)
 */
uint32_t getBytes_bytesToU32Le(const uint8_t* payload)
{
    return (uint32_t)(((uint32_t)payload[3] << 24U)
                      | ((uint32_t)payload[2] << 16U)
                      | ((uint32_t)payload[1] << 8U) | (uint32_t)payload[0]);
}
/**
 * \brief convert 4 bytes in payload to int32_t
 * \param payload - pointer to 4 byte payload
 * \return converted int32_t value
 * \note caller is responsible for ensuring payload is not null and has at least
 * 4 bytes
 * \note this assumes payload is in little-endian format (low byte first)
 */
int32_t getBytes_bytesToS32Le(const uint8_t* payload)
{
    return (int32_t)(getBytes_bytesToU32Le(payload));
}
/**
 * \brief get the LSB of a int16_t value in little-endian format (low byte first)
 * \param value - input int16_t value
 * \return low byte
 */
uint8_t getBytes_s16getLsbLe(int16_t value)
{
    uint16_t uvalue = (uint16_t)value;

    return (uint8_t)(uvalue & (uint16_t)(0xFF));
}

/**
 * \brief get the MSB of a int16_t value in little-endian format (low byte first)
 * \param value - input int16_t value
 * \return high byte
 */
uint8_t getBytes_s16getMsbLe(int16_t value)
{
    uint16_t uvalue = (uint16_t)value;

    return (uint8_t)((uvalue >> 8U) & (uint16_t)(0xFF));
}

/**
 * \brief convert a uint16_t value to 2 bytes and store in payload in little-endian format
 * \param value - input uint16_t value
 * \param low_byte - pointer to store low byte (LSB)
 * \param high_byte - pointer to store high byte (MSB)
 */
void getBytes_encodeU16Le(uint16_t value, uint8_t* low_byte, uint8_t* high_byte)
{
    if ( low_byte != NULL ) {
        *low_byte = getBytes_u16getLsbLe(value);
    }
    if ( high_byte != NULL ) {
        *high_byte = getBytes_u16getMsbLe(value);
    }
}

/**
 * \brief convert a int16_t value to 2 bytes and store in payload in little-endian format
 * \param value - input int16_t value
 * \param low_byte - pointer to store low byte (LSB)
 * \param high_byte - pointer to store high byte (MSB)
 */
void getBytes_encodeS16Le(int16_t value, uint8_t* low_byte, uint8_t* high_byte)
{
    if ( low_byte != NULL ) {
        *low_byte = getBytes_s16getLsbLe(value);
    }
    if ( high_byte != NULL ) {
        *high_byte = getBytes_s16getMsbLe(value);
    }
}
/**
 * \brief convert two bytes to uint16_t
 * \param low_byte - low byte (LSB)
 * \param high_byte - high byte (MSB)
 * \param value - pointer to store converted uint16_t value
 */
void getBytes_decodeU16Le(uint8_t low_byte, uint8_t high_byte, uint16_t* value)
{
    if ( value != NULL ) {
        *value = (uint16_t)(((uint16_t)high_byte << 8U) | (uint16_t)low_byte);
    }
}

/**
 * \brief convert two bytes to int16_t
 * \param low_byte - low byte (LSB)
 * \param high_byte - high byte (MSB)
 * \param value - pointer to store converted int16_t value
 */
void getBytes_decodeS16Le(uint8_t low_byte, uint8_t high_byte, int16_t* value)
{
    if ( value != NULL ) {
        const uint16_t combined = ((uint16_t)high_byte << 8U)
                                  | (uint16_t)low_byte;
        *value = (int16_t)combined;
    }
}

/**
 * \brief convert uint64_t value to 8 bytes and store in payload in little-endian
 * format
 * \param value - input uint64_t value
 * \param payload - pointer to buffer where bytes should be stored
 * \param payload_size - size of the payload buffer (must have space for
 * at least 8 bytes)
 */
void getBytes_u64ToBytesLe(uint64_t value, uint8_t* payload,
                           uint16_t payload_size)
{
    if ( (payload != NULL) && (payload_size >= 8U) ) {
        payload[0] = (uint8_t)(value & 0xFFULL);
        payload[1] = getBytes_shiftAndMask64(value, 8U);
        payload[2] = getBytes_shiftAndMask64(value, 16U);
        payload[3] = getBytes_shiftAndMask64(value, 24U);
        payload[4] = getBytes_shiftAndMask64(value, 32U);
        payload[5] = getBytes_shiftAndMask64(value, 40U);
        payload[6] = getBytes_shiftAndMask64(value, 48U);
        payload[7] = getBytes_shiftAndMask64(value, 56U);
    }
}

/**
 * \brief convert int64_t value to 8 bytes and store in payload in little-endian
 * format
 * \param value - input int64_t value
 * \param payload - pointer to buffer where bytes should be stored
 * \param payload_size - size of the payload buffer (must have space for
 * at least 8 bytes)
 */
void getBytes_s64ToBytesLe(int64_t value, uint8_t* payload,
                           uint16_t payload_size)
{
    getBytes_u64ToBytesLe((uint64_t)value, payload, payload_size);
}
