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
    cout << "Usage: ReSampler -i <SimuOuputFilename_1.root> ... <SimuOuputFilename_N.root> -v <volume_name_surfaceflux> -o <ouput_filename>" << endl;
    cout << endl;
}


int main(int argc, char* argv[])
{


  if(CmdOptionExists(argc, argv, "-h"))
   {
       Usage();
       return EXIT_SUCCESS;
   }

   vector<string> inputFilenames;

    if(CmdOptionExists(argc, argv, "-i"))
    {
        inputFilenames = GetCmdOptionArray<string>(argc, argv, "-i");
    }
    else
    {
        return EXIT_FAILURE;
    }

    string outputFilename = "ReSampler_output.root";
    if(CmdOptionExists(argc, argv, "-o"))
    {
        outputFilename = GetCmdOption<string>(argc, argv, "-o");
    }
    else
    {
        cout<<"outputfilename not specified, default option is set to ReSampler_output.root "<<endl;
    }

    string main_volume = "ImaginaryVacuum";
    if(CmdOptionExists(argc, argv, "-v"))
    {
        main_volume = GetCmdOption<string>(argc, argv, "-v");
    }
    else
    {
        cout<<"Particle name not specified, default option is set to ImaginaryVacuum"<<endl;
    }



TFile *outputfile = new TFile(outputFilename.c_str(),"RECREATE");
TTree *events= new TTree("events","simple tree");
Double_t t,rx,ry,rz,Parent_rx,Parent_ry,Parent_rz,px,py,pz,Eki,Ekf;
Int_t eventID, index, multiplicity;
char volume[16], particle[16], parentparticle[16];

Double_t ARt[20],ARrx[20],ARry[20],ARrz[20],ARpx[20],ARpy[20],ARpz[20],AREki[20],AREkf[20],ARParentrx[20],ARParentry[20],ARParentrz[20];
Int_t AReventID[20];
char ARvolume[20][16], ARparticle[20][16],ARParent[20][16];

events->Branch("t",&t,"t/D");
events->Branch("rx",&rx,"rx/D");
events->Branch("ry",&ry,"ry/D");
events->Branch("rz",&rz,"rz/D");
events->Branch("px",&px,"px/D");
events->Branch("py",&py,"py/D");
events->Branch("pz",&pz,"pz/D");
events->Branch("Eki",&Eki,"Eki/D");
events->Branch("Ekf",&Ekf,"Ekf/D");
events->Branch("eventID",&eventID,"eventID/I");
events->Branch("volume",volume,"volume[16]/C");
events->Branch("particle",particle,"particle[16]/C");
events->Branch("parentparticle",parentparticle,"parentparticle[16]/C");
events->Branch("Parent_rx",&Parent_rx,"Parent_rx/D");
events->Branch("Parent_ry",&Parent_ry,"Parent_ry/D");
events->Branch("Parent_rz",&Parent_rz,"Parent_rz/D");
events->Branch("multiplicity",&multiplicity,"multiplicity/I");
events->Branch("index",&index,"index/I");



for(const string& filename : inputFilenames){


  Printf("file -> %s", filename.c_str());
  TFile* f = TFile::Open(filename.c_str());
  TTree *t1 = (TTree*)f->Get("events");
  Int_t nentries = (Int_t)t1->GetEntries();
  //cout<<nentries<<endl;

  Double_t time,X_pos,Y_pos,Z_pos,X_mom,Y_mom,Z_mom, Intial_E, Final_E;
  Int_t event_ID,track_ID, step_ID, parent_ID;
  //string Vol_ID,Particle_ID;
  char Vol_ID[16], NextVol_ID[16], Particle_ID[16], Process_ID[16];

  Double_t Temp_X_pos,Temp_Y_pos,Temp_Z_pos;
  char Temp_particle[16];
  Int_t index_arr=0,Multiplicity=0;

  t1->SetBranchAddress("eventID",&event_ID);
  t1->SetBranchAddress("trackID",&track_ID);
  t1->SetBranchAddress("particle",Particle_ID);
  t1->SetBranchAddress("parentID",&parent_ID);
  t1->SetBranchAddress("stepID",&step_ID);
  t1->SetBranchAddress("volume",Vol_ID);
  t1->SetBranchAddress("nextVolume",NextVol_ID);
  t1->SetBranchAddress("rx",&X_pos);
  t1->SetBranchAddress("ry",&Y_pos);
  t1->SetBranchAddress("rz",&Z_pos);
  t1->SetBranchAddress("px",&X_mom);
  t1->SetBranchAddress("py",&Y_mom);
  t1->SetBranchAddress("pz",&Z_mom);
  t1->SetBranchAddress("t",&time);
  t1->SetBranchAddress("Eki",&Intial_E);
  t1->SetBranchAddress("Ekf",&Final_E);
  t1->SetBranchAddress("process",Process_ID);

  for(Int_t q=0;q<nentries;q++){
      t1->GetEntry(q);
      Int_t n_secondary;
      std::string Volume(Vol_ID);
      std::string NextVolume(NextVol_ID);
      std::string Particle(Particle_ID);
      std::string Process(Process_ID);

      if ( (parent_ID==0 && Process=="initStep") ) {
        Temp_X_pos=X_pos;
        Temp_Y_pos=Y_pos;
        Temp_Z_pos=Z_pos;
        strncpy(Temp_particle, Particle_ID, 16);

        //cout<<event_ID<<"     "<<track_ID<<"   "<<step_ID<<" "<<Particle<<"   "<<Volume<<"    "<<Process<<"    "<<parent_ID<<endl;
      }

      if ( Volume.compare(main_volume)==0 && NextVolume == "OutOfWorld" ){
        ARt[Multiplicity]=time;
        ARrx[Multiplicity]=X_pos;
        ARry[Multiplicity]=Y_pos;
        ARrz[Multiplicity]=Z_pos;
        ARpx[Multiplicity]=X_mom;
        ARpy[Multiplicity]=Y_mom;
        ARpz[Multiplicity]=Z_mom;
        AREki[Multiplicity]=Intial_E;
        AREkf[Multiplicity]=Final_E;
        AReventID[Multiplicity]=event_ID;
        ARParentrx[Multiplicity]=Temp_X_pos;
        ARParentry[Multiplicity]=Temp_Y_pos;
        ARParentrz[Multiplicity]=Temp_Z_pos;
        strncpy(ARvolume[Multiplicity], Vol_ID, 16);
        strncpy(ARparticle[Multiplicity], Particle_ID, 16);
        strncpy(ARParent[Multiplicity], Temp_particle, 16);
        Multiplicity++;


        //cout<<event_ID<<"     "<<track_ID<<"   "<<step_ID<<" "<<Particle<<"   "<<Volume<<"    "<<Process<<"    "<<parent_ID<<endl;
      }

      if ( (q!=0 && Process=="newEvent") || (q==nentries-1 && Process=="timeReset")   ) {
        //cout<<Multiplicity<<endl;
        for (Int_t i = 0; i < Multiplicity; i++) {
          eventID=AReventID[Multiplicity];
          multiplicity=Multiplicity;
          index=i;
          t=ARt[i];
          rx=ARrx[i];
          ry=ARry[i];
          rz=ARrz[i];
          px= ARpx[i];
          py= ARpy[i];
          pz= ARpz[i];
          Eki=AREki[i];
          Ekf=AREkf[i];
          strncpy(volume, ARvolume[i], 16);
          strncpy(particle, ARparticle[i], 16);
          Parent_rx=ARParentrx[i];
          Parent_ry=ARParentry[i];
          Parent_rz=ARParentrz[i];
          strncpy(parentparticle, ARParent[i], 16);
          events->Fill();
        }
        Multiplicity=0;
        index_arr=0;
      }

    }//loop over nentries


  f->Close();
}//loop over all the files

outputfile->Write();
outputfile->Close();

}
