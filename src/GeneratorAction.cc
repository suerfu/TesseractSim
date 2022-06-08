/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
//
/// \file GeneratorAction.cc
/// \brief Implementation of the GeneratorAction class
#include "CLHEP/Random/RandFlat.h"
#include "GeneratorAction.hh"
#include "GeneratorMessenger.hh"
#include "MuonGenerator.hh"
#include "G4RunManager.hh"
//#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GeneralParticleSource.hh"
//#include "G4SystemOfUnits.hh"
//#include "Randomize.hh"
//#include "G4ThreeVector.hh"
//#include "G4RandomDirection.hh"
//#include "G4IonTable.hh"

#include "TKey.h"



GeneratorAction::GeneratorAction( RunAction* runAction) : G4VUserPrimaryGeneratorAction(),
    fRunAction( runAction), useMuonGenerator(true),myMuonSource(NULL), fgun(NULL), fgps(NULL),
    myParticleTable(NULL), verbosity(1), MsgPrefix("")
{

    fCmdlArgs = fRunAction->GetCommandlineArguments();

    //fgun = new G4ParticleGun();
    //fgps = new G4GeneralParticleSource();

    G4int n_particle = 1;
	//use GPS or muon generator
	if (useMuonGenerator == true) {
		fgun = new G4ParticleGun(n_particle);
		//if(sourceID==1){
		myMuonSource = new MuonGenerator();
		//}
		//if(sourceID==2){
		//  myMuonSource = new InterfaceToMUSUN(); //TODO add MUSIN
		//}
		// default particle
		G4ParticleDefinition* particleDef = myParticleTable->FindParticle("mu-");
		fgun->SetParticleDefinition(particleDef);
		fgun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.));
		fgun->SetParticleEnergy(200. * GeV);
	} else {
	  G4cout << MsgPrefix << "Use General Particle Source." << G4endl;
	  fgps = new G4GeneralParticleSource();
	}
    //this->InitPGA();

    //Npostponed = 0;
    //NDelayed = 0;

    file = 0;
    particle="geantino";

    myParticleTable = G4ParticleTable::GetParticleTable();
    surface_index=0;


    primaryGeneratorMessenger = new GeneratorMessenger(this);

    onwall = false;
    sample = false;

    Theta=0.2;
    E=10;
    wall_x = 11*m;
    wall_y = 12*m;
    wall_z = 3.2*m;

    center = {0,0,0};

    world = 0;
}


GeneratorAction::~GeneratorAction(){

    if( !file==0 ){
        file->Close();
        delete file;
    }
	if (myMuonSource != NULL) {
		delete myMuonSource;
		myMuonSource = NULL;
	}

  	if (fgps != NULL) {
		delete fgps;
		fgps = NULL;
	}

	if (fgun != NULL) {
		delete fgun;
		fgun = NULL;
	}
    delete primaryGeneratorMessenger;
    delete world;
}
void GeneratorAction::UseMuonGenerator(G4bool myMuonStatus) {
	useMuonGenerator = myMuonStatus;
	if(verbosity>=1){
	  G4cout << MsgPrefix << "Use muon source: " << useMuonGenerator << G4endl;
	}
	//this->InitPGA();
}


/*void GeneratorAction::InitPGA() {
	G4int n_particle = 1;
	//use GPS or muon generator
	if (useMuonGenerator == true) {
		fgun = new G4ParticleGun(n_particle);
		//if(sourceID==1){
		myMuonSource = new MuonGenerator();
		//}
		//if(sourceID==2){
		//  myMuonSource = new InterfaceToMUSUN(); //TODO add MUSIN
		//}
		// default particle
		G4ParticleDefinition* particleDef = myParticleTable->FindParticle("mu-");
		fgun->SetParticleDefinition(particleDef);
		fgun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.));
		fgun->SetParticleEnergy(200. * GeV);
	} else {
	  G4cout << MsgPrefix << "Use General Particle Source." << G4endl;
	  fgps = new G4GeneralParticleSource();
	}

}
*/
G4ThreeVector SetPosition(int surface, double wall_x,double wall_y,double wall_z, G4ThreeVector center){
  switch(surface)
          {
              case 0: //Top XY, Z > 0
                  return G4ThreeVector(CLHEP::RandFlat::shoot(-wall_x/2.0,wall_x/2.0),CLHEP::RandFlat::shoot(-wall_y/2.0,wall_y/2.0),
                                                           wall_z/2.0) + center ;

              case 1: //Bottom XY, Z < 0
                  return G4ThreeVector(CLHEP::RandFlat::shoot(-wall_x/2.0,wall_x/2.0),CLHEP::RandFlat::shoot(-wall_y/2.0,wall_y/2.0),
                                                           -wall_z/2.0)+center;

              case 2: //SideRight,  XZ, Y > 0
                  return G4ThreeVector(CLHEP::RandFlat::shoot(-wall_x/2.0,wall_x/2.0),
                                                       wall_y/2.0,
                                                      CLHEP::RandFlat::shoot(-wall_z/2.0,wall_z/2.0))+center;


              case 3: //SideLeft, // XZ, Y < 0
                   return G4ThreeVector( CLHEP::RandFlat::shoot(-wall_x/2.0,wall_x/2.0),-wall_y/2.0,
                                                      CLHEP::RandFlat::shoot(-wall_z/2.0,wall_z/2.0))+center;



              case 4: //Front YZ, X > 0
                   return G4ThreeVector(wall_x/2.0, CLHEP::RandFlat::shoot(-wall_y/2.0,wall_y/2.0),
                                                      CLHEP::RandFlat::shoot(-wall_z/2.0,wall_z/2.0))+center;



              case  5: //Back YZ, X < 0
                  return G4ThreeVector(-wall_x/2.0,CLHEP::RandFlat::shoot(-wall_y/2.0,wall_y/2.0),
                                                      CLHEP::RandFlat::shoot(-wall_z/2.0,wall_z/2.0))+center;
           }
    //fgun->SetParticlePosition( G4ThreeVector(x,y,z) );
}




G4ThreeVector SetDirection(int surface, G4double Theta ){
  G4double phi = CLHEP::RandFlat::shoot(0.,2.*CLHEP::pi);
  G4double cosTheta = Theta;
  G4double sinTheta = std::sqrt(1 - cosTheta*cosTheta);


  switch(surface)
          {
              case 0://Top XY, Z > 0
                  return G4ThreeVector(std::cos(phi)*sinTheta, std::sin(phi)*sinTheta,-cosTheta);

              case 1://Bottom XY, Z < 0
                  return G4ThreeVector(std::cos(phi)*sinTheta, std::sin(phi)*sinTheta, cosTheta);

              case 2://SideRight,  XZ, Y > 0
                  return G4ThreeVector(std::cos(phi)*sinTheta,-cosTheta , std::sin(phi)*sinTheta);


              case 3://SideLeft, // XZ, Y < 0
                  return G4ThreeVector(std::cos(phi)*sinTheta, cosTheta, std::sin(phi)*sinTheta);


              case 4://Front YZ, X > 0
                  return G4ThreeVector(-cosTheta, std::sin(phi)*sinTheta,  std::cos(phi)*sinTheta);


              case 5://Back YZ, X < 0
                  return G4ThreeVector(cosTheta, std::sin(phi)*sinTheta,  std::cos(phi)*sinTheta);

           }
    //fgun->SetParticleMomentumDirection( G4ThreeVector(px,py,pz) );
}

G4double GetTotalSurfaceArea(G4double wall_x,G4double wall_y,G4double wall_z) {
  return 2*(wall_y*wall_z + wall_x*wall_y + wall_z*wall_x);
}

int GetRandomSurfaceIndex(G4double wall_x,G4double wall_y,G4double wall_z){


  G4double Total_Area=GetTotalSurfaceArea(wall_x,wall_y,wall_z);

  G4double Top = wall_x * wall_y/Total_Area;
  G4double Bottom = wall_x * wall_y/Total_Area + Top ;
  G4double SideRight = wall_x * wall_z/Total_Area + Bottom;
  G4double SideLeft = wall_x * wall_z/Total_Area + SideRight;
  G4double Front = wall_y * wall_z/Total_Area + SideLeft;
  G4double Back = wall_y * wall_z/Total_Area + Front;

  double a = CLHEP::RandFlat::shoot(0.0,1.0);

  int index;

  if (a<Top) {
    index=0;
  } else if (Top<=a && a<Bottom) {
    index=1;
  } else if (Bottom<=a && a<SideRight) {
    index=2;
  } else if (SideRight<=a && a<SideLeft) {
    index=3;
  } else if (SideLeft<=a && a<Front) {
    index=4;
  }else{
    index=5;
  }
return index;

}


G4double SetEnergy(G4double E){
    return E * keV;
}

void GeneratorAction::SetParticleName( G4String str ){
     particle=str;
     cout<<"Simulated particle is "<<str<<endl;
}

void GeneratorAction::SetConversionSurfaceNameToIndex(G4String str){
  cout<<"Selected surface is "<<str<<endl;
  if (str == "Top") {
    surface_index=0;
  }
  else if (str == "Bottom") {
    surface_index=1;
  }
  else if (str == "SideRight") {
    surface_index=2;
  }
  else if (str == "SideLeft") {
    surface_index=3;
  }
  else if (str == "Front") {
    surface_index=4;
  }
  else if (str == "Back") {
    surface_index=5;
  }
  else if(str == "All") {
    surface_index=6;//All
    cout<<"Selected surface is the whole cubic surface"<<endl;
  }
  else {
    surface_index=6;
    cout<<"Didnot match any available option, thus selected the default option of generating particles from all the surface"<<endl;
  }
}

void GeneratorAction::SetWallX(G4double ival)
{
  wall_x = ival;
}

void GeneratorAction::SetWallY(G4double ival)
{
  wall_y = ival;
}

void GeneratorAction::SetWallZ(G4double ival)
{
  wall_z = ival;
}

void GeneratorAction::SetBoxCenter(G4ThreeVector boxCenter)
{
  center = boxCenter;
}


void GeneratorAction::SetSpectrum( G4String str ){

    G4cout << "Setting generator spectrum to " << str << G4endl;

    // If a file is already opened, close it first.
    if( file!=0 && file->IsOpen() ){
        file->Close();
    }

    G4cout << "Opening " << str << G4endl;

    // open in read mode (default)
    file = new TFile( str.c_str() );
    if( !file->IsOpen() ){
        G4cerr << "Error opening " << s << G4endl;
        return;
    }

    //h1 = (TH2F*)file->Get("thetaE2_pdf");
    TIter next(file->GetListOfKeys());
    TString T2HF_name;
    TKey *key;
    while ((key=(TKey*)next())) {
     T2HF_name=key->GetName();
    }
    h1 = (TH2F*)file->Get(T2HF_name);

    sample = true;

}


void GeneratorAction::GeneratePrimaries(G4Event* anEvent){


	if (useMuonGenerator == true) {
	  myMuonSource->GenerateMuon();
	  G4double initialEnergy = myMuonSource->GetStartEnergy();
	  G4ThreeVector initialPosition = myMuonSource->GetStartPosition();
	  G4ThreeVector initialDirection = myMuonSource->GetStartDirection();
	  G4String particleID = myMuonSource->GetParticleID();
	  //if(useGeantinos){
	  //  particleID = "geantino";
	 // }
	  G4ParticleDefinition* particleDef = myParticleTable->FindParticle(particleID);
	  fgun->SetParticleDefinition(particleDef);
	  fgun->SetParticleEnergy(initialEnergy);
	  fgun->SetParticleMomentumDirection(initialDirection);
	  fgun->SetParticlePosition(initialPosition);
	  fgun->GeneratePrimaryVertex(anEvent);
	}

  else if( sample==true ){
        h1->GetRandom2(E,Theta);
        if (surface_index==6) {
          int random_surface_index=GetRandomSurfaceIndex(wall_x,wall_y,wall_z);
          fgun->SetParticlePosition( SetPosition(random_surface_index, wall_x,wall_y,wall_z,center) );
          fgun->SetParticleMomentumDirection( SetDirection(random_surface_index,Theta) );
        } else {
          fgun->SetParticlePosition( SetPosition(surface_index, wall_x,wall_y,wall_z,center) );
          fgun->SetParticleMomentumDirection( SetDirection(surface_index,Theta) );
        }
        fgun->SetParticleEnergy(SetEnergy(E));
        fgun->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( particle ) );
        fgun->GeneratePrimaryVertex(anEvent);
      }
    else{
        fgps->GeneratePrimaryVertex(anEvent);
    }
}
