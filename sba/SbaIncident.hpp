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
 * SbaIncident.hpp
 *
 *  Created on: Dec 25, 2012
 *      Author: mainka
 */

#ifndef SBAINCIDENT_HPP_
#define SBAINCIDENT_HPP_

namespace callx {

enum incidentTypeEnum {
    it_GENERIC = 0,
    it_CALL_ATTEMPTS,
    it_CALL_COMPLETION,
    it_CALL_DURATION_AVERAGE,
    it_CALLS_CLOSED_BY_CALLEE,
    it_CALL_DURATION_CUMULATIVE,
    it_CALLS_CONCURRENT
};

static const std::string incidentType[] = { "GENERIC", "CALL_ATTEMPTS",
        "CALL_COMPLETION", "CALL_DURATION_AVERAGE", "CALLS_CLOSED_BY_CALLEE",
        "CALL_DURATION_CUMULATIVE", "CALLS_CONCURRENT" };

inline std::string incidentTypeToString(incidentTypeEnum type) {
    return incidentType[type];
}

class SbaIncident {
public:
    SbaIncident(incidentTypeEnum type,
            std::string user,
            int period,
            float value,
            bool higherValueIsEvil,
            std::string reason)
            : m_timestamp(systemClock::now()),
              m_type(type),
              m_user(user),
              m_period(period),
              m_value(value),
              m_higherValueIsEvil(higherValueIsEvil),
              m_reason(reason) {

    }

    virtual ~SbaIncident() {

    }

    const systemClock::time_point& getTimestamp() const {
        return m_timestamp;
    }

    const std::string& getUser() const {
        return m_user;
    }

    const int getperiod() const {
        return m_period;
    }

    const float getValue() const {
        return m_value;
    }

    const bool getHigherValueIsEvil() const {
        return m_higherValueIsEvil;
    }

    const std::string& getReason() const {
        return m_reason;
    }

    incidentTypeEnum getType() const {
        return m_type;
    }

protected:
    systemClock::time_point m_timestamp;
    incidentTypeEnum m_type;
    std::string m_user;
    int m_period;
    float m_value;
    bool m_higherValueIsEvil;
    std::string m_reason;
};

} /* namespace callx */

#endif /* SBAINCIDENT_HPP_ */
