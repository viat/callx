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
 * ViatDb.hpp
 *
 *  Created on: Dec 26, 2012
 *      Author: mainka
 */

#ifndef VIATDB_HPP_
#define VIATDB_HPP_

#include <pqxx/pqxx>

namespace callx {

class ViatDb {
public:

    // dbConnectStr
    // "dbname=<db name> user=<user> password=<pass> host=<ip>"
    ViatDb(const std::string &dbConnectStr);
    virtual ~ViatDb();

    long insertCall(const std::string &callerName, const std::string &callerUri);

protected:
    std::unique_ptr<pqxx::connection> m_dbCon;

    bool dbCallerExists(const std::string &callerUri,
            long &callerId,
            pqxx::transaction_base &txn);
    bool dbInsertCaller(const std::string &callerName,
            const std::string &callerUri,
            long &callerId,
            pqxx::transaction_base &txn);
    bool dbInsertCall(long callerId, long &callId, pqxx::transaction_base &txn);
};

} /* namespace callx */
#endif /* VIATDB_HPP_ */
