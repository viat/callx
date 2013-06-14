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
 * SbaEventMap.hpp
 *
 *  Created on: Nov 29, 2012
 *      Author: mainka
 */

#ifndef SBAEVENTMAP_HPP_
#define SBAEVENTMAP_HPP_

#include "ThreadFriendlyMap.hpp"
#include "main/CallxSingleton.hpp"
#include "main/CallxTypes.hpp"
#include "sba/SbaEvent.hpp"

namespace callx {

class SbaEvent;

class SbaEventMap:
        public ThreadFriendlyMap<std::string, std::shared_ptr<SbaEventDeque>>,
        public CallxSingleton<SbaEventMap> {

    friend class CallxSingleton<SbaEventMap> ;
public:

    /**
     * Destructor
     */
    virtual ~SbaEventMap() {
        L_t
        << "D'tor";
    }

    /**
     *
     * @return Iterator to the first element of the map.
     */
    std::map<std::string, std::shared_ptr<SbaEventDeque>>::iterator begin() {
        return m_map.begin();
    }

    /**
     *
     * @return Iterator to the last element of the map.
     */
    std::map<std::string, std::shared_ptr<SbaEventDeque>>::iterator end() {
        return m_map.end();
    }

    /**
     *
     * @return String representation (formated) of the map.
     */
    std::string toString() const {
        std::stringstream strstream;
        std::lock_guard<std::mutex> lock(m_mutex);

        // iterate over map, key is caller
        for (auto callerIter = m_map.begin(); callerIter != m_map.end();
                callerIter++) {
            strstream << "\r\nCaller: " << callerIter->first << "\r\n";
            auto callerQueue = callerIter->second;

            // SBA events of the caller
            for (auto eventIter = callerQueue->begin();
                    eventIter != callerQueue->end(); eventIter++) {
                std::shared_ptr<SbaEvent> event = (*eventIter);
                time_t timestamp = systemClock::to_time_t(event->timestamp);
                std::string tempTime(ctime(&timestamp));
                strstream << "Event Type:\t"
                          << sbaEventTypeEnumToString(event->eventType)
                          << "\r\n"
                          << "Call ID:\t"
                          << event->callId
                          << "\r\n"
                          << "Timestamp:\t"
                          << tempTime.substr(0, tempTime.size() - 1)
                          << "\r\n"
                          << "Initiator:\t"
                          << event->transactionInitiator.address
                          << "\r\n"
                          << "Final rsponse:\t";
                if (event->finalResponseCode != 0) {
                    strstream << event->finalResponseCode
                              << " "
                              << event->finalResponseReason
                              << "\r\n";
                } else {
                    strstream << "N/A" << "\r\n";
                };

                switch (event->eventType) {
                case et_INVITE: {
                    auto inviteEvent = std::static_pointer_cast<InviteEvent>(
                            event);

                    strstream << "flags [ack/reinvite/cancel]: "
                              << inviteEvent->ackFlag
                              << "/"
                              << inviteEvent->reinviteFlag
                              << "/"
                              << inviteEvent->cancelFlag
                              << "\r\n";
                }
                break;
                case et_BYE: {
                    auto byeEvent = std::static_pointer_cast<ByeEvent>(event);

                    auto duration = boost::chrono::duration_cast<seconds>(
                            byeEvent->callDuration);

                    auto durationMinutes =
                            boost::chrono::duration_cast<minutes>(
                                    duration % 60);
                    auto durationSeconds = duration - durationMinutes * 60;

                    strstream << "Call duration:\t"
                              << durationMinutes
                              << " "
                              << durationSeconds
                              << "\r\n";
                }
                break;
                case et_CANCEL: {
                    auto optionsEvent = std::static_pointer_cast<OptionsEvent>(
                            event);

                    strstream << "";
                }
                break;
                case et_OPTIONS: {
                    auto optionsEvent = std::static_pointer_cast<OptionsEvent>(
                            event);

                    strstream << "Dialog established:\t"
                              << (optionsEvent->dialogEstablished ? "yes" : "no");
                }
                break;
                default:
                break;
                }
                strstream << "\r\n";
            }
        }
        if (strstream.tellp() == 0) {
            strstream << "\r\nSbaEventMap is empty.\r\n\r\n";
        }
        return strstream.str();
    }

private:

    /**
     * Hidden constructor
     */
    SbaEventMap() {
        L_t
        << "C'tor";
    }
};

} /* namespace callx */

#endif /* SBAEVENTMAP_HPP_ */
