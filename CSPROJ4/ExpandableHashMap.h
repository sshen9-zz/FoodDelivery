//
//  ExpandableHashMap.h
//  CSPROJ4
//
//  Created by Sam Shen on 3/4/20.
//  Copyright © 2020 CS Class. All rights reserved.
//

#ifndef ExpandableHashMap_h
#define ExpandableHashMap_h

#include "provided.h"

#include <list>


template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;
    
private:
    struct Pair{
        KeyType key;
        ValueType val;
    };
    int m_size;
    int m_numBuckets;
    double m_maxLoad;
    std::list<Pair>* m_arr;
    int getBucketNumber(const KeyType& key) const;
    
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    m_numBuckets = 8;
    m_maxLoad = maximumLoadFactor;
    if(maximumLoadFactor<0){
        m_maxLoad = 0.5;
    }
    m_arr = new std::list<Pair>[m_numBuckets];
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    delete [] m_arr;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    delete [] m_arr;
    m_numBuckets = 8;
    m_arr = new std::list<Pair>[m_numBuckets];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_size;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{

    ValueType* ptr = find(key);
    if(ptr!=nullptr){
        (*ptr) = value;
        return;
    }
    
    //if key is not in hashmap,
    int bucket = getBucketNumber(key);
    
    Pair p;
    p.key = key;
    p.val = value;
    m_arr[bucket].push_back(p);
    
    //increment size
    m_size+=1;
    
    if(((static_cast<double>(m_size)) / m_numBuckets) > m_maxLoad){
        //rehash everything in current map into new map that is twice the size
        m_numBuckets*=2;
        std::list<Pair>* temp = new std::list<Pair>[m_numBuckets];
        for(int i=0; i<(m_numBuckets)/2; i++){
            if(m_arr[i].size()!=0){
                typename std::list<Pair>::iterator it = m_arr[i].begin();
                while(it!=m_arr[i].end()){
                    int slot = getBucketNumber((*it).key);
                    temp[slot].push_back((*it));
                    it++;
                }
            }
        }
        
        
        
        delete [] m_arr;
        m_arr = temp;
        
    }
    
    
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    int bucket = getBucketNumber(key);

    typename std::list<Pair>::iterator it = m_arr[bucket].begin();
    while(it!=m_arr[bucket].end()){
        if((*it).key == key){
            return &(*it).val;
        }
        it++;
    }
    return nullptr;
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType& key) const {
    unsigned int hash(const KeyType& k); // prototype unsigned
    unsigned int h = hash(key);
    return h%m_numBuckets;
}


#endif /* ExpandableHashMap_h */
