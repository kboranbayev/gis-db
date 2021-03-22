#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "GIS.h"
#include "GISRecord.h"
#include "BufferPool.h"
#include "Logger.h"

using namespace std;

class CommandProcessor {

    int numCommands;

    //GISRecord<string> * record;

    World world;

    Logger * log;

    BufferPool * bp;

    //ofstream * db;
    string dbName;

    vector<Record<string>> all;

    string wlong, elong, slat, nlat;
    char *wl;

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
    //CommandProcessor (ofstream & db_file, Logger * logger) {
    CommandProcessor (string db_file_name, Logger * logger) {
        //log = &log_file;
        log = logger;
        //db = &db_file;
        dbName = db_file_name;
        numCommands = 0;

        //cout << "Processing " << args << endl;
    }

    void processCommand(string cmd) {
        istringstream iss(cmd);
        string import_file_name;
       
        switch (processCommandString(cmd)) {
            case WORLD:
                log->logWorld(cmd);
                getline(iss, wlong, '\t');
                getline(iss, wlong, '\t');
                getline(iss, elong, '\t');
                getline(iss, slat, '\t');
                getline(iss, nlat, '\t');
     
                world.w_long = dms2sec(wlong);
                world.e_long = dms2sec(elong);
                world.s_lat = dms2sec(slat);
                world.n_lat = dms2sec(nlat);
                // TO DO: world needs validation here

                break;
            case IMPORT:
                numCommands++;
                getline(iss, import_file_name, '\t');
                getline(iss, import_file_name, '\t');
                cout << "File:" << import_file_name << "|" << endl;

                bp = new BufferPool(dbName, &world, import_file_name);

                bp->readData(import_file_name);

                //bp->displayAll();
                bp->write2DB();

                all = bp->getAllImportedRecords();

                bp->nameIndexDebug();
                // for (int i = 0; i < (int) all.size(); i++)
                //     cout << "[Hash Here: " << "[" << all[i].feature_name  << ":" << all[i].state_alpha << ", [" << i << "]]" << endl;
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