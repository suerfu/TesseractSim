/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
#include "GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "TTree.h"

GeometryManager* GeometryManager::me = 0;

GeometryManager* GeometryManager::Get(){
	if (!me){
	  me = new GeometryManager();
	}
	return me;
}


GeometryManager::GeometryManager(){
    material_man = GetMaterialManager();
	//FIXME! add user commands 
	fCheckOverlaps = true;
    DefineMaterials();

	fDimensionFilesSet = false;
	fDimensionsLoaded  = false;
}


GeometryManager::~GeometryManager(){
	dimensions.clear();
}


void GeometryManager::SetFilePath( G4String file, G4String pathAndName){
	dimensionFiles[file] = pathAndName;
}


G4LogicalVolume* GeometryManager::GetLogicalVolume( G4String name ){
    return G4PhysicalVolumeStore::GetInstance()->GetVolume( name )->GetLogicalVolume();
}


G4VPhysicalVolume* GeometryManager::GetPhysicalVolume( G4String name ){
    return G4PhysicalVolumeStore::GetInstance()->GetVolume( name );
}


G4NistManager* GeometryManager::GetMaterialManager(){
    return G4NistManager::Instance();
}


G4Material* GeometryManager::GetMaterial(G4String name){
	//G4cout<<"FindOrBuildMaterial "<<name<<G4endl;
    return material_man->FindOrBuildMaterial( name );
}


G4double  GeometryManager::GetDimensions(G4String name){
	if( dimensions.find(name)==dimensions.end() ){
		return 0;		
	}else{
		return dimensions[name];
	}
}


//Cryostat walls
G4int  GeometryManager::GetCryostatCoordinateNP(int ithLayer){
	if(ithLayer>=4){
		G4cerr<<"Cryostat layer "<<ithLayer<<" not specified in data file!"<<G4endl;
		return 0;
	}else{
		return fCryostatCoordinateNP[ithLayer];
	}
}

G4double*  GeometryManager::GetCryostatCoordinateRI(int ithLayer){
	if(ithLayer>=4){
		G4cerr<<"Cryostat layer "<<ithLayer<<" not specified in data file!"<<G4endl;
		return 0;
	}else{
		return fCryostatCoordinateRI[ithLayer];
	}
}

G4double*  GeometryManager::GetCryostatCoordinateRO(int ithLayer){
	if(ithLayer>=4){
		G4cerr<<"Cryostat layer "<<ithLayer<<" not specified in data file!"<<G4endl;
		return 0;
	}else{
		return fCryostatCoordinateRO[ithLayer];
	}
}

G4double*  GeometryManager::GetCryostatCoordinateZ(int ithLayer){
	if(ithLayer>=4){
		G4cerr<<"Cryostat layer "<<ithLayer<<" not specified in data file!"<<G4endl;
		return 0;
	}else{
		return fCryostatCoordinateZ[ithLayer];
	}
}


//Plates in code stages
G4String  GeometryManager::GetCryoPlateName(int ip){
	if(ip<0||ip>(int)fCryoPlates.size()){
		G4cerr<<"Cryostat plate "<<ip<<" not specified in data file!"<<G4endl;
		return "N.A.";
	}else{
		return fCryoPlates[ip].name;
	}
}

G4String  GeometryManager::GetCryoPlateMaterial(int ip){
	if(ip<0||ip>(int)fCryoPlates.size()){
		G4cerr<<"Cryostat plate "<<ip<<" not specified in data file!"<<G4endl;
		return "N.A.";
	}else{
		return fCryoPlates[ip].material;
	}
}

G4double  GeometryManager::GetCryoPlateR(int ip){
	if(ip<0||ip>(int)fCryoPlates.size()){
		G4cerr<<"Cryostat plate "<<ip<<" not specified in data file!"<<G4endl;
		return -1;
	}else{
		return fCryoPlates[ip].r;
	}
}

G4double  GeometryManager::GetCryoPlateH(int ip){
	if(ip<0||ip>(int)fCryoPlates.size()){
		G4cerr<<"Cryostat plate "<<ip<<" not specified in data file!"<<G4endl;
		return -1;
	}else{
		return fCryoPlates[ip].thickness;
	}
}

G4double  GeometryManager::GetCryoPlateZ(int ip){
	if(ip<0||ip>(int)fCryoPlates.size()){
		G4cerr<<"Cryostat plate "<<ip<<" not specified in data file!"<<G4endl;
		return -1;
	}else{
		return fCryoPlates[ip].z;
	}
}

std::vector< std::pair<G4int, G4ThreeVector> >*  GeometryManager::GetCryoPlateHoles(int ip){
	if(ip<0||ip>(int)fCryoPlates.size()){
		G4cerr<<"Cryostat plate "<<ip<<" not specified in data file!"<<G4endl;
		return NULL;
	}else{
		return fCryoPlates[ip].holes;
	}
}

//Beams in cold stages
G4String  GeometryManager::GetCryoBeamName(int ib){
	if(ib<0||ib>(int)fCryoBeams.size()){
		G4cerr<<"Cryostat beam "<<ib<<" not specified in data file!"<<G4endl;
		return "N.A.";
	}else{
		return fCryoBeams[ib].name;
	}
}

G4String  GeometryManager::GetCryoBeamMaterial(int ib){
	if(ib<0||ib>(int)fCryoBeams.size()){
		G4cerr<<"Cryostat beam "<<ib<<" not specified in data file!"<<G4endl;
		return "N.A.";
	}else{
		return fCryoBeams[ib].material;
	}
}

G4double  GeometryManager::GetCryoBeamRI(int ib){
	if(ib<0||ib>(int)fCryoBeams.size()){
		G4cerr<<"Cryostat beam "<<ib<<" not specified in data file!"<<G4endl;
		return -1;
	}else{
		return fCryoBeams[ib].rI;
	}
}

G4double  GeometryManager::GetCryoBeamRO(int ib){
	if(ib<0||ib>(int)fCryoBeams.size()){
		G4cerr<<"Cryostat beam "<<ib<<" not specified in data file!"<<G4endl;
		return -1;
	}else{
		return fCryoBeams[ib].rO;
	}
}

G4double  GeometryManager::GetCryoBeamL(int ib){
	if(ib<0||ib>(int)fCryoBeams.size()){
		G4cerr<<"Cryostat beam "<<ib<<" not specified in data file!"<<G4endl;
		return -1;
	}else{
		return fCryoBeams[ib].l;
	}
}

G4ThreeVector  GeometryManager::GetCryoBeamPos(int ib){
	if(ib<0||ib>(int)fCryoBeams.size()){
		G4cerr<<"Cryostat beam "<<ib<<" not specified in data file!"<<G4endl;
		return G4ThreeVector(0,0,0);
	}else{
		return fCryoBeams[ib].pos;
	}
}


void  GeometryManager::DefineMaterials( ){

    G4double a;  // mass of a mole;
    G4double z;  // z=mean number of protons
    G4int natoms;
    G4int ncomp;
    G4double density;
    G4String symbol, name;

    // ***********************************
	// Liquid helium
    // ***********************************
    //
    G4Material* LHe = new G4Material("LHe", z=2., a= 4.00*g/mole, density= 0.141*g/cm3);
	//Add color
	materialColor["LHe"] = G4Color(1, 0.1, 1, 0.2);

    // ***********************************
	// Sodium Iodide
    // ***********************************
    //
    G4Material* NaI = new G4Material("NaI", density = 3.67*g/cm3, ncomp=2);
    G4Element* Na = new G4Element( "Sodium", "Na", z = 11., a = 23 * g/mole );
    G4Element* I = new G4Element( "Iodine", "I", z = 53., a = 127 * g/mole );
    NaI->AddElement(Na, natoms = 1);
    NaI->AddElement( I, natoms = 1);
	materialColor["NaI"] = G4Color(0.1, 0.1, 1, 0.2);

    // ***********************************
	// Polyethylene
    // ***********************************
    //
    G4Material* PE = new G4Material("PE", density = 0.96*g/cm3, ncomp=2);
    G4Element* C = new G4Element( "Carbon", "C", z = 6.,  a = 12.01 * g/mole );
    G4Element* H = new G4Element( "Hydrogen", "H", z = 1., a = 1.02 * g/mole );
    PE->AddElement(C, natoms = 1);
    PE->AddElement(H, natoms = 2);
	materialColor["PE"] = G4Color(1, 1, 0.9, 0.2);

    // ***********************************
	// SS 304
    // ***********************************
    //
    G4Material* SS = new G4Material("SS", density = 7.8*g/cm3, ncomp=4);
    G4Element* Mn = new G4Element( "Manganese", "Mn", z = 25.,  a = 54.9 * g/mole );
    G4Element* Cr = new G4Element( "Chromium", "Cr", z = 24., a = 52.0 * g/mole );
    G4Element* Ni = new G4Element( "Nickel", "Ni", z = 28., a = 58.7 * g/mole );
    G4Element* Fe = new G4Element( "Iron", "Fe", z = 26., a = 55.8 * g/mole );
    SS->AddElement(Fe, 0.70);
    SS->AddElement(Mn, 0.02); //mass fraction
	SS->AddElement(Cr, 0.18);
	SS->AddElement(Ni, 0.10);
	materialColor["SS"] = G4Color(0.5, 0.5, 0.9, 0.2);
	
    G4Material* Cu = new G4Material("Cu", density = 8.96*g/cm3, ncomp=1);
    G4Element* CuElement = new G4Element( "Copper", "Cu", z = 29.,  a = 63.5 * g/mole );
    Cu->AddElement(CuElement, natoms = 1);
	materialColor["Cu"] = G4Color(1, 0.1, 0.1, 0.2);

    G4Material* Pb = new G4Material("Pb", density = 11.29*g/cm3, ncomp=1);
    G4Element* PbElement = new G4Element( "Lead", "Pb", z = 82.,  a = 207.2 * g/mole );
    Pb->AddElement(PbElement, natoms = 1);
	materialColor["Pb"] = G4Color(0.1, 0.1, 0.1, 0.2);

    G4Material* Ti = new G4Material("Ti", density = 11.29*g/cm3, ncomp=1);
    G4Element* TiElement = new G4Element( "Titanium", "Ti", z = 22.,  a = 47.9 * g/mole );
    Ti->AddElement(TiElement, natoms = 1);
	materialColor["Ti"] = G4Color(0.1, 1, 0.1, 0.2);

    // ***********************************
	// Rock at Homestake
    // ***********************************
    //
    G4double fracM;
        // Molar fraction
//    G4String symbol, name;
    G4int nComponents;

    G4Material* rock = new G4Material("Rock_SURF", density= 3.26*g/cm3, nComponents = 12);
        // The density for Homestake formation is taken from:
        //      https://sdaos.org/wp-content/uploads/pdfs/2014/roggenthen%2033-41.pdf
        // The chemical composition is taken from:
        //      https://arxiv.org/pdf/0912.0211.pdf

    G4Element* elSi = new G4Element(name = "Silicon"    , symbol = "Si" , z = 14. , a =  28.086*g/mole);
    G4Element* elTi = new G4Element(name = "Titanium"   , symbol = "Ti" , z = 22. , a =  47.867*g/mole);
    G4Element* elAl = new G4Element(name = "Aluminum"   , symbol = "Al" , z = 13. , a =  26.981*g/mole);
    G4Element* elFe = new G4Element(name = "Iron"       , symbol = "Fe" , z = 26. , a =  55.845*g/mole);
    G4Element* elMn = new G4Element(name = "Manganese"  , symbol = "Mn" , z = 25. , a =  54.938*g/mole);
    G4Element* elMg = new G4Element(name = "Magnesium"  , symbol = "Mg" , z = 12. , a =  24.405*g/mole);
    G4Element* elCa = new G4Element(name = "Calsium"    , symbol = "Ca" , z = 20. , a =  40.078*g/mole);
    G4Element* elNa = new G4Element(name = "Sodium"     , symbol = "Na" , z = 11. , a =  22.990*g/mole);
    G4Element* elK  = new G4Element(name = "Potassium"  , symbol = "K" ,  z = 19. , a =  39.098*g/mole);
    G4Element* elP  = new G4Element(name = "Phosphorus" , symbol = "P"  , z = 15. , a =  30.974*g/mole);
    G4Element* elH  = new G4Element(name = "Hydrogen"   , symbol = "H"  , z = 1.  , a =   1.008*g/mole);
    G4Element* elO  = new G4Element(name = "Oxygen"     , symbol = "O"  , z = 8.  , a =  15.999*g/mole);
    
    G4double fracM_O = 0;
        // used to accumulate fractional mass of oxygen in the rock
    G4double fraction = 0;
        // fraction of weight of the composite in the rock.

    // ============ SiO2 =============
    fraction = 0.437;
    fracM = 28.086/(28.086+2*15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elSi, fracM * fraction );
    
    // ============ TiO2 =============
    fraction = 0.0122;
    fracM = 47.867/(47.867+2*15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elTi, fracM * fraction );
    
    // ============ Al2O3 =============
    fraction = 0.136;
    fracM = 2*26.981/(2*26.981+3*15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elAl, fracM * fraction );

    // ============ FeO =============
    fraction = 0.127;
    fracM = 55.845/(55.845+15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elFe, fracM * fraction );
    
    // ============ MnO =============
    fraction = 0.0013;
    fracM = 54.938/(54.938+15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elMn, fracM * fraction );
    
    // ============ MgO =============
    fraction = 0.07;
    fracM = 24.405/(24.405+15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elMg, fracM * fraction );
    
    // ============ CaO =============
    fraction = 0.079;
    fracM = 40.078 / ( 40.078 + 15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elCa, fracM * fraction );

    // ============ Na2O =============
    fraction = 0.0287;
    fracM = 2*22.990 / ( 2*22.990 +15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elNa, fracM * fraction );

    // ============ K2O =============
    fraction = 0.0021;
    fracM = 2*39.098 / ( 2*39.098 +15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elK, fracM * fraction );

    // ============ P2O5 =============
    fraction = 0.0007;
    fracM = 2*30.974 / ( 2*30.974 +5*15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elP, fracM * fraction );

    // ============ H2O =============
    fraction = 0.107;
    fracM = 2*1.008 / ( 2*1.008 +15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elH, fracM * fraction );

    // ============ Oxygen =============
    rock->AddElement( elO, fracM_O );


    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}


void GeometryManager::GeometryHasBeenModified(){
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}


void GeometryManager::LoadDimensions( ){

	if(!fDimensionFilesSet){
		return;
	}
	if(fDimensionsLoaded){
		return;
	}

	if(fGeometryType/100==1){// TESSERACT
		G4cout<<"Loading geometry from"<<G4endl;
		G4cout<<dimensionFiles["dimensionFile"]<<G4endl;
		G4cout<<dimensionFiles["cryostatWallFile"]<<G4endl;
		G4cout<<dimensionFiles["cryoPlateFile"]<<G4endl;
		G4cout<<dimensionFiles["cryoBeamFile"]<<G4endl;

		//General dimensions
		TTree *t = new TTree("tDimension","General Dimensions");
		t->ReadFile(dimensionFiles["dimensionFile"], "name/C:dim/D");
		char name[256];
		double dim;
		t->SetBranchAddress("name", name);
		t->SetBranchAddress("dim", &dim);
		for(int i=0; i<t->GetEntries(); i++){
			t->GetEntry(i);
			dimensions[G4String(name)] = dim*mm;
		}

		//Cryostat walls
		LoadCryoWalls();
		//Cryostat plates
		LoadCryoPlate();
		//Cryostat beams
		LoadCryoBeam();

		fDimensionsLoaded = true;
	}
}


void GeometryManager::LoadCryoWalls(){
	//Cryostat walls
	TTree *tCW = new TTree("tCW", "Cryostat wall coordinates");
	tCW->ReadFile(dimensionFiles["cryostatWallFile"], "z[4]/D:rI[4]/D:rO[4]/D");
	double z[4], rI[4], rO[4];
	tCW->SetBranchAddress("z",z);
	tCW->SetBranchAddress("rI",rI);
	tCW->SetBranchAddress("rO",rO);
	for(int i=0; i<4; i++){
		fCryostatCoordinateNP[i]=0;
		fCryostatCoordinateRI[i] = new G4double[tCW->GetEntries()];
		fCryostatCoordinateRO[i] = new G4double[tCW->GetEntries()];
		fCryostatCoordinateZ[i] = new G4double[tCW->GetEntries()];
	}
	for(int ip=0; ip<tCW->GetEntries(); ip++){
		tCW->GetEntry(ip);
		for(int i=0; i<4; i++){
			fCryostatCoordinateRI[i][ip] = rI[i]*mm;
			fCryostatCoordinateRO[i][ip] = rO[i]*mm;
			fCryostatCoordinateZ[i][ip] = z[i]*mm;
			if(rI[i]<0&&fCryostatCoordinateNP[i]==0){
				fCryostatCoordinateNP[i] = ip;
			}
		}
	}
	for(int i=0; i<4; i++){
		if(fCryostatCoordinateNP[i]==0)
			fCryostatCoordinateNP[i] = tCW->GetEntries();
	}
}


void GeometryManager::LoadCryoPlate(){
	//first load drill chart
	fDrillChart = new std::vector<G4double>;
	std::ifstream cryoPlateFile(dimensionFiles["cryoPlateFile"], std::ifstream::in);
	char temp[256];
	cryoPlateFile.getline(temp, 256);
	cryoPlateFile.getline(temp, 256);
	//drill chart at line 3. Using TString::Tokenize to read the line.
	cryoPlateFile.getline(temp, 256);
	TString drillChart(temp+1);//remove the # in the first position.
	TString drillRToken;
	Ssiz_t from = 0;
	while(drillChart.Tokenize(drillRToken, from, " ")){
		G4double drillR = drillRToken.Atof()*mm;
		fDrillChart->push_back(drillR);
	}
	cryoPlateFile.close();

	//Now load plate dimensions.
	TTree *tCP = new TTree("tCP", "Plates in Cryostat");
	tCP->ReadFile(dimensionFiles["cryoPlateFile"], "name/C:r/D:thickness/D:z/D:material/C:nHoles/I:xhole[6]/D:yhole[6]/D:drill[6]/I");
	char name[256], material[256];
	double r, thickness, z, x[6], y[6];
	int nHoles, drill[6];
	tCP->SetBranchAddress("name", name);
	tCP->SetBranchAddress("r",    &r);
	tCP->SetBranchAddress("thickness", &thickness);
	tCP->SetBranchAddress("z",    &z);
	tCP->SetBranchAddress("material", material);
	tCP->SetBranchAddress("nHoles",&nHoles);
	tCP->SetBranchAddress("xhole", x);
	tCP->SetBranchAddress("yhole", y);
	tCP->SetBranchAddress("drill", drill);
	for(int i=0; i<tCP->GetEntries(); i++){
		tCP->GetEntry(i);
		CryoPlate plate;
		plate.name = name;
		plate.r = r*mm;
		plate.thickness = thickness*mm;
		plate.z = z*mm;
		plate.material = G4String(material);
		plate.holes = new std::vector< std::pair<G4int, G4ThreeVector> >;
		for(int ih=0; ih<nHoles; ih++){
			plate.holes->push_back( 
						std::make_pair( drill[ih], 
							G4ThreeVector(x[ih],y[ih],0)));
		}
		fCryoPlates.push_back(plate);
	}

}


void GeometryManager::LoadCryoBeam(){
	TTree *tCB = new TTree("tCB", "Beams in Cryostat");
	tCB->ReadFile(dimensionFiles["cryoBeamFile"], "name/C:rO/D:rI/D:l/D:pos[3]/D:material/C");
	char name[256], material[256];
	double rO, rI, l, pos[3];
	tCB->SetBranchAddress("name", name);
	tCB->SetBranchAddress("rI",    &rI);
	tCB->SetBranchAddress("rO",    &rO);
	tCB->SetBranchAddress("l",    &l);
	tCB->SetBranchAddress("pos",  pos);
	tCB->SetBranchAddress("material", material);
	for(int i=0; i<tCB->GetEntries(); i++){
		tCB->GetEntry(i);
		CryoBeam beam;
		beam.name = name;
		beam.rI = rI*mm;
		beam.rO = rO*mm;
		beam.l = l*mm;
		beam.pos = G4ThreeVector(pos[0]*mm, pos[1]*mm,pos[2]*mm);
		beam.material = G4String(material);
		fCryoBeams.push_back(beam);
	}
}


void GeometryManager::SetVisAttributes(){
	//G4cout<<"Set colors..."<<G4endl;
    G4PhysicalVolumeStore* store = G4PhysicalVolumeStore::GetInstance();
	for(auto itr = store->begin(); itr!=store->end(); ++itr){
		//G4cout << (*itr)->GetName() << G4endl;
		//G4cout << (*itr)->GetLogicalVolume()->GetVisAttributes() << G4endl;
		//G4cout << " with material " << (*itr)->GetLogicalVolume()->GetMaterial()->GetName() << G4endl;
		if( (*itr)->GetLogicalVolume()->GetVisAttributes()==0 ){
			G4Color color =  materialColor[(*itr)->GetLogicalVolume()->GetMaterial()->GetName()];
			(*itr)->GetLogicalVolume()->SetVisAttributes(color);
		}
	}
}



