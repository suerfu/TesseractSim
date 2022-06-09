/*
 * VMuonSource.hh
 *
 *  Created on: 15.12.2010
 *      Author: kluck
 */

#ifndef VMUONSOURCE_HH_
#define VMUONSOURCE_HH_

#include "G4ThreeVector.hh"

class VMuonSource{

public:
  virtual ~VMuonSource() {};
  virtual void GenerateMuon() = 0;
  virtual void InitSource() = 0;
  virtual G4ThreeVector GetStartPosition() = 0;
  virtual G4ThreeVector GetStartDirection() = 0;
  virtual G4double GetStartEnergy() = 0;
  virtual G4String GetParticleID() = 0;
};

#endif /* VMUONSOURCE_HH_ */