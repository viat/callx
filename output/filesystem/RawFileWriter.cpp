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
 * RawFileWriter.cpp
 *
 *  Created on: Aug 28, 2012
 *      Author: root
 */

#include "RawFileWriter.hpp"
#include "main/callx.hpp"
#include "config/CallxConfig.hpp"
#include <string>
#include <vector>

using namespace std;

namespace callx {

RawFileWriter::RawFileWriter()
        : fileDescriptor(0),
          outFile(NULL),
          totalSize(0) {

    L_t
    << "C'tor";

    tempPathFilename = CallxConfig::getInstance()->wave_output_path
            + "callx_XXXXXX";

    vector<char> tmpFileStr = vector<char>(tempPathFilename.size() + 1);

    strncpy(tmpFileStr.data(), tempPathFilename.c_str(), tempPathFilename.size());
    tmpFileStr[tempPathFilename.size()] = '\0';

    // create temp filename
    int fileDescriptor = mkstemp(tmpFileStr.data());

    if (fileDescriptor == -1) {
        L_e
        << "Error creating temporary file \"" << tempPathFilename << "\": "
                << strerror(errno);
        exit(-1);
    }
    tempPathFilename.assign(tmpFileStr.data());

    // re-open temporary file with fopen (better buffering!?)
    close(fileDescriptor);
    outFile = fopen(tempPathFilename.c_str(), "wb+");
    if (!outFile) {
        L_e
        << "Error re-opening the newly created temporary file: "
                << tempPathFilename;
        exit(-1);
    }

}

RawFileWriter::~RawFileWriter() {
    L_t
    << "D'tor";
    if (outFile)
        fclose(outFile);

    // Delete temporary file if empty, else rename to real name.
    if (totalSize == 0) {
        L_i
        << "File is empty, removing " << tempPathFilename;
        if (remove(tempPathFilename.c_str())) {
            L_i
            << "File could not be deleted, error: " << strerror(errno);
        }
    } else {
        L_i
        << "outFileName: " << filename;
        int result = rename(tempPathFilename.c_str(), filename.c_str());
        if (result != 0) {
            L_e
            << "Error renaming temporary file '" << tempPathFilename << "' to '"
                    << filename << "': " << strerror(errno);
        }
    }
}

void RawFileWriter::setFilename(const string& filename) {
    this->filename = filename;
}

bool RawFileWriter::write(void* buffer, size_t size) {
    totalSize += size;
    return (fwrite(buffer, size, 1, outFile) == size);
}

} /* namespace callx */

