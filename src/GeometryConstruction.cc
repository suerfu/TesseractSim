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

	G4cout<<"Constructing user volumes..."<<G4endl;

	G4int geoType = GeometryManager::Get()->GetGeometryType();
	G4cout<<"geometry type is "<<geoType<<G4endl;

	//Construct geometry.
	//It is a good idea to use only one geoType tag to control major and sub types.
	//It helps to avoid potentially conflicting user commends.
    //
	if(geoType/100==1){ //TESSERACT
	        G4cout<<"TESSERACT"<<G4endl;
			//Each component is instantiated and constructed seperately.
			GeoShielding* TESSERACTShield = new GeoShielding();
			GeoCryostat* TESSERACTCryostat = new GeoCryostat();
			TESSERACTShield->Construct();
			TESSERACTCryostat->Construct();
			if(geoType%100==0){
				G4cout<<" Dummy detector"<<G4endl;
				G4Tubs* virtualDetector_physical = new G4Tubs( "virtualDetectorphysical",
							0,
							GeometryManager::Get()->GetDimensions("MXCWallInnerRadius"),
							(GeometryManager::Get()->GetDimensions("MXCWallHeight") - 20*mm)/2,//20mm is the top beam attachment height.
							0, 2*M_PI);
				G4LogicalVolume * virtualDetectorLogic = new G4LogicalVolume(virtualDetector_physical,
							GeometryManager::Get()->GetMaterial("G4_Galactic"),
							"virtualDetectorLV");
				G4VPhysicalVolume * virtualDetector = new G4PVPlacement(0,
							G4ThreeVector(0,0,-10*mm),
							virtualDetectorLogic,
							"virtualDetector",
							GeometryManager::Get()->GetLogicalVolume("world"),
							false,
							0,
							fCheckOverlaps);

			}else if(geoType%100==1){
				//GeoDetectorHeRALD* detectorHeRALD = new GeoDetectorHeRALD());
				// detectorHeRALD->Construct();
			}else if(geoType%100==2){
				//GeoDetectorSPICE* detectorSPICE = new GeoDetectorSPICE());
				// detectorSPICE->Construct();
			}
	}
    else if( geoType/100==2){

        G4double density = 3.26 * g/cm3;

        G4Material* rockMaterial = GeometryManager::Get()->GetMaterial( "Rock_SURF" );

        G4LogicalVolume* world_lv = GeometryManager::Get()->GetLogicalVolume("world");

        G4Box* rock_solid = new G4Box( "rock_solid", world_x/2.0, world_y/2.0, world_z/4.0);
        G4LogicalVolume* rock_lv = new G4LogicalVolume( rock_solid, rockMaterial, "rock_lv");
        G4VPhysicalVolume* rock_pv = new G4PVPlacement( 0, G4ThreeVector(0,0,-world_z/4), rock_lv, "rockSURF", world_lv, false, 0, fCheckOverlaps);

        G4Material* virtualDetMaterial = GeometryManager::Get()->GetMaterial( "G4_Galactic" );

        G4Box* det_solid = new G4Box( "virtualDetector_solid", world_x/2.0, world_y/2.0, world_z/4.0);
        G4LogicalVolume* det_lv = new G4LogicalVolume( det_solid, virtualDetMaterial, "virtualDetector_lv");
        G4VPhysicalVolume* det_pv = new G4PVPlacement( 0, G4ThreeVector(0,0,world_z/4), det_lv, "virtualDetector", world_lv, false, 0, fCheckOverlaps);

    }
	else{ //Cubic cow
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
