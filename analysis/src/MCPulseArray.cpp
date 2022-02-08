
#include <cmath>

#include "MCPulseArray.h"


bool CompareMCPulse( const MCPulse& a, const MCPulse& b ){
	return a < b;
}


MCPulseArray::MCPulseArray(){
    head = pulseArray.begin();
}


MCPulseArray::MCPulseArray( const MCPulseArray& a ){
    pulseArray = a.pulseArray;
    head = pulseArray.begin();
}


MCPulseArray& MCPulseArray::operator=( const MCPulseArray& a){
    pulseArray = a.pulseArray;
    head = pulseArray.begin();
    return *this;
}


MCPulseArray::~MCPulseArray(){}


double MCPulseArray::GetNextInteractionTime(){
    if( head!=pulseArray.end() )
        return (*head).GetTime();
    return -1;
}


MCPulse MCPulseArray::FindPrimaryInteraction( double start, double window){

    MCPulse val;

    if( pulseArray.size()==0 )
        return val;

    while( head != pulseArray.end() && (*head).GetTime()-start < window ){
        val = val + *head;
        head++;
    }

    return val;
}


MCPulse MCPulseArray::FindCoincidentInteraction( double start, double win_coin, double win_daq){

    MCPulse val;

    // locate the beginning of the event.
    //
    vector<MCPulse>::iterator beg = head;

    for( beg=pulseArray.begin(); beg!=pulseArray.end(); beg++ ){
        double t = beg->GetTime();
        if( abs( float(start-t) ) < win_coin ){
            break;
        }
        else if( t-start>win_coin ){
            return val;
        }
    }

    // locate the end of the event.
    //
    vector<MCPulse>::iterator end = head;

    for( end=pulseArray.begin(); end!=pulseArray.end(); end++ ){
        double t = end->GetTime();
        if( t-start > win_daq ){
            break;
        }
    }

    // Sum the events.
    // 
    for( auto itr = beg; itr!=end; itr++ ){
        val += *itr;
    }

    head = end;

    return val;
}


ostream& operator<<( ostream& os, MCPulseArray& a){
	vector<MCPulse>::iterator itr;
	unsigned int nType;
	for( itr = a.pulseArray.begin(); itr!=a.pulseArray.end(); itr++){
		os << itr->GetTime() << '\t';
		nType = itr->GetNType();
	}
	os << endl;
	for( unsigned int i=0; i<nType; i++){
		for( itr = a.pulseArray.begin(); itr!=a.pulseArray.end(); itr++){
			os << itr->GetEnergy()[i] << '\t';
		}
		os << endl;
	}
	return os;
}

