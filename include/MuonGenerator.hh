#ifndef MuonGenerator_h
#define MuonGenerator_h 2

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include "VMuonSource.hh"

class MuonGeneratorMessenger;

class MuonGenerator : public VMuonSource{

public:
  MuonGenerator();
  ~MuonGenerator();
  void GenerateMuon();
  void InitSource();
  G4ThreeVector GetStartPosition();
  G4ThreeVector GetStartDirection();
  G4double GetStartEnergy();
  G4String GetParticleID();
  void SetPathToProfileFile(G4String path);
  void SetVerboseLevel(G4int myVerbosity);
  void SetEnergyMax(G4double eMax);
  void SetEnergyMin(G4double eMin);
  void SetNbOfZenithBins(G4int zBins);
  void SetNbOfAzimuthBins(G4int aBins);
  void SetNbOfEnergyBins(G4int eBins);
  void SetRadiusHemisphere(G4double radius);
  void SetRadiusSphere(G4double radius);

private:
  G4double calculateMuonProbability(G4double energy, G4double phi,
				    G4double theta, G4double dE);
  void generateMuonDistribution();
  void getParameterToProbability(G4double prob);
  void getRandomMuon();
  void getStartPosition();
  G4double calculateMuonEnergy(G4double energy, G4double theta);
  MuonGeneratorMessenger *myMessenger;
  //Verbosity 0: Prints only a summary of the properties
  //Verbosity 1: Prints also every setting of the paramters
  //Verbosity 2: Prints progress information during the creation of the look-up table, every 10%
  //Verbosity 4: Prints each single entry of the look-up table (very slow!)
  G4int Verbosity;
  G4int PrintEvery;
  G4String MsgPrefix;
  G4double (*bins)[4];
  G4int nbOfBins;
  G4bool generated;
  G4String pathToProfileFile;//The path to the file containing the height profile of the Frejus mountain range
  G4double energyMin;//Lower bound of the muon energy range
  G4double energyMax;//Upper bound of the muon energy range
  G4int eSteps;//Number of bins of the logarithmic energy scale: (Log(energyMax)-Log(energyMin))/eSteps
  G4int pSteps;//Number of bins along the azimuth direction
  G4int tSteps;//Number of bins along the zenith direction
  G4double radiusHemisphere;//Radius of the hemisphere where the muons start. Markus Horn used in his thesis a value of 30.0*m
  G4double radiusSphere;//The deviation of the muon start position on the surface of the outer hemisphere. A deviation of less or equal d metres lead to a sphere of radius d in the centre of the outer hemisphere which is homogeneous illuminated by muons. In Markus' thesis a value of 5*m was used.
  G4ThreeVector startPosition;
  G4double phi0;//Start direction of the muon, specified as azimuth (phi0) and zenith (theta0) angle
  G4double theta0;
  G4ThreeVector startDirection;
  G4double startEnergy;
  G4String particleID;//This generator only created mu-
};

#endif
