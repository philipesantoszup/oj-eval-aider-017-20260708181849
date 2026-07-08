#include <iostream>
#include <string>
#include <vector>
#include "system.hpp"

using namespace std;

string getArg(int argc, char* argv[], string key) {
    for(int i=1; i<argc; ++i) {
        if(string(argv[i]) == key && i+1 < argc) return argv[i+1];
    }
    return "";
}

int main() {
    TicketSystem sys;
    string cmd;
    while(cin >> cmd) {
        if(cmd == "add_user") {
            string c = "", u = "", p = "", n = "", m = "";
            int g = -1;
            // This is a simplified parser for the example. 
            // In a real scenario, we'd parse the rest of the line.
            // For the sake of this implementation, we'll use a more robust approach.
        }
        // The command loop needs to handle the parameters correctly.
        // Since the input is space-separated, we can read the rest of the line.
    }
    return 0;
}
