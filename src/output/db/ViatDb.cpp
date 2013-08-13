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
 * ViatDb.cpp
 *
 *  Created on: Dec 26, 2012
 *      Author: mainka
 */

#include "ViatDb.hpp"
#include "main/callx.hpp"

using namespace std;

namespace callx {

ViatDb::ViatDb(const string &dbConnectStr) {
    m_dbCon.reset(new pqxx::connection(dbConnectStr));
    L_t
            << "m_dbCon->is_open(): "
            << m_dbCon->is_open();
    L_t
            << "m_dbCon->server_version(): "
            << m_dbCon->server_version();
}

ViatDb::~ViatDb() {
}

long ViatDb::insertCall(const string &callerName, const string &callerUri) {

    if (!m_dbCon.get())
        return -1;
    pqxx::work txn(*m_dbCon);
    long callerId;
    long callId;

    if (!dbCallerExists(callerUri, callerId, txn))
        if (!dbInsertCaller(callerName, callerUri, callerId, txn)) {
            return -1;
        }
    if (!dbInsertCall(callerId, callId, txn)) {
        return -1;
    }

    txn.commit();

    return callId;
}

bool ViatDb::dbCallerExists(const string &callerUri,
        long &callerId,
        pqxx::transaction_base &txn) {
    pqxx::result res = txn.exec("SELECT id FROM caller WHERE uri="
            + txn.quote(callerUri));
    if (res.size() != 1) {
        return false;
    }
    callerId = res[0][0].as<int>();
    return true;
}

bool ViatDb::dbInsertCaller(const string &callerName,
        const string &callerUri,
        long &callerId,
        pqxx::transaction_base &txn) {

    pqxx::result res = txn.exec("INSERT INTO caller(name, uri) VALUES("
            + txn.quote(callerName) + ", " + txn.quote(callerUri)
            + ") RETURNING id");
    if (res.size() != 1) {
        return false;
    }
    callerId = res[0][0].as<int>();
    return true;
}

bool ViatDb::dbInsertCall(long callerId,
        long &callId,
        pqxx::transaction_base &txn) {

    pqxx::result res = txn.exec("INSERT INTO call(caller_id) values("
            + txn.quote(callerId) + ") RETURNING id");
    if (res.size() != 1) {
        return false;
    }
    callId = res[0][0].as<int>();
    return true;
}

} /* namespace callx */
