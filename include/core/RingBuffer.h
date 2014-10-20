/*! \file RingBuffer.cpp
 *  \brief Header file for the RingBuffer class
 *
 *  This file contains the template implementation of the RingBuffer class,
 *  which is a simple non-locking ring buffer.
 */

#include <cstring>

namespace Phoenix
{
    namespace Core
    {
        /*! \brief Class that implements a ring buffer
         *
         *  \note Locking is not supported but is unnecessary as long as only
         *  one thread writes to the buffer and only one thread reads from the
         *  buffer.
         */
        template <typename T>
        class RingBuffer
        {
        public:
            /*! \brief Constructor for the RingBuffer class
             *
             *  Creates a ring buffer with default size (128 elements).
             */
            RingBuffer()
            : writeIndex(0), readIndex(0), size(128+1), data(new T[128+1])
            {
                // intentionally left empty
            }

            /*! \brief Constructor for the RingBuffer class
             *
             *  Creates a ring buffer which can hold the given number of
             *  elements.
             *
             *  \param s Size of the ring buffer in elements.
             */
            RingBuffer(std::size_t s)
            // need one padding element to differentiate
            // between empty and full buffer
            : writeIndex(0), readIndex(0), size(s+1), data(new T[s+1])
            {
                // intentionally left empty
            }

            /*! \brief Destructor for the RingBuffer class */
            ~RingBuffer()
            {
                delete [] data;
            }

            /*! \brief Clear the contents of the ring buffer */
            void clear(void)
            {
                // writeIndex == readIndex means buffer is empty;
                writeIndex = readIndex = 0;
            }

            /*! \brief Return number of elements currently stored in the ring
             *  buffer
             *
             *  \return number of elements currently stored in the ring buffer.
             */
            std::size_t used(void) const
            {
                // empty buffer
                if (writeIndex == readIndex)
                    return 0;
                // contiguous block
                else if (writeIndex > readIndex)
                    return writeIndex - readIndex;
                // wrap around
                else
                    return size - (readIndex - writeIndex);
            } 

            /*! \brief Return number of empty slots in the ring buffer
             *
             *  \return number of empty slots in the ring buffer.
             */
            std::size_t free(void) const
            {
                // size - used - padding
                return size - used() - 1;
            }

            /*! \brief Read elements from the ring buffer
             *
             *  Copies given number of elements into the destination buffer
             *  and removes them from the ring buffer.
             *
             *  \param dest Destination buffer.
             *  \param count Number of elements to read from ring buffer.
             *  \return number of elements actually read.
             */
            std::size_t read(T *dest, std::size_t count)
            {
                // can't read more than what is available
                std::size_t available = used();
                if (available == 0)
                	return 0;

                if (count > available)
                    count = available;

                // copy elements to dest buffer
                if (readIndex < writeIndex || readIndex + count <= size)
                {
                    // all wanted elements are in one contiguous block
                    memcpy(dest, data + readIndex, count * sizeof(T));
                }
                else
                {
                    // wanted elements wrap around end of buffer
                    // so we have to read in two blocks
                    std::size_t c1 = size - readIndex;
                    std::size_t c2 = count - c1;
                    memcpy(dest, data + readIndex, c1 * sizeof(T));
                    memcpy(dest + c1, data, c2 * sizeof(T));
                }

                // increment read index, wrapping if necessary
                readIndex += count;
                if (readIndex >= size)
                    readIndex -= size;

                // return number of elements read
                return count;
            }

            /*! \brief Write elements into the ring buffer
             *
             *  Copies given number of elements from the source buffer into
             *  the ring buffer.
             *
             *  \param src Source buffer.
             *  \param count Number of elements to write into ring buffer.
             *  \return number of elements actually written.
             */
            std::size_t write(const T *src, std::size_t count)
            {
                // can't write more than amount of free space
                std::size_t available = free();
                if (count > available)
                    count = available;

                // copy elements into ring buffer
                // write as one contiguous block if possible,
                // else split into two writes
                if (writeIndex < readIndex || writeIndex + count <= size)
                {
                    memcpy(data + writeIndex, src, count * sizeof(T));
                }
                else
                {
                    std::size_t c1 = size - writeIndex;
                    std::size_t c2 = count - c1;
                    memcpy(data + writeIndex, src, c1 * sizeof(T));
                    memcpy(data, src + c1, c2 * sizeof(T));
                }

                // increment write index, wrapping if necessary
                writeIndex += count;
                if (writeIndex >= size)
                    writeIndex -= size;

                // return number of elements written
                return count;
            }

        private:
            /*! \brief Write index */
            std::size_t writeIndex;
            /*! \brief Read index */
            std::size_t readIndex;
            /*! \brief Size of the buffer */
            const std::size_t size;
            /*! \brief Buffer data */
            T * const data;
        };
    }
}
