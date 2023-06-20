const int nPtBins = 8;
const double ptBins[nPtBins+1]  = {1.,2.,3.,4.,5.,6.,8.,12.,24.};

const int nPtBins2 = 6;
const double ptBins2[nPtBins2+1]  = {1.,2.,4.,6.,8.,12.,24.};

struct parameter {
   double value[nPtBins];
   double error[nPtBins];
};

struct oneRun {
   const char* name;
   const int color;
   const parameter mean;
   const parameter width;
   const parameter signal;
   const parameter background;
   const int nEvents;
   TH1F* hMean;
   TH1F* hWidth;
   TH1F* hSignal;
   TH1F* hBackground;
   TH1F* hSignalToBackground;
   TH1F* hSignificance;
};

struct parameter2 {
   double value[nPtBins2];
   double error[nPtBins2];
};

struct oneRun2 {
   const char* name;
   const int color;
   const parameter2 mean;
   const parameter2 width;
   const parameter2 signal;
   const parameter2 background;
   const parameter2 significance;
   const int nEvents;
   TH1F* hMean;
   TH1F* hWidth;
   TH1F* hSignal;
   TH1F* hBackground;
   TH1F* hSignalToBackground;
   TH1F* hSignificance;
};




void fitParameters(){

   std::vector<oneRun> runs;
   std::vector<oneRun2> runs2;

   oneRun LHC22o_pass4_medium = {
      .name = "LHC22o_pass4",
      .color = kRed,
      .mean.value = {2.275, 2.280, 2.281, 2.285, 2.282, 2.288, 2.287, 2.298},
      .mean.error = {0.002, 0.001, 0.001, 0.002, 0.003, 0.003, 0.005, 0.022},
      .width.value = {0.015, 0.012, 0.012, 0.015, 0.018, 0.016, 0.017, 0.022},
      .width.error = {0.012, 0.001, 0.001, 0.001, 0.002, 0.002, 0.004, 0.018},
      .signal.value = {9599, 9629, 7466, 5454, 2981, 1635, 717, 97},
      .signal.error = {16211, 1044, 663, 553, 374, 239, 178, 121},
      .background.value = {2668758, 1086680, 562941, 304915, 147672, 70759, 30068, 9821},
      .background.error = {3579, 517, 364, 312, 242, 146, 103, 74},
      .hMean = new TH1F, 
      .hWidth = new TH1F,
      .hSignalToBackground = new TH1F};
   runs.push_back(LHC22o_pass4_medium);

   oneRun Run2_5TeV = {
      .name = "Run2_5TeV",
      .color = kBlue,
      .mean.value = {2.288, 2.290, 2.292, 2.288, 2.291, 2.292, 2.285, 2.289},
      .mean.error = {0.001, 0.002, 0.001, 0.002, 0.002, 0.002, 0.003, 0.010},
      .width.value = {0.008, 0.008, 0.006, 0.013, 0.009, 0.008, 0.009, 0.027},
      .width.error = {0.002, 0.002, 0.002, 0.003, 0.002, 0.003, 0.002, 0.010},
      .signal.value = {2825, 1463, 1040, 969, 440, 241, 162, 113},
      .signal.error = {546, 347, 285, 166, 91, 72, 44, 55},
      .background.value = {2825/0.0147, 1463/0.0192, 1040/0.0387, 969/0.0520, 440/0.0746, 241/0.0696, 162/0.1082, 113/0.1010 },
      .background.error = {0, 0, 0, 0, 0, 0, 0, 0},
      .hMean = new TH1F, 
      .hWidth = new TH1F,
      .hSignalToBackground = new TH1F};
   runs.push_back(Run2_5TeV);

   oneRun2 Run2_13TeV = {
      .name = "Run2_13TeV",
      .color = 8,
      .mean.value = {2.2883, 2.2882, 2.2893, 2.2889, 2.2869, 2.2909},
      .mean.error = {0.0023, 0.0011, 0.0011, 0.0013, 0.0021, 0.0028},
      .width.value = {0.00745, 0.00775, 0.00976, 0.01010, 0.01180, 0.01540},
      .width.error = {0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001},
      .signal.value = {7650, 11707, 5460, 1620, 659, 238},
      .signal.error = {996, 665, 339, 125, 77, 45},
      .background.value = {942762, 656867, 138365, 17363, 5229, 1800},
      .background.error = {387, 331, 176, 63, 38, 29},
      .hMean = new TH1F, 
      .hWidth = new TH1F,
      .hSignalToBackground = new TH1F};
   runs2.push_back(Run2_13TeV);

   for (auto& thisRun : runs){
      thisRun.hMean->SetName(thisRun.name);
      thisRun.hMean->SetTitle(thisRun.name);
      thisRun.hMean->SetBins(nPtBins, ptBins);
      thisRun.hMean->GetYaxis()->SetRangeUser(2.27,2.31);
      thisRun.hMean->SetLineColor(thisRun.color);
      thisRun.hMean->SetStats(0);
      thisRun.hMean->SetMarkerStyle(4);
      thisRun.hMean->SetMarkerColor(thisRun.color);
      thisRun.hMean->GetXaxis()->SetTitle("#it{p}_{T} (GeV/#it{c})");
      thisRun.hMean->GetYaxis()->SetTitle("fitted mean (GeV/#it{c}^{2})");

      thisRun.hWidth->SetName(thisRun.name);
      thisRun.hWidth->SetTitle(thisRun.name);
      thisRun.hWidth->SetBins(nPtBins, ptBins);
      thisRun.hWidth->GetYaxis()->SetRangeUser(0.0,0.045*1000);
      thisRun.hWidth->SetLineColor(thisRun.color);
      thisRun.hWidth->SetStats(0);
      thisRun.hWidth->SetMarkerStyle(4);
      thisRun.hWidth->SetMarkerColor(thisRun.color);
      thisRun.hWidth->GetXaxis()->SetTitle("#it{p}_{T} (GeV/#it{c})");
      thisRun.hWidth->GetYaxis()->SetTitle("fitted width (MeV/#it{c}^{2})");

      thisRun.hSignalToBackground->SetName(thisRun.name);
      thisRun.hSignalToBackground->SetTitle(thisRun.name);
      thisRun.hSignalToBackground->SetBins(nPtBins, ptBins);
      thisRun.hSignalToBackground->GetYaxis()->SetRangeUser(0.0,0.15);
      thisRun.hSignalToBackground->SetLineColor(thisRun.color);
      thisRun.hSignalToBackground->SetStats(0);
      thisRun.hSignalToBackground->SetMarkerStyle(4);
      thisRun.hSignalToBackground->SetMarkerColor(thisRun.color);
      thisRun.hSignalToBackground->GetXaxis()->SetTitle("#it{p}_{T} (GeV/#it{c})");
      thisRun.hSignalToBackground->GetYaxis()->SetTitle("S/B");



      for (int i=0; i<nPtBins; i++){
         thisRun.hMean->SetBinContent(i+1,thisRun.mean.value[i]);
         thisRun.hMean->SetBinError(i+1,thisRun.mean.error[i]);

         thisRun.hWidth->SetBinContent(i+1,thisRun.width.value[i]*1000);
         thisRun.hWidth->SetBinError(i+1,thisRun.width.error[i]*1000);

         thisRun.hSignalToBackground->SetBinContent(i+1,thisRun.signal.value[i]/thisRun.background.value[i]);
         thisRun.hSignalToBackground->SetBinError(i+1,sqrt(pow(thisRun.signal.error[i]/thisRun.background.value[i],2)+pow(thisRun.background.error[i]*thisRun.signal.value[i]/(thisRun.background.value[i]*thisRun.background.value[i]),2)));
      }
   }

   for (auto& thisRun : runs2){
      thisRun.hMean->SetName(thisRun.name);
      thisRun.hMean->SetTitle(thisRun.name);
      thisRun.hMean->SetBins(nPtBins2, ptBins2);
      //thisRun.hMean->GetYaxis()->SetRangeUser(2.26,2.30);
      thisRun.hMean->SetLineColor(thisRun.color);
      thisRun.hMean->SetStats(0);
      thisRun.hMean->SetMarkerStyle(4);
      thisRun.hMean->SetMarkerColor(thisRun.color);

      thisRun.hWidth->SetName(thisRun.name);
      thisRun.hWidth->SetTitle(thisRun.name);
      thisRun.hWidth->SetBins(nPtBins2, ptBins2);
      //thisRun.hWidth->GetYaxis()->SetRangeUser(2.26,2.30);
      thisRun.hWidth->SetLineColor(thisRun.color);
      thisRun.hWidth->SetStats(0);
      thisRun.hWidth->SetMarkerStyle(4);
      thisRun.hWidth->SetMarkerColor(thisRun.color);

      thisRun.hSignalToBackground->SetName(thisRun.name);
      thisRun.hSignalToBackground->SetTitle(thisRun.name);
      thisRun.hSignalToBackground->SetBins(nPtBins2, ptBins2);
      //thisRun.hSignalToBackground->GetYaxis()->SetRangeUser(0.0,0.15);
      thisRun.hSignalToBackground->SetLineColor(thisRun.color);
      thisRun.hSignalToBackground->SetStats(0);
      thisRun.hSignalToBackground->SetMarkerStyle(4);
      thisRun.hSignalToBackground->SetMarkerColor(thisRun.color);


      for (int i=0; i<nPtBins2; i++){
         thisRun.hMean->SetBinContent(i+1,thisRun.mean.value[i]);
         thisRun.hMean->SetBinError(i+1,thisRun.mean.error[i]);

         thisRun.hWidth->SetBinContent(i+1,thisRun.width.value[i]*1000);
         thisRun.hWidth->SetBinError(i+1,thisRun.width.error[i]*1000);

         thisRun.hSignalToBackground->SetBinContent(i+1,thisRun.signal.value[i]/thisRun.background.value[i]);
         thisRun.hSignalToBackground->SetBinError(i+1,sqrt(pow(thisRun.signal.error[i]/thisRun.background.value[i],2)+pow(thisRun.background.error[i]*thisRun.signal.value[i]/(thisRun.background.value[i]*thisRun.background.value[i]),2)));
      }
   }
   

   //gStyle->SetOptTitle(0);

   TCanvas* canvas_mean = new TCanvas("mean","mean");
   for (auto& run : runs){
      run.hMean->Draw("same P");
   }
   for (auto& run : runs2){
      run.hMean->Draw("same P");
   }
   TLegend *l_mean = canvas_mean->BuildLegend();
   l_mean->SetX1(0.75);
   l_mean->SetX2(0.9);
   l_mean->SetY1(0.75);
   l_mean->SetY2(0.9);
   auto pdg = new TF1("pdg","2.28646",0,30);
   pdg->SetLineColor(kBlack);
   pdg->SetLineStyle(2);
   pdg->SetLineWidth(2);
   pdg->Draw("same");
   canvas_mean->Update();
   runs[0].hMean->SetTitle("Mean");

   TCanvas* canvas_width = new TCanvas("width", "width");
   for (auto& run : runs){
      run.hWidth->Draw("same P");
   }
   for (auto& run : runs2){
      run.hWidth->Draw("same P");
   }
   TLegend *l_width = canvas_width->BuildLegend();
   l_width->SetX1(0.75);
   l_width->SetX2(0.9);
   l_width->SetY1(0.75);
   l_width->SetY2(0.9);
   canvas_width->Update();
   runs[0].hWidth->SetTitle("Width");

   TCanvas* canvas_sigtobg = new TCanvas("sig/bg", "sig/bg");;
   canvas_sigtobg->Update();
   for (auto& run : runs){
      run.hSignalToBackground->Draw("same P");
   }
   for (auto& run : runs2){
      run.hSignalToBackground->Draw("same P");
   }
   TLegend *l_sigtobg = canvas_sigtobg->BuildLegend();
   l_sigtobg->SetX1(0.75);
   l_sigtobg->SetX2(0.9);
   l_sigtobg->SetY1(0.75);
   l_sigtobg->SetY2(0.9);
   canvas_sigtobg->Update();
   runs[0].hSignalToBackground->SetTitle("Signal/Background");



}