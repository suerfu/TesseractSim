#ifndef MCPULSE_H
#define MCPULSE_H 1

#include <string>
#include <vector>
#include <ostream>

using namespace std;

class MCPulse{

public:

	static unsigned int GetNType(){ return nType; }

	MCPulse();

	MCPulse( double t, vector<double> a);

	MCPulse( const MCPulse& a);

	~MCPulse();

	MCPulse& operator=( const MCPulse& a);

	bool operator<( const MCPulse& a ) const { return time < a.time; }

	MCPulse& operator+( const MCPulse& a);

	void operator+=( const MCPulse& a);

	void SetTime( double a ){ time = a; }
	
	double GetTime() const{ return time; }
	
	void SetEnergy( int i, double a ){ if(i<nType) energy[i] = a; }
	
	void SetEnergy( vector<double> a ){
		if(a.size()==nType)
			energy = a;
	}

	vector<double> GetEnergy(){ return energy; }

	friend ostream& operator<<( ostream& os, const MCPulse& a);

private:

	static const unsigned int nType = 3;

	double time;

	vector<double> energy;
};


#endif
