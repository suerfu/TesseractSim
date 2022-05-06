/**************************************************************************
* @file GpsInMaterial.hh
* @author Antoine Cazes <a.cazes@ipnl.in2p3.fr>
* @date 09 july  2020
* @brief Generator class for generating GPS event from a dedicated material
* ************************************************************************/

#ifndef GPSINMATERIALGENERATOR_HH
#define GPSINMATERIALGENERATOR_HH

#include "G4GeneralParticleSource.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "GeneratorAction.hh"

//namespace Generator{

class GpsInMaterial : public GeneratorAction {

  G4VPhysicalVolume* pickVolume();
  G4GeneralParticleSource fGeneralParticleSource;
  double fCumulativeMaterialVolume;
  std::vector<G4VPhysicalVolume*> fVolumesInMaterial;

  public:
  GpsInMaterial() = default;
  void SetMaterial(const G4String& materialName);
  virtual void GeneratePrimaries(G4Event* event) override;

};

//}

#endif /* GPSINMATERIALGENERATOR_HH */
