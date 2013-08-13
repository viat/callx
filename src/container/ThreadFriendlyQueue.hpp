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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * ThreadSafeQueue.hpp
 *
 *  Created on: Aug 20, 2012
 *      Author: mainka
 */

#ifndef THREADFRIENDLYQUEUE_HPP_
#define THREADFRIENDLYQUEUE_HPP_

#include <deque>
#include "main/CallxTypes.hpp"
#include "main/callx.hpp"

namespace callx {

template<typename Type>
class ThreadFriendlyQueue {
public:
    ThreadFriendlyQueue()
            : m_activated(true),
              m_sizeMax(0) {
    }

    virtual ~ThreadFriendlyQueue() {
    }

    void push(Type&& value) {
        lock_guard lock(m_mutex);
        m_queue.push_back(std::forward<Type>(value));
//        m_queue.push_back(value);
        if(size() > m_sizeMax) {
            m_sizeMax++;
        }
        m_cond.notify_one();
    }

    bool waitAndPop(Type& value) {
        unique_lock lock(m_mutex);
        while (m_activated && m_queue.empty()) {
            m_cond.wait(lock);
        }
        if(!m_activated) {
            L_t << "Queue is deactivated, current size is: " << m_queue.size();
            return false;
        }
        value = std::move(m_queue.front());
        m_queue.pop_front();
        return true;
    }

    bool tryPop(Type& value) {
        lock_guard lock(m_mutex);
        if (m_queue.empty()) return false;
        value = std::move(m_queue.front());
        m_queue.pop_front();
        return true;
    }

    bool empty() const {
        return m_queue.empty();
    }

    size_t size() const {
        return m_queue.size();
    }

    size_t sizeMax() const {
        return m_sizeMax;
    }

    void activate() {
        m_activated = true;
    }

    void deactivate() {
        L_t << "Queue deactivation, notifying all waiting threads.";
        m_activated = false;
        m_cond.notify_all();
    }

protected:
    std::deque<Type> m_queue;
    mutable mutex m_mutex;
    condition_variable m_cond;
    bool m_activated;
    size_t m_sizeMax;
};

} /* namespace callx */

#endif /* THREADFRIENDLYQUEUE_HPP_ */
