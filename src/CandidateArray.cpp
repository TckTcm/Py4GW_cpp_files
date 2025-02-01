#pragma once

#include "CandidateArray.h"


    int CandidateArray::push(const Scandidates& candidate) {
        if (size < MaxCandidates && !exists(candidate)) {
            candidates[size] = candidate;
            size++;
            if (size >= MaxCandidates) { size = 0; }
            return size;
        }
        return 0;
    }

    bool CandidateArray::pop() {
        if (size > 0) {
            size--;
            candidates[size] = Scandidates(); // Clear the popped value
            return true;
        }
        return false; // Array is empty
    }

    Scandidates CandidateArray::get(int index) {
        if (index >= 0 && index < size) {
            return candidates[index];
        }
        return Scandidates(); // Invalid index
    }

    bool CandidateArray::set(int index, Scandidates value) {
        if (index >= 0 && index < size) {
            candidates[index] = value;
            return true;
        }
        return false; // Invalid index
    }


    void CandidateArray::advancePtr() {
        if (size < MaxCandidates) {
            size++;
        }
        else {
            reset();
            size = 0;
        }
    }

    bool CandidateArray::exists(const Scandidates& candidate) {
        for (int i = 0; i < size; ++i) {
            if (candidates[i] == candidate) {
                return true;
            }
        }
        return false;
    }

    Scandidates CandidateArray::find(const Scandidates& candidate) {
        for (int i = 0; i < size; ++i) {
            if (candidates[i] == candidate) {
                return candidates[i];
            }
        }
        return Scandidates(); // Return default if not found
    }

    void CandidateArray::print() {
        for (int i = 0; i < size; ++i) {
            std::cout << "Player ID: " << candidates[i].player_ID
               // << ", MapID: " << candidates[i].MapID
                //<< ", Mapregion: " << candidates[i].Mapregion
                << ", MapDistrict: " << candidates[i].MapDistrict << std::endl;
        }
    }


    void CandidateArray::reset() {
        while (size > 0) {
            pop();
        }
    }




int PartyCandidates::Subscribe(const Scandidates& candidate) {
    // Check if the candidate already exists and return its unique key
    for (int i = 0; i < MaxCandidates; ++i) {
        if (AllCandidates[i] == candidate) {
            return UniqueKeys[i];
        }
    }

    // If the candidate does not exist, find an available slot and insert it
    for (int i = 0; i < MaxCandidates; ++i) {
        if (UniqueKeys[i] == 0) { // Slot is available
            int unique_key = GenerateUniqueKey();
            AllCandidates[i] = candidate;
            UniqueKeys[i] = unique_key;
            return unique_key;
        }
    }

    // Return -1 if no space is available
    return -1;
}

void PartyCandidates::DoKeepAlive(int unique_key) {
    int index = FindIndexByKey(unique_key);
    if (index != -1) {
        Keepalive[index]++; // Increment keepalive counter
        if (Keepalive[index] > 100000) { Keepalive[index] = 1; }
    }

    KeepAliveCounter++;

    if (KeepAliveCounter > 50) {
        for (int i = 0; i < MaxCandidates; ++i) {
            if (Keepalive[i] == KeepAliveOld[i]) {
                AllCandidates[i] = Scandidates(); // Reset to default
                UniqueKeys[i] = 0;                // Mark slot as available
                Keepalive[i] = 0;
                KeepAliveOld[i] = 0;
            }
            else {
                KeepAliveOld[i] = Keepalive[i];
            }
        }
        KeepAliveCounter = 0;
    }
}

void PartyCandidates::Unsubscribe(int unique_key) {
    int index = FindIndexByKey(unique_key);
    if (index != -1) {
        AllCandidates[index] = Scandidates(); // Reset to default
        UniqueKeys[index] = 0; // Mark slot as available
    }
}

bool PartyCandidates::IsClientAvailable(int unique_key) {
    return FindIndexByKey(unique_key) != -1;
}

Scandidates* PartyCandidates::ListClients() {
    return AllCandidates;
}

void PartyCandidates::ClearAllClients() {
    for (int i = 0; i < MaxCandidates; ++i) {
        AllCandidates[i] = Scandidates(); // Reset to default
        UniqueKeys[i] = 0;                // Mark slot as available
    }
}

int PartyCandidates::GenerateUniqueKey() {
    if (next_unique_key == 0) { next_unique_key = 1; }
    return next_unique_key++;
}

int PartyCandidates::FindIndexByKey(int unique_key) {
    for (int i = 0; i < MaxCandidates; ++i) {
        if (UniqueKeys[i] == unique_key) {
            return i;
        }
    }
    return -1; // Key not found
}
