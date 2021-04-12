#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "GISRecord.h"
#include "SystemManager.h"
#include "Logger.h"

using namespace std;

class CommandProcessor {

    int command_counter;

    World world;

    Logger * log;

    SystemManager * sys;

    BufferPool * pool;

    string dbName;

    string wlong, elong, slat, nlat;

    int processCommandString(string cmd) {
        if (cmd.at(0) == ';' || cmd.at(0) == ' ') { return IGNORE; }
        else if (cmd.rfind("world") == 0)         { return WORLD; }
        else if (cmd.rfind("import") == 0)        { return IMPORT; }
        else if (cmd.rfind("debug") == 0)         { return DEBUG; }
        else if (cmd.rfind("quit") == 0)          { return QUIT; }
        else if (cmd.rfind("what_is_at") == 0)    { return WHAT_IS_AT; }
        else if (cmd.rfind("what_is_in") == 0)    { return WHAT_IS_IN; }
        else if (cmd.rfind("what_is") == 0)       { return WHAT_IS; }
        else                                      { return IGNORE; }
    }
public:
    CommandProcessor (string db_file_name, Logger * logger) {
        log = logger;
        dbName = db_file_name;
        command_counter = 0;

        pool = new BufferPool(BUFFER_SIZE);
    }

    void processCommand(string cmd) {
        istringstream iss(cmd);
        string import_file_name;
        string param1, param2, param3, param4, param5, param6, param7;

        string result;

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

                long tmp;
                if (world.w_long > world.e_long) {
                    tmp = world.w_long;
                    world.w_long = world.e_long;
                    world.e_long = tmp;
                }

                if (world.s_lat > world.n_lat) {
                    tmp = world.s_lat;
                    world.s_lat = world.n_lat;
                    world.n_lat = tmp;
                }
                
                // TO DO: world needs validation here
                log->logWorld(cmd, world);
                
                break;
            case IMPORT:
                command_counter++;
                log->dashes();

                log->logCommand(cmd, command_counter);
                cout << "Command " << command_counter << ": " << cmd << endl;
                
                getline(iss, import_file_name, '\t');
                getline(iss, import_file_name, '\t');

                sys = new SystemManager(dbName, &world, import_file_name, pool);
                sys->import(import_file_name);
                result = sys->importStr();
                log->logStr(result);
                cout << result << endl;
                break;
            case DEBUG:
                command_counter++;
                getline(iss, param1, '\t');
                getline(iss, param2, '\t');

                log->logCommand(cmd, command_counter);
                cout << "Command " << command_counter << ": " << cmd << endl;

                if (param2 == "pool") {
                    result = sys->str(); 
                }
                if (param2 == "hash") {
                    result = sys->hashDebug();
                }
                if (param2 == "quad") {
                    result = sys->quadDebug();
                }
                if (param2 == "world") {
                    result = sys->worldDebug();
                }
                cout << result << endl;
                log->logStr(result);
                log->dashes();
                break;
            case QUIT:
                command_counter++;
                cout << "Command " << command_counter << ": " << cmd << endl;
                log->logQuit(cmd, command_counter);

                exit(EXIT_SUCCESS);
                break;
            case WHAT_IS:
                command_counter++;
                getline(iss, param1, '\t');
                getline(iss, param2, '\t');
                getline(iss, param3, '\t');
                
                log->logCommand(cmd, command_counter);
                cout << "Command " << command_counter << ": " << cmd << endl;
                
                log->logStr(sys->whatIs(param2, param3));
                cout << sys->whatIs(param2, param3) << endl;
                log->dashes();
                break;
            case WHAT_IS_AT:
                command_counter++;
                getline(iss, param1, '\t');
                getline(iss, param2, '\t');
                getline(iss, param3, '\t');

                log->logCommand(cmd, command_counter);
                cout << "Command " << command_counter << ": " << cmd << endl;
                result = sys->whatIsAt(param2, param3);
                log->logStr(result);
                cout << result << endl;

                log->dashes();
                break;
            case WHAT_IS_IN:
                command_counter++;
                getline(iss, param1, '\t');
                getline(iss, param2, '\t');
                getline(iss, param3, '\t');
                getline(iss, param4, '\t');
                getline(iss, param5, '\t');
                getline(iss, param6, '\t');
                getline(iss, param7, '\t');

                log->logCommand(cmd, command_counter);
                cout << "Command " << command_counter << ": " << cmd << endl;
                
                if (param2.rfind("-long") == 0) {
                    result = sys->whatIsInLong(param3, param4, param5, param6);
                } else if (param2.rfind("-filter") == 0) {
                    result = sys->whatIsInFilter(param4, param5, param6, param7, param3);
                } else {
                    result = sys->whatIsIn(param2, param3, param4, param5);
                }
                log->logStr(result);
                cout << result << endl;

                log->dashes();
                break;
            default:
                log->logComment(cmd);
                break;
        }
    }
};

#endif