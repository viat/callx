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
 * CallxSingleton.hpp
 *
 *  Created on: Aug 24, 2012
 *      Author: mainka
 */

#ifndef CALLXSINGLETON_HPP_
#define CALLXSINGLETON_HPP_

#include <mutex>

namespace callx {

template<typename T>
class CallxSingleton {
public:

    /*
     * Copying not allowed.
     */
    CallxSingleton(const CallxSingleton&) = delete;

    /*
     * Assigning not allowed.
     */
    CallxSingleton& operator=(const CallxSingleton&) = delete;

//    /*
//     * Destructor.
//     */
//    virtual ~CallxSingleton() {
//    }

    /*
     * Returns singleton instance.
     * @return pointer to instance of T
     */
    static T* getInstance() {
        std::call_once(m_once_flag, createInstance);
        return m_instance;
    }

protected:
    /*
     * Hide constructor.
     */
    CallxSingleton() {};

    /*
     * Creates singleton Instance.
     */
    static void createInstance() {
        m_instance = new T;
    }

    /*
     * Singleton instance.
     */
    static T* m_instance;

    /*
     * The once_flag used by call_once() in getInstance()
     */
    static std::once_flag m_once_flag;

};

// initialize static member variables
template<typename T> T* CallxSingleton<T>::m_instance = 0;
template<typename T> std::once_flag CallxSingleton<T>::m_once_flag;

} /* namespace callx */

#endif /* CALLXSINGLETON_HPP_ */
