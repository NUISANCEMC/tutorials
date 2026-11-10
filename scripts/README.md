## Using the NUISANCE flat trees

### Background
The NUISANCE flat trees provide a simple common flat tree format which is the same for all generators. This makes it very easy to compare features of different generators, and explore the physics that is implemented in them.

The flat trees have pre-calculated variables and fundamental interaction variables, like Q<sup>2</sup> (four-momentum transfer squared), W (hadronic mass), E<sub>ν</sub> (neutrino energy), the target nucleus and nucleon PDG codes, and the full particle stack before the interaction, after the nucleon interaction, and after the final state interactions.

Additionally, the flat trees can also be used to directly compare the generators to data.

Scripts to analyse the flat trees proceed via usual ROOT macros and only require ROOT to be installed; there is no need for the full generator stack to be installed (although we do provide containers with and instructions on how to run them in the `tutorials` directory!)

If someone has been kind enough to pre-generate flat-trees for you (see [below](#pre-made-flat-tree-files)), you can get quite knowledgable about generator physics without ever running them!

If the concept of a `TTree` is completely new to you, you'll likely find the ROOT tutorials useful: https://root.cern.ch/root/htmldoc/guides/users-guide/Trees.html.

The example scripts all use the same style of command, and you run the script with
```
root -l 'name_of_script.cpp("INPUT_FLATTREE_FILE.root")'
```
and optional arguments can be given (see the source files).

### Pre-made flat-tree files
In addition to running the generators via containers, you can do analyses on the pre-generated flat-trees that have been prepared. 

We host TBs of pre-generated files at NERSC, and here you'll download a small subset of them, which is enough to get you started. 

Files for T2K's ND280 detector with a CH target, and the DUNE ND with an argon target will be downloaded, for GENIE with CRPA, GENIE G18, NEUT with the DCC model, NuWro v25.3.1, and GiBUU. This will download **~2.5 GB** of files to the `MC_outputs` directory.

These can be obtained by running the script:
```
./download_files_forNuSTEC2026.sh
```
You can extend the script to download more files, other generators, and other flux and target combinations.

### Simple analysis using `TTree::Draw`
An example script using simple `TTree::Draw` commands has been provided in [`flat_bymode.cpp`](flat_bymode.cpp). This script draws the distribution of a variable in the flat-tree, both the total charged-current contribution, and the contribution from each of the interaction modes.

The central part of the script is
```
  const int nModes = 30;
  double scale = tree->GetMaximum("fScaleFactor");

  TH1D **w = new TH1D*[nModes];
  for (int i = 0; i < nModes; ++i) {
    tree->Draw(Form("%s>>h%i(%s)", variable.c_str(), i, binning.c_str()), Form("Mode==%i", i));
  }
  tree->Draw(Form("%s>>h00(%s)", variable.c_str(), binning.c_str()), Form("Mode<%i", nModes));
```
which first draws each of the interaction modes, and then draws all interaction modes below 30 (which means charged current).

We will go over a few examples using and extending this script in the lectures.

### Analysis using the particle stack
Two examples drawing from the full particle stack are also provided, see [`flat_calo_bias.cpp`](flat_calo_bias.cpp) and [`flat_pion_mom.cpp`](flat_pion_mom.cpp). Here, the central part is the looping over the final state particle stacks:
```
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
```
which performs the event selection (here one mu- and one pi+), and saves the index of the pion so the momentum can be retrieved and plotted in the `ppi` histograms.

The particle codes follow the PDG standard, documented in https://pdg.lbl.gov/2020/reviews/rpp2020-rev-monte-carlo-numbering.pdf for example.

### Data-MC comparisons with flat trees

NUISANCE can also compare generators to an array of included datasets, using the `nuiscomp` executable. 

This requires NUISANCE to be set up, for instance through containers as described in the [section in the main readme](../README.md#getting-and-using-the-tutorial-containers). 

Otherwise, the data-MC comparisons follow the same convention as listed there, except the generator name is replaced by `FLATTREE`, so a card would read

```
<nuisance>
  <sample name="T2K_CC1pip_CH_XSec_1Dppi_nu"  input="FLATTREE:scripts/MC_outputs/T2KND_FHC_numu_C8H8_NUWROv25.3.1_1M_0000_NUISFLAT.root"/>
</nuisance>
```

### More advanced usage and features

That's as far as the flat-trees can take you. If you want more advanced features, like

* General event reweighting
* Fitting generator parameters (e.g. M<sub>A</sub><sup>QE</sup>) to external data
* Studies of the impact of systematic parameters on generator predictions

you will need to interface with the generators' reweighting engines and use generator output through NUISANCE, as described in the [section in the main readme](../README.md#analyzing-the-nuisance-output).



