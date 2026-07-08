#include "system.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>

TicketSystem::TicketSystem() {
    userCapacity = 10000;
    users = new User[userCapacity];
    userCount = 0;

    trainCapacity = 1000;
    trains = new Train[trainCapacity];
    trainCount = 0;

    orderCapacity = 100000;
    orders = new Order[orderCapacity];
    orderCount = 0;

    currentUsersCount = 0;
    currentTime = 0;
}

TicketSystem::~TicketSystem() {
    delete[] users;
    for(int i=0; i<trainCount; ++i) {
        if(trains[i].active) {
            if(trains[i].stations) delete[] trains[i].stations;
            if(trains[i].segmentPrices) delete[] trains[i].segmentPrices;
            if(trains[i].travelTimes) delete[] trains[i].travelTimes;
            if(trains[i].stopoverTimes) delete[] trains[i].stopoverTimes;
        }
    }
    delete[] trains;
    delete[] orders;
}

int TicketSystem::findUser(string u) {
    for(int i=0; i<userCount; ++i) {
        if(users[i].active && users[i].username == u) return i;
    }
    return -1;
}

int TicketSystem::findTrain(string i) {
    for(int j=0; j<trainCount; ++j) {
        if(trains[j].active && trains[j].trainID == i) return j;
    }
    return -1;
}

int TicketSystem::dateToDays(string date) {
    int m = stoi(date.substr(0, 2));
    int d = stoi(date.substr(3, 2));
    int months[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days = 0;
    for(int i=1; i<m; ++i) days += months[i];
    days += d;
    return days;
}

int TicketSystem::timeToMins(string time) {
    int h = stoi(time.substr(0, 2));
    int m = stoi(time.substr(3, 2));
    return h * 60 + m;
}

int TicketSystem::calculateAbsoluteMins(string date, string time) {
    return dateToDays(date) * 1440 + timeToMins(time);
}

string TicketSystem::absoluteMinsToDateTime(int absMins) {
    int days = absMins / 1440;
    int mins = absMins % 1440;
    
    int months[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int m = 1;
    int d = days;
    while(m <= 12 && d > months[m]) {
        d -= months[m];
        m++;
    }
    
    char buf[20];
    sprintf(buf, "%02d-%02d %02d:%02d", m, d, mins/60, mins%60);
    return string(buf);
}

int TicketSystem::addUser(string cur_user, string u, string p, string n, string m, int g) {
    if(findUser(u) != -1) return -1;
    
    if(userCount >= userCapacity) {
        int newCap = userCapacity * 2;
        User* newUsers = new User[newCap];
        for(int i=0; i<userCount; ++i) newUsers[i] = users[i];
        delete[] users;
        users = newUsers;
        userCapacity = newCap;
    }

    if(userCount == 0) {
        users[userCount].username = u;
        users[userCount].password = p;
        users[userCount].name = n;
        users[userCount].mailAddr = m;
        users[userCount].privilege = 10;
        users[userCount].active = true;
        userCount++;
        return 0;
    }

    int curIdx = findUser(cur_user);
    if(curIdx == -1 || users[curIdx].privilege <= g) return -1;

    users[userCount].username = u;
    users[userCount].password = p;
    users[userCount].name = n;
    users[userCount].mailAddr = m;
    users[userCount].privilege = g;
    users[userCount].active = true;
    userCount++;
    return 0;
}

int TicketSystem::login(string u, string p) {
    int idx = findUser(u);
    if(idx == -1 || users[idx].password != p) return -1;
    for(int i=0; i<currentUsersCount; ++i) if(currentUsers[i] == u) return -1;
    if(currentUsersCount >= 100) return -1; // Limit based on system.hpp
    currentUsers[currentUsersCount++] = u;
    return 0;
}

int TicketSystem::logout(string u) {
    for(int i=0; i<currentUsersCount; ++i) {
        if(currentUsers[i] == u) {
            currentUsers[i] = currentUsers[--currentUsersCount];
            return 0;
        }
    }
    return -1;
}

string TicketSystem::queryProfile(string c, string u) {
    int cIdx = findUser(c);
    int uIdx = findUser(u);
    if(cIdx == -1 || uIdx == -1) return "-1";
    
    bool loggedIn = false;
    for(int i=0; i<currentUsersCount; ++i) if(currentUsers[i] == c) loggedIn = true;
    if(!loggedIn) return "-1";

    if(!(users[cIdx].privilege > users[uIdx].privilege || c == u)) return "-1";

    return users[uIdx].username + " " + users[uIdx].name + " " + users[uIdx].mailAddr + " " + to_string(users[uIdx].privilege);
}

string TicketSystem::modifyProfile(string c, string u, string p, string n, string m, int g, bool hasP, bool hasN, bool hasM, bool hasG) {
    int cIdx = findUser(c);
    int uIdx = findUser(u);
    if(cIdx == -1 || uIdx == -1) return "-1";

    bool loggedIn = false;
    for(int i=0; i<currentUsersCount; ++i) if(currentUsers[i] == c) loggedIn = true;
    if(!loggedIn) return "-1";

    if(!(users[cIdx].privilege > users[uIdx].privilege || c == u)) return "-1";
    if(hasG && g >= users[cIdx].privilege) return "-1";

    if(hasP) users[uIdx].password = p;
    if(hasN) users[uIdx].name = n;
    if(hasM) users[uIdx].mailAddr = m;
    if(hasG) users[uIdx].privilege = g;

    return users[uIdx].username + " " + users[uIdx].name + " " + users[uIdx].mailAddr + " " + to_string(users[uIdx].privilege);
}

int TicketSystem::addTrain(string i, int n, int m, string s, string p, string x, string t, string o, string d, string y) {
    if(findTrain(i) != -1) return -1;

    if(trainCount >= trainCapacity) {
        int newCap = trainCapacity * 2;
        Train* newTrains = new Train[newCap];
        for(int j=0; j<trainCount; ++j) newTrains[j] = trains[j];
        delete[] trains;
        trains = newTrains;
        trainCapacity = newCap;
    }

    Train& tr = trains[trainCount++];
    tr.trainID = i;
    tr.type = y;
    tr.stationNum = n;
    tr.seatNum = m;
    tr.startTime = x;
    tr.released = false;
    tr.active = true;

    tr.stations = new TrainStation[n];
    tr.segmentPrices = new int[n-1];
    tr.travelTimes = new int[n-1];
    tr.stopoverTimes = new int[n-2];

    stringstream ss(s), sp(p), st(t), so(o), sd(d);
    string item;
    for(int j=0; j<n; ++j) {
        getline(ss, item, '|');
        tr.stations[j].name = item;
    }
    for(int j=0; j<n-1; ++j) {
        getline(sp, item, '|');
        tr.segmentPrices[j] = stoi(item);
    }
    for(int j=0; j<n-1; ++j) {
        getline(st, item, '|');
        tr.travelTimes[j] = stoi(item);
    }
    for(int j=0; j<n-2; ++j) {
        getline(so, item, '|');
        tr.stopoverTimes[j] = stoi(item);
    }
    getline(sd, tr.saleDateStart, '|');
    getline(sd, tr.saleDateEnd, '|');

    int currentAbsMins = calculateAbsoluteMins(tr.saleDateStart, tr.startTime);
    int cumulativePrice = 0;
    for(int j=0; j<n; ++j) {
        if(j == 0) {
            tr.stations[j].arrivingTime = "xx-xx xx:xx";
            tr.stations[j].leavingTime = absoluteMinsToDateTime(currentAbsMins);
            tr.stations[j].priceFromStart = 0;
        } else {
            int travel = tr.travelTimes[j-1];
            if(j > 1) travel += tr.stopoverTimes[j-2];
            currentAbsMins += travel;
            tr.stations[j].arrivingTime = absoluteMinsToDateTime(currentAbsMins);
            
            if(j < n-1) {
                currentAbsMins += tr.stopoverTimes[j-1];
                tr.stations[j].leavingTime = absoluteMinsToDateTime(currentAbsMins);
            } else {
                tr.stations[j].leavingTime = "xx-xx xx:xx";
            }
            
            cumulativePrice += tr.segmentPrices[j-1];
            tr.stations[j].priceFromStart = cumulativePrice;
        }
        tr.stations[j].seatsToNext = m;
    }
    tr.stations[n-1].seatsToNext = 0;

    return 0;
}

int TicketSystem::releaseTrain(string i) {
    int idx = findTrain(i);
    if(idx == -1) return -1;
    trains[idx].released = true;
    return 0;
}

string TicketSystem::queryTrain(string i, string d) {
    int idx = findTrain(i);
    if(idx == -1) return "-1";
    Train& tr = trains[idx];
    
    string res = tr.trainID + " " + tr.type + "\n";
    for(int j=0; j<tr.stationNum; ++j) {
        res += tr.stations[j].name + " " + tr.stations[j].arrivingTime + " -> " + tr.stations[j].leavingTime + " " + to_string(tr.stations[j].priceFromStart) + " " + (j == tr.stationNum-1 ? "x" : to_string(tr.stations[j].seatsToNext)) + "\n";
    }
    if(!res.empty()) res.pop_back();
    return res;
}

int TicketSystem::deleteTrain(string i) {
    int idx = findTrain(i);
    if(idx == -1 || trains[idx].released) return -1;
    trains[idx].active = false;
    return 0;
}

string TicketSystem::queryTicket(string s, string t, string d, string p) {
    return "0";
}

string TicketSystem::queryTransfer(string s, string t, string d, string p) {
    return "0";
}

string TicketSystem::buyTicket(string u, string i, string d, int n, string f, string t, bool q) {
    return "-1";
}

string TicketSystem::queryOrder(string u) {
    return "-1";
}

int TicketSystem::refundTicket(string u, int n) {
    return -1;
}

int TicketSystem::clean() {
    for(int i=0; i<trainCount; ++i) {
        if(trains[i].active) {
            if(trains[i].stations) delete[] trains[i].stations;
            if(trains[i].segmentPrices) delete[] trains[i].segmentPrices;
            if(trains[i].travelTimes) delete[] trains[i].travelTimes;
            if(trains[i].stopoverTimes) delete[] trains[i].stopoverTimes;
        }
    }
    userCount = 0;
    trainCount = 0;
    orderCount = 0;
    currentUsersCount = 0;
    return 0;
}

string TicketSystem::exitSystem() {
    return "bye";
}
