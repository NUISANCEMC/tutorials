from ROOT import gROOT, gStyle, TGaxis, TCanvas, TFile, TLegend, kWhite, kRed, kGray, kBlue, kBlack, gPad

## Don't pop up a canvas
gROOT.SetBatch(1)

## Options to make the histogram prettier
gStyle.SetLineWidth(3)
gStyle.SetOptStat(0)
gStyle.SetOptTitle(0)
gStyle.SetOptFit(0)
TGaxis.SetMaxDigits(3)

gStyle.SetTextSize(0.06)
gStyle.SetLabelSize(0.05,"xyzt")
gStyle.SetTitleSize(0.06,"xyzt")

gStyle.SetPadTickX(1)
gStyle.SetPadTickY(1)
gStyle.SetNdivisions(505, "XY")

## ROOT sometimes tries to ignore what you tell it...
gROOT .ForceStyle()

## Make a canvas (where we will draw the histogram)
can = TCanvas("can", "can", 800, 800)

def get_true_max(hist):
    maxVal = 0
    for x in range(hist.GetNbinsX()):
        thisVal = hist.GetBinContent(x+1)+hist.GetBinError(x+1)
        if thisVal > maxVal:
            maxVal = thisVal
    return maxVal
    
def make_dial_validation_plot(nominal_file_name, negvar_file_name, posvar_file_name, measurement_name, output_plot_name):

    ## Open the file
    nom_file = TFile(nominal_file_name)

    ## Get the generator prediction histogram
    nom_mc_hist = nom_file.Get(measurement_name+"_MC")
    nom_mc_hist .SetDirectory(0)
    
    ## Get the data histogram
    data_hist = nom_file.Get(measurement_name+"_data")
    data_hist .SetDirectory(0)
    
    ## This is a bit of a NUISANCE hack, the chisq between data and MC is stored as the MC histogram title
    nom_chisq = nom_mc_hist.GetTitle()

    ## Now the negative dial variation
    neg_file = TFile(negvar_file_name)
    neg_mc_hist = neg_file.Get(measurement_name+"_MC")
    neg_mc_hist .SetDirectory(0)
    neg_chisq = neg_mc_hist.GetTitle()

    ## Now the positive dial variation
    pos_file = TFile(posvar_file_name)
    pos_mc_hist = pos_file.Get(measurement_name+"_MC")
    pos_mc_hist .SetDirectory(0)    
    pos_chisq = pos_mc_hist.GetTitle()

    ## Presentation is important
    nom_mc_hist  .SetLineWidth(3)
    nom_mc_hist  .SetLineColor(kGray)
    neg_mc_hist  .SetLineWidth(3)
    neg_mc_hist  .SetLineColor(kRed)    
    pos_mc_hist  .SetLineWidth(3)
    pos_mc_hist  .SetLineColor(kBlue)
    data_hist .SetLineWidth(3)
    data_hist .SetLineColor(kBlack)

    ## Make a legend
    leg = TLegend(0.2, 0.86, 1.00, 0.99, "", "NDC")

    ## By default ROOT legends can be ugly... make it nicer
    leg .SetShadowColor(0)
    leg .SetFillColor(0)
    leg .SetLineWidth(0)
    leg .SetTextSize(0.04)
    leg .SetLineColor(kWhite)
    leg .SetNColumns(2)

    ## The information to be conveyed
    leg .AddEntry(data_hist, "Data", "l")
    leg .AddEntry(neg_mc_hist, "-ve (#chi^{2} = %.2f)"%float(neg_chisq), "l")
    leg .AddEntry(nom_mc_hist, "Nom. (#chi^{2} = %.2f)"%float(nom_chisq), "l")
    leg .AddEntry(pos_mc_hist, "+ve (#chi^{2} = %.2f)"%float(pos_chisq), "l")    
    
    ## Sort out the maximum value
    maxVal = 0
    for hist in [nom_mc_hist, neg_mc_hist, pos_mc_hist, data_hist]:
        thisMax = get_true_max(hist)
        if thisMax > maxVal:
            maxVal = thisMax

    ## Note that all style options should be applied to thee first histogram we draw
    ## Add a small buffer to make it look better
    data_hist .SetMaximum(maxVal*1.1)
            
    ## Now draw everything on the canvas
    can .cd()
    data_hist .Draw()
    data_hist .GetYaxis().SetTitleOffset(0.9)
    neg_mc_hist .Draw("HIST SAME")
    pos_mc_hist .Draw("HIST SAME")
    nom_mc_hist .Draw("HIST SAME")
    data_hist   .Draw("SAME")
    leg .Draw("SAME")

    ## Make the borders sensible
    gPad .SetRightMargin(0.03)
    gPad .SetTopMargin(0.2)
    gPad .SetBottomMargin(0.15)
    gPad .SetLeftMargin(0.15)
    can .Update()

    ## This actually makes the plot
    can .SaveAs(output_plot_name)
    print(output_plot_name)
    

if __name__ == "__main__":

    nominal_file_name  = "nominal_GENIEv3_example.root"
    negvar_file_name   = "negMaCCRES_GENIEv3_example.root"
    posvar_file_name   = "posMaCCRES_GENIEv3_example.root"
    
    samples = ["MINERvA_CC0pinp_STV_XSec_1Dpmu_nu", \
               "MINERvA_CC1pip_XSec_1DTpi_nu_2017", \
               "MINERvA_CC1pi0_XSec_1DTpi_nu"]

    for sample in samples:
        output_plot_name = "plots/validation_MaCCRES_"+sample+"_GENIEv3.png"
        make_dial_validation_plot(nominal_file_name, \
                                  negvar_file_name, \
                                  posvar_file_name, \
                                  sample, output_plot_name)
