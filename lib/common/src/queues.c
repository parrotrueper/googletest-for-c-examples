/**
 * \file queues.c
 * \brief  implementation of a circular buffer, "queues" not queue so it does not
 * clash with standard queue libraries
 **/
#include <stddef.h>

#include "queues.h"

/* False positives from pclint, are triggered because it does not find calls
to some of these functions outside of this module, even though they are called
from other modules. */
/*lint -esym{714, queues_popByte} */
/*lint -esym{714, queues_popData} */
/*lint -esym{714, queues_pushByte} */
/*lint -esym{714, queues_reset} */
/*lint -esym{714, queues_initialise} */

/**
 * \brief checks whether a queue is valid and initialised
 * \param queue_instance Pointer to the circular buffer instance
 * \returns true if valid and initialised, false if not
 **/
static inline bool queues_isValid(const st_queues_instance_t* queue_instance)
{
    bool result = false;
    if ( queue_instance != NULL ) {
        result = queue_instance->is_initialised;
    }
    return result;
}
/**
 * \brief check whether there is enough space in the buffer to write x number of
 * bytes
 * \param queue_instance Pointer to the circular buffer instance
 * \param in_length number of bytes to write
 * \returns true if enough space, false if not
 **/
static inline bool queues_hasSpace(const st_queues_instance_t* queue_instance,
                                   uint16_t in_length)
{
    return ((queues_getNumBytesPending(queue_instance) + in_length)
            < queue_instance->length);
}

/**
 * \brief check how much of the amount of desired data can actually be read
 * \param queue_instance Pointer to the circular buffer instance
 * \param in_length number of bytes desired to read
 * \returns number of bytes that can actually be read
 **/
static inline uint16_t queues_getReadableAmount(
    const st_queues_instance_t* queue_instance, uint16_t in_length)
{
    uint16_t available = 0;
    uint16_t amount    = 0;

    /* how much data can actually be read from the buffer */
    available = queues_getNumBytesPending(queue_instance);

    /* If they want to read less then we'll only read that amount */
    if ( in_length < available ) {
        amount = in_length;
    } else {
        amount = available;
    }

    return amount;
}
/**
 * \brief helper to update read and write offsets with wrap around
 * \param offset current offset
 * \param length length of the buffer
 * \returns updated offset
 */
static inline uint16_t queues_updateOffset(uint16_t offset, uint16_t length)
{
    uint16_t new_offset = offset;

    /* Have we come to the end of the buffer? */
    if ( new_offset == (length - 1U) ) {
        /* Yes - back to start again */
        new_offset = 0;
    } else {
        new_offset++;
    }
    return new_offset;
}
/**
 * \brief Get the number of bytes in the buffer that are available to be read
 * \param queue_instance Pointer to the circular buffer instance
 * \returns number of bytes available to be read
 **/
uint16_t queues_getNumBytesPending(const st_queues_instance_t* queue_instance)
{
    uint16_t write_position = 0;
    uint16_t read_position  = 0;
    uint16_t amount         = 0;

    if ( queues_isValid(queue_instance) ) {
        /* Take snapshot of read and write offset as these could be changed by
        an interrupt */
        write_position = queue_instance->write_offset;
        read_position  = queue_instance->read_offset;
    }

    /* Write pointer ahead of read pointer? */
    if ( write_position >= read_position ) {
        /* Amount of data is Write Pos - Read Pos */
        amount = write_position - read_position;
    } else
    /* Write Pointer is before Read Pointer (wrap around case) */
    {
        /* Amount of data is the buffer size, minus the difference between the
        read and write offsets */
        amount = queue_instance->length;
        amount -= (uint16_t)(read_position - write_position);
    }

    return amount;
}

/**
 * \brief checks whether the queue is full
 * \param queue_instance Pointer to the circular buffer instance
 * \returns true if full, false if not
 **/
bool queues_isFull(const st_queues_instance_t* queue_instance)
{
    bool result = false;

    if ( queues_isValid(queue_instance) ) {
        result = ((queues_getNumBytesPending(queue_instance) + 1U)
                  == queue_instance->length);
    }

    return result;
}

/**
 * \brief check whether the queue is empty
 * \param queue_instance Pointer to the circular buffer instance
 * \returns true if empty, false if not
 */
bool queues_isEmpty(const st_queues_instance_t* queue_instance)
{
    bool result = false;

    if ( queues_isValid(queue_instance) ) {
        uint16_t write_position = queue_instance->write_offset;
        uint16_t read_position  = queue_instance->read_offset;
        if ( write_position == read_position ) {
            result = true;
        }
    }
    return result;
}

/**
 * \brief read one byte from the queue if available
 * \param queue_instance Pointer to the circular buffer instance
 * \param out_buffer pointer to variable to put read byte into
 * \returns true if successful, false if not
 */
bool queues_popByte(st_queues_instance_t* queue_instance, uint8_t* out_buffer)
{
    bool result = false;

    if ( queues_isValid(queue_instance) && (out_buffer != NULL) ) {
        /* Is there any data to read? */
        result = !queues_isEmpty(queue_instance);
    }

    if ( result ) {
        *out_buffer = queue_instance->buffer[queue_instance->read_offset];

        queue_instance->read_offset = queues_updateOffset(
            queue_instance->read_offset, queue_instance->length);
    }

    return result;
}

/**
 * \brief read a number of bytes from the queue
 * \param queue_instance Pointer to the circular buffer instance
 * \param out_buffer pointer to buffer to put read data into
 * \param read_length number of bytes to read
 * \returns number of bytes actually read
 */
uint16_t queues_popData(st_queues_instance_t* queue_instance,
                        uint8_t* out_buffer, uint16_t read_length)
{
    uint16_t copied     = 0;
    uint16_t byte_count = 0;

    if ( queues_isValid(queue_instance) && (out_buffer != NULL)
         && (read_length != 0U) ) {
        copied = queues_getReadableAmount(queue_instance, read_length);
    }

    for ( byte_count = 0; byte_count < copied; byte_count++ ) {
        out_buffer[byte_count] = queue_instance
                                     ->buffer[queue_instance->read_offset];

        /* Have we come to the end of the buffer? */
        queue_instance->read_offset = queues_updateOffset(
            queue_instance->read_offset, queue_instance->length);
    }

    return copied;
}

/**
 * \brief write a number of bytes into the queue
 * \param queue_instance Pointer to the circular buffer instance
 * \param in_data pointer to data to write
 * \param in_length number of bytes to write
 * \returns true if successful, false if not
 */
bool queues_pushData(st_queues_instance_t* queue_instance,
                     const uint8_t* in_data, uint16_t in_length)
{
    uint16_t write_offset;
    uint16_t byte_count;
    bool result = false;

    if ( queues_isValid(queue_instance) && (in_data != NULL)
         && (in_length != 0U) ) {
        result = queues_hasSpace(queue_instance, in_length);
    }

    /* Do we have enough space in the buffer? */
    if ( result ) {
        /* Take a snapshot of the position of the write offset pointer. We only
        update it at the end rather than incrementally show the data in the
        buffer */
        write_offset = queue_instance->write_offset;

        for ( byte_count = 0; byte_count < in_length; byte_count++ ) {
            queue_instance->buffer[write_offset] = in_data[byte_count];

            write_offset = queues_updateOffset(write_offset,
                                               queue_instance->length);
        }

        /* Update the write pointer so the data "appears in the buffer" */
        queue_instance->write_offset = write_offset;
    }

    return result;
}

/**
 * \brief write one byte into the queue
 * \param queue_instance Pointer to the circular buffer instance
 * \param in_byte byte to write
 * \returns true if successful, false if not
 */
bool queues_pushByte(st_queues_instance_t* queue_instance, uint8_t in_byte)
{
    return queues_pushData(queue_instance, &in_byte, 1);
}

/**
 * \brief reset the queue to empty
 * \param queue_instance Pointer to the circular buffer instance
 */
void queues_reset(st_queues_instance_t* queue_instance)
{
    if ( queues_isValid(queue_instance) ) {
        queue_instance->write_offset = 0;
        queue_instance->read_offset  = 0;
    }
}

/**
 * \brief Initialise a buffer so it can work as a circular buffer
 * \param raw_buffer Pointer to buffer to become circular buffer
 * \param length Size of the buffer in bytes
 * \param queue_instance Pointer to structure that is populated and used as an
 * instance handle for all other exposed interfaces
 * \returns true if successful, false if not
 */
bool queues_initialise(uint8_t* raw_buffer, uint16_t length,
                       st_queues_instance_t* queue_instance)
{
    bool initialised = false;

    if ( queue_instance != NULL ) {
        queue_instance->is_initialised = false;

        if ( (raw_buffer != NULL) && (length != 0U) ) {
            queue_instance->buffer         = raw_buffer;
            queue_instance->length         = length;
            queue_instance->write_offset   = 0;
            queue_instance->read_offset    = 0;
            queue_instance->is_initialised = true;
        }
        initialised = queue_instance->is_initialised;
    }

    return initialised;
}
