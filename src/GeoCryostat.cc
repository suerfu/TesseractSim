
#include "GeoCryostat.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polycone.hh"
#include "G4VSolid.hh"
#include "G4MultiUnion.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

GeoCryostat::GeoCryostat( ){


    fCheckOverlaps = GeoManager::Get()->GetCheckOverlaps();
}


void GeoCryostat::Construct(){
    
	G4String motherName="world";
    motherLogic = GeoManager::Get()->GetLogicalVolume(motherName);
    if( motherLogic==0 ){
        G4cerr << "Cannot find the mother volume "<<motherName<< G4endl;
        return;
    }

	ConstructTanks();
	ConstructColdParts();
}

void GeoCryostat::ConstructTanks(){
	//300K, 50K, 4K, still
	G4String names[4] = {"Cryostat300K", "Cryostat50K", "Cryostat4K", "CryostatStill"};
	for(int i=0; i<4; i++){
		G4int nP = GeoManager::Get()->GetCryostatCoordinateNP(i);
		G4double* rI =  GeoManager::Get()->GetCryostatCoordinateRI(i);
		G4double* rO =  GeoManager::Get()->GetCryostatCoordinateRO(i);
		G4double* z =  GeoManager::Get()->GetCryostatCoordinateZ(i);
		G4cout<<"Construct cryostat wall: "<<nP<<" coordinates"<<G4endl;
		for(int ip=0; ip<nP;ip++){
			G4cout<<rI[ip]<<" "<<rO[ip]<<" "<<z[ip]<<G4endl;
		}
		G4Polycone* cryostatSolid = new G4Polycone(names[i], 0, 2*M_PI, nP, z, rI, rO);
		G4LogicalVolume* cryostatLogic = new G4LogicalVolume( cryostatSolid,
																GeoManager::Get()->GetMaterial("Ti"),
																names[i]+"LV");
		G4VPhysicalVolume* cryostatPhysical = new G4PVPlacement(0,
																G4ThreeVector(0,0,0),
																cryostatLogic,
																names[i]+"Physical",
																motherLogic,
																false,
																0,
																fCheckOverlaps);
		GeoManager::Get()->Add( names[i], cryostatLogic, cryostatPhysical);
	}
}

void GeoCryostat::ConstructColdParts(){
	G4String name = "Cryostat";
	//Drills
	G4int nDrillSizes = GeoManager::Get()->GetDrillChart()->size();
	G4Tubs** drills = new G4Tubs*[nDrillSizes];
	for(int ih=0; ih<nDrillSizes; ih++){
		drills[ih] = new G4Tubs( name+"Drill",
								0, 
								(*GeoManager::Get()->GetDrillChart())[ih],
								GeoManager::Get()->GetDimensions("maxPlateThickness")/2,//make sure to drill through all plates.
								0, 2*M_PI);
	}
	
	//Plate with holes
	for(int ip=0; ip<GeoManager::Get()->GetNumberOfCryoPlates(); ip++){
		G4String plateName = GeoManager::Get()->GetCryoPlateName(ip);
		G4cout<<plateName<<G4endl;
		G4Tubs* plate = new G4Tubs( name+plateName, 
									0, 
									GeoManager::Get()->GetCryoPlateR(ip),
									GeoManager::Get()->GetCryoPlateH(ip)/2,
									0, 2*M_PI);
		G4VSolid* plateWithHoles;
		auto holes = GeoManager::Get()->GetCryoPlateHoles(ip);
		G4cout<<holes->size()<<" holes"<<G4endl;
		if(int(holes->size())>0){
			//drill holes
			G4MultiUnion* holeUnion = new G4MultiUnion( name+plateName+"holes");
			for(auto ih = holes->begin(); ih != holes->end(); ++ih){
				G4cout<<"drill with OD "<<drills[ih->first]->GetOuterRadius()<<" at "<<ih->second.x()<<","<<ih->second.y()<<G4endl;
				G4Transform3D trans(G4RotationMatrix(), ih->second);
				holeUnion->AddNode((*drills[ih->first]), trans);
			}
			plateWithHoles = new G4SubtractionSolid( name+plateName,
												plate,
												holeUnion,
												0,
												G4ThreeVector(0,0,0)
												);
		}else{
			plateWithHoles = plate;
		}
		G4LogicalVolume * plateLogic = new G4LogicalVolume( plateWithHoles,
													GeoManager::Get()->GetMaterial(GeoManager::Get()->GetCryoPlateMaterial(ip)),
													name+plateName+"LV");
		G4VPhysicalVolume* platePhysical = new G4PVPlacement(0,
													G4ThreeVector(0,0,GeoManager::Get()->GetCryoPlateZ(ip)),
													plateLogic,
													name+plateName+"Physical",
													motherLogic,
													false,
													0,
													fCheckOverlaps);
		GeoManager::Get()->Add( name+plateName,
								plateLogic, platePhysical);
	}
	G4cout<<"plates done"<<G4endl;
	
	//Support beams
	for(int ib=0; ib<GeoManager::Get()->GetNumberOfCryoBeams(); ib++){
		G4String beamName = GeoManager::Get()->GetCryoBeamName(ib);
		G4Tubs* beam = new G4Tubs( name+beamName,
									GeoManager::Get()->GetCryoBeamRI(ib),
									GeoManager::Get()->GetCryoBeamRO(ib),
									GeoManager::Get()->GetCryoBeamL(ib)/2,
									0, 2*M_PI);
		G4LogicalVolume * beamLogic = new G4LogicalVolume( beam,
									GeoManager::Get()->GetMaterial(GeoManager::Get()->GetCryoBeamMaterial(ib)),
									name+beamName+"LV");
		G4VPhysicalVolume* beamPhysical = new G4PVPlacement(0,
									GeoManager::Get()->GetCryoBeamPos(ib),
									beamLogic,
									name+beamName+"Physical",
									motherLogic,
									false,
									0,
									fCheckOverlaps);
		GeoManager::Get()->Add( name+beamName,
								beamLogic, beamPhysical);
	}

	//Mixing chammber (inner most shield). Top and bottom are placed as "plates with holes"!
	G4Tubs* MXCWallTubs = new G4Tubs( name+"MXCWallTubs", 
									GeoManager::Get()->GetDimensions("MXCWallInnerRadius"),
									GeoManager::Get()->GetDimensions("MXCWallOuterRadius"),
									GeoManager::Get()->GetDimensions("MXCWallHeight")/2,
									0, 2*M_PI);
	G4LogicalVolume * MXCWallLogic = new G4LogicalVolume( MXCWallTubs, 
									GeoManager::Get()->GetMaterial("Cu"),
									name+"MXCWallLV");
	G4VPhysicalVolume* MXCWallPhysical = new G4PVPlacement(0,
									G4ThreeVector(0, 0, GeoManager::Get()->GetDimensions("detectorZOffset")),
									MXCWallLogic,
									name + "MXCWallPhysical",
									motherLogic,
									false,
									0,
									fCheckOverlaps);
	GeoManager::Get()->Add( name+"MXCWall",
								MXCWallLogic, MXCWallPhysical);
	
}

