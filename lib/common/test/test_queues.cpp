#include <gtest/gtest.h>

#include "queues.h"
using namespace testing;

/* ------------------------------------------------------------
 * Test cases for queues module
 * ------------------------------------------------------------ */
/* queues_initialise
-----------------------------*/
TEST(Queues, initialise_null_instance)
{
    uint8_t buffer[10];
    bool result = queues_initialise(buffer, sizeof(buffer), NULL);
    EXPECT_FALSE(result);
}
TEST(Queues, initialise_null_buffer)
{
    st_queues_instance_t queue_instance;
    bool result = queues_initialise(NULL, 10, &queue_instance);
    EXPECT_FALSE(result);
}
TEST(Queues, initialise_zero_length)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[10];
    bool result = queues_initialise(buffer, 0, &queue_instance);
    EXPECT_FALSE(result);
}
TEST(Queues, initialise_valid)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[10];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);
}

/* queues_reset
-----------------------------*/
TEST(TestQueues, queues_reset_valid)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[10];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    /* Push some data */
    result = queues_pushByte(&queue_instance, 0x55);
    EXPECT_TRUE(result);
    result = queues_pushByte(&queue_instance, 0xAA);
    EXPECT_TRUE(result);

    /* Reset the queue */
    queues_reset(&queue_instance);

    /* Check that it is empty */
    EXPECT_TRUE(queues_isEmpty(&queue_instance));
}

/* queues_pushByte
-----------------------------*/
TEST(Queues, pushByte_valid)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[5];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    /* Push bytes until full */
    for ( size_t i = 0; i < sizeof(buffer) - 1; i++ ) {
        result = queues_pushByte(&queue_instance, static_cast<uint8_t>(i));
        EXPECT_TRUE(result);
    }

    /* Next push should fail as the buffer is full */
    result = queues_pushByte(&queue_instance, 0xFF);
    EXPECT_FALSE(result);
}

/* queues_pushData
-----------------------------*/
TEST(Queues, pushData_valid)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[10];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    uint8_t data_to_push[6] = {1, 2, 3, 4, 5, 6};

    /* Push data */
    result = queues_pushData(&queue_instance, data_to_push,
                             sizeof(data_to_push));
    EXPECT_TRUE(result);

    /* Check number of bytes pending */
    uint16_t pending = queues_getNumBytesPending(&queue_instance);
    EXPECT_EQ(pending, sizeof(data_to_push));
}

/* queues_popData
-----------------------------*/
TEST(Queues, popData_valid)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[10];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    uint8_t data_to_push[6] = {1, 2, 3, 4, 5, 6};

    /* Push data */
    result = queues_pushData(&queue_instance, data_to_push,
                             sizeof(data_to_push));
    EXPECT_TRUE(result);

    uint8_t data_popped[6] = {0};

    /* Pop data */
    uint16_t bytes_popped = queues_popData(&queue_instance, data_popped,
                                           sizeof(data_popped));
    EXPECT_EQ(bytes_popped, sizeof(data_to_push));

    /* Verify popped data */
    for ( size_t i = 0; i < sizeof(data_to_push); i++ ) {
        EXPECT_EQ(data_popped[i], data_to_push[i]);
    }
}

/* queues_popByte
-----------------------------*/
TEST(Queues, popByte_valid)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[5];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    /* Push a byte */
    result = queues_pushByte(&queue_instance, 0xAB);
    EXPECT_TRUE(result);

    uint8_t byte_popped = 0;

    /* Pop the byte */
    result = queues_popByte(&queue_instance, &byte_popped);
    EXPECT_TRUE(result);
    EXPECT_EQ(byte_popped, 0xAB);

    /* Next pop should fail as the buffer is empty */
    result = queues_popByte(&queue_instance, &byte_popped);
    EXPECT_FALSE(result);
}

/* queues_isEmpty
-----------------------------*/
TEST(Queues, isEmpty_valid)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[5];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    /* Initially should be empty */
    EXPECT_TRUE(queues_isEmpty(&queue_instance));

    /* Push a byte */
    result = queues_pushByte(&queue_instance, 0xCD);
    EXPECT_TRUE(result);

    /* Now should not be empty */
    EXPECT_FALSE(queues_isEmpty(&queue_instance));

    /* Pop the byte */
    uint8_t byte_popped = 0;
    result              = queues_popByte(&queue_instance, &byte_popped);
    EXPECT_TRUE(result);

    /* Should be empty again */
    EXPECT_TRUE(queues_isEmpty(&queue_instance));
}

/* queues_isFull
-----------------------------*/
TEST(Queues, isFull_valid)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[5];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    /* Initially should not be full */
    EXPECT_FALSE(queues_isFull(&queue_instance));
    EXPECT_TRUE(queues_isEmpty(&queue_instance));

    /* Push bytes until full */
    for ( size_t i = 0; i < sizeof(buffer) - 1; i++ ) {
        result = queues_pushByte(&queue_instance, static_cast<uint8_t>(i));
        EXPECT_TRUE(result);
    }

    /* Now should be full */
    EXPECT_TRUE(queues_isFull(&queue_instance));

    /* Pop a byte */
    uint8_t byte_popped = 0;
    result              = queues_popByte(&queue_instance, &byte_popped);
    EXPECT_TRUE(result);

    /* Should not be full now */
    EXPECT_FALSE(queues_isFull(&queue_instance));
}

/* queues_getNumBytesPending
-----------------------------*/
TEST(Queues, getNumBytesPending_valid)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[10];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    /* Initially should have 0 bytes pending */
    EXPECT_EQ(queues_getNumBytesPending(&queue_instance), 0);

    /* Push some bytes */
    for ( size_t i = 0; i < 5; i++ ) {
        result = queues_pushByte(&queue_instance, static_cast<uint8_t>(i));
        EXPECT_TRUE(result);
    }

    /* Should have 5 bytes pending */
    EXPECT_EQ(queues_getNumBytesPending(&queue_instance), 5);

    /* Pop some bytes */
    uint8_t byte_popped = 0;
    for ( size_t i = 0; i < 3; i++ ) {
        result = queues_popByte(&queue_instance, &byte_popped);
        EXPECT_TRUE(result);
    }

    /* Should have 2 bytes pending */
    EXPECT_EQ(queues_getNumBytesPending(&queue_instance), 2);
}
TEST(Queues, getNumBytesPending_wrap_around)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[4];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    /* Fill buffer to capacity-1 (circular buffer leaves one slot empty) */
    for ( size_t i = 0; i < sizeof(buffer) - 1; ++i ) {
        result = queues_pushByte(&queue_instance, static_cast<uint8_t>(i + 1));
        EXPECT_TRUE(result);
    }
    EXPECT_TRUE(queues_isFull(&queue_instance));
    EXPECT_EQ(queues_getNumBytesPending(&queue_instance), 3);

    /* Pop two bytes */
    uint8_t byte_popped = 0;
    for ( size_t i = 0; i < 2; ++i ) {
        result = queues_popByte(&queue_instance, &byte_popped);
        EXPECT_TRUE(result);
    }
    EXPECT_EQ(queues_getNumBytesPending(&queue_instance), 1);

    /* Push two more bytes to force wrap-around */
    for ( size_t i = 0; i < 2; ++i ) {
        result = queues_pushByte(&queue_instance, static_cast<uint8_t>(i + 10));
        EXPECT_TRUE(result);
    }
    EXPECT_TRUE(queues_isFull(&queue_instance));
    EXPECT_EQ(queues_getNumBytesPending(&queue_instance), 3);
}

/* Wrap-around and edge cases */
TEST(Queues, wrap_around_push_and_pop)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[4];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    // Fill buffer to capacity-1 (circular buffer leaves one slot empty)
    for ( size_t i = 0; i < sizeof(buffer) - 1; ++i ) {
        result = queues_pushByte(&queue_instance, static_cast<uint8_t>(i + 1));
        EXPECT_TRUE(result);
    }
    EXPECT_TRUE(queues_isFull(&queue_instance));

    // Pop one byte, then push another to force wrap-around
    uint8_t byte_popped = 0;
    result              = queues_popByte(&queue_instance, &byte_popped);
    EXPECT_TRUE(result);
    EXPECT_EQ(byte_popped, 1);

    result = queues_pushByte(&queue_instance, 99);
    EXPECT_TRUE(result);

    // Pop all and check order (should be 2,3,99)
    result = queues_popByte(&queue_instance, &byte_popped);
    EXPECT_TRUE(result);
    EXPECT_EQ(byte_popped, 2);
    result = queues_popByte(&queue_instance, &byte_popped);
    EXPECT_TRUE(result);
    EXPECT_EQ(byte_popped, 3);
    result = queues_popByte(&queue_instance, &byte_popped);
    EXPECT_TRUE(result);
    EXPECT_EQ(byte_popped, 99);
    // Now empty
    EXPECT_TRUE(queues_isEmpty(&queue_instance));
}

TEST(Queues, pushData_too_large)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[5];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    uint8_t data[5] = {1, 2, 3, 4, 5};
    // Only 4 bytes can be stored (length-1 rule)
    result = queues_pushData(&queue_instance, data, 5);
    EXPECT_FALSE(result);
}

TEST(Queues, popData_partial_and_null)
{
    st_queues_instance_t queue_instance;
    uint8_t buffer[6];
    bool result = queues_initialise(buffer, sizeof(buffer), &queue_instance);
    EXPECT_TRUE(result);

    uint8_t data[4] = {10, 20, 30, 40};
    result          = queues_pushData(&queue_instance, data, 4);
    EXPECT_TRUE(result);

    uint8_t out[2] = {0};
    // Pop only 2 bytes
    uint16_t popped = queues_popData(&queue_instance, out, 2);
    EXPECT_EQ(popped, 2);
    EXPECT_EQ(out[0], 10);
    EXPECT_EQ(out[1], 20);

    // Pop with NULL buffer
    popped = queues_popData(&queue_instance, NULL, 2);
    EXPECT_EQ(popped, 0);

    // Pop with zero length
    popped = queues_popData(&queue_instance, out, 0);
    EXPECT_EQ(popped, 0);
}

TEST(Queues, pushByte_and_pushData_null_instance)
{
    uint8_t data[2] = {1, 2};
    // Should fail with null instance
    bool result = queues_pushByte(NULL, 0x55);
    EXPECT_FALSE(result);
    result = queues_pushData(NULL, data, 2);
    EXPECT_FALSE(result);
}

TEST(TestQueues, queues_reset_null_instance)
{
    // Should not crash
    queues_reset(NULL);
    SUCCEED();
}
