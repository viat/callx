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
 * SigBasedAna.hpp
 *
 *  Created on: Nov 27, 2012
 *      Author: mainka
 */

#ifndef SIGBASEDANA_HPP_
#define SIGBASEDANA_HPP_

#include "sip/sip.hpp"
#include "main/callx.hpp"
#include "main/CallxThread.hpp"

namespace callx {

class SbaEventMap;
class CallxConfig;
class SbaIncidentMap;

class SigBasedAna:
        public CallxThread {
public:
    SigBasedAna();
    virtual ~SigBasedAna();

    void worker();
    bool stop();

protected:
    CallxConfig *m_callxConfig;
    SbaEventMap *m_sbaEventMap;
    SbaIncidentMap *m_sbaIncidentMap;

    condition_variable m_workerCond;
    mutex m_workerCondMutex;

    systemClock::time_point m_analyzeTs;

    std::string m_currCaller;
    std::shared_ptr<SbaEventDeque> m_currSbaEventDeque;

    void analyze();
    void analyzeCallAttempts();
    void analyzeCallsConcurrent();
    void analyzeCallCompletion();
    void analyzeCallDurationAverage();
    void analyzeCallDurationCumulative();
    void analyzeCallsClosedByCallee();

private:

    /**
     * Checks if the event timestamp lies between m_analyzeTs and
     * (analyzeTs - minutes). The member variable m_analyzeTs holds the start of
     * the analyzer run and is the same for all analysis.
     * @param eventTs Timestamp of the event.
     * @param minutes Analyze period of the current analysis (see config).
     * @return True if in period.
     */
    bool isInPeriod(systemClock::time_point eventTs, int period);

    /**
     * Pushed Incident into Callers incident set.
     * @param caller Caller address
     * @param incident Shared pointer to SbaIncident object.
     */
    void insertIncident(const std::string& caller,
            std::shared_ptr<SbaIncident> incident);
};

} /* namespace callx */

#endif /* SIGBASEDANA_HPP_ */
