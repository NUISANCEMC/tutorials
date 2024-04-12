void flat_calo_bias(std::string filename) {

  TFile *file = new TFile(filename.c_str(), "open");
  TTree *tree = (TTree*)file->Get("FlatTree_VARS");

  const int nmax = 100;
  float E[nmax];
  float px[nmax];
  float py[nmax];
  float pz[nmax];
  int pdg[nmax];

  float Enu_QE;
  float Enu_true;
  float CosLep;
  float ELep;
  int Mode;
  int nfsp;

  tree->SetBranchStatus("*", false);
  tree->SetBranchStatus("E", true);
  tree->SetBranchAddress("E", &E);
  tree->SetBranchStatus("px", true);
  tree->SetBranchAddress("px", &px);
  tree->SetBranchStatus("py", true);
  tree->SetBranchAddress("py", &py);
  tree->SetBranchStatus("pz", true);
  tree->SetBranchAddress("pz", &pz);
  tree->SetBranchStatus("pdg", true);
  tree->SetBranchAddress("pdg", &pdg);
  tree->SetBranchStatus("Mode", true);
  tree->SetBranchAddress("Mode", &Mode);
  tree->SetBranchStatus("nfsp", true);
  tree->SetBranchAddress("nfsp", &nfsp);
  tree->SetBranchStatus("Enu_QE", true);
  tree->SetBranchAddress("Enu_QE", &Enu_QE);
  tree->SetBranchStatus("Enu_true", true);
  tree->SetBranchAddress("Enu_true", &Enu_true);

  tree->SetBranchStatus("CosLep", true);
  tree->SetBranchAddress("CosLep", &CosLep);
  tree->SetBranchStatus("ELep", true);
  tree->SetBranchAddress("ELep", &ELep);

  tree->SetBranchStatus("fScaleFactor", true);
  double scale = tree->GetMaximum("fScaleFactor");
  tree->SetBranchStatus("fScaleFactor", false);

  int nentries = tree->GetEntriesFast();
  int nModes = tree->GetMaximum("Mode")+1;

  TH1D **hist = new TH1D*[nModes];
  for (int i = 0; i < nModes; ++i) {
    hist[i] = new TH1D(Form("hist_%i", i), Form("hist_%i;#frac{E_{rec}-E_{true}}{E_{true}};d#sigma", i), 50, -1, 0.2);
  }

  for (int i = 0; i < nentries; ++i) {
    tree->GetEntry(i);

    if (i % (nentries/5) == 0) std::cout << i << "/" << nentries << " (" << (double(i)/nentries)*100 << "%)" << std::endl;

    int nMuons = 0;
    int nPions = 0;
    int nPi0 = 0;
    double Eavail = 0;
    // Loop over final state particles
    for (int j = 0; j < nfsp; ++j) {
      double p = sqrt(px[j]*px[j]+py[j]*py[j]+pz[j]*pz[j]);
      // For protons or charged pions, add KE to E avail
      //if (abs(pdg[j]) == 211 || pdg[j] == 2212) {
      //if (pdg[j] == 2212) {
      if ((pdg[j] == 2212 && p > 0.4) || (pdg[j] == 2112 && p > 0.5)) {
        double p2 = px[j]*px[j]+py[j]*py[j]+pz[j]*pz[j];
        double mass = sqrt(E[j]*E[j]-p2);
        Eavail += E[j]-mass;
      }
      // For photons and pi0, add all the energy
      //if (pdg[j] == 211 || pdg[j] == 111 || pdg[j] == 22 || pdg[j] == 130 || pdg[j] == 310 || pdg[j] == 311 || pdg[j] == 321) Eavail += E[j];
      else if ((pdg[j] == 211 || pdg[j] == 111) && p > 0.2) Eavail += E[j];
      else if (pdg[j] == 22) Eavail += E[j];
      else if (pdg[j] == 13) {
        nMuons++;
        Eavail += E[j];
      }
    }

    if (nMuons != 1) continue;
    double bias = (Eavail-Enu_true)/Enu_true;
    hist[Mode]->Fill(bias);
  }

  bool drawn = false;
  int nhists = 0;
  double max = 0;
  int drawhist = -1;
  for (int i = 0; i < nModes; ++i) {
    if (hist[i]->Integral() == 0) continue;
    if (!drawn) {
      hist[i]->Draw("hist");
      drawn = true;
      drawhist = i;
    } else {
      hist[i]->Draw("same,hist");
    }
    hist[i]->Scale(scale);
    max = hist[i]->GetMaximum() > max ? hist[i]->GetMaximum() : max;
    hist[i]->SetLineColor(nhists+1);
    hist[i]->SetMarkerSize(0);
    if (nhists == 9) hist[i]->SetLineColor(11);
    nhists++;
  }
  hist[drawhist]->GetYaxis()->SetRangeUser(0, max*1.2);

}
