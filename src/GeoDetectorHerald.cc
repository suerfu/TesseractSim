
#include "GeoDetectorHerald.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

GeoDetectorHerald::GeoDetectorHerald(){


    fCheckOverlaps = false;

}


void GeoDetectorHerald::Construct(){

    G4LogicalVolume* motherLogic = GeometryManager::Get()->GetLogicalVolume("world");
    if( motherLogic==0 ){
        G4cerr << "Cannot find the logical volume of world." << G4endl;
        return;
    }

  G4String name = "Detector";

  G4Tubs* Helium_cell = new G4Tubs( name+"Helium_cell", 0.0*cm, 5.0*cm,
                      10.0*cm/2,
                      0, 2*M_PI);

  //Logic volumes
  G4LogicalVolume* Helium_cellLogic = new G4LogicalVolume(Helium_cell,
                          GeometryManager::Get()->GetMaterial("LHe"),
                          name+"Helium_cellLV");
  //Placements
  G4VPhysicalVolume* Helium_cellPhysical = new G4PVPlacement( 0,
                          G4ThreeVector(0,0,7.5*mm), //Haha, right, there is a 7.5mm offset--Xinran
                          Helium_cellLogic,
                          name+"Helium_cellPhysical",
                          motherLogic, //Inside the main world
                          false,
                          0,
                          fCheckOverlaps);

}
