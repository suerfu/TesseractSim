/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

// $Id: GeneratorAction.hh $
//
/// \file GeneratorAction.hh
/// \brief Definition of the GeneratorAction class

#ifndef GeneratorAction_h
#define GeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
//#include "G4ThreeVector.hh"
//#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4GeneralParticleSource.hh"

#include "GeometryManager.hh"

#include "RunAction.hh"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"

class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;
class GeneratorMessenger;
class VMuonSource;
class MuonGenerator;

using std::string;

class GeneratorAction : public G4VUserPrimaryGeneratorAction {

public:

    GeneratorAction( RunAction* runAction);

    virtual ~GeneratorAction();

    virtual void GeneratePrimaries(G4Event* event);

    void UseMuonGenerator(G4bool myMuonStatus);
        // used to specify if Muon Generator should be used as particle source

    void SetSpectrum( G4String str );
        // used to specify the gamma spectrum from which to sample energy & momentum

    void SetParticleName( G4String str );
          // used to specify the particle that is being simulated

    void SetConversionSurfaceNameToIndex( G4String str );
          // used to covert the Surfacename(specified in the macro) to surfaceindex

    void SetWallX(G4double ival);
    // enter the x_length of the wall

    void SetWallY(G4double ival);
    // enter the y_length of the wall

    void SetWallZ(G4double ival);
    // enter the z_length of the wall

    void SetBoxCenter(G4ThreeVector boxCenter);

   

    void ConfineOnWall();
        // When this method is called, it is assumed that the primary particle position is on the surface of the wall.
        // Momentum direction w.r.t normal of the wall has theta and phi as polar and azimuth angle.

private:

    //void InitPGA();	

    GeneratorMessenger* primaryGeneratorMessenger;
    RunAction* fRunAction;
    CommandlineArguments* fCmdlArgs;
    G4ParticleGun*  fgun;
    G4GeneralParticleSource*  fgps;
    bool use_gps;
    G4String particle;
    int surface_index;
	G4bool useMuonGenerator;
	VMuonSource *myMuonSource;
    //MuonGenerator  *myMuonSource;
	G4ParticleTable *myParticleTable;
    G4int verbosity;
	G4String MsgPrefix;
    TFile* file;
        // Pointer to the ROOT file.
     TH2F* h1;
     // Pointer to the 2d histogram
    G4double E;
        // Energy
    G4double Theta;
    bool onwall;
        // Flag variable to denote whether particle position should be independently sampled from world surface.
        // If true, generator will use theta and phi as w.r.t. the normal direction at the sampled position.
        // Otherwise, the position and direction in the ROOT file will be used as it is.
    bool sample;
        // Flag variable to denote whether particle position and momentum should be sampled from ROOT file.
    G4double wall_x;
    G4double wall_y;
    G4double wall_z;
        // Dimentions of the experimental hall.
    G4ThreeVector center;
    G4Box* world;
        // Pointer to the experimental hall object. Used to randomly sample surface points.
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
