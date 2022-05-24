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
#include "G4SPSPosDistribution.hh"
//#include "G4SystemOfUnits.hh"
//#include "Randomize.hh"
//#include "G4ThreeVector.hh"
//#include "G4RandomDirection.hh"
//#include "G4IonTable.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VisExtent.hh"


#include "TKey.h"

/// Standard includes
#include <algorithm>
#include <sstream>
#include <iterator>



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
    GpsInMaterial=false;

    Theta=0.2;
    E=10;
    wall_x = 11*m;
    wall_y = 12*m;
    wall_z = 3.2*m;

    center = {0,0,0};

    world = 0;

    fVolumesInMaterial.clear();
    fCumulativeMaterialVolume = 0;

}


GeneratorAction::~GeneratorAction(){

    if( !file==0 ){
        file->Close();
        delete file;
    }

    delete primaryGeneratorMessenger;
    delete world;
}



  G4VPhysicalVolume* GpsInMaterialPickVolume(double CumulativeMaterialVolume,  std::vector<G4VPhysicalVolume*> VolumesInMaterial ) {

  G4double r=CLHEP::RandFlat::shoot(0.,CumulativeMaterialVolume);
  G4int selectedVolume=0;
  G4double sum=0;
  do {
    //sum+=VolumesInMaterial[selectedVolume]->GetLogicalVolume()->GetSolid()->GetCubicVolume()/CLHEP::cm3;
    sum+=VolumesInMaterial[selectedVolume]->GetLogicalVolume()->GetMass( false, false )/CLHEP::kg;
    ++selectedVolume;
  } while(sum<r);

  return VolumesInMaterial[selectedVolume-1];

}



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
     cout<<"Simulated particle is"<<str<<endl;
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

void GeneratorAction::GpsInMaterialBuild( bool activate ){

    G4cout << "Building GPSInMaterial "<< G4endl;
    GpsInMaterial= activate;

}

void  GeneratorAction::GpsInMaterialSetMaterial(G4String materialName)
  {
    fVolumesInMaterial.clear();
    fCumulativeMaterialVolume = 0;

    cout<<"Selected Material is "<<materialName<<endl;


    G4PhysicalVolumeStore *PVStore = G4PhysicalVolumeStore::GetInstance();
    G4int i = 0;
    while (i<(G4int)PVStore->size()) {

      G4VPhysicalVolume* pv = (*PVStore)[i];

      if (pv->GetLogicalVolume()->GetMaterial()->GetName() == materialName) {
	         fVolumesInMaterial.push_back(pv);
	         //fCumulativeMaterialVolume+=pv->GetLogicalVolume()->GetSolid()->GetCubicVolume()/CLHEP::cm3;
           fCumulativeMaterialVolume+=pv->GetLogicalVolume()->GetMass( false, false )/CLHEP::kg;
      }
      i++;
    }

    if(fVolumesInMaterial.empty())
      throw std::runtime_error("Generator::GpsInMaterial::SetMaterial did not find volume made of '"+materialName+"'");

  }



void GeneratorAction::GeneratePrimaries(G4Event* anEvent){

    if( sample==true ){
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
      else if ( GpsInMaterial==true  ) {

            if(fVolumesInMaterial.empty())
      throw std::runtime_error("Generator::GpsInMaterial::GeneratePrimaries : no material set");

            G4VPhysicalVolume* selectedVolume = GpsInMaterialPickVolume( fCumulativeMaterialVolume,  fVolumesInMaterial );

            G4VisExtent extent=selectedVolume->GetLogicalVolume()->GetSolid()->GetExtent();
            G4ThreeVector translationToVolumeCenter((extent.GetXmax()+extent.GetXmin())/2.,(extent.GetYmax()+extent.GetYmin())/2.,(extent.GetZmax()+extent.GetZmin())/2.);

            G4SPSPosDistribution* pd= fgps->GetCurrentSource()->GetPosDist();
            pd->ConfineSourceToVolume(selectedVolume->GetName());
            pd->SetPosDisType("Volume");
            pd->SetPosDisShape("Para");
            pd->SetCentreCoords(selectedVolume->GetTranslation()+translationToVolumeCenter);
            pd->SetHalfX((extent.GetXmax()-extent.GetXmin())/2.);
            pd->SetHalfY((extent.GetYmax()-extent.GetYmin())/2.);
            pd->SetHalfZ((extent.GetZmax()-extent.GetZmin())/2.);

            fgps->GeneratePrimaryVertex(anEvent);


      }else{
            fgps->GeneratePrimaryVertex(anEvent);
    }
}
