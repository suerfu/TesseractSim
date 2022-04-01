/// Standard includes
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <functional>
#include <map>
#include <stdlib.h>

// ROOT includes
#include "TSystem.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TRint.h"
#include "TStyle.h"
#include "THStack.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TFile.h"
#include "TTree.h"
using namespace std;


//////////////////////
// String operation //
//////////////////////

template<typename T>
T strTo(const char* str)
{
    T t;
    std::stringstream converter(str);
    converter >> t;

    return t;
}

template<typename T>
T strTo(const std::string& str)
{
    return strTo<T>(str.c_str());
}


bool CmdOptionExists(int argc, char** argv, const std::string& option)
{
    char** begin = argv;
    char** end = argv + argc;
    return std::find(begin, end, option) != end;
}


template<typename T = std::string>
T GetCmdOption(int argc, char** argv, const std::string& option, int optionNum = 1)
{
    if(!CmdOptionExists(argc, argv, option))
    {
        //Message::Error(strForm("Missing option %s", option.c_str()));
        abort();
    }

    char** begin = argv;
    char** end = argv + argc;

    char ** itr = std::find(begin, end, option);

    for(int iOpt = 0; iOpt < optionNum; ++iOpt)
    {
        if(itr != end && ++itr != end && ((*itr)[0] != '-' || std::isdigit((*itr)[1])))
        {
            if(iOpt + 1 == optionNum)
                return strTo<T>(*itr);
        }
        else
        {
            std::string pos = "nth";
            if(iOpt == 0) pos = "1st";
            else if(iOpt == 1) pos = "2nd";
            else if(iOpt == 2) pos = "3rd";
            else pos = std::to_string(iOpt + 1) + "th";

            //Message::Error(strForm("Missing %s parameter for option %s", pos.c_str(), option.c_str()));
            exit(EXIT_FAILURE);

            return 0;
        }
    }

    return 0;
}


template<typename T = std::string>
std::vector<T> GetCmdOptionArray(int argc, char** argv, const std::string& option)
{
    if(!CmdOptionExists(argc, argv, option))
    {
        printf("Missing option %s \n",option.c_str());
        abort();
    }

    std::vector<std::string> options;

    char** begin = argv;
    char** end = argv + argc;

    char ** itr = std::find(begin, end, option);

    while(itr++, itr != end && ((*itr)[0] != '-' || std::isdigit((*itr)[1])))
    {
        if(itr != end)
        {
            options.push_back(strTo<T>(*itr));
        }
    }

    if(options.size() == 0)
    {
        printf("Missing parameter for option %s \n",option.c_str());
        exit(EXIT_FAILURE);
    }

    return options;
}



void Usage()
{
    cout << endl;
    cout << "Usage: EnergyDepVSIntialEnergy -p <ProcessedFile name> -o <ouput_filename>" << endl;
    cout << endl;
}


int main(int argc, char* argv[])
{


  if(CmdOptionExists(argc, argv, "-h") || argc==1 )
   {
       Usage();
       return EXIT_SUCCESS;
   }


    string outputFilename = "output.root";
    if(CmdOptionExists(argc, argv, "-o"))
    {
        outputFilename = GetCmdOption<string>(argc, argv, "-o");
    }
    else
    {
        cout<<"Output Filename not specified, default option is set to output.root "<<endl;
    }

    string ProcessedFiles;
    if(CmdOptionExists(argc, argv, "-p"))
    {
        ProcessedFiles = GetCmdOption<string>(argc, argv, "-p");
    }
    else
    {
        return EXIT_FAILURE;
    }

    Double_t Edep,IntialE,rx,ry,rz,px,py,pz;

    TFile *outputfile = new TFile(outputFilename.c_str(),"RECREATE");
    TTree *events= new TTree("events","simple tree");

    events->Branch("Edep",&Edep,"Edep/D");
    events->Branch("IntialE",&IntialE,"IntialE/D");
    events->Branch("rx",&rx,"rx/D");
    events->Branch("ry",&ry,"ry/D");
    events->Branch("rz",&rz,"rz/D");
    events->Branch("px",&px,"px/D");
    events->Branch("py",&py,"py/D");
    events->Branch("pz",&pz,"pz/D");


    //TFile* f = TFile::Open("/Volumes/GoogleDrive/My Drive/GraduateWork/TESSERACT/Tesseract_sim_output/geom2_helium/ProcessedFiles/GammaK40_processed.root");
    TFile* f = new TFile(ProcessedFiles.c_str());

    if( !f->IsOpen() ){
           cout<< "Error opening " << ProcessedFiles.c_str() <<endl;
    }
    else {

      if(f->IsZombie()) {
        cout<<"The file "<<ProcessedFiles.c_str()<<"is corrupted"<<endl;
      }
      else{
        TTree *t = (TTree*)f->Get("events");
        Int_t nentries = (Int_t)t->GetEntries();
        Double_t E_dep;
        Int_t Event_ID;
        char name_file[200];
        t->SetBranchAddress("edep_virtualDetector",&E_dep);//keV
        t->SetBranchAddress("file",name_file);
        t->SetBranchAddress("eventID",&Event_ID);

        for (Int_t i=0;i<nentries;i++){
          t->GetEntry(i);
          std::string filename(name_file);
          if (E_dep<1000.0) {

            cout<<filename<<"  "<<Event_ID<<endl;

            TFile* f1 = new TFile(filename.c_str());

            TTree *t1 = (TTree*)f1->Get("events");
            Int_t nentries = (Int_t)t1->GetEntries();
            //cout<<nentries<<endl;

            Double_t X_pos,Y_pos,Z_pos,X_mom,Y_mom,Z_mom, Intial_E;
            Int_t event_ID, step_ID, parent_ID;
            char Process_ID[16];

            t1->SetBranchAddress("eventID",&event_ID);
            t1->SetBranchAddress("parentID",&parent_ID);
            t1->SetBranchAddress("stepID",&step_ID);
            t1->SetBranchAddress("rx",&X_pos);
            t1->SetBranchAddress("ry",&Y_pos);
            t1->SetBranchAddress("rz",&Z_pos);
            t1->SetBranchAddress("px",&X_mom);
            t1->SetBranchAddress("py",&Y_mom);
            t1->SetBranchAddress("pz",&Z_mom);
            t1->SetBranchAddress("Eki",&Intial_E);
            t1->SetBranchAddress("process",Process_ID);

            for(Int_t q=0;q<nentries;q++){
                t1->GetEntry(q);
                std::string Process(Process_ID);

                if ( (event_ID==Event_ID && parent_ID==0 && step_ID==0 && Process=="initStep") ) {
                  rx=X_pos;
                  ry=Y_pos;
                  rz=Z_pos;
                  px=X_mom;
                  py=Y_mom;
                  pz=Z_mom;
                  IntialE=Intial_E;
                  Edep=E_dep;
                  cout<<event_ID<<"  "<<step_ID<<"  "<<Process<<"    "<<Edep<<"    "<<IntialE<<endl;
                  events->Fill();
                }

              }
             f1->Close();
            }

          }//end for looping over the entries
          f->Close();
  }
}
outputfile->Write();
outputfile->Close();
}
