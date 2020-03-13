//
//  DeliveryOptimizer.cpp
//  CSPROJ4
//
//  Created by Sam Shen on 3/4/20.
//  Copyright © 2020 CS Class. All rights reserved.
//

#include <stdio.h>
#include "provided.h"
#include <vector>
#include <cmath>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    double calculateDist(GeoCoord depot, vector<DeliveryRequest>& deliveries) const;
    const StreetMap* m_sm;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_sm = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

double DeliveryOptimizerImpl::calculateDist(GeoCoord depot, vector<DeliveryRequest>& deliveries) const{
    double d = 0;
    GeoCoord last = depot;
    for(int i=0; i<deliveries.size(); i++){
        d+=distanceEarthMiles(last, deliveries[i].location);
        last = deliveries[i].location;
    }
    
    d+=distanceEarthMiles(last, depot);
    return d;
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = calculateDist(depot, deliveries);
    
    if(deliveries.size()<=1){
        return;
    }
    
    //Set initial temp
    double T = 100000;

    //Cooling rate
    double coolingRate = 0.003;
    
    vector<DeliveryRequest> bestOrder = deliveries;
    
    while(T>1){
        vector<DeliveryRequest> newOrder = deliveries;
        
        //generate two random indicies
        int index1 = rand() % deliveries.size();
        int index2 = index1;
        while(index2 == index1){
            index2 = rand() % deliveries.size();
        }
        
        //swap the two locations in the new order
        DeliveryRequest temp = newOrder[index1];
        newOrder[index1] = newOrder[index2];
        newOrder[index2] = temp;
        
        //calculate new distance in new order and current order
        double new_dist = calculateDist(depot, newOrder);
        double cur_dist = calculateDist(depot, deliveries);
        
        if(new_dist<cur_dist){
            deliveries = newOrder;
        }else{
            double p = exp((cur_dist-new_dist)/T);
            double d = rand() % 100000;
            d/=100000;
            if(p>d){
                //accept the change
                deliveries = newOrder;
            }
        }
        
        new_dist = calculateDist(depot, deliveries);
        double best_dist = calculateDist(depot, bestOrder);
        if(new_dist<best_dist){
            bestOrder = deliveries;
        }
        
        T*=(1-coolingRate);
    }
    deliveries = bestOrder;
    newCrowDistance = calculateDist(depot, bestOrder);

}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
