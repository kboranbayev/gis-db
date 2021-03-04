#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "GIS.h"
#include "GISRecord.h"
#include "Logger.h"

using namespace std;

class CommandProcessor {

    int numCommands;

    //GISRecord * record;

    Logger * log;

    ofstream * db;

    int processCommandString(string cmd) {
        if (cmd.at(0) == ';') { return IGNORE; }
        else if (cmd.rfind("world") == 0) { return WORLD; }
        else if (cmd.rfind("import") == 0) { return IMPORT; }
        else if (cmd.rfind("debug") == 0) { return DEBUG; }
        else if (cmd.rfind("quit") == 0) { return QUIT; }
        else if (cmd.rfind("what_is_at") == 0) { return WHAT_IS_AT; }
        else if (cmd.rfind("what_is_in") == 0) { return WHAT_IS_IN; }
        else { return IGNORE; }
    }
public:
    CommandProcessor (ofstream & db_file, Logger * logger) {
        //log = &log_file;
        log = logger;
        db = &db_file;
        numCommands = 0;
        //cout << "Processing " << args << endl;

    }

    void processCommand(string cmd) {
        istringstream iss(cmd);
        string wlong, elong, slat, nlat;
        switch (processCommandString(cmd)) {
            case WORLD:
                log->logWorld(cmd);
                getline(iss, wlong, '\t');
                getline(iss, wlong, '\t');
                getline(iss, elong, '\t');
                getline(iss, slat, '\t');
                getline(iss, nlat, '\t');
                cout << wlong << endl;
                cout << elong << endl;
                cout << slat << endl;
                cout << nlat << endl;
                break;
            case IMPORT:
                numCommands++;
                
                break;
            case DEBUG:
                numCommands++;
                
                break;
            case QUIT:
                numCommands++;
                
                log->logQuit(cmd, numCommands);
                break;
            case WHAT_IS_AT:
                numCommands++;
                
                break;
            case WHAT_IS_IN:
                numCommands++;
                
                break;
            default:
                log->logComment(cmd);
                break;
        }
    }
};

#endif