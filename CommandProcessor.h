#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "GIS.h"
#include "SystemManager.h"
#include "Logger.h"

using namespace std;

class CommandProcessor {

    int numCommands;

    World world;

    Logger * log;

    SystemManager * sys;

    //ofstream * db;
    string dbName;

    string wlong, elong, slat, nlat;
    char *wl;

    int processCommandString(string cmd) {
        if (cmd.at(0) == ';' || cmd.at(0) == ' ') { return IGNORE; }
        else if (cmd.rfind("world") == 0) { return WORLD; }
        else if (cmd.rfind("import") == 0) { return IMPORT; }
        else if (cmd.rfind("debug") == 0) { return DEBUG; }
        else if (cmd.rfind("quit") == 0) { return QUIT; }
        else if (cmd.rfind("what_is_at") == 0) { return WHAT_IS_AT; }
        else if (cmd.rfind("what_is_in") == 0) { return WHAT_IS_IN; }
        else if (cmd.rfind("what_is") == 0) { return WHAT_IS; }
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
        string param1, param2, param3;

        switch (processCommandString(cmd)) {
            case WORLD:
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
                log->logWorld(cmd, world);
                
                break;
            case IMPORT:
                numCommands++;
                log->logCommand(cmd, numCommands);
                getline(iss, import_file_name, '\t');
                getline(iss, import_file_name, '\t');
                cout << "File:" << import_file_name << "|" << endl;

                sys = new SystemManager(dbName, &world, import_file_name);
                cout << "F" << endl;
                log->logStr(sys->import(import_file_name));
                cout << "T" << endl;
                log->dashes();
                sys->str();
                break;
            case DEBUG:
                numCommands++;
                // log command inputs
                getline(iss, param1, '\t');
                getline(iss, param2, '\t');
                log->logCommand(cmd, numCommands);

                if (param2 == "pool") {
                    log->logStr(sys->str());
                }
                if (param2 == "hash") {
                    log->logStr(sys->hashDebug());
                }
                if (param2 == "quad") {
                    log->logStr(sys->quadDebug());
                }

                break;
            case QUIT:
                numCommands++;
                
                log->logQuit(cmd, numCommands);
                break;
            case WHAT_IS:
                numCommands++;
                getline(iss, param1, '\t');
                getline(iss, param2, '\t');
                getline(iss, param3, '\t');
                
                log->logCommand(cmd, numCommands);
                
                log->logStr(sys->whatIs(param2, param3));
                break;
            case WHAT_IS_AT:
                numCommands++;
                numCommands++;
                getline(iss, param1, '\t');
                getline(iss, param2, '\t');
                getline(iss, param3, '\t');

                log->logCommand(cmd, numCommands);

                log->logStr(sys->whatIsAt(param2, param3));
                
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