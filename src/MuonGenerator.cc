#include "MuonGenerator.hh"
#include "MuonGeneratorMessenger.hh"
#include "Geography.hh"
#include <fstream>
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"

#define A_GAISS 1.84*1.e4
#define SPEC_INDEX 2.77

MuonGenerator::MuonGenerator() : Verbosity(1),
					   PrintEvery(10000),
					   MsgPrefix(""),
					   bins(NULL),
					   nbOfBins(0),
					   generated(false),
					   pathToProfileFile("frejus.dat"),
					   energyMin(200.0*GeV),
					   energyMax(2000.0*GeV),
					   eSteps(100),
					   pSteps(360),
					   tSteps(90),
					   radiusHemisphere(30.0*m),
					   radiusSphere(5.0*m),
					   startPosition(NULL),
					   phi0(0.0),
					   theta0(0.0),
					   startDirection(NULL),
					   startEnergy(0.0*GeV),
					   particleID("mu-")
{
  myMessenger = new MuonGeneratorMessenger(this);

  MsgPrefix = "MuonGenerator> ";
}


MuonGenerator::~MuonGenerator() {
  delete myMessenger;
}

void MuonGenerator::SetPathToProfileFile(G4String path){
  pathToProfileFile = path;
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Set Path to profile file of mountain range " << pathToProfileFile << G4endl;
  }
}

void MuonGenerator::SetVerboseLevel(G4int myVerbosity){
  Verbosity = myVerbosity;
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Set verbosity to " << Verbosity << G4endl;
  }
}

void MuonGenerator::SetEnergyMax(G4double eMax){
  energyMax = eMax;
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Set upper bound of muon energy E_max to " << energyMax/GeV << " GeV." << G4endl;
  }
}

void MuonGenerator::SetEnergyMin(G4double eMin){
  energyMin = eMin;
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Set lower bound of muon energy E_min to " << energyMin/GeV << " GeV." << G4endl;
  }
}

void MuonGenerator::SetNbOfZenithBins(G4int zBins){
  tSteps = zBins;
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Divide zenith direction [0,Pi/2] into " << tSteps << " bins." << G4endl;
  }
  //Calculate how many steps are equivalent to 10% of total look-up table
  PrintEvery = tSteps*pSteps*eSteps/10;
}

void MuonGenerator::SetNbOfAzimuthBins(G4int aBins){
  pSteps = aBins;
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Divide azimuth direction [0,2*Pi] into " << pSteps << " bins." << G4endl;
  }
  //Calculate how many steps are equivalent to 10% of total look-up table
  PrintEvery = tSteps*pSteps*eSteps/10;
}

void MuonGenerator::SetNbOfEnergyBins(G4int eBins){
  eSteps = eBins;
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Divide energy range [Log(E_min), Log(E_max)] into " << eSteps << " bins." << G4endl;
  }
  //Calculate how many steps are equivalent to 10% of total look-up table
  PrintEvery = tSteps*pSteps*eSteps/10;
}

void MuonGenerator::SetRadiusHemisphere(G4double radius){
  radiusHemisphere = radius;
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Set radius of the hemisphere to " << radiusHemisphere/m << " m." << G4endl;
  }
}

void MuonGenerator::SetRadiusSphere(G4double radius){
  radiusSphere = radius;
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Set radius of the sphere to " << radiusSphere/m << " m." << G4endl;
  }
}

void MuonGenerator::GenerateMuon(){
  getRandomMuon();
  getStartPosition();
}

void MuonGenerator::InitSource(){
  if(Verbosity>=1){
    G4cout << MsgPrefix << "Generate look-up table ..." << G4endl;
  }
  generateMuonDistribution();
  if(Verbosity>=1){
    G4cout << MsgPrefix << " done" << G4endl;
  }
}

G4ThreeVector MuonGenerator::GetStartPosition(){
  if(Verbosity>=3){
    G4cout << MsgPrefix << "Start position (x|y|z) [m]: " << startPosition.x()/m << "|" << startPosition.y()/m << "|" << startPosition.z()/m << G4endl;
  }
  return startPosition;
}

G4ThreeVector MuonGenerator::GetStartDirection(){
  if(Verbosity>=3){
    G4cout << MsgPrefix << "Start direction (x|y|z) [mm]: " << startDirection.x()/m << "|" << startDirection.y()/m << "|" << startDirection.z()/m << G4endl;
  }
  return startDirection;
}

G4double MuonGenerator::GetStartEnergy(){
  if(Verbosity>=3){
    G4cout << MsgPrefix << "Start energy [GeV]: " << startEnergy/GeV << G4endl;
  }
  return startEnergy;
}

G4String MuonGenerator::GetParticleID(){
  if(Verbosity>=3){
    G4cout << MsgPrefix << "Particle ID: " << particleID << G4endl;
  }
  return particleID;
}

void MuonGenerator::getStartPosition() {
  //initial position of muon vertex from muon probability distribution
  //G4double phi0 = muonData[2];
  //G4double theta0 = muonData[3];
  //Direction of the muon origin
  G4ThreeVector direction = G4ThreeVector(sin(theta0 * deg) * cos(phi0* deg),
					  sin(theta0 * deg) * sin(phi0 * deg),
					  cos(theta0 * deg));
  //Point of muon origin on the hemisphere
  G4ThreeVector center = radiusHemisphere*direction;
  
  G4double x = radiusHemisphere + 100.;
  G4double y = radiusHemisphere + 100.;
  G4double z = radiusHemisphere + 100.;
  G4double t = 0.0;
  G4double p = 0.0;
  //generate random start position in max. distance radiusSphere from initial position
  while ((std::sqrt((center.x() - x) * (center.x() - x) + (center.y() - y)
		    * (center.y() - y) + (center.z() - z) * (center.z() - z)) > radiusSphere)) {
    
    t = std::acos(G4UniformRand());
    //G4double t = G4UniformRand() * pi / 2.;
    p = G4UniformRand() * 2. * pi;
    x = radiusHemisphere * sin(t) * cos(p);

    z = radiusHemisphere * cos(t);
    y = radiusHemisphere * sin(t) * sin(p);
  }
  //muonData[2] = p / deg;
  //muonData[3] = t / deg;
  startPosition = G4ThreeVector(x, y, z);
  //Zenith,Azimuth points outwards to the point where the muon originates. But Geant4 needs the direction in which the muons propagate: Add minus sign to z-coordinate 
  startDirection = -1.0*direction;
}

G4double MuonGenerator::calculateMuonEnergy(G4double energy,
		G4double depth) {
	G4double energyB = 4.38E-4;
	G4double energyE = 495.*GeV;
	/*G4double energyA = 0.217*GeV;
	G4double newEnergy = energyA / energyB * ((energyB / energyA * energy + 1)
	 * exp(energyB * depth) - 1);*/
	G4double newEnergy = (energy+energyE)*exp(energyB*depth)-energyE;
	//G4cout << energy << " - " << newEnergy << G4endl;
	return newEnergy;
}

G4double MuonGenerator::calculateMuonProbability(G4double energySL,
		G4double phi, G4double theta, G4double dE) {
	G4double result = 0.0;
	/*if ((energySL > 100.0 * GeV / cos(theta * deg)) & (theta < 70.0) & (phi
	  != 360.0)) {*/
	if ((energySL > 100.0 * GeV / cos(theta * deg)) & (phi
							   != 360.0)) {
	  if (theta == 0.0) {
	    result = A_GAISS * (0.14 * pow(energySL / GeV, -SPEC_INDEX)) * (1
									    / (1 + (1.1 * energySL * cos(theta * deg) / (115.0 * GeV)))
									    + 0.054 / (1 + (1.1 * energySL * cos(theta * deg) / (850.0
																 * GeV)))) * dE * sin(theta * deg) / 360.0;
	  } else {
	    result = A_GAISS * (0.14 * pow(energySL / GeV, -SPEC_INDEX)) * (1
									    / (1 + (1.1 * energySL * cos(theta * deg) / (115.0 * GeV)))
									    + 0.054 / (1 + (1.1 * energySL * cos(theta * deg) / (850.0
																 * GeV)))) * dE * sin(theta * deg);
	  }
	}
	/*G4double tmp1 = A_GAISS * (0.14 * pow(energySL / GeV, -SPEC_INDEX));
	G4cout << "t1=" << tmp1 << G4endl;
	G4double tmp2 = 1
	  / (1 + (1.1 * energySL * cos(theta * deg) / (115.0 * GeV)));
	G4cout << "t2=" << tmp2 << G4endl;
	G4double tmp3 = 0.054 / (1 + (1.1 * energySL * cos(theta * deg) / (850.0
									   * GeV)));
	G4cout << "t3=" << tmp3 << G4endl;
	G4double tmp4 = dE * sin(theta * deg);
	G4cout << "t4=" << tmp4 << G4endl;
	G4double tmp5 = tmp1*(tmp2+tmp3)*tmp4;
	G4cout << "t5=" << tmp5 << G4endl;
	result = tmp5;*/
	return result;
}

void MuonGenerator::generateMuonDistribution() {
	//Profile of mountain range
	Geography *myMap = new Geography(pathToProfileFile);

	G4int nbOfBinsMax = eSteps * pSteps * (tSteps + 1);
	bins = new G4double[nbOfBinsMax][4];
	G4int bin = 0;
	G4int i = 0;
	for (G4int e = 0; e < eSteps; ++e) {
		//linear scale
		//G4double dE     = (energyMax-energyMin)/eSteps;
		//G4double energy = dE*e+energyMin;

		//logarithmic scale
		G4double eWidth = (log10(energyMax / GeV) - log10(energyMin / GeV)) / (eSteps);
		G4double energy = pow(10, (log10(energyMin / GeV) + e * eWidth)) * GeV;
		G4double dE = pow(10, (log10(energyMin / GeV) + (e + 1) * eWidth))
				* GeV - energy;

		for (G4int p = 0; p < pSteps; ++p) {
			G4double phi = 360.0 * p / pSteps;
			for (G4int t = 0; t <= tSteps; ++t) {
				G4double theta = 90.0 * t / tSteps;
				//corrected zenith angle due to the erath curvature
				G4double theta_ = asin(6600 * sin(theta * deg) / (6600+18.600)) / deg;
				//Get rock overburden in zenith, azimuth direction
				G4double depth_ = (myMap->getDepth((G4int) theta, (G4int) phi)) - radiusHemisphere/m;//Subtract the radius of the hemisphere on that the muons starts
				G4double depth = depth_ * 2.74; //meter water equivalent, because depth is in m instead of cm, it's not necessary to divide it by 100g/cm2 for normalization to water
				//Energy at sealevel
				G4double energySL = calculateMuonEnergy(energy, depth);
				G4double dESL = dE*exp(4.38E-4*depth);
				G4double solidAngelCorrection = cos(theta*deg)*(6600.0/(6600.0+18.6))/sqrt(1-(6600.0*sin(theta*deg)/(6600.0+18.6))*(6600.0*sin(theta*deg)/(6600.0+18.6)));
				//Probability for the a muon with the start properties (zenith, azimuth, energy)
				G4double probability = calculateMuonProbability(energySL, phi,
										theta_, dESL);
				//G4cout << probability << G4endl;
				//G4cout << solidAngelCorrection << G4endl;
				probability *= solidAngelCorrection;
				//Print every entry of the look-up table
				//Or print only a part of the entries, default is every 10th.
				if((Verbosity>=4)||((Verbosity>=2)&&(i%PrintEvery==0))){
				  G4cout << MsgPrefix << "Bin " << bin << ": Energy [GeV]=" << energy/GeV << " - dE[GeV]=" << dE/GeV << " - energySL [GeV]=" << energySL/GeV << " dESL[GeV]=" << dESL/GeV << " - (theta|phi)=(" << theta << "|" << phi << ") - theta_=" << theta_ << " - depth [m]=" << depth_ << " - mwe=" << depth << " - prob=" << probability << G4endl;
				}
				//Remove all combination with zero probability
				if (probability != 0.0) {
					bins[bin][0] = probability;
					bins[bin][1] = energy;
					bins[bin][2] = phi;
					bins[bin][3] = theta;
					bin++;
				}
				i++;
			}
		}
	}

	G4double sum = bins[0][0];
	for (G4int i = 1; i < bin; ++i) {
		sum += bins[i][0];
		bins[i][0] += bins[i - 1][0];
	}

	G4cout << "Integrate probability for [" << energyMin << ":" << energyMax << "]=" << sum << G4endl;
	for (G4int i = 0; i < bin; ++i) {
		bins[i][0] /= sum;
	}

	nbOfBins = bin;
	generated = true;
	delete myMap;
}

//according to: http://www.bl.physik.uni-muenchen.de/~schubert/doc/geant4/html/G4PhysicsOrderedFreeVector_8cc-source.html
void MuonGenerator::getParameterToProbability(G4double prob) {
	if (!generated) {
		G4cerr
				<< "MuonGenerator::getParameterToProbability : no muon distribution generated"
				<< G4endl;
	}
	G4int n1 = 0;
	G4int n2 = nbOfBins / 2;
	G4int n3 = nbOfBins - 1;
	while (n1 != n3 - 1) {
		if (prob > bins[n2][0]) {
			n1 = n2;
		} else {
			n3 = n2;
		}
		n2 = n1 + (n3 - n1 + 1) / 2;
	}

	/*    G4cout <<  bins[n1][0]
	 << " " << bins[n1][1]
	 << " " << bins[n1][2]
	 << " " << bins[n1][3] << G4endl;*/
	//G4double probability = bins[n1][0];
	startEnergy = bins[n1][1];
	phi0 = bins[n1][2];
	theta0 = bins[n1][3];
}

void MuonGenerator::getRandomMuon() {
	if (!generated) {
		G4cerr
				<< "MuonGenerator::getRandomMuon : no muon distribution generated"
				<< G4endl;
	}
	G4double rnd = G4UniformRand();
	getParameterToProbability(rnd);
}

