//
//  DeliveryPlanner.cpp
//  CSPROJ4
//
//  Created by Sam Shen on 3/4/20.
//  Copyright © 2020 CS Class. All rights reserved.
//

#include <stdio.h>
#include <sstream>
#include <fstream>
#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    string getTurnDirection(double a) const;
    const StreetMap* m_sm;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_sm = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    vector<DeliveryRequest> newOrder = deliveries;
    DeliveryOptimizer d_optimizer(m_sm);
    double oldDist;
    double newDist;
    d_optimizer.optimizeDeliveryOrder(depot, newOrder, oldDist, newDist);
    
    
    totalDistanceTravelled = 0;
    GeoCoord last = depot;
    for(int i=0; i<newOrder.size(); i++){
        PointToPointRouter p(m_sm);
        list<StreetSegment> routes;
        double d;
        if(last == newOrder[i].location){
            DeliveryCommand deliver;
            deliver.initAsDeliverCommand(newOrder[i].item);
            commands.push_back(deliver);
            continue;
        }
        DeliveryResult status_code = p.generatePointToPointRoute(last, newOrder[i].location, routes, d);
        if(status_code!=DELIVERY_SUCCESS){
            return status_code;
        }
        list<StreetSegment>::iterator it = routes.begin();
        StreetSegment lastStreet = (*it);
        double streetDist = 0;
        while(it!=routes.end()){
            StreetSegment curStreet = (*it);
            
            if(curStreet == routes.back()){
                //generate a proceed command and deliver command;
                DeliveryCommand proceed;
                double a = angleOfLine(lastStreet);
                string dir = getTurnDirection(a);
                streetDist += distanceEarthMiles(curStreet.start, curStreet.end);
                totalDistanceTravelled += distanceEarthMiles(curStreet.start, curStreet.end);
                proceed.initAsProceedCommand(dir, curStreet.name, streetDist);
                DeliveryCommand deliver;
                deliver.initAsDeliverCommand(newOrder[i].item);
                commands.push_back(proceed);
                commands.push_back(deliver);
                break;
            }
            if(curStreet.name!=lastStreet.name){
                //generate delivery command
                DeliveryCommand command;
                double a = angleOfLine(lastStreet);
                string dir = getTurnDirection(a);
                command.initAsProceedCommand(dir, lastStreet.name, streetDist);
                commands.push_back(command);
                DeliveryCommand turn;
                a = angleBetween2Lines(lastStreet, curStreet);
                if(a<1 || a>359){
                    //proceed onto the street
                    lastStreet = curStreet;
                    streetDist = 0;
                    continue;
                }else if(a>=1 && a<180){
                    //generate left turn:
                    turn.initAsTurnCommand("left", curStreet.name);
                    commands.push_back(turn);
                }else if(a>=180 && a<359){
                    //generate right turn
                    turn.initAsTurnCommand("right", curStreet.name);
                    commands.push_back(turn);
                }
                
                lastStreet = curStreet;
                streetDist = 0;
            }
            
            streetDist += distanceEarthMiles(curStreet.start, curStreet.end);
            totalDistanceTravelled += distanceEarthMiles(curStreet.start, curStreet.end);
            it++;
        }
        last = newOrder[i].location;
    }
    
    
    //GO BACK HOME
    //
    //
    
    PointToPointRouter p(m_sm);
    list<StreetSegment> routes;
    double d;
    p.generatePointToPointRoute(newOrder[newOrder.size()-1].location, depot, routes, d);
    list<StreetSegment>::iterator it = routes.begin();
    StreetSegment lastStreet = (*it);
    double streetDist = 0;
    while(it!=routes.end()){
        
        StreetSegment curStreet = (*it);
        
        if(curStreet == routes.back()){
            //generate a proceed command and deliver command;
            DeliveryCommand proceed;
            double a = angleOfLine(lastStreet);
            string dir = getTurnDirection(a);
            streetDist += distanceEarthMiles(curStreet.start, curStreet.end);
            totalDistanceTravelled += distanceEarthMiles(curStreet.start, curStreet.end);
            proceed.initAsProceedCommand(dir, curStreet.name, streetDist);
            commands.push_back(proceed);
            break;
        }
        if(curStreet.name!=lastStreet.name){
            //generate delivery command
            DeliveryCommand command;
            double a = angleOfLine(lastStreet);
            string dir = getTurnDirection(a);
            command.initAsProceedCommand(dir, lastStreet.name, streetDist);
            commands.push_back(command);
            DeliveryCommand turn;
            a = angleBetween2Lines(lastStreet, curStreet);
            if(a<1 || a>359){
                //proceed onto the street
                lastStreet = curStreet;
                streetDist = 0;
                continue;
            }else if(a>=1 && a<180){
                //generate left turn:
                turn.initAsTurnCommand("left", curStreet.name);
                commands.push_back(turn);
            }else if(a>=180 && a<359){
                //generate right turn
                turn.initAsTurnCommand("right", curStreet.name);
                commands.push_back(turn);
            }
            
            lastStreet = curStreet;
            streetDist = 0;
        }
        
        streetDist += distanceEarthMiles(curStreet.start, curStreet.end);
        totalDistanceTravelled += distanceEarthMiles(curStreet.start, curStreet.end);
        it++;
    }
    
    
    return DELIVERY_SUCCESS;
}

string DeliveryPlannerImpl::getTurnDirection(double a) const{
    string dir;
    if(a<22.5 && a>=0){
        dir = "east";
    }else if(a<67.5){
        dir = "northeast";
    }else if(a<112.5){
        dir = "north";
    }else if(a<157.5){
        dir = "northwest";
    }else if(a<202.5){
        dir = "west";
    }else if(a<247.5){
        dir = "southwest";
    }else if(a<292.5){
        dir = "south";
    }else if(a<337.5){
        dir = "southeast";
    }else{
        dir = "east";
    }
    return dir;
}


//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
