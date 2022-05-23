
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


    fCheckOverlaps = GeometryManager::Get()->GetCheckOverlaps();
}


void GeoCryostat::Construct(){

	G4String motherName="world";
    motherLogic = GeometryManager::Get()->GetLogicalVolume(motherName);
    if( motherLogic==0 ){
        G4cerr << "Cannot find the mother volume "<<motherName<< G4endl;
        return;
    }

	ConstructTanks();
	ConstructColdParts();
}

void GeoCryostat::ConstructTanks(){
	//300K, 50K, 4K, still
	G4String names[4] = {"Cryo300K", "Cryo50K", "Cryo4K", "CryoStill"};
	G4String material[4] = {"Ti", "Cu", "Cu", "Cu"};
	for(int i=0; i<4; i++){
		G4int nP = GeometryManager::Get()->GetCryostatCoordinateNP(i);
		G4double* rI =  GeometryManager::Get()->GetCryostatCoordinateRI(i);
		G4double* rO =  GeometryManager::Get()->GetCryostatCoordinateRO(i);
		G4double* z =  GeometryManager::Get()->GetCryostatCoordinateZ(i);
		G4cout<<"Construct cryostat wall: "<<nP<<" coordinates"<<G4endl;
		for(int ip=0; ip<nP;ip++){
			G4cout<<rI[ip]<<" "<<rO[ip]<<" "<<z[ip]<<G4endl;
		}
		G4Polycone* cryostatSolid = new G4Polycone(names[i]+"Solid", 0, 2*M_PI, nP, z, rI, rO);
		G4LogicalVolume* cryostatLogic = new G4LogicalVolume( cryostatSolid,
																GeometryManager::Get()->GetMaterial(material[i]),
																names[i]+"LV");
		G4VPhysicalVolume* cryostatPhysical = new G4PVPlacement(0,
																G4ThreeVector(0,0,0),
																cryostatLogic,
																names[i],
																motherLogic,
																false,
																0,
																fCheckOverlaps);
	}
}

void GeoCryostat::ConstructColdParts(){
	G4String name = "CP";
	//Drills
	G4int nDrillSizes = GeometryManager::Get()->GetDrillChart()->size();
	G4Tubs** drills = new G4Tubs*[nDrillSizes];
	for(int ih=0; ih<nDrillSizes; ih++){
		drills[ih] = new G4Tubs( name+"Drill",
								0,
								(*GeometryManager::Get()->GetDrillChart())[ih],
								GeometryManager::Get()->GetDimensions("maxPlateThickness")/2,//make sure to drill through all plates.
								0, 2*M_PI);
	}

	//Plate with holes
	for(int ip=0; ip<GeometryManager::Get()->GetNumberOfCryoPlates(); ip++){
		G4String plateName = GeometryManager::Get()->GetCryoPlateName(ip);
		G4cout<<plateName<<G4endl;
		G4Tubs* plate = new G4Tubs( name+plateName+"Solid",
									0,
									GeometryManager::Get()->GetCryoPlateR(ip),
									GeometryManager::Get()->GetCryoPlateH(ip)/2,
									0, 2*M_PI);
		G4VSolid* plateWithHoles;
		auto holes = GeometryManager::Get()->GetCryoPlateHoles(ip);
		G4cout<<holes->size()<<" holes"<<G4endl;
		if(int(holes->size())>0){
			//drill holes
			//Union of all holes
			G4MultiUnion* holeUnion = new G4MultiUnion( name+plateName+"Holes");
			for(auto ih = holes->begin(); ih != holes->end(); ++ih){
				G4cout<<"drill with OD "<<drills[ih->first]->GetOuterRadius()<<" at "<<ih->second.x()<<","<<ih->second.y()<<G4endl;
				G4Transform3D trans(G4RotationMatrix(), ih->second);
				holeUnion->AddNode((*drills[ih->first]), trans);
			}
			//Must voxelize unions!
			holeUnion->Voxelize();
			//drill
			plateWithHoles = new G4SubtractionSolid( name+plateName+"Sub",
												plate,
												holeUnion,
												0,
												G4ThreeVector(0,0,0)
												);
		}else{
			plateWithHoles = plate;
		}
		G4LogicalVolume * plateLogic = new G4LogicalVolume( plateWithHoles,
													GeometryManager::Get()->GetMaterial(GeometryManager::Get()->GetCryoPlateMaterial(ip)),
													name+plateName+"LV");
		G4VPhysicalVolume* platePhysical = new G4PVPlacement(0,
													G4ThreeVector(0,0,GeometryManager::Get()->GetCryoPlateZ(ip)),
													plateLogic,
													name+plateName,
													motherLogic,
													false,
													0,
													fCheckOverlaps);
	}
	G4cout<<"plates done"<<G4endl;

	//Support beams
	for(int ib=0; ib<GeometryManager::Get()->GetNumberOfCryoBeams(); ib++){
		G4String beamName = GeometryManager::Get()->GetCryoBeamName(ib);
		G4Tubs* beam = new G4Tubs( name+beamName+"Solid",
									GeometryManager::Get()->GetCryoBeamRI(ib),
									GeometryManager::Get()->GetCryoBeamRO(ib),
									GeometryManager::Get()->GetCryoBeamL(ib)/2,
									0, 2*M_PI);
		G4LogicalVolume * beamLogic = new G4LogicalVolume( beam,
									GeometryManager::Get()->GetMaterial(GeometryManager::Get()->GetCryoBeamMaterial(ib)),
									name+beamName+"LV");
		G4VPhysicalVolume* beamPhysical = new G4PVPlacement(0,
									GeometryManager::Get()->GetCryoBeamPos(ib),
									beamLogic,
									name+beamName,
									motherLogic,
									false,
									0,
									fCheckOverlaps);
	}

	//Mixing chammber (inner most shield). Top and bottom are placed as "plates with holes"!
	G4Tubs* MXCWallTubs = new G4Tubs( name+"MXCWallSolid",
									GeometryManager::Get()->GetDimensions("MXCWallInnerRadius"),
									GeometryManager::Get()->GetDimensions("MXCWallOuterRadius"),
									GeometryManager::Get()->GetDimensions("MXCWallHeight")/2,
									0, 2*M_PI);
	G4LogicalVolume * MXCWallLogic = new G4LogicalVolume( MXCWallTubs,
									GeometryManager::Get()->GetMaterial("PureCu"),
									name+"MXCWallLV");
	G4VPhysicalVolume* MXCWallPhysical = new G4PVPlacement(0,
									G4ThreeVector(0, 0, 0),//GeometryManager::Get()->GetDimensions("detectorZOffset")),
									MXCWallLogic,
									name + "MXCWall",
									motherLogic,
									false,
									0,
									fCheckOverlaps);
}
