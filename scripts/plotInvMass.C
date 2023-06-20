#include <string> 

void makePlots(TH1F** histos, bool plot_all, int rebin){

   for (int i=0; i<10; i++){
      histos[i]->Rebin(rebin);
      //histos[i]->GetXaxis()->SetRangeUser(2.0,2.5);
      histos[i]->GetYaxis()->SetTitle("#entries");
      histos[i]->SetTitle(histos[i]->GetName());
      histos[i]->SetStats(0);
   }
   auto canvas_full= new TCanvas("full");
   histos[0]->Draw();

   auto canvas_full2= new TCanvas("full2");
   histos[9]->Draw();

   if (plot_all){
      auto canvas_pT = new TCanvas("pT");
      TPad* pads[8];
      for(int i=0;i<8;i++){
         int posX = i % 2;
         int posY = i/2;
         pads[i] = new TPad("pad","pad",0.0+0.5*posX,1.0-(1./4.)-(1./4.)*posY,0.5+0.5*posX,1.0-(1./4.)*posY);
         pads[i]->Draw();
      }
      for (int i=0;i<8;i++){
         pads[i]->cd();
         histos[i+1]->Draw();
      }
   }
}

void plotInvMass(int rebin = 1, TString in_file = "~/alice/MyStuff/Lc_tests/AnalysisResults.root"){

   TFile* file = TFile::Open(in_file.Data());

   TH2F* histo = (TH2F*)file->GetDirectory("hf-task-lc-to-k0s-p")->Get("hMassVsPtCand");
   
   TH1F* mass[10];

   mass[0] = (TH1F*)histo->ProjectionX("cascade candidates",0,-1);
   mass[1]=(TH1F*)histo->ProjectionX("1 GeV/#it{c} < #it{p}_{T} < 2 GeV/#it{c}",1,1);
   mass[2]=(TH1F*)histo->ProjectionX("2 GeV/#it{c} < #it{p}_{T} < 3 GeV/#it{c}",2,2);
   mass[3]=(TH1F*)histo->ProjectionX("3 GeV/#it{c} < #it{p}_{T} < 4 GeV/#it{c}",3,3);
   mass[4]=(TH1F*)histo->ProjectionX("4 GeV/#it{c} < #it{p}_{T} < 5 GeV/#it{c}",4,4);
   mass[5]=(TH1F*)histo->ProjectionX("5 GeV/#it{c} < #it{p}_{T} < 6 GeV/#it{c}",5,5);
   mass[6]=(TH1F*)histo->ProjectionX("6 GeV/#it{c} < #it{p}_{T} < 8 GeV/#it{c}",6,6);
   mass[7]=(TH1F*)histo->ProjectionX("8 GeV/#it{c} < #it{p}_{T} < 12 GeV/#it{c}",7,7);
   mass[8]=(TH1F*)histo->ProjectionX("12 GeV/#it{c} < #it{p}_{T} < 24 GeV/#it{c}",8,8);
   mass[9]=(TH1F*)histo->ProjectionX("3 GeV/#it{c} < #it{p}_{T} < 12 GeV/#it{c}",3,7);

 
   TH1F* meanFix = new TH1F("hRawYieldsMean","hRawYieldsMean",9,0,1);
   for (int i=0; i<9; i++){
      meanFix->SetBinContent(i+1,2.286);
   }
   
   

   TFile* myFile = new TFile("AnalysisResults_test.root","RECREATE");
   meanFix->Write();
   mass[0]->Write("invMass_Bin0");
   mass[1]->Write("invMass_Bin1");
   mass[2]->Write("invMass_Bin2");
   mass[3]->Write("invMass_Bin3");
   mass[4]->Write("invMass_Bin4");
   mass[5]->Write("invMass_Bin5");
   mass[6]->Write("invMass_Bin6");
   mass[7]->Write("invMass_Bin7");
   mass[8]->Write("invMass_Bin8");
   mass[9]->Write("invMass_Bin3To7");


    makePlots(mass,true,rebin);
}