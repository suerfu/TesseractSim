
#include "GeoWalls.hh"
#include "GeometryManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

GeoWalls::GeoWalls( ){



    fCheckOverlaps = GeometryManager::Get()->GetCheckOverlaps();

 //   fGeoWallsMessenger = new GeoWallsMessenger( this );
}


void GeoWalls::Construct(){

    G4LogicalVolume* worldLogic = GeometryManager::Get()->GetLogicalVolume("world");
    if( worldLogic==0 ){
        G4cerr << "Cannot find the logical volume of world." << G4endl;
        return;
    }

    G4String name = "MJD_HALL";


	G4double shieldZOffset= GeometryManager::Get()->GetDimensions("shieldZOffset");

	G4bool hasWall = true;
	G4double Wall1_Thickness = GeometryManager::Get()->GetDimensions("Wall1_Thickness");
	hasWall *= Wall1_Thickness>0;
	G4double Wall2_Thickness = GeometryManager::Get()->GetDimensions("Wall2_Thickness");
	hasWall *= Wall2_Thickness>0;
  G4double Ceiling_Thickness = GeometryManager::Get()->GetDimensions("Ceiling_Thickness");
	hasWall *= Ceiling_Thickness>0;
  G4double Floor_Thickness = GeometryManager::Get()->GetDimensions("Floor_Thickness");
	hasWall *= Floor_Thickness>0;
  G4double Finish_Wall_thickness = GeometryManager::Get()->GetDimensions("Finish_Wall_thickness");
	hasWall *= Finish_Wall_thickness>0;
  G4double Wall1_Xoffset = GeometryManager::Get()->GetDimensions("Wall1_Xoffset");
	hasWall *= Wall1_Xoffset>0;
  G4double Wall3_Xoffset = GeometryManager::Get()->GetDimensions("Wall3_Xoffset");
	hasWall *= Wall3_Xoffset>0;
  G4double Wall4_Yoffset = GeometryManager::Get()->GetDimensions("Wall4_Yoffset");
	hasWall *= Wall4_Yoffset>0;
  G4double Wall2_Yoffset = GeometryManager::Get()->GetDimensions("Wall2_Yoffset");
	hasWall *= Wall2_Yoffset>0;
  G4double Ceiling_Zoffset = GeometryManager::Get()->GetDimensions("Ceiling_Zoffset");
	hasWall *= Ceiling_Zoffset>0;
  G4double Floor_Zoffset = GeometryManager::Get()->GetDimensions("Floor_Zoffset");
	hasWall *= Floor_Zoffset>0;


	//Wall
	if(hasWall){

    //Wall that are made up of Rock_SURF
    G4Box* Main = new G4Box("Main", (Wall1_Thickness+Wall1_Xoffset+Wall3_Xoffset+Finish_Wall_thickness)/2.0,
                              (Wall2_Thickness+Wall2_Yoffset+Wall4_Yoffset+Finish_Wall_thickness)/2.0,
                              (Ceiling_Thickness+ Ceiling_Zoffset + Floor_Thickness + Floor_Zoffset)/2.0  );

    G4Box* hole = new G4Box("hole",(Wall1_Xoffset+Wall3_Xoffset+Finish_Wall_thickness)/2.0,
                                   (Wall2_Yoffset+Wall4_Yoffset+Finish_Wall_thickness)/2.0,
                                    (Ceiling_Zoffset + Floor_Zoffset)/2.0);

    //G4SubtractionSolid* WallBox= new G4SubtractionSolid("WallBox", Main, hole, 0, G4ThreeVector{Wall1_Thickness,-Wall2_Thickness,0});
    G4SubtractionSolid* WallBox= new G4SubtractionSolid("WallBox", Main, hole, 0,
                                            G4ThreeVector{ (Wall1_Thickness)/2.0,
                                                          -(Wall2_Thickness)/2.0,
                                                          0});


		G4LogicalVolume* WallLogic = new G4LogicalVolume( WallBox,
															GeometryManager::Get()->GetMaterial( "Rock_SURF" ),
															name+"WallLV");

		G4VPhysicalVolume* WallPhysical = new G4PVPlacement( 0,
															G4ThreeVector(-(Wall1_Thickness+Wall1_Xoffset) +(Wall1_Thickness+Wall1_Xoffset+Wall3_Xoffset+Finish_Wall_thickness)/2.0,
                                             +(Wall2_Thickness+Wall2_Yoffset)-(Wall2_Thickness+Wall2_Yoffset+Wall4_Yoffset+Finish_Wall_thickness)/2.0,
																            Ceiling_Thickness+ Ceiling_Zoffset - (Ceiling_Thickness+ Ceiling_Zoffset + Floor_Thickness + Floor_Zoffset)/2.0 ),
															WallLogic,
															name+"Wall",
															worldLogic,
															false,
															0,
															fCheckOverlaps);

//  WallLogic->SetVisAttributes(G4VisAttributes::Invisible);


  //Wall that are made up of Concrete
  G4Box* Main2 = new G4Box("Main2", (Wall1_Xoffset+Wall3_Xoffset+Finish_Wall_thickness)/2.0,
                            (Wall2_Yoffset+Wall4_Yoffset+Finish_Wall_thickness)/2.0,
                            ( Ceiling_Zoffset + Floor_Zoffset)/2.0  );

  G4Box* hole2 = new G4Box("hole2",(Wall1_Xoffset+Wall3_Xoffset)/2.0,
                                 (Wall2_Yoffset+Wall4_Yoffset)/2.0,
                                  ( Ceiling_Zoffset +  Floor_Zoffset)/2.0  );

  G4SubtractionSolid* WallBox_concrete= new G4SubtractionSolid("WallBox_concrete", Main2, hole2, 0,
                                          G4ThreeVector{-Finish_Wall_thickness/2.0,Finish_Wall_thickness/2.0,0});

  G4LogicalVolume* WallLogic_concrete = new G4LogicalVolume( WallBox_concrete,
                            //GeometryManager::Get()->GetMaterial( "concrete" ),
                            GeometryManager::Get()->GetMaterial( "Rock_SURF" ),
                            name+"WallLV_concrete");

  G4VPhysicalVolume* WallPhysical_concrete = new G4PVPlacement( 0,
                            G4ThreeVector(-Wall1_Xoffset +(Wall1_Xoffset+Wall3_Xoffset+Finish_Wall_thickness)/2.0 ,
                                          Wall2_Yoffset-(Wall2_Yoffset+Wall4_Yoffset+Finish_Wall_thickness)/2.0 ,
                                          Ceiling_Zoffset - ( Ceiling_Zoffset  + Floor_Zoffset)/2.0 ),
                            WallLogic_concrete,
                            name+"Wall_concrete",
                            worldLogic,
                            false,
                            0,
                            fCheckOverlaps);

  //WallLogic_concrete->SetVisAttributes(G4VisAttributes::Invisible);

}
}
