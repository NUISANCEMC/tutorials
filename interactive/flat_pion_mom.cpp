void flat_pion_mom(std::string filename) {
  TFile *file = new TFile(filename.c_str(), "open");
  TTree *tree = (TTree*)file->Get("FlatTree_VARS");
  int nentries = tree->GetEntriesFast();

  const int nmax = 100;
  float E[nmax];
  float px[nmax];
  float py[nmax];
  float pz[nmax];
  int pdg[nmax];

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

  tree->SetBranchStatus("fScaleFactor", true);
  double scale = tree->GetMaximum("fScaleFactor");
  tree->SetBranchStatus("fScaleFactor", false);

  int nModes = tree->GetMaximum("Mode");

  TH1D **ppi = new TH1D*[nModes];
  for (int i = 0; i < nModes; ++i) {
    ppi[i] = new TH1D(Form("ppi_%i", i), Form("ppi_%i;p_{#pi} (GeV/c);d#sigma/dp_{#pi} (cm^{2}/(GeV/c)/nucleon)", i), 50, 0, 2);
  }
  TH1D *ppit = new TH1D("ppit", "ppit;p_{#pi} (GeV/c);d#sigma/dp_{#pi} (cm^{2}/(GeV/c)/nucleon)", 50, 0, 2);

  for (int i = 0; i < nentries; ++i) {
    tree->GetEntry(i);

    if (i % (nentries/5) == 0) std::cout << i << "/" << nentries << " (" << (double(i)/nentries)*100 << "%)" << std::endl;

    int nPions = 0;
    int PiIndex = 0;
    int nMuons = 0;

    for (int j = 0; j < nfsp; ++j) {
      if (pdg[j] == 211) {
        nPions++;
        PiIndex = j;
      }
      if (pdg[j] == 13) nMuons++;
    }

    if (nPions != 1) continue;
    if (nMuons != 1) continue;

    double ptot = sqrt(px[PiIndex]*px[PiIndex]+py[PiIndex]*py[PiIndex]+pz[PiIndex]*pz[PiIndex]);
    ppi[Mode]->Fill(ptot);
    ppit->Fill(ptot);
  }

  bool drawn = false;
  double max = 0;
  int first = 0;
  int nhists = 0;
  for (int i = 0; i < nModes; ++i) {
    if (ppi[i]->Integral() == 0) continue;
    ppi[i]->Scale(scale, "width");
    max = ppi[i]->GetMaximum() > max ? ppi[i]->GetMaximum() : max;
    if (!drawn) {
      first = i;
      ppi[i]->Draw("hist");
      drawn = true;
    }
    else ppi[i]->Draw("hist,same");
    ppi[i]->SetLineColor(nhists+1);
    ppi[i]->SetMarkerSize(0);
    if (nhists == 9) ppi[i]->SetLineColor(11);
    nhists++;
  }
  ppit->Scale(scale, "width");
  ppi[first]->GetYaxis()->SetRangeUser(0, ppit->GetMaximum()*1.2);
  ppit->Draw("same,hist");
  ppit->SetLineColor(kBlack);
  ppit->SetLineWidth(3);
  ppit->SetLineStyle(kDashed);
  ppit->SetMarkerSize(0);
}
