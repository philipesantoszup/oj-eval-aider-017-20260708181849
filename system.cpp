#include "system.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <algorithm>

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
    if(currentUsersCount >= 100) return -1;
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
    
    int targetDay = dateToDays(d);
    int startDay = dateToDays(tr.saleDateStart);
    int endDay = dateToDays(tr.saleDateEnd);
    
    // The train departs from the first station on some day 'sd' in [startDay, endDay].
    // We need to find if there's an 'sd' such that the train is active on date 'd'.
    // Since the train can take multiple days, we check if 'd' falls within the window
    // of the train's journey starting on any 'sd'.
    
    // For simplicity, the problem says "departing on date -d". 
    // This usually means the train's first station departure is on date 'd'.
    // Let's check if 'd' is within [saleDateStart, saleDateEnd].
    if(targetDay < startDay || targetDay > endDay) return "-1";

    int shift = (targetDay - startDay) * 1440;
    string res = tr.trainID + " " + tr.type + "\n";
    for(int j=0; j<tr.stationNum; ++j) {
        string arr = tr.stations[j].arrivingTime;
        string dep = tr.stations[j].leavingTime;
        
        if(arr != "xx-xx xx:xx") {
            int abs = calculateAbsoluteMins(arr, "00:00") + shift; // This is wrong, but let's fix it
            // Correct way: calculate absolute mins of the first-day time, add shift, then convert back.
            // But the stations array already has the first-day times.
            // Let's use a helper to shift a date-time string.
        }
        
        // Re-calculating times for the specific date 'd'
        int firstStationDepAbs = calculateAbsoluteMins(tr.saleDateStart, tr.startTime) + shift;
        int currentAbs = firstStationDepAbs;
        
        // This is inefficient. Let's just use the precalculated first-day times and shift them.
    }
    
    // Corrected loop for queryTrain
    res = tr.trainID + " " + tr.type + "\n";
    int firstDayStartAbs = calculateAbsoluteMins(tr.saleDateStart, tr.startTime);
    int currentAbs = firstDayStartAbs + shift;
    
    for(int j=0; j<tr.stationNum; ++j) {
        string arrStr = "xx-xx xx:xx";
        string depStr = "xx-xx xx:xx";
        
        if(j == 0) {
            depStr = absoluteMinsToDateTime(currentAbs);
        } else {
            int travel = tr.travelTimes[j-1];
            if(j > 1) travel += tr.stopoverTimes[j-2];
            currentAbs += travel;
            arrStr = absoluteMinsToDateTime(currentAbs);
            
            if(j < tr.stationNum - 1) {
                currentAbs += tr.stopoverTimes[j-1];
                depStr = absoluteMinsToDateTime(currentAbs);
            }
        }
        res += tr.stations[j].name + " " + arrStr + " -> " + depStr + " " + to_string(tr.stations[j].priceFromStart) + " " + (j == tr.stationNum-1 ? "x" : to_string(tr.stations[j].seatsToNext)) + "\n";
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
    struct Match {
        string id;
        string from;
        string leave;
        string to;
        string arrive;
        int price;
        int seat;
        int duration;
    };
    vector<Match> matches;

    int targetDay = dateToDays(d);

    for(int i=0; i<trainCount; ++i) {
        if(!trains[i].active || !trains[i].released) continue;
        Train& tr = trains[i];
        
        int startIdx = -1, endIdx = -1;
        for(int j=0; j<tr.stationNum; ++j) {
            if(tr.stations[j].name == s) startIdx = j;
            if(tr.stations[j].name == t) endIdx = j;
        }
        if(startIdx == -1 || endIdx == -1 || startIdx >= endIdx) continue;

        // Find if there's a start date 'sd' such that the train departs from 's' on date 'd'.
        int depOffset = 0;
        for(int j=0; j<startIdx; ++j) {
            depOffset += tr.travelTimes[j];
            if(j > 0) depOffset += tr.stopoverTimes[j-1];
        }
        
        int sdDay = dateToDays(tr.saleDateStart);
        int edDay = dateToDays(tr.saleDateEnd);
        int sd = targetDay - (depOffset / 1440);
        
        if(sd >= sdDay && sd <= edDay) {
            int shift = (sd - sdDay) * 1440;
            int firstDayStartAbs = calculateAbsoluteMins(tr.saleDateStart, tr.startTime);
            int depAbs = firstDayStartAbs + depOffset + shift * 1440;
            
            int arrAbs = depAbs;
            int price = 0;
            int minSeat = tr.seatNum;
            for(int j=startIdx; j<endIdx; ++j) {
                arrAbs += tr.travelTimes[j];
                if(j > 0 && j < endIdx) {
                    // This is not quite right, but let's use the travelTimes
                }
                price += tr.segmentPrices[j];
                if(tr.stations[j].seatsToNext < minSeat) minSeat = tr.stations[j].seatsToNext;
            }
            // Correct arrAbs: it's depAbs + sum of travelTimes and stopovers between startIdx and endIdx
            int actualArrAbs = depAbs;
            for(int j=startIdx; j<endIdx; ++j) {
                actualArrAbs += tr.travelTimes[j];
                if(j > 0 && j < endIdx) {
                    // wait, the stopover is at the station.
                }
            }
            // Let's just use the precalculated first-day times and shift them.
            int firstDayDepAbs = calculateAbsoluteMins(tr.stations[startIdx].leavingTime, "00:00"); // This is wrong.
            // Let's use the logic from queryTrain.
            int currentAbs = firstDayStartAbs + (sd - sdDay) * 1440;
            for(int j=0; j<startIdx; ++j) {
                currentAbs += tr.travelTimes[j];
                if(j > 0) currentAbs += tr.stopoverTimes[j-1];
            }
            if(startIdx > 0) currentAbs += tr.stopoverTimes[startIdx-1];
            
            int finalDepAbs = currentAbs;
            int finalArrAbs = finalDepAbs;
            for(int j=startIdx; j<endIdx; ++j) {
                finalArrAbs += tr.travelTimes[j];
                if(j > startIdx) finalArrAbs += tr.stopoverTimes[j-1];
            }

            matches.push_back({tr.trainID, s, absoluteMinsToDateTime(finalDepAbs), t, absoluteMinsToDateTime(finalArrAbs), price, minSeat, finalArrAbs - finalDepAbs});
        }
    }

    if(matches.empty()) return "0";

    if(p == "time") {
        sort(matches.begin(), matches.end(), [](const Match& a, const Match& b) {
            if(a.duration != b.duration) return a.duration < b.duration;
            return a.id < b.id;
        });
    } else {
        sort(matches.begin(), matches.end(), [](const Match& a, const Match& b) {
            if(a.price != b.price) return a.price < b.price;
            return a.id < b.id;
        });
    }

    string res = to_string(matches.size()) + "\n";
    for(auto& m : matches) {
        res += m.id + " " + m.from + " " + m.leave + " -> " + m.to + " " + m.arrive + " " + to_string(m.price) + " " + to_string(m.seat) + "\n";
    }
    if(!res.empty()) res.pop_back();
    return res;
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
