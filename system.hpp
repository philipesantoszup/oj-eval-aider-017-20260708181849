#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>

using namespace std;

struct User {
    string username;
    string password;
    string name;
    string mailAddr;
    int privilege;
    bool active;
};

struct TrainStation {
    string name;
    string arrivingTime; // mm-dd hr:mi
    string leavingTime; // mm-dd hr:mi
    int priceFromStart;
    int seatsToNext;
};

struct Train {
    string trainID;
    string type;
    int stationNum;
    int seatNum;
    string startTime;
    string saleDateStart;
    string saleDateEnd;
    bool released;
    
    TrainStation* stations;
    int* segmentPrices;
    int* travelTimes;
    int* stopoverTimes;
    
    bool active;
};

struct Order {
    string username;
    string trainID;
    string fromStation;
    string toStation;
    string leavingTime;
    string arrivingTime;
    int price;
    int num;
    string status; // "success", "pending", "refunded"
    long long timestamp;
    bool active;
};

class TicketSystem {
public:
    TicketSystem();
    ~TicketSystem();

    int addUser(string cur_user, string u, string p, string n, string m, int g);
    int login(string u, string p);
    int logout(string u);
    string queryProfile(string c, string u);
    string modifyProfile(string c, string u, string p, string n, string m, int g, bool hasP, bool hasN, bool hasM, bool hasG);
    
    int addTrain(string i, int n, int m, string s, string p, string x, string t, string o, string d, string y);
    int releaseTrain(string i);
    string queryTrain(string i, string d);
    int deleteTrain(string i);
    
    string queryTicket(string s, string t, string d, string p);
    string queryTransfer(string s, string t, string d, string p);
    string buyTicket(string u, string i, string d, int n, string f, string t, bool q);
    string queryOrder(string u);
    int refundTicket(string u, int n);
    
    int clean();
    string exitSystem();

private:
    User* users;
    int userCount;
    int userCapacity;

    Train* trains;
    int trainCount;
    int trainCapacity;

    Order* orders;
    int orderCount;
    int orderCapacity;

    string currentUsers[100];
    int currentUsersCount;

    long long currentTime;

    // Helper functions
    int findUser(string u);
    int findTrain(string i);
    int findOrder(string u, int n);
    int dateToDays(string date);
    string daysToDate(int days);
    int timeToMins(string time);
    string minsToTime(int mins);
    int calculateAbsoluteMins(string date, string time);
    string absoluteMinsToDateTime(int absMins);
};

#endif
