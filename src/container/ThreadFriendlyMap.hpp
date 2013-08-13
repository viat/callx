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
 * ThreadSafeMap.hpp
 *
 *  Created on: Aug 26, 2012
 *      Author: mainka
 */

#ifndef THREADFRIENDLYMAP_HPP_
#define THREADFRIENDLYMAP_HPP_

#include <map>
#include "main/CallxTypes.hpp"
#include "main/callx.hpp"

namespace callx {

template<typename Key, typename Type>
class ThreadFriendlyMap {
public:
    ThreadFriendlyMap()
            : m_sizeMax(0) {
    }

    virtual ~ThreadFriendlyMap() {
    }

    void insert(std::pair<const Key&, Type>&& p) {
        unique_lock lock(m_mutex);
        m_map.insert(std::forward<std::pair<const Key&, Type>>(p));
        if (size() > m_sizeMax)
            m_sizeMax++;
    }

    bool find(const Key& k, Type& t) {
        lock_guard lock(m_mutex);
        auto iter = m_map.find(k);
        if (iter == m_map.end()) {
            return false;
        }
        t = iter->second;
        return true;
    }

    size_t erase(const Key& k) {
        unique_lock lock(m_mutex);
        return m_map.erase(k);
    }

    size_t size() const {
        return m_map.size();
    }

    size_t sizeMax() const {
        return m_sizeMax;
    }

    void eraseAll() {
        unique_lock lock(m_mutex);
        m_map.erase(m_map.begin(), m_map.end());
    }

protected:
    std::map<Key, Type> m_map;
    mutable mutex m_mutex;
    size_t m_sizeMax;
};

} /* namespace callx */

#endif /* THREADFRIENDLYMAP_HPP_ */
