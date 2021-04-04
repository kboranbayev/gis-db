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
        *log << "------------------------------------------------------------------------------------------" << endl;
    }

    void logWorld(string command, World world) {
        *log << command << endl;
        dashes();
        *log << "Latitude/longitude values in index entries are shown as signed integers, in total seconds." << endl;
        dashes();
        *log << "						World boundaries are set to:" << endl;
        *log << "						           " << world.n_lat << endl;
        *log << "						" << world.w_long << "              " << world.e_long << endl;
        *log << "						           " << world.s_lat << endl;
        dashes();
    }

    void logCommand(string command, int commandNo) {
        dashes();
        *log << "Command " << commandNo << ": " << command << endl;
    }

    void logStr(string os) {
        *log << endl << os;
    }

    void logComment(string comment) {
        *log << comment << endl;
    }

    void logQuit(string command, int commandNo) {
        time_t now = time(0);
        *log << "Command " << commandNo << ": " << command << endl;
        *log << "\nTerminating execution of commands." << endl;
        dashes();
        *log << "End time: " <<  ctime(&now) << endl;
        log->close();
    }
};


#endif