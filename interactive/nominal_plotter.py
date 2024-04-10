from ROOT import gROOT, gStyle, TGaxis, TCanvas, TFile, TLegend, kWhite, kRed, kBlack, gPad

## Don't pop up a canvas
gROOT.SetBatch(1)

## Options to make the histogram prettier
gStyle.SetLineWidth(3)
gStyle.SetOptStat(0)
gStyle.SetOptTitle(0)
gStyle.SetOptFit(0)
TGaxis.SetMaxDigits(3)

gStyle.SetTextSize(0.07)
gStyle.SetLabelSize(0.06,"xyzt")
gStyle.SetTitleSize(0.07,"xyzt")

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
    
def make_basic_plot(input_file_name, measurement_name, output_plot_name):

    ## Open the file
    in_file = TFile(input_file_name)

    ## Get the generator prediction histogram
    mc_hist = in_file.Get(measurement_name+"_MC")

    ## Get the data histogram
    data_hist = in_file.Get(measurement_name+"_data")

    ## This is a bit of a NUISANCE hack, the chisq between data and MC is stored as the MC histogram title
    chisq = mc_hist.GetTitle()

    ## Presentation is important
    mc_hist  .SetLineWidth(3)
    mc_hist  .SetLineColor(kRed)
    data_hist.SetLineWidth(3)
    data_hist.SetLineColor(kBlack)

    ## Make a legend
    leg = TLegend(0.35, 0.82, 1.00, 0.99, "", "NDC")

    ## By default ROOT legends can be ugly... make it nicer
    leg .SetShadowColor(0)
    leg .SetFillColor(0)
    leg .SetLineWidth(0)
    leg .SetTextSize(0.04)
    leg .SetLineColor(kWhite)
    leg .SetNColumns(2)

    ## The information to be conveyed
    leg .AddEntry(data_hist, "Data", "l")
    leg .AddEntry(mc_hist, "MC (#chi^{2} = "+chisq+")", "l")

    ## Sort out the maximum value
    maxVal = 0
    for hist in [mc_hist, data_hist]:
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
    mc_hist .Draw("HIST SAME")
    leg .Draw("SAME")

    ## Make the borders sensible
    gPad .SetRightMargin(0.02)
    gPad .SetTopMargin(0.2)
    gPad .SetBottomMargin(0.15)
    gPad .SetLeftMargin(0.15)
    can .Update()

    ## This actually makes the plot
    can .SaveAs(output_plot_name)
    print(output_plot_name)
    

if __name__ == "__main__":

    input_file_name  = "NUISANCE_example_with_GENIEv3.root"
    measurement_name = "MINERvA_CC1pi0_XSec_1DTpi_nu"
    output_plot_name = measurement_name+"_GENIEv3.png"
    
    make_basic_plot(input_file_name, measurement_name, output_plot_name)
