CallX (Call eXtractor)
======================

<pre>
1. Overview
2. Running CallX
2.1 Compile and install Boost
2.2 Compile and install the GSM library
2.3 Compile and run CallX
3. License
</pre>

##1. Overview
CallX is an application for the extraction of signaling and media data in
VoIP Networks. It comes with a built-in SIP signaling traffic analyzer to
classify callers (behavioral analysis). The signaling-based analysis consists
of the following 6 individual analyses:
- Call Attempts
- Call Completion
- Call Duration Average
- Call Duration Cumulative
- Calls Closed by Callee
- Calls Concurrent
The classification (normal/suspicious) is based on configurable thresholds.

CallX can be configured to be used as a standalone application or in
conjunction with the VIAT fingerprint generator FeatureX (Feature eXtractor).
The media data can be exported to wave files (standalone application) or it
can be sent over the network using the TCP protocol to another application
(e.g. FeatureX). The export may be dependent (configurable) on the user
classification (preselection). Depending on the hardware CallX should be able
to process several hundred calls in parallel.

A data flow diagram of CallX is available here: http://viat.fh-koeln.de/wp-content/uploads/2013/08/CallX-Poster-20130613.pdf

##2. Running CallX
The source code should compile successful with GCC 4.7. It has been tested on
Debian 7 (Wheezy) with GCC 4.7.2. To simplify this document, it is assumed that
you are running a Debian 7 system. The prerequisites are some Boost libraries
(system, date_time, filesystem, regex, log, chrono, thread) and the GSM library
written by Jutta Degener and Carsten Bormann.

It follows a short installation guideline.

###2.1 Compile and install Boost
Download Boost at http://www.boost.org (Version >= 1.54)

You will first have to install some Debian packages:

        sudo apt-get install build-essential python-dev libpcap0.8-dev libpqxx3-dev
        
There may be some more packages missing, depending on your system
configuration.

        $ ./bootstrap.sh --with-libraries=system,date_time,filesystem,regex,log,\
        chrono,thread --prefix=/usr/local --includedir=include --libdir=lib
        $ sudo ./b2 install --prefix=/usr/local/

###2.2 Compile and install the GSM library
Download and extract the source code from http://www.quut.com/gsm (Version 1.0
patchlevel 13). You should read the files COPYRIGHT, README and INSTALL.
It may be sufficient if you type:

        $ make
        $ mkdir /usr/local/include/gsm
        $ cp inc/* /usr/local/include/gsm
        $ cp lib/libgsm.a /usr/local/lib/

###2.3 Compile and run CallX
Change the working directory to callx/Release and type

        $ make all

There is an config example (src/callx.cfg). You should edit this file before
starting the application. CallX assumes that a config file named callx.cfg is
located in the directory /etc/viat. You may specify another config file using
the -c option.

You should now be able to start the application.

        $ ./callx [-c <config file>]

CallX needs the capability to set the device into promiscuous mode.

***+++ Check the log file +++***

It is possible to connect to the console port using a terminal program. Be
careful if you enable the console. There are no special safety precautions
other than the limitation to localhost.

In case of having serious trouble compiling/running CallX send an email to
mail@bmainka.de.

##3. License
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

