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


void Combine(){
  TFile* file1= new TFile("/Volumes/GoogleDrive/My Drive/GraduateWork/TESSERACT/Tesseract_sim_output/geom3_helium/Histogram_Processed/GammaK40.root");
  TH1F* GammaK40 = (TH1F*)file1->Get("H_Dep");
  TFile* file2= new TFile("/Volumes/GoogleDrive/My Drive/GraduateWork/TESSERACT/Tesseract_sim_output/geom3_helium/Histogram_Processed/GammaU238.root");
  TH1F* GammaU238 = (TH1F*)file2->Get("H_Dep");
  TFile* file3 = new TFile("/Volumes/GoogleDrive/My Drive/GraduateWork/TESSERACT/Tesseract_sim_output/geom3_helium/Histogram_Processed/GammaTh232.root");
  TH1F* GammaTh232 = (TH1F*)file3->Get("H_Dep");

  //TFile* file4 = new TFile("/Volumes/GoogleDrive/My Drive/GraduateWork/TESSERACT/Tesseract_sim_output/geom3_helium/Histogram_Processed/NeutronTh232.root");
  //TH1F* NeutronTh232 = (TH1F*)file4->Get("H_Dep");
  //TFile* file5 = new TFile("/Volumes/GoogleDrive/My Drive/GraduateWork/TESSERACT/Tesseract_sim_output/geom3_helium/Histogram_Processed/NeutronUnat.root");
  //TH1F* NeutronUnat = (TH1F*)file5->Get("H_Dep");

  TH1F* h_ER= new TH1F(*GammaK40);
  h_ER->Add(GammaU238,1);
  h_ER->Add(GammaTh232,1);

  //TH1F* h_NR= new TH1F(*NeutronTh232);
  //h_NR->Add(NeutronUnat,1);

  //TH1F* h_sum= new TH1F(*h_ER);
  //h_sum->Add(h_NR,1);


  THStack* hsE4 = new THStack();
  TLegend* leg4 = new TLegend(0.1,0.7,0.48,0.9);
  //leg = new TLegend(0.8,0.8,1,1);


  leg4->AddEntry(h_ER, Form("Total"));
  leg4->AddEntry(GammaK40, Form("K40"));
  leg4->AddEntry(GammaU238, Form("U238"));
  leg4->AddEntry(GammaTh232, Form("Th232"));
  //leg4->AddEntry(H_degree_10, "9.5 degree");

  TCanvas *c30 = new TCanvas();
  c30->SetGrid();
  c30->SetLogx();
  //c30->SetLogy();
  //gStyle->SetLineWidth(2);

  h_ER->SetLineStyle(1);
  h_ER->SetLineWidth(3);
  h_ER->GetXaxis()->SetRange(1,1000);
  h_ER->SetLineColor(2);
  h_ER->Draw("SAME");

  GammaK40->SetLineStyle(1);
  GammaK40->SetLineWidth(1);
  GammaK40->GetXaxis()->SetRange(1,1000);
  GammaK40->SetLineColor(7);
  GammaK40->Draw("SAME");

  GammaU238->SetLineStyle(1);
  GammaU238->SetLineWidth(1);
  GammaU238->GetXaxis()->SetRange(1,1000);
  GammaU238->SetLineColor(3);
  GammaU238->Draw("SAME");

  GammaTh232->SetLineStyle(1);
  GammaTh232->SetLineWidth(1);
  GammaTh232->GetXaxis()->SetRange(1,1000);
  GammaTh232->SetLineColor(6);
  GammaTh232->Draw("SAME");


  h_ER->SetTitle(Form("ER Background Rate"));
  h_ER->SetTitleSize(0.8);
  h_ER->SetXTitle("Energy [keV]");
  h_ER->SetTitleSize(0.05, "X");
  h_ER->SetLabelSize(0.04,"X");
  h_ER->SetYTitle("Events [counts/kg/keV/day]");
  h_ER->SetTitleSize(0.05, "Y");
  h_ER->SetLabelSize(0.04,"Y");

  leg4->Draw("same");



/*
  THStack* hsE5 = new THStack();
  TLegend* leg5 = new TLegend(0.1,0.7,0.48,0.9);
  //leg = new TLegend(0.8,0.8,1,1);


  leg5->AddEntry(h_NR, Form("Total NR"));
  leg5->AddEntry(NeutronTh232, Form("Th232"));
  leg5->AddEntry(NeutronUnat, Form("Unat"));



  TCanvas *c302 = new TCanvas();
  c302->SetGrid();
  c302->SetLogx();
  //c30->SetLogy();
  //gStyle->SetLineWidth(2);

  h_NR->SetLineStyle(1);
  h_NR->SetLineWidth(3);
  h_NR->GetXaxis()->SetRange(0,100);
  h_NR->SetLineColor(4);
  h_NR->Draw("SAME");

  NeutronTh232->SetLineStyle(1);
  NeutronTh232->SetLineWidth(1);
  NeutronTh232->GetXaxis()->SetRange(0,100);
  NeutronTh232->SetLineColor(6);
  NeutronTh232->Draw("SAME");

  NeutronUnat->SetLineStyle(1);
  NeutronUnat->SetLineWidth(1);
  NeutronUnat->GetXaxis()->SetRange(0,100);
  NeutronUnat->SetLineColor(3);
  NeutronUnat->Draw("SAME");

  h_NR->SetTitle(Form("NR Background Rate"));
  h_NR->SetTitleSize(0.8);
  h_NR->SetXTitle("Energy [keV]");
  h_NR->SetTitleSize(0.05, "X");
  h_NR->SetLabelSize(0.04,"X");
  h_NR->SetYTitle("Events [counts/kg/keV/day]");
  h_NR->SetTitleSize(0.05, "Y");
  h_NR->SetLabelSize(0.04,"Y");

  leg5->Draw("same");






  THStack* hsE6 = new THStack();
  TLegend* leg6 = new TLegend(0.1,0.7,0.48,0.9);
  //leg = new TLegend(0.8,0.8,1,1);


  leg6->AddEntry(h_sum, Form("Total ER + NR"));
  leg6->AddEntry(h_ER,Form("ER"));
  leg6->AddEntry(h_NR, Form("NR"));

  TCanvas *c303 = new TCanvas();
  c303->SetGrid();
  c303->SetLogx();
  //c30->SetLogy();
  //gStyle->SetLineWidth(2);

  h_sum->SetLineStyle(1);
  h_sum->SetLineWidth(3);
  h_sum->GetXaxis()->SetRange(0,100);
  h_sum->SetLineColor(1);
  h_sum->Draw("SAME");

  h_ER->SetLineStyle(1);
  h_ER->SetLineWidth(1);
  h_ER->GetXaxis()->SetRange(0,100);
  h_ER->SetLineColor(2);
  h_ER->Draw("SAME");

  h_NR->SetLineStyle(1);
  h_NR->SetLineWidth(1);
  h_NR->GetXaxis()->SetRange(0,100);
  h_NR->SetLineColor(4);
  h_NR->Draw("SAME");

  h_sum->SetTitle(Form("Total Background Rate"));
  h_sum->SetTitleSize(0.8);
  h_sum->SetXTitle("Energy [keV]");
  h_sum->SetTitleSize(0.05, "X");
  h_sum->SetLabelSize(0.04,"X");
  h_sum->SetYTitle("Events [counts/kg/keV/day]");
  h_sum->SetTitleSize(0.05, "Y");
  h_sum->SetLabelSize(0.04,"Y");

  leg6->Draw("same");
*/


}
