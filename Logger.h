#ifndef LOGGER_H
#define LOGGER_H

#include "GIS.h"

using namespace std;

class Logger {
    ofstream * log;

public:
    Logger (ofstream & logFile, string script) {
        log = & logFile;
        *log << INFO << endl;
        *log << "Begin of GIS Program log:" << endl;
        *log << "dbFile: db.txt" << endl;
        *log << "script: " << script << endl;
        *log << "log: log.txt" << endl;

        time_t now = time(0);
        *log << "Start Time: " <<  ctime(&now) << endl;
    }

    void dashes() {
        *log << "------------------------------------------------------------------------------------------\n" << endl;
    }

    void logWorld(string command) {
        *log << command << endl;
        dashes();
        *log << "Latitude/longitude values in index entries are shown as signed integers, in total seconds." << endl;
        dashes();
    } 

    void logCommand(string command, int commandNo) {
        *log << "Command " << commandNo << ": " << command << endl;
        dashes();
    }

    void logComment(string comment) {
        *log << comment << endl;
    }

    void logQuit(string command, int commandNo) {
        time_t now = time(0);
        *log << "Command " << commandNo << ": " << command << endl;
        dashes();
        *log << "Terminating execution of commands." << endl;
        *log << "End time: " <<  ctime(&now) << endl;
        log->close();
    }
};


#endif