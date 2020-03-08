//
//  StreetMap.cpp
//  CSPROJ4
//
//  Created by Sam Shen on 3/4/20.
//  Copyright © 2020 CS Class. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "provided.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include <cctype>
#include "ExpandableHashMap.h"
#include <stack>
using namespace std;

template <typename T>
std::string to_string_with_precision(const T value, const int n = 7)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << value;
    return out.str();
}

unsigned int hash(const GeoCoord& g)
{
    return hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> m_map;
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream inf("/Users/sshen9/Desktop/CSPROJ4/CSPROJ4/mapdata.txt");
    if(!inf){
        //cant find the file
        return false;
    }
    
    stack<string> streetNames;
    
    string line;
    while(getline(inf, line)){
        istringstream iss(line);
        double la1;
        double lo1;
        double la2;
        double lo2;
        if(!(iss>>la1>>lo1>>la2>>lo2)){
            for(int i=0; i<line.size(); i++){
                if(!isdigit(line[i])){
                    streetNames.push(line);
                    break;
                }
            }
            continue;
        }
        
        string streetName = streetNames.top();
        GeoCoord start(to_string_with_precision(la1), to_string_with_precision(lo1));
        GeoCoord end(to_string_with_precision(la2), to_string_with_precision(lo2));
        StreetSegment s(start, end, streetName);
        
//        cout<<start.latitudeText<<","<<start.longitudeText<<"     "<<end.latitudeText<<","<<end.longitudeText<<endl;
        
        vector<StreetSegment>* p = m_map.find(start);
        if(p!=nullptr){
            (*p).push_back(s);
        }else{
            vector<StreetSegment> vec;
            vec.push_back(s);
            m_map.associate(start, vec);
        }
        
        
        
        StreetSegment s_reverse(end, start, streetName);
        p = m_map.find(end);
        if(p!=nullptr){
            (*p).push_back(s_reverse);
        }else{
            vector<StreetSegment> reverse_vec;
            reverse_vec.push_back(s_reverse);
            m_map.associate(end, reverse_vec);
        }
    }
    
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment>* p = m_map.find(gc);
    if(p==nullptr){

        return false;
    }
    segs.clear();
    
    for(int i=0; i<(*p).size(); i++){
        segs.push_back((*p)[i]);
    }
    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}

