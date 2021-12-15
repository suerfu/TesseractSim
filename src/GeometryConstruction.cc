/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
/// \file GeometryConstruction.cc
/// \brief Implementation of the GeometryConstruction class

#include "GeometryConstruction.hh"

#include "G4SubtractionSolid.hh"

#include "GeometryConstructionMessenger.hh"
#include "GeometryManager.hh"
#include "GeoShielding.hh"
#include "GeoCryostat.hh"
#include "FarsideDetectorMessenger.hh"
#include "FarsideDetector.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

//#include "G4IStore.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <sstream>



GeometryConstruction::GeometryConstruction() : G4VUserDetectorConstruction(){

    fCheckOverlaps = true;
    fDetectorMessenger = new GeometryConstructionMessenger( this );

    // Set default values for world.
	//FIXME! change to dimension of the experimental hall.

    world_x = 10.*m;
    world_y = 10.*m;
    world_z = 10.*m;

    simple_cube = new SimpleCube();
    fFarsideMessenger = new FarsideDetectorMessenger();
}



GeometryConstruction::~GeometryConstruction(){
    delete fDetectorMessenger;
    delete fFarsideMessenger;
}



G4VPhysicalVolume* GeometryConstruction::Construct(){

    G4cout << "Constructing geometry...\n";

    G4VPhysicalVolume* world_pv = ConstructWorld();

    /* Other user-defined geometries goes here */
    ConstructUserVolumes();

	//set visualization attributes according to material.
	GeometryManager::Get()->SetVisAttributes();

    return world_pv;
}



G4VPhysicalVolume* GeometryConstruction::ConstructWorld(){

    G4String world_name = "world";
    G4Material* world_material = GeometryManager::Get()->GetMaterial("G4_Galactic");//fGeometryManager->GetMaterial("G4_Galactic");

    G4Box* world_solid = new G4Box( world_name+"_sld", world_x/2.0, world_y/2.0, world_z/2.0);
    G4LogicalVolume* world_lv = new G4LogicalVolume( world_solid, world_material, world_name+"_lv");
    G4VPhysicalVolume* world_pv = new G4PVPlacement( 0, G4ThreeVector(0,0,0), world_lv, world_name, 0, false, 0,fCheckOverlaps);

    world_lv->SetVisAttributes( G4VisAttributes::Invisible );

    return world_pv;
}


void GeometryConstruction::ConstructUserVolumes(){
	G4cout<<"Construct user volumes..."<<G4endl;
	// At this point GeometryManager::SetFilePath() has been called by GeometryConstructionMessenger
	// Mark that we are ready to load dimensions!
	GeometryManager::Get()->GeometryTypeAndFilesSet();
	// Load dimensions.
	GeometryManager::Get()->LoadDimensions();
	G4int geoType = GeometryManager::Get()->GetGeometryType();
	G4cout<<"geometry type is "<<geoType<<G4endl;

	//Construct geometry.
	//It is a good idea to use only one geoType tag to control major and sub types.
	//It helps to avoid potentially conflicting user commends.
	if(geoType==0){ //TESSERACT
	        G4cout<<"TESSERACT"<<G4endl;
			//Each component is instantiated and constructed seperately.
			GeoShielding* TESSERACTShield = new GeoShielding();
			GeoCryostat* TESSERACTCryostat = new GeoCryostat();
			//GeoDetectorSPICE* detectorSPICE = new GeoDetectorSPICE());
      GeoDetectorHerald* detectorHERALD = new GeoDetectorHerald();
			TESSERACTShield->Construct();
			TESSERACTCryostat->Construct();
			//FIXME! Add HERALD to another geoType.
			// detectorSPICE->Construct();
      detectorHERALD->Construct();
	}
	else if(geoType==1){ //Cubic cow
		simple_cube->Construct();

		const int Nfs = 6;
		FarsideDetector* fs[Nfs];
		for( int i=0; i<Nfs; i++){
			std::stringstream name;
			name << "fs" << i;
			fs[i] = new FarsideDetector();// fGeometryManager );

			G4double distance = 50 * cm;
			G4double angle = i*CLHEP::twopi/Nfs;
			G4ThreeVector pos( distance*cos(angle), distance*sin(angle),0 );
			fs[i] -> PlaceDetector( name.str(), pos );
		}
	}
	else{
		G4cerr<<"GeometryConstruction:: Geometry Type"<<geoType<<" not defined!"<<G4endl;
	}
	G4cout<<"User volumes constructed!!!"<<G4endl;


}




/*
G4VIStore* GeometryConstruction::CreateImportanceStore(){

    //===============  Importance Sampling to speed up simulation ==============//

    G4IStore *istore = G4IStore::GetInstance();

    istore->AddImportanceGeometryCell( 1, *world_pv);
    for( unsigned int i=0; i<list.size(); i++ ){
        istore->AddImportanceGeometryCell( list[i].bias, *list[i].phy);
    }

    return istore;
}
*/
