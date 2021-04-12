/*

    @author:        Kuanysh Boranbayev

    @date:          April 12, 2021

    @description:   Geographic Information Systems. Program reads GIS record file. 
                    The program needs 3 arguments representing file names:
                        usage: ./GIS <database_file_name> <command_script_name> <log_file_name>
                    
                    Note: if a database or log file already exists, the program will override the existing ones.
                    
                    The program needs to set world boundaries before importing a record file.
                    Each line of the file containing GIS record will be parsed and imported into a given database file.
                    The program will save the imported GIS record's offset in the database file.
                    
                    Program has two separate tables to manage separate search operations:
                        1) Hash Table containing feature and state name of a GIS record along with an offset.
                        2) PR Quad Tree containing coordinate points of latitude and longitude of a GIS record along with an offset.
                    
                    Search command options:

                    what_is<tab><feature_name><tab><state_abbreviation>
                    what_is_at<tab><latitude><tab><longitude>
                    what_is_in<tab><latitude><tab>longitude><tab><half-height><tab><half-width>
                    what_is_in<tab>-long<tab><latitude><tab>longitude><tab><half-height><tab><half-width>
                    what_is_in<tab>-filter<tab>[ pop | water | structure ]<tab><latitude><tab>longitude><tab><half-height><tab><half-width>
*/
#include "CommandProcessor.h"
#include "Logger.h"


using namespace std;

int main(int argc, char **argv) {
    string command;

    if (argc < 4) {
        cerr << "usage: " << argv[0] << " <database_file_name> <command_script_file_name> <log_file_name>" << endl;
        exit(1);
    }

    ifstream script(argv[2]);
    ofstream log(argv[3], ios::out), db(argv[1], ios::out);

    log.open(argv[3], ios::out);
    log.close();

    log.open(argv[3], ios::app);

    if (script.fail()) {
        cerr << "Script File: " << argv[2] << endl;
        exit(2);
    }

    Logger * logger = new Logger(log, argv[1], argv[2], argv[3]);

    CommandProcessor * cp = new CommandProcessor (argv[1], logger);

    while (getline(script, command)) {
        cp->processCommand(command);
    }

    if (log.is_open())
        log.close();
    if (db.is_open())
        db.close();

    return 0;
}