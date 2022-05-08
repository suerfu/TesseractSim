/*
    written by B. Suerfu
    on 2022-05-06
    suerfu@berkeley.edu
*/

// history:
// 2022-05-06 B. Suerfu initial version

// description:
// This file will read in processed ROOT files with RQ and plots the spectrum in energy region-of-interest
// by weighing different sets of files accordingly.

// usage:
// ./PlotSpectrum 
//      --voi VolumeName
//      --hist Nbins min max
//      --title Name1
//      --add Activity1/Flux1 foo1.root [foo2.root ...]
//      --add Activity2/Flux2 bar1.root [bar2.root ...]
//      --color C1 [integer using ROOT code scheme]
//      --style S1 [integer using ROOT code scheme]

// Key option --title is used to separate multiple curves.
// After each title keyword, --add can be used to add multiple subcomponent.
// If needed, --color and --style should be specified after each --title keyword.

// Activity in Bq/kg
// Flux in particles/m2/s (total flux used in the simulation)
// Above two quantities are needed for computing the normalized simulation time

// The color and line style will be applied to the corresponding background spectra in the same order as histograms are added

#include "TFile.h"
#include "TH1F.h"
#include "TMacro.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>


using namespace std;


// Checks if commandline argument has all the necessary keywords.
// Returns true if passes checks.
// Otherwise prints missing information & usage and returns false
//
bool CheckArguments( int argc, char* argv[] );


// Outputs the actual usage with various options and notions, etc.
//
void PrintUsage( string name );


struct Spectrum{

    Spectrum(){
        title = "N/A";
        style = 0;
        color = 0;
        width = 1;
    }

    string title;

    int style;
    int color;
    int width;

    map<string, vector<string> > components;
        // first keyword is activity/flux
        // second is basically a list of files to be processed and added
};


// Obtains a ROOT macro file from the specified ROOT file by name.
//
TMacro GetMacro( string rootName, string macroName);


// Returns the run duration from the specified macro.
//
double GetDuration( TMacro macro );


// Returns the active mass from the specified macro.
//
double GetActiveMass( TMacro macro, string voi );


// Fill histogram from another ROOT tree.
//
void FillHistFromTree( TH1F* hist, string rootName, string voi, string treeName = "events" );


int main( int argc, char* argv[]){

    if( CheckArguments( argc, argv)==false )
        return -1;

    // Get the basic histogram parameters
    //
    unsigned int Nbins = 0;
    float minBin = 0 ;
    float maxBin = 0;

    for( unsigned int i=0; i<argc; i++){
        if( string(argv[i])=="--hist" ){
            Nbins = atoi(argv[++i]);
            minBin = atof( argv[++i]);
            maxBin = atof( argv[++i]);
            break;
        }
    }

    // Check if file needs to be saved.
    //
    TFile* file = 0;
    for( unsigned int i=0; i<argc; i++){
        if( string(argv[i])=="--output" ){
            file = new TFile( argv[++i], "NEW");
            break;
        }
    }

    // Get volume of interest.
    //
    string voi = "";
    for( unsigned int i=0; i<argc; i++){
        if( string(argv[i])=="--voi" ){
            voi = string( argv[++i]);
            break;
        }
    }

    double activeMass = -1;
        // will be obtained from macro in the ROOT file.

    vector<Spectrum> spectrumArray;

    // Iterate over commandline arguments to sort out how the files should be processed.
    //
    for( int i=0; i<argc; i++){

        string foo = string( argv[i] );

        // If current argument is --title, then enter a sub-loop
        // In the subloop, components are created and added to the curve
        //
        if( foo=="--title" ){

            Spectrum temp;
            string activity = "1e-10";
            temp.title = argv[++i];

            // Subloop to add components, set line color, style and line width
            //
            for( ; i<argc; i++ ){

                if( string(argv[i])=="--title" ){
                    i--;
                    break;
                        // Since in the next iteration, i will point past --title
                        // Decrement it to place it at the right position.
                }

                else if( string(argv[i])=="--color" )
                    temp.color = atoi( argv[++i] );
                else if( string(argv[i])=="--style" )
                    temp.style = atoi( argv[++i] );
                else if( string(argv[i])=="--width" )
                    temp.width = atoi( argv[++i] );

                else if( string(argv[i])=="--add" ){
                    activity = argv[++i];

                    while( i++ && i<argc && argv[i][0]!='-' ){
                        temp.components[ activity ].push_back( argv[i] );
                    }
                    i--;
                }
            }

            spectrumArray.push_back( temp );

        }
        else{
            continue;
        }
    }

    // ROOT objects to store the results.
    //
    vector<TH1F*> histArray;
        // used to store final results in the desired output
    vector<TH1F> tempArray;
        // used to store temporary components.
    TCanvas canvas;
    TLegend legend;

    
    // This code block is simply used to check the output structure.
    //
    for( auto i=spectrumArray.begin(); i!=spectrumArray.end(); i++ ){
        cout << endl;
        cout << "Processing title: " << i->title << endl;

        TH1F* hist = new TH1F( i->title.c_str(), i->title.c_str(), Nbins, minBin, maxBin );

        hist->SetLineColor( i->color );
        hist->SetLineStyle( i->style );
        hist->SetLineWidth( i->width );

        for( auto j=i->components.begin(); j!=i->components.end(); j++ ){

            double activity = atof( j->first.c_str() );
            cout << "Activity: " << activity << endl;

            TH1F temp( j->first.c_str(), "", Nbins, minBin, maxBin );
            double duration = 0;

            for( auto k=j->second.begin(); k!=j->second.end(); k++){
                cout << "\t adding" << *k << endl;

                FillHistFromTree( &temp, *k, voi );

                TMacro mac_duration = GetMacro( *k, "duration" );
                duration = GetDuration( mac_duration );

                TMacro mac_mass = GetMacro( *k, "geometryTable" );
                activeMass = GetActiveMass( mac_mass, voi );

            }
            cout << endl;

            temp.Sumw2();
            duration /= (24*3600);  // convert from seconds to days.
            double dE = (maxBin-minBin)/Nbins;
            temp.Scale( 1./duration/activeMass/dE );
            hist->Add( &temp );
        }

        histArray.push_back(hist);

    }
    
    if( file!=0 ){
        file->cd();
            // Since other files have been opened, one needs to change working directory back and save.
        for( auto itr = histArray.begin(); itr!=histArray.end(); itr++ ){
            (*itr)->Write();
        }
        canvas.Write();
        file->Write();
        file->Close();
    }

    return 0;

}


TMacro GetMacro( string rootName, string macroName){

    TFile* file = TFile::Open( rootName.c_str(), "READ");

    TIter next( file->GetListOfKeys() );
    TString mac_name;
    TKey *key;

    while( (key=(TKey*)next()) ){

        mac_name = key->GetName();

        if ( mac_name.EqualTo(macroName) ){
            TMacro mac( *((TMacro*)file->Get( mac_name )) );
            file->Close();
            return mac;
        }
    }

    cerr << "Failed to get macro " << macroName << " from file " << rootName << endl;

    file->Close();

    return TMacro();

}


void FillHistFromTree( TH1F* hist, string rootName, string voi, string treeName ){

    TFile file( rootName.c_str(), "READ" );
    TTree* tree = (TTree*) file.Get( treeName.c_str() );

    double energy;
    string plotVar = string( "edep_" ) + voi;
    tree->SetBranchAddress( plotVar.c_str(), &energy);

    for( unsigned long long n=0; n<tree->GetEntries(); n++){
        tree->GetEntry(n);
        if( energy>1e-9 ){
            //if( 1000*energy_veto<vetoThreshold )
            hist->Fill( energy );
            //}
        }
    }

    file.Close();
}



double GetActiveMass( TMacro geoMacro, string voi ){

    double mass = 1;

    auto tmp = geoMacro.GetLineWith( voi.c_str() );

    if( tmp!= 0 ){
        stringstream ss(  tmp->String().Data() );
        string foo;
        double mass;
        ss >> foo >> mass;
    }
    else{
        cerr << "Failed to get active mass of " << voi << endl;
    }

    return mass;
}


double GetDuration( TMacro macro ){

    double duration = -1;

    auto tmp = (TObjString*) ( macro.GetListOfLines()->First() );
    TString str( tmp->GetString() );

    stringstream ss( str.Data() );
    ss >> duration;
    
    return duration;
}


bool CheckArguments( int argc, char* argv[] ){
    
    // A few flag variables for checking
    //
    bool have_title = false;
    bool have_hist = false;
    bool have_add = false;
    bool have_help = false;
    bool have_voi = false;
    
    // Loop over the arguments to check if keywords are specified or not
    //
    for( int i=0; i<argc; i++){

        string foo = string(argv[i]);

        if( foo=="--title" )    
            have_title = true;
        else if( foo=="--hist" )
            have_hist = true;
        else if( foo=="--add" )
            have_add = true;
        else if( foo=="--help" )
            have_help = true;
        else if( foo=="--voi" )
            have_voi = true;
    }

    if( have_help==true ){
        PrintUsage( argv[0] );
        return false;
    }
    if( have_title==false ){
        cerr << "No spectrum is added. Specify it with --title Name." << endl;
        PrintUsage( argv[0]);
        return false;
    }
    if( have_hist==false ){
        cerr << "Histogram parameters not specified. Specify them with --hist Nbins Min Max." << endl;
        PrintUsage( argv[0]);
        return false;
    }
    if( have_add==false ){
        cerr << "Spectrum components not added. Add them with --add activity(Bq/kg)/flux(Nb/m2/s) Input0.root [Input1.root ...]." << endl;
        PrintUsage( argv[0]);
        return false;
    }
    if( have_voi==false){
        cerr << "Volume of interest not specified. Specify it with --voi NameOfVolume." << endl;
        PrintUsage( argv[0]);
        return false;
    }
        
    return true;
}


void PrintUsage( string name ){
    cout << "Usage: " << name << "--hist Nbins Min Max --voi VolOfInterest --title Title1 --add Act1/Flux1 foo1.root bar1.root [--add foo2.root bar2.root] [--color N --style N --width N]" << endl;
    cout << "\t Use --title to add multiple curves onto the same plot\n";
    cout << "\t Activity/flux in the unit of Bq/kg and No./m2/s, respectively.\n";
    cout << "\t --color, --style and --width are used to specify optional line color and line style using ROOT code scheme.\n";
}
