#include "Geography.hh"
#include <fstream>

Geography::Geography(G4String pathToDataFile) {
	G4cout << "Try to open topography: " << pathToDataFile
			<< " ... ";
	std::ifstream file;
	file.open(pathToDataFile, std::ifstream::in);
	if (file.good() != true) {
		G4cerr << "failure" << G4endl;
	} else {
		G4cout << "success" << G4endl;
	}
	for (int i = 1; i <= 90; i++) {
		for (int j = 0; j < 360; j++) {
			file >> geoMap[i][j];
		}
	}
	file.close();
}

Geography::~Geography() {
}

G4double Geography::getDepth(size_t theta, size_t phi) {
// TODO need to account for 14 deg slant in phi of davis cavern
	G4double depth = 0.0;
	if ((phi > 360.0) || (theta > 90.0)) {
		depth = 0.0;
	} else if (theta == 0.0) {
		depth = 1508.0; // This is at 4850 at SURF BUT change depth to whatever the site
	} else if ((phi == 360.0) || (phi == 0.0)) {
		depth = fabs(geoMap[theta][phi]); // 360° --> 0° 
	} else {
		if (phi >= 14.0) {
		  if((theta>=nbZenitSteps)||(phi>=nbAzimuthSteps)){
		    G4cerr << "eurGeography: (" << theta << "|" << phi << ") out of array boundaries!" << G4endl;
		  }
		  depth = fabs(geoMap[theta][phi]); // 
		} else {
		  if((theta>=nbZenitSteps)||(phi>=nbAzimuthSteps)){
		    G4cerr << "eurGeography: (" << theta << "|" << phi << ") out of array boundaries!" << G4endl;
		  }
		  depth = fabs(geoMap[theta][phi]); //
		}
	}
	return depth;
}

