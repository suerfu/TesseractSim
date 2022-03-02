
#include "GeoShielding.hh"
#include "GeometryManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

GeoShielding::GeoShielding( ){

//    L_in = 5 * cm;
//    L_out = 10 * cm;

    fCheckOverlaps = GeometryManager::Get()->GetCheckOverlaps();

 //   fGeoShieldingMessenger = new GeoShieldingMessenger( this );
}


void GeoShielding::Construct(){
    
    G4LogicalVolume* worldLogic = GeometryManager::Get()->GetLogicalVolume("world");
    if( worldLogic==0 ){
        G4cerr << "Cannot find the logical volume of world." << G4endl;
        return;
    }

    G4String name = "Shield";
	
	G4double boxWidth     = GeometryManager::Get()->GetDimensions("shieldWidth");
	G4double boxHeight    = GeometryManager::Get()->GetDimensions("shieldHeight");
	//G4double SSThickness  = GeometryManager::Get()->GetDimensions("SSThickness");
	//G4double PbThickness  = GeometryManager::Get()->GetDimensions("PbThickness");
	//G4double PEThickness  = GeometryManager::Get()->GetDimensions("PEThickness");
	G4double cavityRadius = GeometryManager::Get()->GetDimensions("cavityRadius");
	G4double cavityOffset = GeometryManager::Get()->GetDimensions("cavityOffset");
	G4double neckRadius   = GeometryManager::Get()->GetDimensions("neckRadius");
	G4double shieldZOffset= GeometryManager::Get()->GetDimensions("shieldZOffset");

	//additional shielding
	G4bool hasCollar = true;
	G4double collarHeight = GeometryManager::Get()->GetDimensions("collarHeight");
	hasCollar *= collarHeight>0;
	G4double collarThickness = GeometryManager::Get()->GetDimensions("collarThickness");
	hasCollar *= collarThickness>0;
	G4int    collarMaterial = GeometryManager::Get()->GetDimensions("collarMaterial"); // 1 Pb 2 PE 
	hasCollar *= collarMaterial>0;

	G4bool hasDonut = true;
	G4double donutHeight = GeometryManager::Get()->GetDimensions("donutHeight");
	hasDonut *= donutHeight>0;
	G4double donutOD = GeometryManager::Get()->GetDimensions("donutOD");
	hasDonut *= donutOD>0;
	G4double donutID = GeometryManager::Get()->GetDimensions("donutID");
	G4double donutZOffset= GeometryManager::Get()->GetDimensions("donutZOffset");
	G4int    donutMaterial = GeometryManager::Get()->GetDimensions("donutMaterial"); //1 Pb 2 PE
	hasDonut *= donutMaterial>0;


	//Cubic shield
	//SS Layer
	//Basic solids
	G4Tubs* cavityTubs = new G4Tubs( name+"Cavity", 0, cavityRadius, 
											boxHeight/2, 
											0, 2*M_PI);
	G4String layerName[3] = {"SS","Pb","PE"};
	G4LogicalVolume* shieldMotherVolume = worldLogic;
	for(int i=0; i<3; i++){
		G4double layerThickness = GeometryManager::Get()->GetDimensions(layerName[i]+"Thickness");
		G4Box* shieldBox = new G4Box( name+layerName[i]+"Box", boxWidth/2, 
												boxWidth/2, 
												boxHeight/2);
		//Subtractions of the cavity for the cryostate
		G4VSolid * shieldBoxSub = new G4SubtractionSolid( name+layerName[i]+"BoxSub", shieldBox, cavityTubs, 
													0, G4ThreeVector(0, 0, cavityOffset));
		//Logic volumes
		G4LogicalVolume* shieldLogic = new G4LogicalVolume( shieldBoxSub, 
														GeometryManager::Get()->GetMaterial(layerName[i]),
														name+layerName[i]+"BoxLV");
		//Placements
		G4VPhysicalVolume* shieldPhysical = new G4PVPlacement( 0, 
														G4ThreeVector(0,0, i==0?shieldZOffset:0), 
														shieldLogic, 
														name+layerName[i]+"BoxPhysical",
														shieldMotherVolume,
														false,
														0,
														fCheckOverlaps);
		if(neckRadius<cavityRadius){
			G4Tubs* neckTubs = new G4Tubs( name+layerName[i]+"NeckTubs", neckRadius, cavityRadius, 
													layerThickness/2, 
													0, 2*M_PI);
			G4LogicalVolume* neckLogic = new G4LogicalVolume( neckTubs,
															GeometryManager::Get()->GetMaterial(layerName[i]),
															name+layerName[i]+"NeckLV");
			G4VPhysicalVolume* neckPhysical = new G4PVPlacement( 0, 
															G4ThreeVector(0,0,boxHeight/2 - layerThickness/2 + shieldZOffset), 
															neckLogic, 
															name+layerName[i]+"NeckPhysical",
															worldLogic,
															false,
															0,
															fCheckOverlaps);
		}
		boxWidth -= layerThickness*2;
		boxHeight -= layerThickness*2;
		shieldMotherVolume = shieldLogic;
	}

	//collar
	if(hasCollar){
		G4Tubs* collarTubs = new G4Tubs( name+"CollarTubs", cavityRadius, cavityRadius+collarThickness,
												collarHeight/2,
												0, 2*M_PI);
		G4LogicalVolume* collarLogic = new G4LogicalVolume( collarTubs,
															GeometryManager::Get()->GetMaterial(collarMaterial==1?"Pb"
																								:collarMaterial==2?"PE"
																								:"G4_Galactic"),
															name+"CollarLV");
		G4VPhysicalVolume* collarPhysical = new G4PVPlacement( 0,
															G4ThreeVector(0,0,
																GeometryManager::Get()->GetDimensions("shieldHeight")/2
																+collarHeight/2+shieldZOffset),
															collarLogic,
															name+"CollarPhysical",
															worldLogic,
															false,
															0,
															fCheckOverlaps);
	}

	//donut above the fridge
	if(hasDonut){
		G4Tubs* donutTubs = new G4Tubs( name+"DonutTubs", donutID/2, donutOD/2,
												donutHeight/2,
												0, 2*M_PI);
		G4LogicalVolume* donutLogic = new G4LogicalVolume( donutTubs,
															GeometryManager::Get()->GetMaterial(donutMaterial==1?"Pb"
																								:donutMaterial==2?"PE"
																								:"G4_Galactic"),
															name+"DonutLV");
		G4VPhysicalVolume* donutPhysical = new G4PVPlacement( 0,
															G4ThreeVector(0,0,donutZOffset),
															donutLogic,
															name+"DonutPhysical",
															worldLogic,
															false,
															0,
															fCheckOverlaps);
	}


/*
	//Pb layer
	//Basic solids
    G4Box* PbBox = new G4Box( name+"PbBox", boxWidth/2, 
											boxWidth/2, 
											boxHeight/2);
	//Subtractions of the cavity for the cryostate
	G4VSolid * PbBoxSub = new G4SubtractionSolid( name+"PbBoxSub", PbBox, cavityTubs, 
			  									0, G4ThreeVector(0, 0, cavityOffset));
	//Logic volumes
	G4LogicalVolume* PbLogic = new G4LogicalVolume( PbBoxSub, 
													GeometryManager::Get()->GetMaterial("Pb"),
													name+"PbBoxLV");
	//Placements
	G4VPhysicalVolume* PbPhysical = new G4PVPlacement( 0, 
													G4ThreeVector(0,0,0), 
													PbLogic, 
													name+"PbBoxPhysical",
													SSLogic, //Inside SS!
													false,
													0,
													fCheckOverlaps);
	//Neck extension
	if(neckRadius<cavityRadius){
		G4Tubs* neckPbTubs = new G4Tubs( name+"NeckPbTubs", neckRadius, cavityRadius, 
												PbThickness/2, 
												0, 2*M_PI);
		G4LogicalVolume* neckPbLogic = new G4LogicalVolume( neckPbTubs,
														GeometryManager::Get()->GetMaterial("Pb"),
														name+"NeckPbLV");
		G4VPhysicalVolume* neckPbPhysical = new G4PVPlacement( 0, 
														G4ThreeVector(0,0,boxHeight/2 - PbThickness/2+ shieldZOffset), 
														neckPbLogic, 
														name+"neckPbPhysical",
														worldLogic,
														false,
														0,
														fCheckOverlaps);
	}
	boxWidth -= PbThickness*2;
	boxHeight -= PbThickness*2;

	//PE layer
	//Basic solids
    G4Box* PEBox = new G4Box( name+"PEBox", boxWidth/2, 
											boxWidth/2, 
											boxHeight/2);
	//Subtractions of the cavity for the cryostate
	G4VSolid * PEBoxSub = new G4SubtractionSolid( name+"PEBoxSub", PEBox, cavityTubs, 
			  									0, G4ThreeVector(0, 0, cavityOffset));
	//Logic volumes
	G4LogicalVolume* PELogic = new G4LogicalVolume( PEBoxSub, 
													GeometryManager::Get()->GetMaterial("PE"),
													name+"PEBoxLV");
	//Placements
	G4VPhysicalVolume* PEPhysical = new G4PVPlacement( 0, 
													G4ThreeVector(0,0,0), 
													PELogic, 
													name+"PEBoxPhysical",
													PbLogic, //Inside Pb!
													false,
													0,
													fCheckOverlaps);
	//Neck extension
	if(neckRadius<cavityRadius){
		G4Tubs* neckPETubs = new G4Tubs( name+"NeckPETubs", neckRadius, cavityRadius, 
												PEThickness/2, 
												0, 2*M_PI);
		G4LogicalVolume* neckPELogic = new G4LogicalVolume( neckPETubs,
														GeometryManager::Get()->GetMaterial("PE"),
														name+"NeckPELV");
		G4VPhysicalVolume* neckPEPhysical = new G4PVPlacement( 0, 
														G4ThreeVector(0,0,boxHeight/2 - PEThickness/2 + shieldZOffset), 
														neckPELogic, 
														name+"neckPEPhysical",
														worldLogic,
														false,
														0,
														fCheckOverlaps);
	}
	boxWidth -= PEThickness*2;
	boxHeight -= PEThickness*2;
*/
}
