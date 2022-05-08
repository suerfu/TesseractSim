#include "G4PhysicalVolumeStore.hh"
#include "G4VisExtent.hh"

#include "GpsInMaterial.hh"
#include "G4RunManager.hh"

//namespace Generator
//{


  G4VPhysicalVolume* GpsInMaterial::pickVolume() {

    G4double r=CLHEP::RandFlat::shoot(0.,fCumulativeMaterialVolume);
    G4int selectedVolume=0;
    G4double sum=0;
    do {
      sum+=fVolumesInMaterial[selectedVolume]->GetLogicalVolume()->GetSolid()->GetCubicVolume()/CLHEP::cm3;
      ++selectedVolume;
    } while(sum<r);

    return fVolumesInMaterial[selectedVolume-1];

  }



    void GpsInMaterial::GeneratePrimaries(G4Event* event)
    {


      if(fVolumesInMaterial.empty())
	throw std::runtime_error("Generator::GpsInMaterial::GeneratePrimaries : no material set");

      G4VPhysicalVolume* selectedVolume = pickVolume();

      G4VisExtent extent=selectedVolume->GetLogicalVolume()->GetSolid()->GetExtent();
      G4ThreeVector translationToVolumeCenter((extent.GetXmax()+extent.GetXmin())/2.,(extent.GetYmax()+extent.GetYmin())/2.,(extent.GetZmax()+extent.GetZmin())/2.);

      G4SPSPosDistribution* pd= fGeneralParticleSource.GetCurrentSource()->GetPosDist();
      pd->ConfineSourceToVolume(selectedVolume->GetName());
      pd->SetPosDisType("Volume");
      pd->SetPosDisShape("Para");
      pd->SetCentreCoords(selectedVolume->GetTranslation()+translationToVolumeCenter);
      pd->SetHalfX((extent.GetXmax()-extent.GetXmin())/2.);
      pd->SetHalfY((extent.GetYmax()-extent.GetYmin())/2.);
      pd->SetHalfZ((extent.GetZmax()-extent.GetZmin())/2.);


      fGeneralParticleSource.GeneratePrimaryVertex(event);

    }


  void  GpsInMaterial::SetMaterial(const G4String& materialName)
  {

    fVolumesInMaterial.clear();
    fCumulativeMaterialVolume = 0;
    G4PhysicalVolumeStore *PVStore = G4PhysicalVolumeStore::GetInstance();
    G4int i = 0;
    while (i<(G4int)PVStore->size()) {

      G4VPhysicalVolume* pv = (*PVStore)[i];

      if (pv->GetLogicalVolume()->GetMaterial()->GetName() == materialName) {
	fVolumesInMaterial.push_back(pv);
	fCumulativeMaterialVolume+=pv->GetLogicalVolume()->GetSolid()->GetCubicVolume()/CLHEP::cm3;
      }
      i++;
    }

    if(fVolumesInMaterial.empty())
      throw std::runtime_error("Generator::GpsInMaterial::SetMaterial did not find volume made of '"+materialName+"'");


  }


//}
