#include <iostream>
#include <sstream>
#include<string>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"
#include "THStack.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TMinuit.h"
#include "TColor.h"
#include "TLine.h"
#include "TLatex.h"
#include "TSystem.h"
#include "TMacro.h"
using namespace std;

class MacroInfo{
  public:
    double X,Y,Z,NoOfParticle,Volume,SurfaceArea;
    string Xunit,Yunit,Zunit;

    void GetBoxDim_Unit(TMacro* h1, string phrase);
    void GetNoOfParticle(TMacro* h1, string Xphrase);
    //void GetVolume();
    void GetSurfaceArea();
    //void Get
};

void MacroInfo::GetBoxDim_Unit(TMacro* h1, string phrase) {

  string str;
  std::string length(h1->GetLineWith((phrase+"_x").c_str())->GetString());
  stringstream ssx(length.c_str());
  ssx>>str>>X>>Xunit;

  std::string breadth(h1->GetLineWith((phrase+"_y").c_str())->GetString());
  stringstream ssy(breadth.c_str());
  ssy>>str>>Y>>Yunit;

  std::string height(h1->GetLineWith((phrase+"_z").c_str())->GetString());
  stringstream ssz(height.c_str());
  ssz>>str>>Z>>Zunit;
}

void MacroInfo::GetNoOfParticle(TMacro* h1, string phrase) {

  string str;
  std::string length(h1->GetLineWith((phrase).c_str())->GetString());
  stringstream ssx(length.c_str());
  ssx>>str>>NoOfParticle;

}

//void MacroInfo::GetVolume() {
//  if (Yunit =="m") {
//      Volume=X*Y*Z;
//  }
//  if (Yunit =="cm") {
//      Volume=X*Y*Z/(100*100*100);
//  }
//
//}

void MacroInfo::GetSurfaceArea() {
  if (Yunit =="m") {
    SurfaceArea=2*(X*Y+X*Z+Y*Z);
  }
  if (Yunit =="cm") {
    SurfaceArea=2*(X*Y+X*Z+Y*Z)/(100*100);
  }

}

MacroInfo GetMacroInfo(TFile* file, TString Filename){
  MacroInfo m;
  TIter next(file->GetListOfKeys());
  TString T2HF_name;
  TKey *key;
  while ((key=(TKey*)next())) {
   T2HF_name=key->GetName();

    if (T2HF_name.EqualTo(Filename)) {

        TMacro* h1 = (TMacro*)file->Get(T2HF_name);
        m.GetBoxDim_Unit(h1 ,"/generator/wall");
        m.GetNoOfParticle(h1,"/run/beamOn");
        m.GetSurfaceArea();
    }

  }
 return m;
}


void GetGammaK40(const char* ext = ".root"){

  const char* inDir = "/nfs/turbo/lsa-penningb/data/simulations/TESsims/geom3_helium/GammaK40/";
  char* dir = gSystem->ExpandPathName(inDir);
  Printf("%s", dir);
  void* dirp = gSystem->OpenDirectory(dir);
  cout<<gSystem->OpenDirectory(dir)<<endl;
  const char* entry;
  const char* filename[10000];
  Int_t n = 0;
  Int_t count = 0;
  TString str;
  long double TotalParticles=0.0;
  double Surface_Area;

  while((entry = (char*)gSystem->GetDirEntry(dirp))) {
    str = entry;
    if(str.EndsWith(ext))
      filename[n++] = gSystem->ConcatFileName(dir, entry);
  }



  for (Int_t i = 0; i < n; i++){
      Printf("%d file -> %s",i, filename[i]);
      TFile* file = new TFile(filename[i]);

      if( !file->IsOpen() ){
             cout<< "Error opening " << filename[i] <<endl;
             continue;
      }
      else {
          if(file->IsZombie()) {
            cout<<"The file "<<filename[i]<<"is corrupted"<<endl;
            continue;
          }
          else{
            std::string name(filename[i]);
            //MacroInfo m=GetMacroInfo(file, name.substr(96,17)); //Flux_virtual_
            MacroInfo m=GetMacroInfo(file, "Flux_GammaK40_0");
            //cout<<name.substr(96,17)<<"   "<<m.SurfaceArea<<endl;
            TotalParticles = m.NoOfParticle + TotalParticles;
            Surface_Area=m.SurfaceArea;
          }
      }

    }


  cout<<"Total number of particle simulated is "<<TotalParticles<<"and the surface area is"<<Surface_Area<<endl;


  TH1F *H_Dep = new TH1F("H_Dep","Energy Dep [keV]",100,0,1000);//keV for DRU
  H_Dep->SetStats(0);


  TFile* f = TFile::Open("/nfs/turbo/lsa-penningb/data/simulations/TESsims/Analysis/Analysis_File/geom3_helium/GammaK40/GammaK40_processed_files.root");
  TTree *t = (TTree*)f->Get("events");
  Int_t nentries = (Int_t)t->GetEntries();
  Double_t Edep;
  t->SetBranchAddress("edep_virtualDetector",&Edep);//keV

  for (Int_t i=0;i<nentries;i++){
      t->GetEntry(i);
      if (Edep<1000.0) {
          H_Dep->Fill(Edep);
      }

  }//end for looping over the entries
  f->Close();



  double specific_activity_K40=15.753487731999997;// perSec per m^2 due to 1 Bq/kg from Suerfu
  double activity_K40_SURF=220;//Bq/kg
  double Particle_per_second= specific_activity_K40*activity_K40_SURF*Surface_Area; //perSec
  double TotalTime= TotalParticles/Particle_per_second;
  double Mass= 3.14*10*10*18*0.141/1000; //kg

  cout<<"Total time in Days "<<TotalTime*1.15741e-5<<endl;
  cout<<"Mass of Helium in kg "<<Mass<<endl;
  cout<<"Bin Width "<<H_Dep->GetBinWidth(1)<<endl;

  Double_t factor = 1.;
  H_Dep->Scale(factor/(TotalTime*1.15741e-5* Mass*H_Dep->GetBinWidth(1)));

  TFile *myfile = new TFile("GammaK40.root", "RECREATE");
  H_Dep->Write();
  myfile->Close();


}
