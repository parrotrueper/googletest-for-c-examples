/**
 * \file queues.h
 * \brief  interfaces that implement a circular buffer aka queue.
 **/

#ifndef QUEUES_H__
#define QUEUES_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include <stdbool.h>
#include <stdint.h>


/**
 * \brief Structure that manages a circular buffer. Populated at initialisation
 * time.
 * Pointers to buffers MUST have persistence
 */
typedef struct {
    uint16_t length;                /* Size of the attached buffer in bytes */
    volatile uint16_t read_offset;  /* Index offset where we are reading from */
    volatile uint16_t write_offset; /* Index offset where we are writing to */
    uint8_t* buffer;                /* Pointer to the actual buffer */
    bool is_initialised;            /* Flag to indicate initialisation done */

} st_queues_instance_t;

/**
 * \brief Initialise a buffer so it can work as a circular buffer
 * \param raw_buffer Pointer to buffer to become circular buffer
 * \param length Size of the buffer in bytes
 * \param queue_instance Pointer to structure that is populated and used as an
 * instance handle for all other exposed interfaces
 * \returns true if successful, false if not
 */
bool queues_initialise(uint8_t* raw_buffer, uint16_t length,
                       st_queues_instance_t* queue_instance);
/**
 * \brief reset the queue to empty
 * \param queue_instance Pointer to the circular buffer instance
 */
void queues_reset(st_queues_instance_t* queue_instance);
/**
 * \brief write one byte into the queue
 * \param queue_instance Pointer to the circular buffer instance
 * \param in_byte byte to write
 * \returns true if successful, false if not
 */
bool queues_pushByte(st_queues_instance_t* queue_instance, uint8_t in_byte);
/**
 * \brief write a number of bytes into the queue
 * \param queue_instance Pointer to the circular buffer instance
 * \param in_data pointer to data to write
 * \param in_length number of bytes to write
 * \returns true if successful, false if not
 */
bool queues_pushData(st_queues_instance_t* queue_instance,
                     const uint8_t* in_data, uint16_t in_length);
/**
 * \brief read a number of bytes from the queue
 * \param queue_instance Pointer to the circular buffer instance
 * \param out_buffer pointer to buffer to put read data into
 * \param read_length number of bytes to read
 * \returns number of bytes actually read
 */
uint16_t queues_popData(st_queues_instance_t* queue_instance,
                        uint8_t* out_buffer, uint16_t read_length);
/**
 * \brief read one byte from the queue if available
 * \param queue_instance Pointer to the circular buffer instance
 * \param out_buffer pointer to variable to put read byte into
 * \returns true if successful, false if not
 */
bool queues_popByte(st_queues_instance_t* queue_instance, uint8_t* out_buffer);
/**
 * \brief check whether the queue is empty
 * \param queue_instance Pointer to the circular buffer instance
 * \returns true if empty, false if not
 */
bool queues_isEmpty(const st_queues_instance_t* queue_instance);
/**
 * \brief checks whether the queue is full
 * \param queue_instance Pointer to the circular buffer instance
 * \returns true if full, false if not
 **/
bool queues_isFull(const st_queues_instance_t* queue_instance);
/**
 * \brief Get the number of bytes in the buffer that are available to be read
 * \param queue_instance Pointer to the circular buffer instance
 * \returns number of bytes available to be read
 **/
uint16_t queues_getNumBytesPending(const st_queues_instance_t* queue_instance);


/*******************************************************************************
 *                      ISR friendly functions
 ******************************************************************************/
static inline bool queues_isrIsBusy(const st_queues_instance_t* queue_instance)
{

    bool result = true;
    if ( queue_instance->write_offset == queue_instance->read_offset ) {
        result = false;
    }
    return result;
}

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /*  QUEUES_H__ */
