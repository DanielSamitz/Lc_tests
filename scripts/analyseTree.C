// TODO:
//  2D histo for hCandidate
//  McGen
//  check if faster with TChain (merge at least trees per file)

#include <TDatabasePDG.h>
#include <TPDGCode.h>
#include <nlohmann/json.hpp>

std::vector<float> cut_mK0s;
std::vector<float> cut_mLambda;
std::vector<float> cut_mGamma;
std::vector<float> cut_ptBach;
std::vector<float> cut_ptV0dau;
std::vector<float> cut_ptV0;
std::vector<float> cut_d0Bach;
std::vector<float> cut_d0V0;
// float cut_Tpc[2];
// float cut_Tof[2];
// bool cut_ReqTof[2];
float cut_Tpc_lowP;
float cut_Tpc_highP;
float cut_Tof_lowP;
float cut_Tof_highP;
bool cut_ReqTof_lowP;
bool cut_ReqTof_highP;
bool doMc;
float cut_Pid_threshold;
float cut_v0Radius;
float cut_v0DaugDCAToPV;
float cut_v0DauDCA;
std::vector<float> cut_decLengthMin;
std::vector<float> cut_decLengthMax;
float cut_v0CosPA;
std::vector<float> ptBins;
int nptBins;

enum CandidateRejection {
  ptCand = 0,
  mK0s,
  mLambda,
  mGamma,
  ptBach,
  ptV0Dau,
  ptV0,
  d0Bach,
  d0V0,
  tpc,
  tof,
  v0Radius,
  v0DauDCAToPV,
  v0DauDCA,
  decLengthMin,
  decLengthMax,
  v0CosPA,
  NCandidateRejection
};

const int nBinsCandidates = 2 + CandidateRejection::NCandidateRejection;
std::string labels[nBinsCandidates];

Float_t fDecayLength, fDecayLengthXY, fPtProng0, fPProng0, fPtProng1,
    fImpactParameter0, fImpactParameter1, fV0Radius, fV0CosPA, fV0MLambda,
    fV0MAntiLambda, fV0MK0Short, fV0MGamma, fV0CtK0Short, fV0CtLambda,
    fDCAV0Daughters, fPtV0Pos, fPtV0Neg, fDCANegToPV, fDCAPosToPV,
    fNSigmaTPCPr0, fNSigmaTOFPr0, fM, fPt, fCPA, fCPAXY, fCt, fEta, fPhi, fY,
    fE;
Char_t fFlagMc;

int set_cut(float &cut, const char *name, nlohmann::json this_json) {
  if (!this_json.contains(name)) {
    printf("ERROR: Could not find %s\n", name);
    return 1;
  }
  cut = std::stod((std::string)this_json[name]);
  return 0;
}

int set_cut(bool &cut, const char *name, nlohmann::json this_json) {
  if (!this_json.contains(name)) {
    printf("ERROR: Could not find %s\n", name);
    return 1;
  }
  cut = false;
  if ((std::string)this_json[name] == "true") {
    cut = true;
  }
  return 0;
}

int set_cut(float cut[2], const char *name, const char *name1,
            const char *name2, nlohmann::json this_json) {
  if (!this_json.contains(name)) {
    printf("ERROR: Could not find %s\n", name);
    return 1;
  }
  nlohmann::json arr = this_json[name];
  if (!arr.contains(name1)) {
    printf("ERROR: Could not find %s\n", name1);
    return 1;
  }
  if (!arr.contains(name2)) {
    printf("ERROR: Could not find %s\n", name2);
    return 1;
  }
  cut[0] = std::stod((std::string)arr[name1]);
  cut[1] = std::stod((std::string)arr[name2]);
  return 0;
}

int set_cut(bool cut[2], const char *name, const char *name1, const char *name2,
            nlohmann::json this_json) {
  if (!this_json.contains(name)) {
    printf("ERROR: Could not find %s\n", name);
    return 1;
  }
  nlohmann::json arr = this_json[name];
  if (!arr.contains(name1)) {
    printf("ERROR: Could not find %s\n", name1);
    return 1;
  }
  if (!arr.contains(name2)) {
    printf("ERROR: Could not find %s\n", name2);
    return 1;
  }
  cut[0] = false;
  if ((std::string)arr[name1] == "true") {
    cut[0] = true;
  }
  cut[1] = false;
  if ((std::string)arr[name2] == "true") {
    cut[1] = true;
  }
  return 0;
}

int set_cut(std::vector<float> &cut, const char *name, const char *name1,
            nlohmann::json this_json) {
  if (!this_json.contains(name)) {
    printf("ERROR: Could not find %s\n", name);
    return 1;
  }
  nlohmann::json arr = this_json[name];
  if (!arr.contains(name1)) {
    printf("ERROR: Could not find %s\n", name1);
    return 1;
  }
  for (auto value : arr[name1])
    cut.push_back(std::stod((std::string)value));
  return 0;
}

int set_cut(std::vector<float> &cut, int which, const char *name,
            const char *name1, nlohmann::json this_json) {
  if (!this_json.contains(name)) {
    printf("ERROR: Could not find %s\n", name);
    return 1;
  }
  nlohmann::json arr = this_json[name];
  if (!arr.contains(name1)) {
    printf("ERROR: Could not find %s\n", name1);
    return 1;
  }
  for (auto bins : arr[name1]) {
    cut.push_back(std::stod((std::string)bins[which]));
  }
  return 0;
}

int read_json(TString config_file) {
  std::ifstream fFile(config_file.Data());
  if (!fFile) {
    printf("ERROR: Could not find config file %s\n", config_file.Data());
    return 1;
  }
  nlohmann::json configfile_toplevel = nlohmann::json::parse(fFile);
  if (!configfile_toplevel.contains("hf-candidate-selector-lc-to-k0s-p")) {
    printf("ERROR: Could not find hf-candidate-selector-lc-to-k0s-p\n");
    return 1;
  }
  nlohmann::json configfile =
      configfile_toplevel["hf-candidate-selector-lc-to-k0s-p"];
  if (set_cut(cut_Pid_threshold, "pPidThreshold", configfile) != 0)
    return 1;
  // if (set_cut(cut_Tpc,"nSigmaTpcMax","lowP","highP",configfile)!=0) return 1;
  // if (set_cut(cut_Tof,"nSigmaTofMax","lowP","highP",configfile)!=0) return 1;
  // if (set_cut(cut_ReqTof,"reqTof","lowP","highP",configfile)!=0) return 1;
  if (set_cut(cut_Tpc_lowP, "nSigmaTpcMaxLowP", configfile) != 0)
    return 1;
  if (set_cut(cut_Tpc_highP, "nSigmaTpcMaxHighP", configfile) != 0)
    return 1;
  if (set_cut(cut_Tof_lowP, "nSigmaTofMaxLowP", configfile) != 0)
    return 1;
  if (set_cut(cut_Tof_highP, "nSigmaTofMaxHighP", configfile) != 0)
    return 1;
  if (set_cut(cut_ReqTof_lowP, "requireTofLowP", configfile) != 0)
    return 1;
  if (set_cut(cut_ReqTof_highP, "requireTofHighP", configfile) != 0)
    return 1;
  if (set_cut(cut_v0Radius, "v0RadiusMin", configfile) != 0)
    return 1;
  if (set_cut(cut_v0DaugDCAToPV, "v0DauDCAToPVMin", configfile) != 0)
    return 1;
  if (set_cut(cut_v0DauDCA, "v0DauDCAMax", configfile) != 0)
    return 1;
  if (set_cut(cut_v0CosPA, "v0CosPAMin", configfile) != 0)
    return 1;
  if (set_cut(doMc, "processMc", configfile) != 0)
    return 1;
  if (set_cut(ptBins, "binsPt", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_mK0s, 0, "cuts", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_mLambda, 1, "cuts", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_mGamma, 2, "cuts", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_ptBach, 3, "cuts", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_ptV0dau, 4, "cuts", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_ptV0, 5, "cuts", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_d0Bach, 6, "cuts", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_d0V0, 7, "cuts", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_decLengthMin, 8, "cuts", "values", configfile) != 0)
    return 1;
  if (set_cut(cut_decLengthMax, 9, "cuts", "values", configfile) != 0)
    return 1;
  nptBins = ptBins.size() - 1;
  return 0;
}

int findBin(float pT) {
  int found_bin = -1;
  for (float bin_limit : ptBins) {
    if (pT >= bin_limit) {
      found_bin++;
    } else {
      return found_bin;
    }
  }
  return -1;
}
double getMassPDG(int pdg) {
  const TParticlePDG *particle = TDatabasePDG::Instance()->GetParticle(pdg);
  return particle->Mass();
}

void select(uint32_t &status) {

  float candPt = fPt;
  int ptBin = findBin(candPt);
  if (ptBin == -1) {
    SETBIT(status, 1 + CandidateRejection::ptCand);
    return;
  }

  if (std::abs(fV0MK0Short - getMassPDG(kK0Short)) > cut_mK0s[ptBin]) {
    SETBIT(status, CandidateRejection::mK0s);
  }

  if ((std::abs(fV0MLambda - getMassPDG(kLambda0)) < cut_mLambda[ptBin]) ||
      (std::abs(fV0MAntiLambda - getMassPDG(kLambda0)) < cut_mLambda[ptBin])) {
    SETBIT(status, CandidateRejection::mLambda);
  }

  if (std::abs(fV0MGamma - getMassPDG(kGamma)) < cut_mGamma[ptBin]) {
    SETBIT(status, CandidateRejection::mGamma);
  }

  if (fPtProng0 < cut_ptBach[ptBin]) {
    SETBIT(status, CandidateRejection::ptBach);
  }

  if ((fPtV0Pos < cut_ptV0dau[ptBin]) || (fPtV0Neg < cut_ptV0dau[ptBin])) {
    SETBIT(status, CandidateRejection::ptV0Dau);
  }

  if (fPtProng1 < cut_ptV0[ptBin]) {
    SETBIT(status, CandidateRejection::ptV0);
  }

  if (std::abs(fImpactParameter0) > cut_d0Bach[ptBin]) {
    SETBIT(status, CandidateRejection::d0Bach);
  }

  if (std::abs(fImpactParameter1) > cut_d0V0[ptBin]) {
    SETBIT(status, CandidateRejection::d0V0);
  }

  if (fV0Radius < cut_v0Radius) {
    SETBIT(status, CandidateRejection::v0Radius);
  }

  if (fV0CosPA < cut_v0CosPA) {
    SETBIT(status, CandidateRejection::v0CosPA);
  }

  if (fDCAV0Daughters > cut_v0DauDCA) {
    SETBIT(status, CandidateRejection::v0DauDCA);
  }

  if ((std::abs(fDCANegToPV) < cut_v0DaugDCAToPV) ||
      (std::abs(fDCAPosToPV) < cut_v0DaugDCAToPV)) {
    SETBIT(status, CandidateRejection::v0DauDCAToPV);
  }

  float nSigmaTpcMax = cut_Tpc_lowP;
  if (fPProng0 >= cut_Pid_threshold) {
    nSigmaTpcMax = cut_Tpc_highP;
  }
  if (std::abs(fNSigmaTPCPr0) > nSigmaTpcMax) {
    SETBIT(status, CandidateRejection::tpc);
  }

  bool requireTof = cut_ReqTof_lowP;
  float nSigmaTofMax = cut_Tof_lowP;
  bool hasTOF = false;
  if (fNSigmaTOFPr0 > -998.) {
    hasTOF = true;
  }
  if (fPProng0 >= cut_Pid_threshold) {
    requireTof = cut_ReqTof_highP;
    nSigmaTofMax = cut_Tof_highP;
  }
  bool tof = true;
  if (!hasTOF) {
    if (requireTof) {
      tof = false;
    }
  } else {
    if (std::abs(fNSigmaTOFPr0) > nSigmaTofMax) {
      tof = false;
    }
  }
  if (!tof) {
    SETBIT(status, CandidateRejection::tof);
  }

  if (fDecayLength < cut_decLengthMin[ptBin]) {
    SETBIT(status, CandidateRejection::decLengthMin);
  }
  if (fDecayLength > cut_decLengthMax[ptBin]) {
    SETBIT(status, CandidateRejection::decLengthMax);
  }
}

void make_axis(TH2F *hist, double xAxis[3]) {
  double bins[nptBins + 1];
  int counter = 0;
  for (auto b : ptBins) {
    bins[counter] = b;
    counter++;
  }
  int nxBins = std::round(xAxis[0]);
  double xBins[nxBins + 1];
  for (int i = 0; i < nxBins + 1; i++) {
    xBins[i] = xAxis[1] + i * (xAxis[2] - xAxis[1]) / nxBins;
  }
  hist->SetBins(nxBins, xBins, nptBins, bins);
}
void make_axis(TH1F *hist, double xAxis[3]) {
  int nxBins = std::round(xAxis[0]);
  double xBins[nxBins + 1];
  for (int i = 0; i < nxBins + 1; i++) {
    xBins[i] = xAxis[1] + i * (xAxis[2] - xAxis[1]) / nxBins;
  }
  hist->SetBins(nxBins, xBins);
}

void make_axis(TH2F *hist, double xAxis[3], double yAxis[3]) {
  int nxBins = std::round(xAxis[0]);
  double xBins[nxBins + 1];
  for (int i = 0; i < nxBins + 1; i++) {
    xBins[i] = xAxis[1] + i * (xAxis[2] - xAxis[1]) / nxBins;
  }
  int nyBins = std::round(yAxis[0]);
  double yBins[nyBins + 1];
  for (int i = 0; i < nyBins + 1; i++) {
    yBins[i] = yAxis[1] + i * (yAxis[2] - yAxis[1]) / nyBins;
  }
  hist->SetBins(nxBins, xBins, nyBins, yBins);
}

int analyseTree(TString input_files, TString config_file,
                TString output_file = "AnalysisResults_tree.root") {

  if (read_json(config_file) != 0) {
    return 1;
  }

  labels[0] = "processed";
  labels[1] = "selected";
  labels[2 + CandidateRejection::ptCand] = "rej. ptCand";
  labels[2 + CandidateRejection::mK0s] = "rej. mK0s";
  labels[2 + CandidateRejection::mLambda] = "rej. mLambda";
  labels[2 + CandidateRejection::mGamma] = "rej. mGamma";
  labels[2 + CandidateRejection::ptBach] = "rej. ptBach";
  labels[2 + CandidateRejection::ptV0Dau] = "rej. ptV0Dau";
  labels[2 + CandidateRejection::ptV0] = "rej. ptV0";
  labels[2 + CandidateRejection::d0Bach] = "rej. d0Bach";
  labels[2 + CandidateRejection::d0V0] = "rej. d0V0";
  labels[2 + CandidateRejection::tpc] = "rej. TPC";
  labels[2 + CandidateRejection::tof] = "rej. TOF";
  labels[2 + CandidateRejection::v0CosPA] = "rej. v0CosPA";
  labels[2 + CandidateRejection::v0DauDCA] = "rej. v0DauDCA";
  labels[2 + CandidateRejection::v0DauDCAToPV] = "rej. v0DauDCAToPV";
  labels[2 + CandidateRejection::v0Radius] = "rej. v0Radius";
  labels[2 + CandidateRejection::decLengthMin] = "rej. decLengthMin";
  labels[2 + CandidateRejection::decLengthMax] = "rej. decLengthMax";

  TH1I *hCandidates = new TH1I("hCandidates", "hCandidates", nBinsCandidates, 0,
                               nBinsCandidates);
  TH1I *hCandidatesRecSig = new TH1I("hCandidatesRecSig", "hCandidatesRecSig",
                                     nBinsCandidates, 0, nBinsCandidates);
  TH1I *hCandidatesRecBg = new TH1I("hCandidatesRecBg", "hCandidatesRecBg",
                                    nBinsCandidates, 0, nBinsCandidates);
  for (int iBin = 0; iBin < nBinsCandidates; iBin++) {
    hCandidates->GetXaxis()->SetBinLabel(iBin + 1, labels[iBin].data());
    hCandidatesRecSig->GetXaxis()->SetBinLabel(iBin + 1, labels[iBin].data());
    hCandidatesRecBg->GetXaxis()->SetBinLabel(iBin + 1, labels[iBin].data());
  }
  const int nHistos = 25;
  TH1F *th1[nHistos];
  TH2F *th2[nHistos];
  TH1F *th1sig[nHistos];
  TH2F *th2sig[nHistos];
  TH1F *th1bg[nHistos];
  TH2F *th2bg[nHistos];
  TString histnames[nHistos] = {
      "hEtaCand",       "hPhiCand",        "hMass",          "hPtBach",
      "hPtV0",          "hd0Bach",         "hd0V0",          "hd0V0pos",
      "hd0V0neg",       "hPtV0pos",        "hPtV0neg",       "hV0CPA",
      "hV0Radius",      "hV0DCADaughters", "hV0MK0Short",    "hV0MLambda",
      "hV0MAntiLambda", "hV0Gamma",        "hCtV0K0Short",   "hCtV0Lambda",
      "hCPACand",       "hCPAxyCand",      "hDecLengthCand", "hDecLengthXYCand",
      "hCtCand"};
  double axisPt[3] = {300, 0.0f, 30.0f};
  double axisEta[3] = {500, -2.0f, 2.0f};
  double axisPhi[3] = {100, 0.f, 6.3f};
  double axisMassCand[3] = {600, 1.98f, 2.58f};
  double axisd0[3] = {500, -0.5f, 0.5f};
  double axisd0V0Daughters[3] = {1000, -5.0f, 5.0f};
  double axisV0CPA[3] = {500, 0.98f, 1.0001f};
  double axisV0Radius[3] = {1000, 0.f, 40.f};
  double axisV0DCADaughters[3] = {200, 0.f, 2.f};
  double axisMassK0Short[3] = {500, 0.4f, 0.6f};
  double axisMassLambda[3] = {500, 1.0f, 1.2f};
  double axisMassGamma[3] = {500, 0.0f, 0.4f};
  double axisProperLifetimeV0[3] = {1000, 0.f, 80.f};
  double axisCPACand[3] = {110, -1.1f, 1.1f};
  double axisDecLength[3] = {200, 0.f, 2.0f};
  double axisProperLifetime[3] = {100, 0.f, 0.2f};

  double *axes[nHistos];
  axes[0] = axisEta;
  axes[1] = axisPhi;
  axes[2] = axisMassCand;
  axes[3] = axisPt;
  axes[4] = axisPt;
  axes[5] = axisd0;
  axes[6] = axisd0;
  axes[7] = axisd0V0Daughters;
  axes[8] = axisd0V0Daughters;
  axes[9] = axisPt;
  axes[10] = axisPt;
  axes[11] = axisV0CPA;
  axes[12] = axisV0Radius;
  axes[13] = axisV0DCADaughters;
  axes[14] = axisMassK0Short;
  axes[15] = axisMassLambda;
  axes[16] = axisMassLambda;
  axes[17] = axisMassGamma;
  axes[18] = axisProperLifetimeV0;
  axes[19] = axisProperLifetimeV0;
  axes[20] = axisCPACand;
  axes[21] = axisCPACand;
  axes[22] = axisDecLength;
  axes[23] = axisDecLength;
  axes[24] = axisProperLifetime;

  for (int i = 0; i < nHistos; i++) {
    TH1F *temp1 = new TH1F(histnames[i], histnames[i], 1, 0, 1);
    TH2F *temp2 = new TH2F(histnames[i] + "VsPtCand", histnames[i] + "VsPtCand",
                           1, 0, 1, 1, 0, 1);
    make_axis(temp1, axes[i]);
    make_axis(temp2, axes[i]);
    th1[i] = temp1;
    th2[i] = temp2;
    if (doMc) {
      TH1F *temp1sig =
          new TH1F(histnames[i] + "RecSig", histnames[i] + "RecSig", 1, 0, 1);
      TH2F *temp2sig =
          new TH2F(histnames[i] + "VsPtCand" + "RecSig",
                   histnames[i] + "VsPtCand" + "RecSig", 1, 0, 1, 1, 0, 1);
      TH1F *temp1bg =
          new TH1F(histnames[i] + "RecBg", histnames[i] + "RecBg", 1, 0, 1);
      TH2F *temp2bg =
          new TH2F(histnames[i] + "VsPtCand" + "RecBg",
                   histnames[i] + "VsPtCand" + "RecBg", 1, 0, 1, 1, 0, 1);
      make_axis(temp1sig, axes[i]);
      make_axis(temp2sig, axes[i]);
      make_axis(temp1bg, axes[i]);
      make_axis(temp2bg, axes[i]);
      th1sig[i] = temp1sig;
      th2sig[i] = temp2sig;
      th1bg[i] = temp1bg;
      th2bg[i] = temp2bg;
    }
  }

  TH1F *hPtCand = new TH1F("hPtCand", "hPtCand", 1, 0, 1);
  make_axis(hPtCand, axisPt);
  TH1F *hPtCandRecSig = new TH1F("hPtCandRecSig", "hPtCandRecSig", 1, 0, 1);
  TH1F *hPtCandRecBg = new TH1F("hPtCandRecBg", "hPtCandRecBg", 1, 0, 1);
  make_axis(hPtCandRecSig, axisPt);
  make_axis(hPtCandRecBg, axisPt);

  double axisNSigma[3] = {100, -6.f, 6.f};
  double axisPidP[3] = {100, 0.f, 10.0f};
  TH1F *hTPCNSigmaPrBach =
      new TH1F("hTPCNSigmaPrBach", "hTPCNSigmaPrBach", 1, 0, 1);
  TH2F *hPBachVsTPCNSigmaPrBach = new TH2F(
      "hPBachVsTPCNSigmaPrBach", "hPBachVsTPCNSigmaPrBach", 1, 0, 1, 1, 0, 1);
  TH1F *hTOFNSigmaPrBach =
      new TH1F("hTOFNSigmaPrBach", "hTOFNSigmaPrBach", 1, 0, 1);
  TH2F *hPBachVsTOFNSigmaPrBach = new TH2F(
      "hPBachVsTOFNSigmaPrBach", "hPBachVsTOFNSigmaPrBach", 1, 0, 1, 1, 0, 1);
  make_axis(hTPCNSigmaPrBach, axisNSigma);
  make_axis(hPBachVsTPCNSigmaPrBach, axisPidP, axisNSigma);
  make_axis(hTOFNSigmaPrBach, axisNSigma);
  make_axis(hPBachVsTOFNSigmaPrBach, axisPidP, axisNSigma);

  TH1F *hTPCNSigmaPrBachRecSig =
      new TH1F("hTPCNSigmaPrBachRecSig", "hTPCNSigmaPrBachRecSig", 1, 0, 1);
  TH2F *hPBachVsTPCNSigmaPrBachRecSig =
      new TH2F("hPBachVsTPCNSigmaPrBachRecSig", "hPBachVsTPCNSigmaPrBachRecSig",
               1, 0, 1, 1, 0, 1);
  TH1F *hTOFNSigmaPrBachRecSig =
      new TH1F("hTOFNSigmaPrBachRecSig", "hTOFNSigmaPrBachRecSig", 1, 0, 1);
  TH2F *hPBachVsTOFNSigmaPrBachRecSig =
      new TH2F("hPBachVsTOFNSigmaPrBachRecSig", "hPBachVsTOFNSigmaPrBachRecSig",
               1, 0, 1, 1, 0, 1);
  make_axis(hTPCNSigmaPrBachRecSig, axisNSigma);
  make_axis(hPBachVsTPCNSigmaPrBachRecSig, axisPidP, axisNSigma);
  make_axis(hTOFNSigmaPrBachRecSig, axisNSigma);
  make_axis(hPBachVsTOFNSigmaPrBachRecSig, axisPidP, axisNSigma);
  TH1F *hTPCNSigmaPrBachRecBg =
      new TH1F("hTPCNSigmaPrBachRecBg", "hTPCNSigmaPrBachRecBg", 1, 0, 1);
  TH2F *hPBachVsTPCNSigmaPrBachRecBg =
      new TH2F("hPBachVsTPCNSigmaPrBachRecBg", "hPBachVsTPCNSigmaPrBachRecBg",
               1, 0, 1, 1, 0, 1);
  TH1F *hTOFNSigmaPrBachRecBg =
      new TH1F("hTOFNSigmaPrBachRecBg", "hTOFNSigmaPrBachRecBg", 1, 0, 1);
  TH2F *hPBachVsTOFNSigmaPrBachRecBg =
      new TH2F("hPBachVsTOFNSigmaPrBachRecBg", "hPBachVsTOFNSigmaPrBachRecBg",
               1, 0, 1, 1, 0, 1);
  make_axis(hTPCNSigmaPrBachRecBg, axisNSigma);
  make_axis(hPBachVsTPCNSigmaPrBachRecBg, axisPidP, axisNSigma);
  make_axis(hTOFNSigmaPrBachRecBg, axisNSigma);
  make_axis(hPBachVsTOFNSigmaPrBachRecBg, axisPidP, axisNSigma);

  std::vector<TString> list_of_filenames;
  if (input_files.EndsWith(".root")) {
    list_of_filenames.push_back(input_files);
  } else {
    std::string line;
    ifstream txtfile;
    txtfile.open(input_files);
    while (getline(txtfile, line)) {
      list_of_filenames.push_back(TString(line));
    }
  }

  for (TString filename : list_of_filenames) {
    printf("Running on file %s\n", filename.Data());

    TFile *file = TFile::Open(filename);
    TIter next(file->GetListOfKeys());
    TKey *key = nullptr;

    while ((key = static_cast<TKey *>(next()))) {
      auto obj = key->ReadObj();
      if (auto nextInDir = dynamic_cast<TDirectory *>(obj)) {
        TString tree_path = nextInDir->GetName();
        tree_path.Append("/O2hfcandcascfull");
        TTree *tree = new TTree("tree", "O2hfcandcascfull");
        file->GetObject(tree_path, tree);
        Int_t nentries = (Int_t)tree->GetEntries();

        tree->SetBranchAddress("fDecayLength", &fDecayLength);
        tree->SetBranchAddress("fDecayLengthXY", &fDecayLengthXY);
        tree->SetBranchAddress("fPtProng0", &fPtProng0);
        tree->SetBranchAddress("fPProng0", &fPProng0);
        tree->SetBranchAddress("fPtProng1", &fPtProng1);
        tree->SetBranchAddress("fImpactParameter0", &fImpactParameter0);
        tree->SetBranchAddress("fImpactParameter1", &fImpactParameter1);
        tree->SetBranchAddress("fV0Radius", &fV0Radius);
        tree->SetBranchAddress("fV0CosPA", &fV0CosPA);
        tree->SetBranchAddress("fV0MLambda", &fV0MLambda);
        tree->SetBranchAddress("fV0MAntiLambda", &fV0MAntiLambda);
        tree->SetBranchAddress("fV0MK0Short", &fV0MK0Short);
        tree->SetBranchAddress("fV0MGamma", &fV0MGamma);
        tree->SetBranchAddress("fDCAV0Daughters", &fDCAV0Daughters);
        tree->SetBranchAddress("fPtV0Pos", &fPtV0Pos);
        tree->SetBranchAddress("fDCAPosToPV", &fDCAPosToPV);
        tree->SetBranchAddress("fPtV0Neg", &fPtV0Neg);
        tree->SetBranchAddress("fDCANegToPV", &fDCANegToPV);
        tree->SetBranchAddress("fNSigmaTPCPr0", &fNSigmaTPCPr0);
        tree->SetBranchAddress("fNSigmaTOFPr0", &fNSigmaTOFPr0);
        tree->SetBranchAddress("fM", &fM);
        tree->SetBranchAddress("fPt", &fPt);
        tree->SetBranchAddress("fCPA", &fCPA);
        tree->SetBranchAddress("fCPAXY", &fCPAXY);
        tree->SetBranchAddress("fCt", &fCt);
        tree->SetBranchAddress("fEta", &fEta);
        tree->SetBranchAddress("fPhi", &fPhi);
        tree->SetBranchAddress("fY", &fY);
        tree->SetBranchAddress("fE", &fE);
        tree->SetBranchAddress("fFlagMc", &fFlagMc);
        tree->SetBranchAddress("fV0CtK0Short", &fV0CtK0Short);
        tree->SetBranchAddress("fV0CtLambda", &fV0CtLambda);

        for (int i = 0; i < nentries; i++) {
          tree->GetEntry(i);

          uint32_t statusLc = 0;
          select(statusLc);

          hCandidates->Fill(0.5);
          if (doMc) {
            if (fFlagMc == 1) {
              hCandidatesRecSig->Fill(0.5);
            } else {
              hCandidatesRecBg->Fill(0.5);
            }
          }
          if (statusLc == 0) {
            hCandidates->Fill(1.5);
            if (doMc) {
              if (fFlagMc == 1) {
                hCandidatesRecSig->Fill(1.5);
              } else {
                hCandidatesRecBg->Fill(1.5);
              }
            }
          }
          int bin = 2;
          for (uint32_t i = 1;
               i < TMath::Power(2, CandidateRejection::NCandidateRejection) - 1;
               i *= 2) {
            if (i & statusLc) {
              hCandidates->Fill(bin + 0.5);
              if (doMc) {
                if (fFlagMc == 1) {
                  hCandidatesRecSig->Fill(bin + 0.5);
                } else {
                  hCandidatesRecBg->Fill(bin + 0.5);
                }
              }
            }
            bin++;
          }

          if (statusLc == 0) {
            double vars[nHistos] = {fEta,
                                    fPhi,
                                    fM,
                                    fPtProng0,
                                    fPtProng1,
                                    fImpactParameter0,
                                    fImpactParameter1,
                                    fDCAPosToPV,
                                    fDCANegToPV,
                                    fPtV0Pos,
                                    fPtV0Neg,
                                    fV0CosPA,
                                    fV0Radius,
                                    fDCAV0Daughters,
                                    fV0MK0Short,
                                    fV0MLambda,
                                    fV0MAntiLambda,
                                    fV0MGamma,
                                    fV0CtK0Short,
                                    fV0CtLambda,
                                    fCPA,
                                    fCPAXY,
                                    fDecayLength,
                                    fDecayLengthXY,
                                    fCt};
            for (int i = 0; i < nHistos; i++) {
              th1[i]->Fill(vars[i]);
              th2[i]->Fill(vars[i], fPt);
            }
            hPtCand->Fill(fPt);
            hTPCNSigmaPrBach->Fill(fNSigmaTPCPr0);
            hPBachVsTPCNSigmaPrBach->Fill(fPProng0, fNSigmaTPCPr0);
            hTOFNSigmaPrBach->Fill(fNSigmaTOFPr0);
            hPBachVsTOFNSigmaPrBach->Fill(fPProng0, fNSigmaTOFPr0);
            if (doMc) {
              if (fFlagMc == 1) {
                for (int i = 0; i < nHistos; i++) {
                  th1sig[i]->Fill(vars[i]);
                  th2sig[i]->Fill(vars[i], fPt);
                }
                hPtCandRecSig->Fill(fPt);
                hTPCNSigmaPrBachRecSig->Fill(fNSigmaTPCPr0);
                hPBachVsTPCNSigmaPrBachRecSig->Fill(fPProng0, fNSigmaTPCPr0);
                hTOFNSigmaPrBachRecSig->Fill(fNSigmaTOFPr0);
                hPBachVsTOFNSigmaPrBachRecSig->Fill(fPProng0, fNSigmaTOFPr0);
              } else {
                for (int i = 0; i < nHistos; i++) {
                  th1bg[i]->Fill(vars[i]);
                  th2bg[i]->Fill(vars[i], fPt);
                }
                hPtCandRecBg->Fill(fPt);
                hTPCNSigmaPrBachRecBg->Fill(fNSigmaTPCPr0);
                hPBachVsTPCNSigmaPrBachRecBg->Fill(fPProng0, fNSigmaTPCPr0);
                hTOFNSigmaPrBachRecBg->Fill(fNSigmaTOFPr0);
                hPBachVsTOFNSigmaPrBachRecBg->Fill(fPProng0, fNSigmaTOFPr0);
              }
            }
          }
        }
        delete tree;
      }
    }
  }
  TFile *outFile = new TFile(output_file, "RECREATE");
  TDirectory *dir1 = outFile->mkdir("hf-candidate-selector-lc-to-k0s-p");
  dir1->cd();
  hCandidates->Write();
  if (doMc) {
    hCandidatesRecSig->Write();
    hCandidatesRecBg->Write();
  }
  outFile->cd();
  dir1->Write();
  TDirectory *dir = outFile->mkdir("hf-task-lc-to-k0s-p");
  dir->cd();
  hPtCand->Write();
  for (int i = 0; i < nHistos; i++) {
    th1[i]->Write();
    th2[i]->Write();
  }
  hTPCNSigmaPrBach->Write();
  hPBachVsTPCNSigmaPrBach->Write();
  hTOFNSigmaPrBach->Write();
  hPBachVsTOFNSigmaPrBach->Write();
  if (doMc) {
    TDirectory *dirMc = dir->mkdir("MC");
    dirMc->cd();
    TDirectory *dirRec = dirMc->mkdir("Rec");
    dirRec->cd();
    hPtCandRecSig->Write();
    hPtCandRecBg->Write();
    for (int i = 0; i < nHistos; i++) {
      th1sig[i]->Write();
      th2sig[i]->Write();
      th1bg[i]->Write();
      th2bg[i]->Write();
    }
    hTPCNSigmaPrBachRecSig->Write();
    hPBachVsTPCNSigmaPrBachRecSig->Write();
    hTPCNSigmaPrBachRecBg->Write();
    hPBachVsTPCNSigmaPrBachRecBg->Write();
    hTOFNSigmaPrBachRecSig->Write();
    hPBachVsTOFNSigmaPrBachRecSig->Write();
    hTOFNSigmaPrBachRecBg->Write();
    hPBachVsTOFNSigmaPrBachRecBg->Write();
    dirMc->cd();
    dirRec->Write();
    dir->cd();
    dirMc->Write();
  }
  outFile->cd();
  dir->Write();
  return 0;
}