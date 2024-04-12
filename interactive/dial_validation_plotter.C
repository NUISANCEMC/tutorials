#include <iostream>
#include <iomanip>
#include <sstream>

double get_true_max(TH1* hist){
  double maxVal = 0;
  for (int x = 0; x < hist->GetNbinsX(); ++x){
    double thisVal = hist->GetBinContent(x+1)+hist->GetBinError(x+1);
    if (thisVal > maxVal) maxVal = thisVal;
  }
  return maxVal;
}

void make_dial_validation_plot(std::string nominal_file_name,
			       std::string negvar_file_name,
			       std::string posvar_file_name,
			       std::string measurement_name,
			       std::string output_plot_name){

  gROOT->SetBatch(1);
  
  // Options to make the histogram prettier
  gStyle->SetLineWidth(3);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetOptFit(0);
  TGaxis::SetMaxDigits(3);
  
  gStyle->SetTextSize(0.06);
  gStyle->SetLabelSize(0.05,"xyzt");
  gStyle->SetTitleSize(0.06,"xyzt");
  
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetNdivisions(505, "XY");
  
  // ROOT sometimes tries to ignore what you tell it...
  gROOT ->ForceStyle();
  
  // Make a canvas (where we will draw the histogram)
  TCanvas *can = new TCanvas("can", "can", 800, 800);
  
  // Open the file
  TFile *nom_file = new TFile(nominal_file_name.c_str());

  // Get the generator prediction histogram
  TH1D *nom_mc_hist = (TH1D*)nom_file->Get((measurement_name+"_MC").c_str());
  nom_mc_hist ->SetDirectory(0);

  // Get the data histogram
  TH1D *data_hist = (TH1D*)nom_file->Get((measurement_name+"_data").c_str());
  data_hist ->SetDirectory(0);

  // This is a bit of a NUISANCE hack, the chisq between data and MC is stored as the MC histogram title
  std::stringstream nom_chisq;
  nom_chisq << std::fixed << std::setprecision(2) << std::stod(nom_mc_hist->GetTitle());

  // Now the negative dial variation
  TFile *neg_file = new TFile(negvar_file_name.c_str());
  TH1D *neg_mc_hist = (TH1D*)neg_file->Get((measurement_name+"_MC").c_str());
  neg_mc_hist ->SetDirectory(0);
  std::stringstream neg_chisq;
  neg_chisq << std::fixed << std::setprecision(2) << std::stod(neg_mc_hist->GetTitle());
  
  // Now the positive dial variation
  TFile *pos_file = new TFile(posvar_file_name.c_str());
  TH1D *pos_mc_hist = (TH1D*)pos_file->Get((measurement_name+"_MC").c_str());
  pos_mc_hist ->SetDirectory(0);
  std::stringstream pos_chisq;
  pos_chisq << std::fixed << std::setprecision(2) << std::stod(pos_mc_hist->GetTitle());
  
  // Presentation is important
  nom_mc_hist ->SetLineWidth(3);
  nom_mc_hist ->SetLineColor(kGray);
  neg_mc_hist ->SetLineWidth(3);
  neg_mc_hist ->SetLineColor(kRed);
  pos_mc_hist ->SetLineWidth(3);
  pos_mc_hist ->SetLineColor(kBlue);
  data_hist   ->SetLineWidth(3);
  data_hist   ->SetLineColor(kBlack);

  // Make a legend
  TLegend *leg = new TLegend(0.2, 0.86, 1.00, 0.99, "", "NDC");

  // By default ROOT legends can be ugly... make it nicer
  leg ->SetShadowColor(0);
  leg ->SetFillColor(0);
  leg ->SetLineWidth(0);
  leg ->SetTextSize(0.04);
  leg ->SetLineColor(kWhite);
  leg ->SetNColumns(2);

  // The information to be conveyed
  leg ->AddEntry(data_hist, "Data", "l");
  leg ->AddEntry(neg_mc_hist, ("-ve (#chi^{2} = "+neg_chisq.str()+")").c_str(), "l");
  leg ->AddEntry(nom_mc_hist, ("Nom. (#chi^{2} = "+nom_chisq.str()+")").c_str(), "l");
  leg ->AddEntry(pos_mc_hist, ("+ve (#chi^{2} = "+pos_chisq.str()+")").c_str(), "l");
  
  // Sort out the maximum value
  double maxVal = get_true_max(data_hist);
  if (nom_mc_hist->GetMaximum() > maxVal) maxVal = nom_mc_hist->GetMaximum();
  if (neg_mc_hist->GetMaximum() > maxVal) maxVal = neg_mc_hist->GetMaximum();
  if (pos_mc_hist->GetMaximum() > maxVal) maxVal = pos_mc_hist->GetMaximum();

	    
  // Note that all style options should be applied to the first histogram we draw
  // Add a small buffer to make it look better
  data_hist ->SetMaximum(maxVal*1.1);
  
  // Now draw everything on the canvas
  can ->cd();
  data_hist ->Draw();
  data_hist ->GetYaxis()->SetTitleOffset(0.9);
  neg_mc_hist ->Draw("HIST SAME");
  pos_mc_hist ->Draw("HIST SAME");
  nom_mc_hist ->Draw("HIST SAME");
  data_hist   ->Draw("SAME");
  leg ->Draw("SAME");

  // Make the borders sensible
  gPad ->SetRightMargin(0.03);
  gPad ->SetTopMargin(0.2);
  gPad ->SetBottomMargin(0.15);
  gPad ->SetLeftMargin(0.15);
  can ->Update();

  // This actually makes the plot
  can ->SaveAs(output_plot_name.c_str());
  return;
};
    

// ROOT CINT expects a function with the same name as the script
// A feature is that subsequent command line arguments are passed to this function
void dial_validation_plotter(){

  std::string nominal_file_name  = "nominal_GENIEv3_example.root";
  std::string negvar_file_name   = "dialvar_negMaCCRES_GENIEv3_example.root";
  std::string posvar_file_name   = "dialvar_posMaCCRES_GENIEv3_example.root";
	
  std::string samples[] = {"MINERvA_CC0pinp_STV_XSec_1Dpmu_nu",
    "MINERvA_CC1pip_XSec_1DTpi_nu_2017",
    "MINERvA_CC1pi0_XSec_1DTpi_nu"};
  
  for (const auto& sample : samples) {
    std::string output_plot_name =  "plots/validation_MaCCRES_"+sample+"_GENIEv3.png";
    make_dial_validation_plot(nominal_file_name,
			      negvar_file_name,
			      posvar_file_name,
			      sample, output_plot_name);
  }
  
  return;
}
    
