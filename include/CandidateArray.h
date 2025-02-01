#pragma once

#include "Headers.h"

#ifndef cadidates_H
#define cadidates_H

struct Scandidates {
    int player_ID = 0;
    GW::Constants::MapID MapID = GW::Constants::MapID::None;
    GW::Constants::ServerRegion MapRegion = GW::Constants::ServerRegion::Unknown;
    int MapDistrict = 0;

    bool operator==(const Scandidates& other) const {
        return player_ID == other.player_ID &&
            MapID == other.MapID &&
            MapRegion == other.MapRegion &&
            MapDistrict == other.MapDistrict;
    }
};
const int MaxCandidates = 16;

class CandidateArray {
public:
    
    CandidateArray::CandidateArray() : size(0) {
        for (int i = 0; i < MaxCandidates; ++i) {
            candidates[i] = { 0, GW::Constants::MapID::None, GW::Constants::ServerRegion::Unknown, 0 }; // Initialize all elements to default Scandidates
        }
    }

    int CandidateArray::push(const Scandidates& candidate);
    bool CandidateArray::pop(); 
    Scandidates CandidateArray::get(int index);
    bool CandidateArray::set(int index, Scandidates value);
    int CandidateArray::getSize() const { return size; }
    void CandidateArray::advancePtr() ;
    bool CandidateArray::exists(const Scandidates& candidate);
    Scandidates CandidateArray::find(const Scandidates& candidate);
    void CandidateArray::print();
    void CandidateArray::reset();

private:
    Scandidates candidates[MaxCandidates];
    int size =0;
};


class PartyCandidates {
public:
    PartyCandidates() : next_unique_key(1), KeepAliveCounter(0) {
        // Initialize arrays
        for (int i = 0; i < MaxCandidates; ++i) {
            AllCandidates[i] = Scandidates();
            UniqueKeys[i] = 0;
            Keepalive[i] = 0;
            KeepAliveOld[i] = 0;
        }
    }

    int Subscribe(const Scandidates& candidate);
    void Unsubscribe(int unique_key);
    bool IsClientAvailable(int unique_key);
    Scandidates* ListClients();
    void ClearAllClients();
    void DoKeepAlive(int unique_key);
    int GetEachKeepalive(int index) { return Keepalive[index]; }
    int GetKeepaliveCounter() { return KeepAliveCounter; }

private:
    Scandidates AllCandidates[MaxCandidates];
    int UniqueKeys[MaxCandidates] = { 0 };
    int Keepalive[MaxCandidates] = { 0 };
    int KeepAliveOld[MaxCandidates] = { 0 };
    int next_unique_key=1;

    int GenerateUniqueKey();
    int FindIndexByKey(int unique_key);
    int KeepAliveCounter=0;
    //const int KeepAliveTarget=50; gives problems not sure why
};



#endif // cadidates_H