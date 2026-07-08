#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "system.hpp"

using namespace std;

string getParam(const vector<string>& params, string key) {
    for (size_t i = 0; i < params.size(); ++i) {
        if (params[i] == key && i + 1 < params.size()) {
            return params[i + 1];
        }
    }
    return "";
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    TicketSystem sys;
    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string cmd;
        ss >> cmd;
        if (cmd.empty()) continue;

        vector<string> params;
        string p;
        while (ss >> p) {
            params.push_back(p);
        }

        if (cmd == "add_user") {
            string c = getParam(params, "-c");
            string u = getParam(params, "-u");
            string p_val = getParam(params, "-p");
            string n = getParam(params, "-n");
            string m = getParam(params, "-m");
            string g_str = getParam(params, "-g");
            int g = g_str.empty() ? -1 : stoi(g_str);
            cout << sys.addUser(c, u, p_val, n, m, g) << "\n";
        } else if (cmd == "login") {
            string u = getParam(params, "-u");
            string p = getParam(params, "-p");
            cout << sys.login(u, p) << "\n";
        } else if (cmd == "logout") {
            string u = getParam(params, "-u");
            cout << sys.logout(u) << "\n";
        } else if (cmd == "query_profile") {
            string c = getParam(params, "-c");
            string u = getParam(params, "-u");
            cout << sys.queryProfile(c, u) << "\n";
        } else if (cmd == "modify_profile") {
            string c = getParam(params, "-c");
            string u = getParam(params, "-u");
            string p = getParam(params, "-p");
            string n = getParam(params, "-n");
            string m = getParam(params, "-m");
            string g_str = getParam(params, "-g");
            
            bool hasP = false, hasN = false, hasM = false, hasG = false;
            int g = -1;
            for(size_t i=0; i<params.size(); ++i) {
                if(params[i] == "-p") hasP = true;
                if(params[i] == "-n") hasN = true;
                if(params[i] == "-m") hasM = true;
                if(params[i] == "-g") {
                    hasG = true;
                    g = stoi(params[i+1]);
                }
            }
            cout << sys.modifyProfile(c, u, p, n, m, g, hasP, hasN, hasM, hasG) << "\n";
        } else if (cmd == "add_train") {
            string i = getParam(params, "-i");
            int n = stoi(getParam(params, "-n"));
            int m = stoi(getParam(params, "-m"));
            string s = getParam(params, "-s");
            string p_val = getParam(params, "-p");
            string x = getParam(params, "-x");
            string t = getParam(params, "-t");
            string o = getParam(params, "-o");
            string d = getParam(params, "-d");
            string y = getParam(params, "-y");
            cout << sys.addTrain(i, n, m, s, p_val, x, t, o, d, y) << "\n";
        } else if (cmd == "release_train") {
            string i = getParam(params, "-i");
            cout << sys.releaseTrain(i) << "\n";
        } else if (cmd == "query_train") {
            string i = getParam(params, "-i");
            string d = getParam(params, "-d");
            cout << sys.queryTrain(i, d) << "\n";
        } else if (cmd == "delete_train") {
            string i = getParam(params, "-i");
            cout << sys.deleteTrain(i) << "\n";
        } else if (cmd == "query_ticket") {
            string s = getParam(params, "-s");
            string t = getParam(params, "-t");
            string d = getParam(params, "-d");
            string p = getParam(params, "-p");
            cout << sys.queryTicket(s, t, d, p) << "\n";
        } else if (cmd == "query_transfer") {
            string s = getParam(params, "-s");
            string t = getParam(params, "-t");
            string d = getParam(params, "-d");
            string p = getParam(params, "-p");
            cout << sys.queryTransfer(s, t, d, p) << "\n";
        } else if (cmd == "buy_ticket") {
            string u = getParam(params, "-u");
            string i = getParam(params, "-i");
            string d = getParam(params, "-d");
            int n = stoi(getParam(params, "-n"));
            string f = getParam(params, "-f");
            string t = getParam(params, "-t");
            string q_str = getParam(params, "-q");
            bool q = (q_str == "true");
            cout << sys.buyTicket(u, i, d, n, f, t, q) << "\n";
        } else if (cmd == "query_order") {
            string u = getParam(params, "-u");
            cout << sys.queryOrder(u) << "\n";
        } else if (cmd == "refund_ticket") {
            string u = getParam(params, "-u");
            string n_str = getParam(params, "-n");
            int n = n_str.empty() ? 1 : stoi(n_str);
            cout << sys.refundTicket(u, n) << "\n";
        } else if (cmd == "clean") {
            cout << sys.clean() << "\n";
        } else if (cmd == "exit") {
            cout << sys.exitSystem() << "\n";
            break;
        }
    }
    return 0;
}
