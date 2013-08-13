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
 * SbaIncidentMap.hpp
 *
 *  Created on: Dec 25, 2012
 *      Author: mainka
 */

#ifndef SBAINCIDENTMAP_HPP_
#define SBAINCIDENTMAP_HPP_

#include "main/CallxSingleton.hpp"
#include "ThreadFriendlyMap.hpp"
#include "main/CallxTypes.hpp"
#include "sba/SbaIncident.hpp"

namespace callx {

class SbaIncident;

class SbaIncidentMap:
        public callx::CallxSingleton<SbaIncidentMap>,
        public callx::ThreadFriendlyMap<std::string,
                std::shared_ptr<SbaIncidentSet>> {

    friend class CallxSingleton<SbaIncidentMap> ;

public:

    /**
     * Destructor
     */
    virtual ~SbaIncidentMap() {
        L_t
        << "D'tor";
    }

    /**
     *
     * @return String representation (formated) of the map.
     */
    std::string toString() const {
        std::stringstream strstream;
        std::lock_guard<std::mutex> lock(m_mutex);

        // Return if there are no caller in the SbaIncidentMap.
        if (m_map.empty()) {
            strstream << "\r\nSbaIncidentMap is empty.\r\n\r\n";
            return strstream.str();
        }

        strstream << "\r\n"
                  << "Worst incidents per caller:"
                  << "\r\n\r\n";

        // Iterate over caller in IncidentMap.
        for (auto callerIter = m_map.begin(); callerIter != m_map.end();
                callerIter++) {
            auto incidentSet = callerIter->second;
            strstream << "-----------------------------------------------"
                      << "\r\n"
                      << "caller:\t"
                      << callerIter->first
                      << "\r\n"
                      << "-----------------------------------------------"
                      << "\r\n\r\n";

            // Temporary map of the worst (evilest) incidents.
            std::map<incidentTypeEnum, std::shared_ptr<SbaIncident>> incidentEvilMap;

            for (auto incidentIter = incidentSet->begin();
                    incidentIter != incidentSet->end(); incidentIter++) {
                auto incident = *incidentIter;

                // Find current incident in incidentEvilMap.
                auto incidentMaxIter =
                        incidentEvilMap.find(incident->getType());

                // Insert incident, if it is the first of this type in
                // the incidentEvilMap.
                if (incidentMaxIter == incidentEvilMap.end()) {

                    // insert incident
                    incidentEvilMap.insert( { incident->getType(), incident });
                } else {

                    // There is an incident of the same type in the incidentEvilMap.
                    // Check if the current incident from the incidentSet should
                    // overwrite the one in the incidentEvilMap. The decision is
                    // based on the value and on the information if a smaller
                    // or a bigger value is more 'evil'.
                    if ((incident->getValue()
                            > incidentMaxIter->second->getValue()
                            && incident->getHigherValueIsEvil())
                            || (incident->getValue()
                                    < incidentMaxIter->second->getValue()
                                    && !incident->getHigherValueIsEvil())) {

                        incidentEvilMap.erase(incident->getType()); // not necessary...
                        incidentEvilMap.insert( { incident->getType(), incident });
                    }
                }
            }

            // Output of the evilest incidents.
            for (auto incidentIter = incidentEvilMap.begin();
                    incidentIter != incidentEvilMap.end(); incidentIter++) {
                auto incident = incidentIter->second;
                time_t timestamp =
                        systemClock::to_time_t(incident->getTimestamp());
                std::string tempTime(ctime(&timestamp));

                strstream << incidentTypeToString(incident->getType())
                          << " ("
                          << tempTime.substr(0, tempTime.size() - 1)
                          << ") "
                          << "\r\n"
                          << incident->getReason()
                          << "\r\n\r\n";

            }
            strstream << "\r\n";
        }

        return strstream.str();
    }

protected:

    /**
     * Hidden constructor
     */
    SbaIncidentMap() {
        L_t
        << "C'tor";
    }

}
;

} /* namespace callx */

#endif /* SBAINCIDENTMAP_HPP_ */
