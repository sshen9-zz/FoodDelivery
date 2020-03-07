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
    double distance(const GeoCoord c1, const GeoCoord c2) const;
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

double PointToPointRouterImpl::distance(const GeoCoord c1, const GeoCoord c2) const{
    return sqrt((c1.latitude-c2.latitude)*(c1.latitude-c2.latitude)+(c1.longitude-c2.longitude)*(c1.longitude-c2.longitude));
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
        cur = q.top().key;
        q.pop();
        if(cur == end){
            break;
        }

        //add all geocoords connecting to the start coord
        vector<StreetSegment> v;
        m_smPtr->getSegmentsThatStartWith(cur, v);
        for(int i=0; i<v.size(); i++){
            double new_cost = *(cost_so_far.find(cur))+distance(cur, v[i].end);
            double *p = cost_so_far.find(v[i].end);
            if(p == nullptr || new_cost<(*p)){
                cost_so_far.associate(v[i].end, new_cost);
                double priority = new_cost+distance(v[i].end, end);
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
    //reverse the street segments
    stack<GeoCoord> stk;
    while(c!=start){
        stk.push(c);
        c = *came_from.find(c);
    }
    
    while(!stk.empty()){
        GeoCoord c1 = stk.top();
        stk.pop();
        GeoCoord c2 = stk.top();
        stk.pop();
        string name;
        vector<StreetSegment> v;
        m_smPtr->getSegmentsThatStartWith(c1, v);
        for(int i=0; i<v.size(); i++){
            if(v[i].end == c2){
                name = v[i].name;
            }
        }
        
        StreetSegment s(c1, c2, name);
        route.push_back(s);
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
