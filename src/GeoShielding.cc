
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


  G4bool hasOuterShield = true;
  G4double OuterShieldThickness = GeometryManager::Get()->GetDimensions("OuterShieldThickness");
  hasOuterShield *= OuterShieldThickness>0;
  G4int    OuterShieldMaterial = GeometryManager::Get()->GetDimensions("OuterShieldMaterial"); //1 boratedPoly_15 2 PE
  hasOuterShield *= OuterShieldMaterial>0;


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
		G4Box* shieldBox = new G4Box( name+layerName[i]+"BoxSolid", boxWidth/2,
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
														name+layerName[i]+"Box",
														shieldMotherVolume,
														false,
														0,
														fCheckOverlaps);
		if(neckRadius<cavityRadius){
			G4Tubs* neckTubs = new G4Tubs( name+layerName[i]+"NeckSolid", neckRadius, cavityRadius,
													layerThickness/2,
													0, 2*M_PI);
			G4LogicalVolume* neckLogic = new G4LogicalVolume( neckTubs,
															GeometryManager::Get()->GetMaterial(layerName[i]),
															name+layerName[i]+"NeckLV");
			G4VPhysicalVolume* neckPhysical = new G4PVPlacement( 0,
															G4ThreeVector(0,0,boxHeight/2 - layerThickness/2 + shieldZOffset),
															neckLogic,
															name+layerName[i]+"Neck",
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
															name+"Collar",
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
															name+"Donut",
															worldLogic,
															false,
															0,
															fCheckOverlaps);
	}

  if (hasOuterShield) {

    boxWidth     = GeometryManager::Get()->GetDimensions("shieldWidth");
    boxHeight    = GeometryManager::Get()->GetDimensions("shieldHeight");
    G4Box* boratedWaterSheild = new G4Box("BoratedWaterSheild", boxWidth/2.0+OuterShieldThickness, boxWidth/2.0+OuterShieldThickness, boxHeight/2.0);
    G4Box* hole = new G4Box("Hole",boxWidth /2.0, boxWidth /2.0, boxHeight);
    G4SubtractionSolid* bucketShielding= new G4SubtractionSolid("BucketShielding", boratedWaterSheild, hole, 0, G4ThreeVector{0,0,0});

    G4LogicalVolume* bucketShieldingLogic = new G4LogicalVolume( bucketShielding,
                              GeometryManager::Get()->GetMaterial(OuterShieldMaterial==1?"boratedPoly_15"
                                                :OuterShieldMaterial==2?"PE"
                                                :"G4_Galactic"),
                             name+"bucketShieldingLV");

    G4VPhysicalVolume* bucketShieldingPhysical = new G4PVPlacement( 0,
                              G4ThreeVector(0,0,shieldZOffset),
                              bucketShieldingLogic,
                              "boratedWaterSheild",
                              worldLogic,
                              false,
                              0,
                              fCheckOverlaps);




    G4Box* TopBorateWater = new G4Box("TopBorateWater", boxWidth /2.0+OuterShieldThickness,
                                         boxWidth /2.0+OuterShieldThickness,
                                         OuterShieldThickness/2.0);

    G4Tubs* donutTubsHole = new G4Tubs("DonutTubsHole", 0, (cavityRadius+collarThickness),OuterShieldThickness, 0, 2*M_PI);

    G4SubtractionSolid* TopBorateWaterSheild= new G4SubtractionSolid("TopBorateWaterSheild", TopBorateWater, donutTubsHole, 0, G4ThreeVector{0,0,0} );

    G4LogicalVolume* TopBorateWaterSheildLogic = new G4LogicalVolume( TopBorateWaterSheild,
                      GeometryManager::Get()->GetMaterial(OuterShieldMaterial==1?"boratedPoly_15":OuterShieldMaterial==2?"PE":"G4_Galactic")
                                                               ,name+"bucketShieldingLV");

    G4VPhysicalVolume* TopBorateWaterSheildPhysical = new G4PVPlacement( 0,
      G4ThreeVector(0,0,GeometryManager::Get()->GetDimensions("shieldHeight")/2+shieldZOffset+OuterShieldThickness/2.0),
                                                    TopBorateWaterSheildLogic,"TopBorateWaterSheild",worldLogic,false,0,fCheckOverlaps);


    G4Box* BottomBorateWaterSheild= new G4Box("BottomBorateWaterSheild", boxWidth /2.0+OuterShieldThickness,
                                                 boxWidth /2.0+OuterShieldThickness,
                                                 OuterShieldThickness/2.0);

    G4LogicalVolume* BottomBorateWaterSheildLogic = new G4LogicalVolume( BottomBorateWaterSheild,
                        GeometryManager::Get()->GetMaterial(OuterShieldMaterial==1?"boratedPoly_15":OuterShieldMaterial==2?"PE":"G4_Galactic")
                                                               ,name+"BottomBorateWaterSheildLV");

    G4VPhysicalVolume* BottomBorateWaterSheildPhysical = new G4PVPlacement( 0, G4ThreeVector(0,0,-GeometryManager::Get()->GetDimensions("shieldHeight")/2
                                                                              +shieldZOffset-OuterShieldThickness/2.0),
                      BottomBorateWaterSheildLogic, "BottomBorateWaterSheild",worldLogic,false,0,fCheckOverlaps);

  }


}
