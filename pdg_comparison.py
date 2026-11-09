from ROOT import gROOT, gStyle, TGaxis, TFile, TH1D, TCanvas, TLegend, kRed, kAzure, kGreen, kOrange

gROOT.SetBatch(True)

gStyle.SetOptStat(0)
gStyle.SetOptTitle(0)
gStyle.SetOptFit(0)
TGaxis.SetMaxDigits(3)

gStyle.SetTextSize(0.06)
gStyle.SetLabelSize(0.05,"xyzt")
gStyle.SetTitleSize(0.06,"xyzt")

gStyle.SetPadTickX(1)
gStyle.SetPadTickY(1)
gStyle.SetErrorX(0)

def get_pdg_counts(fname, omit=()):
    omit = set(omit)

    f = TFile.Open(fname)
    tree = f.Get("FlatTree_VARS")
    tree.SetBranchStatus("*", 0)
    tree.SetBranchStatus("pdg", 1)
    tree.SetBranchStatus("nfsp", 1)
    tree.SetBranchStatus("InputWeight", 1)    

    counts = {}
    for event in tree:
        pdgs = getattr(event, "pdg")
        n = getattr(event, "nfsp")
        w = getattr(event, "InputWeight")
        
        for k in range(n):
            p = int(pdgs[k])
            if p in omit: continue
            counts[p] = counts.get(p, 0) + w

    f.Close()
    return counts


def plot_pdg_frequencies(files,
                         labels,
                         normalize=False,
                         group_width=0.8,
                         omit=(),
                         use_names=False):
    # Get counts per file
    counts_per_file = [get_pdg_counts(f, omit) for f in files]

    # Union of all PDG codes across every file, sorted
    all_pdgs = sorted(set().union(*[c.keys() for c in counts_per_file]))
    n_pdg = len(all_pdgs)
    n_files = len(files)

    # Each file's bar is group_width / n_files of a bin wide
    bar_width = group_width / n_files

    colors = [kAzure + 1, kRed + 1, kGreen + 2, kOrange + 1]
    
    can = TCanvas(f"pdg_hist", "", max(2000, 40 * n_pdg), 600)
    can .SetLogy()
    can .SetBottomMargin(0.22)
    can .SetRightMargin(0.02)
    can .SetTopMargin(0.02)
    
    hists = []
    for i, (counts, color) in enumerate(zip(counts_per_file, colors)):
        # One bin per PDG code, so the x axis is categorical
        h = TH1D(f"h_{i}", "", n_pdg, 0, n_pdg)
        h.SetDirectory(0)
        for j, p in enumerate(all_pdgs):
            h.SetBinContent(j + 1, counts.get(p, 0))

        if normalize and h.Integral() > 0:
            h.Scale(1.0 / h.Integral())
            h.Sumw2(False)

        h.SetFillColor(color)
        h.SetBarWidth(0.8 * bar_width)
        h.SetBarOffset((1 - group_width) / 2 + (i+0.1) * bar_width)
        hists.append(h)
        
    # Log scale needs a positive range; zero-height bars are simply not drawn
    nonzero = [h.GetBinContent(b) for h in hists for b in range(1, n_pdg + 1) if h.GetBinContent(b) > 0]

    hists[0].SetMinimum(0.5 * min(nonzero))
    hists[0].SetMaximum(3.0 * max(nonzero))

    for j, p in enumerate(all_pdgs):
        hists[0].GetXaxis().SetBinLabel(j + 1, str(p))
    hists[0].GetXaxis().LabelsOption("v")
    hists[0].GetXaxis().SetTitle("PDG code")
    hists[0].GetXaxis().SetTitleOffset(2.6)
    hists[0].GetYaxis().SetTitle("Fraction" if normalize else "Frequency")

    hists[0].Draw("BAR")
    for h in hists[1:]:
        h.Draw("BARSAME")
    
    leg = TLegend(0.85, 0.75, 0.95, 0.92)
    leg .SetBorderSize(0)
    leg .SetTextSize(0.04)
    leg .SetFillStyle(0)
    for h, label in zip(hists, labels):
        leg .AddEntry(h, label, "f")
    leg .Draw()

    can .RedrawAxis()
    can .SaveAs("plots/pdg_frequencies.pdf")


if __name__ == "__main__":

    file_list = [
        "MC_outputs/GENIEv3_AR23_MINERvA_LE_FHC_numu_NUISFLAT.root",
        "MC_outputs/NEUT592_MINERvA_LE_FHC_numu_NUISFLAT.root",
        "MC_outputs/GiBUU_MINERvA_LE_FHC_numu_20k_NUISFLAT.root",
        "MC_outputs/NUWROv25.11.1_MINERvA_LE_FHC_numu_NUISFLAT.root"
    ]
    name_list = ["GENIE", "NEUT","GiBUU",  "NuWro"]

    omit_pdgs = [
        2000000101, ## GENIE uses this as a nuclear remnant placeholder
        1000060120, ## Carbon nucleus
    ]
    
    plot_pdg_frequencies(
        file_list,
        name_list,
        normalize=True,
        omit=omit_pdgs,
    )
