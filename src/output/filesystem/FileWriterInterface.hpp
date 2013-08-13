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
 * FileWriterInterface.hpp
 *
 *  Created on: Aug 28, 2012
 *      Author: mainka
 */

#ifndef FILEWRITERINTERFACE_HPP_
#define FILEWRITERINTERFACE_HPP_

#include <string>

namespace callx {

class FileWriterInterface {
public:
  FileWriterInterface() {};
  virtual ~FileWriterInterface() {};

  virtual void setFilename(const std::string&) = 0;
  virtual bool write(void* buffer, size_t size) = 0;
};

} /* namespace callx */

#endif /* FILEWRITERINTERFACE_HPP_ */
