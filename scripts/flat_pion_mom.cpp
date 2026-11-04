void flat_pion_mom(std::string filename) {
  // Open the file and get the TTree
  TFile *file = new TFile(filename.c_str(), "open");
  TTree *tree = (TTree*)file->Get("FlatTree_VARS");
  int nentries = tree->GetEntriesFast();

  // Set up the maximum number of particles, largely to allocate the arrays
  const int nmax = 100;
  // The arrays that will be holding the variables
  // The particle energy
  float E[nmax];
  // The particle momentum in x
  float px[nmax];
  // The particle momentum in y
  float py[nmax];
  // The particle momentum in z
  float pz[nmax];
  // The particle's PDG code
  int pdg[nmax];

  // The interaction mode
  int Mode;
  // The number of final state particles in an event
  int nfsp;

  // Turn off all branches and selectively turn on those that we use
  // This makes reading the TTree significantly faster
  tree->SetBranchStatus("*", false);
  tree->SetBranchStatus("E", true);
  tree->SetBranchAddress("E", E);
  tree->SetBranchStatus("px", true);
  tree->SetBranchAddress("px", px);
  tree->SetBranchStatus("py", true);
  tree->SetBranchAddress("py", py);
  tree->SetBranchStatus("pz", true);
  tree->SetBranchAddress("pz", pz);
  tree->SetBranchStatus("pdg", true);
  tree->SetBranchAddress("pdg", pdg);
  tree->SetBranchStatus("Mode", true);
  tree->SetBranchAddress("Mode", &Mode);
  tree->SetBranchStatus("nfsp", true);
  tree->SetBranchAddress("nfsp", &nfsp);

  // Get the scaling to a cross section
  tree->SetBranchStatus("fScaleFactor", true);
  double scale = tree->GetMaximum("fScaleFactor");
  tree->SetBranchStatus("fScaleFactor", false);

  // The the number of modes that the file contains
  int nModes = tree->GetMaximum("Mode");

  // The array of histogram pointers: one per mode
  TH1D **ppi = new TH1D*[nModes];

  // Loop over the interaction modes and make the histogram objects
  for (int i = 0; i < nModes; ++i) {
    ppi[i] = new TH1D(Form("ppi_%i", i), Form("ppi_%i;p_{#pi} (GeV/c);d#sigma/dp_{#pi} (cm^{2}/(GeV/c)/nucleon)", i), 50, 0, 2);
  }
  // The histogram containing the sum over all modes
  TH1D *ppit = new TH1D("ppit", "ppit;p_{#pi} (GeV/c);d#sigma/dp_{#pi} (cm^{2}/(GeV/c)/nucleon)", 50, 0, 2);

  // Loop over all the entries in the TTree
  for (int i = 0; i < nentries; ++i) {
    // Get the ith entry; this loads up the E, px, py, pz etc arrays that we set in SetBranchAddress
    tree->GetEntry(i);

    // Output progress to screen
    if (i % (nentries/5) == 0) std::cout << i << "/" << nentries << " (" << (double(i)/nentries)*100 << "%)" << std::endl;

    // How many pions
    int nPions = 0;
    // What index in the particle stack is the pion
    int PiIndex = 0;
    // How many muons
    int nMuons = 0;

    // Loop over the final state particles in the event
    for (int j = 0; j < nfsp; ++j) {
      // PDG code 211 means positive pion
      if (pdg[j] == 211) {
        nPions++;
        PiIndex = j;
      }
      // PDG code 13 means negative muon
      if (pdg[j] == 13) nMuons++;
    }

    // Look for events with 1 positive muon and one positive pion
    // And don't be more exclusive than that 
    // (e.g. could check proton multiplicity, or make sure there are no neutral or negative pions, or positive muons)
    if (nPions != 1) continue;
    if (nMuons != 1) continue;

    // Calculate the magnitude of the momentum
    double ptot = sqrt(px[PiIndex]*px[PiIndex]+py[PiIndex]*py[PiIndex]+pz[PiIndex]*pz[PiIndex]);

    // Fill the histograms
    ppi[Mode]->Fill(ptot);
    ppit->Fill(ptot);
  }

  // Onwards is just drawing and plot cosmetics
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
