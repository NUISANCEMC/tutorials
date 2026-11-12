// Feed the filename containing the NUISANCE flat-tree, and the variable in the tree to draw
void flat_bymode(std::string filename, std::string variable="Enu_true") {

  std::string binning = "50, 0, 10";
  if (variable == "Enu_true") binning = "50, 0, 10";
  else if (variable == "W") binning = "50, 0.8, 4";

  // Open the ROOT file
  TFile *file = new TFile(filename.c_str(), "open");

  // Open the TTree which is our flat-tree
  TTree *tree = (TTree*)file->Get("FlatTree_VARS");

  // Set the number of modes that we are going to loop over to produce plots which are split by true interaction mode
  // This is equivalent to all CC modes
  int nModes = tree->GetMaximum("Mode")+1;
  // The scale factor we use to convert "number of events in the tree" to a differential cross section
  double scale = tree->GetMaximum("fScaleFactor");

  // An array of pointers to TH1Ds; this is the object that we will draw in to
  // (A more careful NUISANCEr would of course use unique pointers or a vector here)
  TH1D **w = new TH1D*[nModes];

  // The TCanvas we'll be drawing into
  TCanvas *canv = new TCanvas("canv", "canv", 1024, 1024);

  // Loop over each of the mode
  for (int i = 0; i < nModes; ++i) {
    // We draw the variables "W" from the tree into ">>" the "h%i"th object, where i is the Nth mode. 
    // We use 50 bins between 0.5 and 3 GeV/c2 "(50,0.5,3), and make a selection that the true interaction mode is the number i "(Form("Mode==%i", i))"
    tree->Draw(Form("%s>>h%i(%s)", variable.c_str(), i, binning.c_str()), Form("Weight*(Mode==%i)", i));
    // Retrieve the drawn object
    w[i] = (TH1D*)gDirectory->Get(Form("h%i", i));
    // Set a useful title
    w[i]->SetTitle(Form("%s (Mode==%i);%s;d#sigma/d%s (cm^{2}/nucleon)", variable.c_str(), i, variable.c_str(), variable.c_str()));
  }

  // Draw the W distribution without any selection on a particular mode; include all modes below nModes (this is all CC)
  tree->Draw(Form("%s>>h00(%s)", variable.c_str(), binning.c_str()), Form("Weight*(Mode<%i)", nModes));
  TH1D *h0 = (TH1D*)gDirectory->Get("h00");
  // Scale the histogram from number of events to a differential cross section in the x-axis variable (in this case, W)
  h0->Scale(scale, "width");

  // A variable to keep track of if we've drawn a histogram yet or not
  bool drawn = false;
  int first = 0;
  int nhists = 0;
  // Loop over the modes, with the intent to draw them
  for (int i = 0; i < nModes; ++i) {
    // Only draw histograms that are filled
    if (w[i]->Integral() == 0) continue;
    // Scale to a differential cross section
    w[i]->Scale(scale, "width");
    // Register that we have now drawn a histogram, and remember which mode it was
    if (!drawn) {
      first = i;
      w[i]->Draw("hist");
      drawn = true;
    }
    else w[i]->Draw("hist,same");
    w[i]->SetLineColor(nhists+1);
    w[i]->SetMarkerSize(0);
    w[i]->SetLineWidth(2);
    if (nhists == 9) w[i]->SetLineColor(11);
    nhists++;
  }

  h0->Draw("same,hist");
  h0->SetLineColor(kBlack);
  h0->SetLineWidth(3);
  h0->SetLineStyle(kDashed);
  w[first]->GetYaxis()->SetRangeUser(1E-40, h0->GetMaximum()*1.2);

  TLegend *leg = canv->BuildLegend();
  leg->SetTextSize(0.02);
  leg->SetX1(0.7);
  leg->SetX2(0.9);
  leg->SetY1(0.1);
  leg->SetY1(0.9);
}
