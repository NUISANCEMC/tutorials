"""
Weight distribution (InputWeight) for a NUISANCE flat tree built from
GiBUU carbon and hydrogen samples, split by target, with the Kish
effective sample size of each in the legend.

Usage: python weights.py flattree.root [output_stem] [nbins]
"""
import sys
import math
import array
import ROOT
from ROOT import gROOT, gStyle, TGaxis, TFile, TH1D, TCanvas, TLegend, kRed, kAzure

gROOT.SetBatch(True)

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


def main(fname, nbins=100):
    nbins = int(nbins)

    f = TFile.Open(fname)
    t = f.Get("FlatTree_VARS")

    ## Only read the branches we need
    t.SetBranchStatus("*", 0)
    t.SetBranchStatus("tgta", 1)
    t.SetBranchStatus("InputWeight", 1)

    weights = {
        "C12": array.array("d"),
        "H1": array.array("d"),
    }

    ## Get all weights from the tree
    for ev in t:
        if ev.tgta == 1:
            weights["H1"].append(ev.InputWeight)
        elif ev.tgta == 12:
            weights["C12"].append(ev.InputWeight)
        else:
            raise ValueError(f"Found unrecognised target A {ev.tgta}")

    ## Calculate statistics
    stats = {}
    for name, w in weights.items():
        n = len(w)
        sw = math.fsum(w)
        sw2 = math.fsum(x * x for x in w)
        ess = sw * sw / sw2 if sw2 > 0 else 0.0
        pos = [x for x in w if x > 0]
        stats[name] = dict(n=n, ess=ess, npos=len(pos),
                           wmin=min(pos) if pos else None,
                           wmax=max(pos) if pos else None)
        print(f"{name:3s}: N = {n}, N_eff = {ess:.1f} "
              f"({100 * ess / n if n else 0:.2f}% of N), "
              f"non-positive weights = {n - len(pos)}")

    ## Find a logarithmic binning that works for both targets
    wmins = [s["wmin"] for s in stats.values() if s["npos"]]
    wmaxs = [s["wmax"] for s in stats.values() if s["npos"]]
    lo, hi = math.log10(min(wmins)), math.log10(max(wmaxs))
    pad = max(0.05 * (hi - lo), 0.1)
    lo, hi = lo - pad, hi + pad
    edges = array.array("d", [10 ** (lo + (hi - lo) * i / nbins)
                              for i in range(nbins + 1)])

    ## ---- Fill histograms ----
    hists = {}
    for name, w in weights.items():
        h = TH1D(f"h_{name}", ";Weight;N. Events", nbins, edges)
        h.SetDirectory(0)
        for x in w:
            if x > 0:
                h.Fill(x)
        hists[name] = h

    ## Draw histogram
    c = TCanvas("c", "c", 900, 700)
    c.SetLogx()
    c.SetLogy()
    c.SetTopMargin(0.02)
    c.SetBottomMargin(0.14)
    c.SetLeftMargin(0.12)
    c.SetRightMargin(0.02)

    ymax = max(h.GetMaximum() for h in hists.values())
    hists["C12"].SetLineColor(kRed+1)
    hists["C12"].SetLineWidth(3)
    hists["C12"].GetYaxis().SetTitleOffset(0.9)
    hists["C12"].GetXaxis().SetTitleOffset(1.05)   
    hists["C12"].SetMaximum(ymax * 5)
    hists["C12"].SetMinimum(0.5)
    hists["C12"].Draw("HIST")
    hists["H1"].SetLineColor(kAzure+2)
    hists["H1"].SetLineWidth(3)
    hists["H1"].Draw("HIST SAME")

    ## Add a legend
    leg = TLegend(0.14, 0.82, 0.5, 0.95)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg .SetTextSize(0.04)
    for name in ["C12", "H1"]:
        s = stats[name]
        leg.AddEntry(hists[name], f"{name:3s}: N = {s['n']}, N_{{eff}} = {s['ess']:.0f}", "l")
    leg.Draw("SAME")

    c.RedrawAxis()
    c.SaveAs("plots/gibuu_weights.pdf")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    main(*sys.argv[1:])
