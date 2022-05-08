#ifndef GEOWALLS_H
#define GEOWALLS_H 1

#include "GeometryManager.hh"


#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

//class GeoWallsMessenger;

class GeoWalls{

public:

    GeoWalls();

    ~GeoWalls();

    void Construct();

private:

    //GeoWallsMessenger* fGeoWallsMessenger;

    G4bool fCheckOverlaps;

};


#endif
