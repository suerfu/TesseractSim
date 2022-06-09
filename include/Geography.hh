#ifndef Geography_h
#define Geography_h 1

#include "globals.hh"

class Geography {

public:
	Geography(G4String pathToDataFile);
	~Geography();
	G4double getDepth(size_t theta, size_t phi);

private:
  static const size_t nbZenitSteps = 91;
  static const size_t nbAzimuthSteps = 360;
	G4double geoMap[nbZenitSteps][nbAzimuthSteps];
};

#endif