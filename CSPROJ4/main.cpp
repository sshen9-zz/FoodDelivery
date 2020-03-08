//
//  main.cpp
//  CSPROJ4
//
//  Created by Sam Shen on 3/4/20.
//  Copyright © 2020 CS Class. All rights reserved.
//

#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "ExpandableHashMap.h"
using namespace std;

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v);
bool parseDelivery(string line, string& lat, string& lon, string& item);

int main(int argc, char *argv[])
{
    StreetMap m;
    m.load("mapdata.txt");
    StreetMap* ptr = &m;
    PointToPointRouter p(ptr);
    
    GeoCoord g1("34.0388859","-118.4974725");
    GeoCoord g2("34.0493948", "-118.4636955");
    //strange case
    
//    GeoCoord g1("34.0625329", "-118.4470263");
//    GeoCoord g2("34.0711774", "-118.4495120");
    
    
    list<StreetSegment> route;
    double d;
    p.generatePointToPointRoute(g1, g2, route, d);
    list<StreetSegment>::iterator it = route.begin();
    while(it!=route.end()){
        GeoCoord g1 = (*it).start;
        GeoCoord g2 = (*it).end;
        cout<<"("<<g1.latitudeText<<","<<g1.longitudeText<<")"<<" "<<"("<<g2.latitudeText<<","<<g2.longitudeText<<") "<<(*it).name<<endl;
        it++;
    }
    

    cout<<d<<endl;
    
    
//    if (argc != 3)
//    {
//        cout << "Usage: " << argv[0] << " mapdata.txt deliveries.txt" << endl;
//        return 1;
//    }
//
//    StreetMap sm;
//
//    if (!sm.load(argv[1]))
//    {
//        cout << "Unable to load map data file " << argv[1] << endl;
//        return 1;
//    }
//
//    GeoCoord depot;
//    vector<DeliveryRequest> deliveries;
//    if (!loadDeliveryRequests(argv[2], depot, deliveries))
//    {
//        cout << "Unable to load delivery request file " << argv[2] << endl;
//        return 1;
//    }
//
//    cout << "Generating route...\n\n";
//
//    DeliveryPlanner dp(&sm);
//    vector<DeliveryCommand> dcs;
//    double totalMiles;
//    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
//    if (result == BAD_COORD)
//    {
//        cout << "One or more depot or delivery coordinates are invalid." << endl;
//        return 1;
//    }
//    if (result == NO_ROUTE)
//    {
//        cout << "No route can be found to deliver all items." << endl;
//        return 1;
//    }
//    cout << "Starting at the depot...\n";
//    for (const auto& dc : dcs)
//        cout << dc.description() << endl;
//    cout << "You are back at the depot and your deliveries are done!\n";
//    cout.setf(ios::fixed);
//    cout.precision(2);
//    cout << totalMiles << " miles travelled for all deliveries." << endl;
}

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v)
{
    ifstream inf(deliveriesFile);
    if (!inf)
        return false;
    string lat;
    string lon;
    inf >> lat >> lon;
    inf.ignore(10000, '\n');
    depot = GeoCoord(lat, lon);
    string line;
    while (getline(inf, line))
    {
        string item;
        if (parseDelivery(line, lat, lon, item))
            v.push_back(DeliveryRequest(item, GeoCoord(lat, lon)));
    }
    return true;
}

bool parseDelivery(string line, string& lat, string& lon, string& item)
{
    const size_t colon = line.find(':');
    if (colon == string::npos)
    {
        cout << "Missing colon in deliveries file line: " << line << endl;
        return false;
    }
    istringstream iss(line.substr(0, colon));
    if (!(iss >> lat >> lon))
    {
        cout << "Bad format in deliveries file line: " << line << endl;
        return false;
    }
    item = line.substr(colon + 1);
    if (item.empty())
    {
        cout << "Missing item in deliveries file line: " << line << endl;
        return false;
    }
    return true;
}
