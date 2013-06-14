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
 * Config.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: mainka
 */

#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace callx {

Config::Config() {
}

Config::~Config() {
}

bool Config::parseConfig(const std::string& path) {

    // one config line
    string line;

    // open config file
    ifstream cfgFile(path.c_str());
    if (!cfgFile.is_open()) {
        return false;
    }

    // Test if the file can be read. This test fails, if the file is empty or
    // if it is a directory.
    getline(cfgFile, line);
    if (!cfgFile.good()) {
        return false;
    }
    cfgFile.seekg(0);

    // attribute and value temp strings
    string attribute;
    string value;
    size_t pos;

    // parse config file
    while (cfgFile.good()) {
        getline(cfgFile, line);

        // Cut off a comment, if available.
        if (!((pos = line.find("#")) == string::npos)) {
            line = line.substr(0, pos);
        };

        // Find the position of the equal sign.
        pos = line.find("=");
        if (pos == 0) {
            // no equal sign -> no value!
            continue;
        }

        // setting attribute and value
        attribute = line.substr(0, pos);
        attribute.erase(0, attribute.find_first_not_of(" "));
        attribute.erase(attribute.find_last_not_of(" ") + 1);
        value = line.substr(pos + 1);
        value.erase(0, value.find_first_not_of(" "));
        value.erase(value.find_last_not_of(" ") + 1);

        if (attribute.size() && value.size()) {
            m_cfgMap.insert(make_pair(attribute, value));
        }

    }
    return true;
}

string Config::getStr(const std::string& attribute, std::string defaultValue) {
    string value;
    auto it = m_cfgMap.find(attribute);
    if (it == m_cfgMap.end()) {
        value = defaultValue;
    } else
        value = it->second;
    return value;
}

int Config::getInt(const std::string& attribute, int defaultValue) {
    int intValue;
    std::string strValue = getStr(attribute);
    if (strValue.empty())
        return defaultValue;
    stringstream strStream(strValue);
    strStream >> intValue;
    return intValue;
}

bool Config::getBool(const std::string& attribute, bool defaultValue) {
    return boost::iequals(getStr(attribute), "true")
            || boost::iequals(getStr(attribute), "yes")
            || boost::iequals(getStr(attribute), "1");
}

} /* namespace callx */
