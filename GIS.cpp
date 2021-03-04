
#include "CommandProcessor.h"
#include "Logger.h"


using namespace std;

int main(int argc, char **argv) {
    string command;

    if (argc < 4) {
        cout << "usage: " << argv[0] << " <database_file_name> <command_script_file_name> <log_file_name>" << endl;
        exit(1);
    }

    ifstream script(argv[2]);
    ofstream log(argv[3], ios::out), db(argv[1], ios::out);

    log.open(argv[3], ios::out);
    db.open(argv[1], ios::out);
    log.close();
    db.close();
    log.open(argv[3], ios::app);
    db.open(argv[1], ios::app);

    if (script.fail()) {
        cerr << "Script File: " << argv[2] << endl;
        exit(2);
    }

    Logger * logger = new Logger(log, argv[3]);

    CommandProcessor * cp = new CommandProcessor (db, logger);

    while (getline(script, command)) {
        cp->processCommand(command);
    }

    if (log.is_open())
        log.close();
    if (db.is_open())
        db.close();

    return 0;
}