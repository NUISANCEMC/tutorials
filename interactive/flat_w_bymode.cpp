void flat_w_bymode(std::string filename) {
  TFile *file = new TFile(filename.c_str(), "open");
  TTree *tree = (TTree*)file->Get("FlatTree_VARS");
  int nModes = 30;
  double scale = tree->GetMaximum("fScaleFactor");

  TH1D **w = new TH1D*[nModes];
  for (int i = 0; i < nModes; ++i) {
    tree->Draw(Form("W>>h%i(50,0.5,3)", i), Form("Mode==%i", i));
    w[i] = (TH1D*)gDirectory->Get(Form("h%i", i));
    w[i]->SetTitle(Form("W (Mode==%i);W (GeV/c^{2});d#sigma/dW (cm^{2}/(GeV/c^{2})/nucleon)", i));
  }
  tree->Draw("W>>h00(50, 0.5, 3)", "Mode<30");
  TH1D *h0 = (TH1D*)gDirectory->Get("h00");
  h0->Scale(scale, "width");

  bool drawn = false;
  double max = 0;
  int first = 0;
  int nhists = 0;
  for (int i = 0; i < nModes; ++i) {
    if (w[i]->Integral() == 0) continue;
    w[i]->Scale(scale, "width");
    max = w[i]->GetMaximum() > max ? w[i]->GetMaximum() : max;
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
  w[first]->GetYaxis()->SetRangeUser(1E-40, 30E-39);
}
