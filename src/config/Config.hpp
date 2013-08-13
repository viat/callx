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
 * Config.hpp
 *
 *  Created on: Jul 23, 2012
 *      Author: mainka
 */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <map>

namespace callx {

class Config {

public:
    Config();
    virtual ~Config();

    /*
     * Read config and fill the map (cfgMap).
     */
    bool parseConfig(const std::string& path);

    /*
     *  Get str-value of attribute.
     *  Return empty string if not available.
     */
    std::string getStr(const std::string& attribute, std::string defaultValue =
            "");

    /*
     *  Get int-value of attribute.
     *  return 0 if not available or if str-to-int conversion failed.
     */
    int getInt(const std::string& attribute, int defaultValue = 0);

    /*
     * returns true or false
     */
    bool getBool(const std::string& attribute, bool defaultValue = false);

private:

    // the map (attribute values pairs)
    std::map<std::string, std::string> m_cfgMap;
};

} /* namespace callx */

#endif /* CONFIG_HPP_ */
