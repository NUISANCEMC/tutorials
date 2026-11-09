### Using the NUISANCE flat trees

#### Background
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

#### Pre-made flat-tree files
In addition to running the generators via containers, you can do analyses on the pre-generated flat-trees that have been prepared. 

We host TBs of pre-generated files at NERSC, and here you'll download a small subset of them, which is enough to get you started. 

Files for T2K's ND280 detector with a CH target, and the DUNE ND with an argon target will be downloaded, for GENIE with CRPA, GENIE G18, NEUT with the DCC model, NuWro v25.3.1, and GiBUU. This will download **~2.5 GB** of files to the `MC_outputs` directory.

These can be obtained by running the script:
```
./download_files_forNuSTEC2026.sh
```
You can extend the script to download more files, other generators, and other flux and target combinations.

#### Simple analysis using `TTree::Draw`
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

#### Analysis using the particle stack
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

### Making simple data--MC comparisons
There are a number of NUISANCE utilities for different purposes, but most of what will be needed in this project can be accomplished with `nuiscomp`, which essentially takes the output from the generators and makes an appropriate comparison to published data. Extensive documentation can be found here: https://nuisance.hepforge.org/tutorials/nuiscomp.html, and you will need to refer to it for more complex uses (e.g., fitting variable parameters).

A simple example XML card file input to `nuiscomp` is given by `nominal_GENIEv3_example.sh`, which can be run with:
```
apptainer exec nuisance_nustec2026.sif nuiscomp -c nominal_GENIEv3_example.card -o nominal_GENIEv3_example.root
```
In this example, data and the GENIEv3 files generated above are compared for four MINERvA low energy measurements, you can include as many different measurements as you want in each NUISANCE card file **as long as all use the same input generator**. You just have to ensure that you use the correctly generated event file for each flux--target pair!

The MINERvA measurements included are:
* `MINERvA_CC0pinp_STV_XSec_1Dpmu_nu`
  + [PRL 121 (2018) 2, 022504](https://inspirehep.net/literature/1673175) (supplementary material)
  + Source code in `${NUISANCE}/src/MINERvA/MINERvA_CC0pinp_STV_XSec_1D_nu.cxx`
* `MINERvA_CC1pip_XSec_1DTpi_nu_2017`
  + New data provided here https://minerva.fnal.gov/pion-data-release-page/ for an update of the [PRD 92, 092008 (2015)](https://inspirehep.net/literature/1303106) analysis
  + Source code in `${NUISANCE}/src/MINERvA/MINERvA_CC1pip_XSec_1D_2017Update.cxx`
* `MINERvA_CC1pi0_XSec_1DTpi_nu`
  + [PRD 96 (2017) 7, 072003](https://inspirehep.net/literature/1615865) (Fig. 12)
  + Source code in `${NUISANCE}/src/MINERvA/MINERvA_CC1pi0_XSec_1D_nu.cxx:`
* `MINERvA_CCinc_XSec_2DEavq3_nu`
  + [PRL 116 (2016) 071802](https://inspirehep.net/literature/1405301) (Fig. 2)
  + Source code in `${NUISANCE}/src/MINERvA/MINERvA_CCinc_XSec_2DEavq3_nu.cxx`

You can update the card to use NEUT or NuWro by updating each line from, e.g.:
```
<sample name="MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"  input="GENIE:MC_outputs/GENIEv3_AR23_MINERvA_LE_FHC_numu_NUIS.root"/>
```
to either:
```
<sample name="MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"  input="NuWro:MC_outputs/NUWROv25.11.1_MINERvA_LE_FHC_numu.root"/>
```
or:
```
<sample name="MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"  input="NEUT:MC_outputs/NEUT592_MINERvA_LE_FHC_numu.root"/>
```

To see all the measurements available, you can run the `nuissamples` executable, and it will tell you all the measurements it knows about. The source code for NUISANCE can be found here: https://nuisance.hepforge.org/tutorials/nuiscomp.html for future reference.

Other tags can be added to the card file to change the NUISANCE behaviour. Two have been added in this case:
```
<config UseSVDInverse="1" />
```
If run without this line, *for these measurement* NUISANCE will refuse to run because the covariance matrix provided with the data is not invertible. This line forces NUISANCE to *approximate* the inversion using Singular Value Decomposition. This might seem drastic, and maybe is, but is extremely common. Most matrices provided by experiments are not invertible!
```
<config drawopts="MC|DATA|COV|INVCOV" />
```
This controls the outputs that are saved by NUISANCE, but this is more than enough for our purposes here.

### Analyzing the NUISANCE output
The output of NUISANCE executables contains a lot of information in a ROOT data format. You can open up the file and look at the contents through the ROOT interactive command prompt with:
```
apptainer exec nuisance_nustec2026.sif root -l <file_name.root>
```
And then explore the file with the ROOT TBrowser GUI with:
```
TBrowser b
```
**Note that this opens up a GUI, and will be extremely slow if you do it over a remote connection. This will work seamlessly for apptainer, but possibly not for docker where X11 forwarding is... less seamless**

Alternatively, you could install ROOT on your local machine and use it to analyze the output files if you prefer. The NUISANCE output has no dependencies at all, you can open the files and explore them on any machine which has ROOT installed.

Inside the file you'll find a number of ROOT objects:
* `likelihood_hist`: the chi-square calculated between the data and MC, summarized for all samples
* `ndof_hist`: the number of degrees of freedom for each data sample, summarized
* `likedivndof_hist`: the ratio of the two above
* `<sample_name>_data`: the real data provided by an experiment
* `<sample_name>_MC`: the Monte Carlo prediction suitable to compare with the data
* `<sample_name>_COV`: the covariance between data points (again provided by the experiment)
* `<sample_name>_INVCOV`: the inverse of the covariance (note that with SVD included, this is not exact!)
* `<sample_name>_Chi2NMinusOne`: the chi-square calculated between data and MC if bin x is omitted from the calculation
* **2D only** `<sample_name>_data_1D`: the real data provided by an experiment linearized to form a 1D histogram
* **2D only** `<sample_name>_MC_1D`: the Monte Carlo prediction linearized to form a 1D histogram
(Where `<sample_name>` can be any of the samples requested in the card file)

There are two example scripts for producing a simple plot comparing data and simulation for the 1D datasets produced using the example NUISANCE card file above. The scripts produce identical plots, but one is written for python, the other is in C++, although note that the latter is actually interpreted but a C++ interpreter ROOT users (from ROOT v6, it is actually compiled with a just in time compiler). These can be run with:
```
apptainer exec  nuisance_nustec2026.sif python3 nominal_plotter.py
```
Or:
```
apptainer exec  nuisance_nustec2026.sif root -q -l -b nominal_plotter.C
```
The output image in both cases is a png image for each sample we we asked for, e.g. `nominal_MINERvA_CC1pi0_XSec_1DTpi_nu_GENIEv3.png`. These scripts also don't have any dependencies in the container, so can be run with a local ROOT installation, or with python locally as long as PyROOT has been enabled.

It is also possible to make a similar plot with the linearized 2D data and prediction with small tweak to the plotting script, or it can just be inspected in the `TBrowser`.

### More advanced usage and features

That's as far as the flat-trees can take you. If you want more advanced features, like

* General event reweighting
* Fitting generator parameters (e.g. M_A^{QE}) to external data
* Studies of the impact of systematic parameters on generator predictions

you will need to interface with the generators' reweighting engines and use generator output through NUISANCE, as described in the main [README.md](../README.md#analyzing-the-nuisance-output).



