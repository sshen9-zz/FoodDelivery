//
//  PointToPointRouter.cpp
//  CSPROJ4
//
//  Created by Sam Shen on 3/4/20.
//  Copyright © 2020 CS Class. All rights reserved.
//

#include <stdio.h>
#include "provided.h"
#include <list>
#include <queue>
#include "ExpandableHashMap.h"
#include <stack>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_smPtr;
};

struct PriorityPair{
    PriorityPair(const GeoCoord& g, const double d):key(g), val(d)
    {}
    PriorityPair();
    GeoCoord key;
    double val;
};

bool operator<(const PriorityPair& p1, const PriorityPair& p2){
    return (p1.val<p2.val);
}

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_smPtr = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{

    
    if(start == end){
        totalDistanceTravelled = 0;
        route.clear();
        return DELIVERY_SUCCESS;
    }
    
    vector<StreetSegment> temp;
    if(!(m_smPtr->getSegmentsThatStartWith(start, temp) || !(m_smPtr->getSegmentsThatStartWith(end, temp)))){
        return BAD_COORD;
    }

    
    priority_queue<PriorityPair> q;
    
    ExpandableHashMap<GeoCoord, double> cost_so_far;
    ExpandableHashMap<GeoCoord, GeoCoord> came_from;
    
    q.push(PriorityPair(start, 0));
    
    cost_so_far.associate(start, 0);
    came_from.associate(start, start);

    GeoCoord cur;
    

    while(!q.empty()){
        string s;
        cur = q.top().key;
        q.pop();
        if(cur == end){
            break;
        }

        //add all geocoords connecting to the start coord
        vector<StreetSegment> v;
        m_smPtr->getSegmentsThatStartWith(cur, v);
        for(int i=0; i<v.size(); i++){
            double new_cost = *(cost_so_far.find(cur))+distanceEarthMiles(cur, v[i].end);
            double *p = cost_so_far.find(v[i].end);
            if(p == nullptr || new_cost<(*p)){
                cost_so_far.associate(v[i].end, new_cost);
                
                double priority = -(new_cost+distanceEarthMiles(v[i].end, end));
                q.push(PriorityPair(v[i].end, priority));
                came_from.associate(v[i].end, cur);
            }
            
        }
        
    }
    
    if(cur!=end){
        return NO_ROUTE;
    }
    
    route.clear();
    
    GeoCoord c = end;
    stack<StreetSegment> stk;
    //reverse the street segments
    while(c!=start){
        GeoCoord c2 = c;
        c = *came_from.find(c);
        string name;
        vector<StreetSegment> v;
        m_smPtr->getSegmentsThatStartWith(c, v);
        for(int i=0; i<v.size(); i++){
            if(v[i].end == c2){
                name = v[i].name;
            }
        }
        StreetSegment s(c, c2, name);
        stk.push(s);
    }
    
    while(!stk.empty()){
        route.push_back(stk.top());
        stk.pop();
    }

    totalDistanceTravelled = *(cost_so_far.find(end));

    return DELIVERY_SUCCESS;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
