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

#include "G4RunManager.hh"
//#include "G4Event.hh"
#include "G4ParticleGun.hh"
//#include "G4ParticleTable.hh"
//#include "G4ParticleDefinition.hh"
#include "G4GeneralParticleSource.hh"
//#include "G4SystemOfUnits.hh"
//#include "Randomize.hh"
//#include "G4ThreeVector.hh"
//#include "G4RandomDirection.hh"
//#include "G4IonTable.hh"



GeneratorAction::GeneratorAction( RunAction* runAction) : G4VUserPrimaryGeneratorAction(),
    fRunAction( runAction)
{

    fCmdlArgs = fRunAction->GetCommandlineArguments();

    fgun = new G4ParticleGun();
    fgps = new G4GeneralParticleSource();

    //Npostponed = 0;
    //NDelayed = 0;

    file = 0;
    particle="geantino";

    surface_index=0;


    primaryGeneratorMessenger = new GeneratorMessenger(this);

    onwall = false;
    sample = false;

    Theta=0.2;
    E=10;
    wall_x = 11*m;
    wall_y = 12*m;
    wall_z = 3.2*m;

    world = 0;
}


GeneratorAction::~GeneratorAction(){

    if( !file==0 ){
        file->Close();
        delete file;
    }

    delete primaryGeneratorMessenger;
    delete world;
}

G4ThreeVector SetPosition(int surface, double wall_x,double wall_y,double wall_z){
  switch(surface)
          {
              case 0: //Top XY, Z > 0
                  return G4ThreeVector(CLHEP::RandFlat::shoot(-wall_x/2.0,wall_x/2.0),CLHEP::RandFlat::shoot(-wall_y/2.0,wall_y/2.0),
                                                           wall_z/2.0);

              case 1: //Bottom XY, Z < 0
                  return G4ThreeVector(CLHEP::RandFlat::shoot(-wall_x/2.0,wall_x/2.0),CLHEP::RandFlat::shoot(-wall_y/2.0,wall_y/2.0),
                                                           -wall_z/2.0);

              case 2: //SideRight,  XZ, Y > 0
                  return G4ThreeVector(CLHEP::RandFlat::shoot(-wall_x/2.0,wall_x/2.0),
                                                       wall_y/2.0,
                                                      CLHEP::RandFlat::shoot(-wall_z/2.0,wall_z/2.0));


              case 3: //SideLeft, // XZ, Y < 0
                   return G4ThreeVector( CLHEP::RandFlat::shoot(-wall_x/2.0,wall_x/2.0),-wall_y/2.0,
                                                      CLHEP::RandFlat::shoot(-wall_z/2.0,wall_z/2.0));



              case 4: //Front YZ, X > 0
                   return G4ThreeVector(wall_x/2.0, CLHEP::RandFlat::shoot(-wall_y/2.0,wall_y/2.0),
                                                      CLHEP::RandFlat::shoot(-wall_z/2.0,wall_z/2.0));



              case  5: //Back YZ, X < 0
                  return G4ThreeVector(-wall_x/2.0,CLHEP::RandFlat::shoot(-wall_y/2.0,wall_y/2.0),
                                                      CLHEP::RandFlat::shoot(-wall_z/2.0,wall_z/2.0));
           }
    //fgun->SetParticlePosition( G4ThreeVector(x,y,z) );
}


double Sin(double cosTheta)
{
    return std::sqrt(1 - std::pow(cosTheta, 2));
}

double UniformCosTheta()
{
    return CLHEP::RandFlat::shoot(-1.,1.);
}


G4ThreeVector SetDirection(int surface, double Theta ){
  G4double phi = CLHEP::RandFlat::shoot(0.,2.*CLHEP::pi);
  G4double cosTheta = Theta;
  G4double sinTheta = Sin(cosTheta);


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
                  return G4ThreeVector(cosTheta, std::sin(phi)*sinTheta,  std::cos(phi)*sinTheta);


              case 5://Back YZ, X < 0
                  return G4ThreeVector(cosTheta, std::sin(phi)*sinTheta,  std::cos(phi)*sinTheta);

           }
    //fgun->SetParticleMomentumDirection( G4ThreeVector(px,py,pz) );
}


G4double SetEnergy(double E){
    return E;
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
  }else {
    surface_index=0;
    cout<<"Selected surface is "<<str<< "is not found, thus default option Top is seleccted"<<endl;
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

    h1 = (TH2F*)file->Get("thetaE2_pdf");

    sample = true;

}


void GeneratorAction::GeneratePrimaries(G4Event* anEvent){

    if( sample==true ){
        h1->GetRandom2(E,Theta);
        fgun->SetParticlePosition( SetPosition(surface_index, wall_x,wall_y,wall_z) );
        fgun->SetParticleMomentumDirection( SetDirection(Theta,surface_index) );
        fgun->SetParticleEnergy(SetEnergy(E));
        fgun->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( particle ) );
        fgun->GeneratePrimaryVertex(anEvent);
      }
    else{
        fgps->GeneratePrimaryVertex(anEvent);
    }
}
