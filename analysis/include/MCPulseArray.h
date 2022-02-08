#ifndef MCPULSEARRAY_H
#define MCPULSEARRAY_H 1

#include "MCPulse.h"

#include <algorithm>

using namespace std;

bool CompareMCPulse( const MCPulse& a, const MCPulse& b );

class MCPulseArray{

public:

	MCPulseArray();

	MCPulseArray( const MCPulseArray& a );

	MCPulseArray& operator=( const MCPulseArray& a);

	~MCPulseArray();

    void Clear(){ pulseArray.clear(); }

	void Sort(){
        std::sort( pulseArray.begin(), pulseArray.end(), CompareMCPulse );
        ResetIterator();
    }

	void PushBack( MCPulse a){
        pulseArray.push_back(a);
    }

	void ResetIterator(){ head = pulseArray.begin(); }

	friend ostream& operator<<( ostream& os, MCPulseArray& a);

	double GetNextInteractionTime();

	MCPulse FindPrimaryInteraction( double start, double window);

	MCPulse FindCoincidentInteraction( double start, double win_coin, double win_daq);

private:

	vector<MCPulse> pulseArray;

	vector<MCPulse>::iterator head;

};



#endif
