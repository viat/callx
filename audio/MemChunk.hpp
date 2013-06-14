/**
 * This file is part of callx. The application callx performs the call
 * extraction as well as the signaling-based analysis in the VIAT system.
 *
 * http://viat.fh-koeln.de
 *
 * Copyright (C) 2013 Bernhard Mainka (mail@bmainka.de),
 * Cologne University of Applied Sciences
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * You along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * MemChunk.h
 *
 *  Created on: Dec 15, 2012
 *      Author: mainka
 */

#ifndef MEMCHUNK_HPP_
#define MEMCHUNK_HPP_

namespace callx {

/**
 * Offers an interface to manage a chunk of memory with configurable size on
 * initialization. The interface assists avoiding a buffer overrun.
 * However, it offers direct access to the memory to avoid unnecessary copies
 * of the data while filling the chunk.
 */
class MemChunk {
public:

    /**
     *
     * @param size Chunk size in bytes.
     */
    MemChunk(size_t size)
            : m_data(NULL),
              m_currDataPointer(NULL),
              m_size(size),
              m_fillingLevel(0) {

        m_data = new char[m_size];
        m_currDataPointer = m_data;
    }

    /*
     * Destructor, deletes the previously allocated piece of memory.
     */
    ~MemChunk() {
        delete[] m_data;
    }

    /**
     * Declares size bytes being stored in the chunk.
     * @param size The amount of bytes that are requested to add to the chunk.
     * @param buffer If the return value is true, buffer points to the address
     * where size additional bytes can be written.
     * @return True if enough space is left to store size additional bytes in
     * allocated memory.
     */
    bool declare(size_t size, void** buffer) {

        // enough space left?
        if (m_fillingLevel + size > m_size)
            return false;

        // yes!
        *buffer = m_currDataPointer;
        m_fillingLevel += size;
        m_currDataPointer += size;
        return true;
    }

    size_t fillingLevel() {
        return m_fillingLevel;
    }

    void* data() {
        return m_data;
    }

private:
    char* m_data;
    char* m_currDataPointer;
    size_t m_size;
    size_t m_fillingLevel;
};

} /* namespace callx */
#endif /* MEMCHUNK_HPP_ */
