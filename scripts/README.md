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

### Event reweighting
All of the generators discussed in this tutorial have variable parameters, which can be modified when running the simulation. However, once you generate some events, it will be obvious that this is a rather slow process, and if you want to explore the parameter space in a meaningful way, it would be very computationally expensive to re-generate a large number of events at each set of parameters of interest.

Event reweighting is a way to overcome this issue in a computationally efficient way. It is widely used, but has strong limitations. The procedure is relatively simple. If you have a set of events produced with some nominal parameter set, and want to approximate a set of events with some alternative parameter set, for each event, you calculate the probability for that event to have been produced with the nominal and alternative parameter set. Then, you take the ratio of the alternative probability divided by the nominal probability and obtain a "weight", reflecting how much more or less probable that event would be with the alternative, rather than nominal, parameter set. Then, the distribution of weighted events will look the same as the distribution of events had they been produced with the alternative parameter set.

Event reweighting packages are available inside NEUT and GENIE, and are actively developed additional. Additionally, experimental collaborations often implement additional uncertainties through custom reweighting packages which may or may not be open source. NUISANCE itself also has a mechanism for implementing new  (but simple) uncertainties.

Although commonly used, a fundamental limitation of reweighting is that regions of phase space which are not occupied in the initial model cannot be reweighted into existence.

### Varying parameters
NUISANCE can link to various reweighting packages in order to calculate reweighting factors event by event for a given set of parameter variations. These reweighting packages could be from the event generators themselves, or additional reweighting packages such as those produced by many experiments. It is also possible to implement simple reweighting parameters in NUISANCE, although historically this has mostly been done for rapid development of a dial destined for another package. Again, NUISANCE is not (or at least rarely) doing the heavy lifting, it is a high-level tool that builds on top of the existing event generators!

In this case, the parameter variations can be carried out by the generators, or NUISANCE, or another package which specifies how to reweight events, or all of the above.

To include a parameter variation, a line can be added to the NUISANCE card file with a format like:
```
  <parameter name="MaCCRES" nominal="-0.5" type="genie_parameter"/>
```
where this changes the value of a "genie_parameter" (meaning [GENIEReWeight](https://github.com/GENIE-MC/Reweight)) called "MaCCRES" (the axial mass value for the CC-resonant axial form factor), to -0.5, where the meaning of -0.5 is interpreted by the reweighting package.

In the case of GENIE, and most other reweighting packaged, the reweighted dial value can be calculated with $V = N \times (1 + R \times S)$ where $V$ is the new value, $N$ is the nominal value, $R$ is the reweighting factor (e.g. -0.5 in this example), and $S$ is the $\pm$1 $\sigma$ shift value. It can be challenging to look up $N$ and $S$, and generally requires generator specific knowledge.

These may also change based on the model used to initially generate the events, which must be known at runtime. For the case of GENIE, this means telling NUISANCE what GENIE tune was used (which then tells GENIERW how to configure itself), by adding a line to the card file, for example:
```
 <config GENIETune="AR23_20i_00_000" />
 ```
 You can look up these values in `$GENIE/config`. For example, you can find the $\pm$1 $\sigma$ shift values from `${GENIE}/config/GSystUncertaintyTable.xml`, and the nominal value from `$GENIE/config/AR23_20i/CommonParam.xml`, where only the latter is tune-specific.

Finally, one can also change the meaning of the dial value change to be as a fraction of the initial value by adding the following line to the NUISANCE card file:
```
<config setabstwk="1" />
```

To see the utility of this reweighting in action, there are two example card files, with a positive and negative 1$\sigma$ shift to `MaCCRES` which can generate output with the commands:
```
apptainer exec nuisance_nustec2026.sif nuiscomp -c dialvar_negMaCCRES_GENIEv3_example.card -o dialvar_negMaCCRES_GENIEv3_example.root
apptainer exec nuisance_nustec2026.sif nuiscomp -c dialvar_posMaCCRES_GENIEv3_example.card -o dialvar_posMaCCRES_GENIEv3_example.root
```

And then the scripts `dial_validation_plotter.C` or `dial_validation_plotter.py` both make validation plots showing the impact of those shifts on the predictions for the MINERvA samples of interest. Run these with either:
```
apptainer exec  nuisance_nustec2026.sif python3 dial_validation_plotter.py
```
Or:
```
apptainer exec  nuisance_nustec2026.sif root -q -l -b dial_validation_plotter.C
```
The output image in both cases is a png image showing how the $\pm$1$\sigma$ shift in GENIE's `MaCCRES` dial modifies the prediction for each sample made in the above files. E.g. `validation_MaCCRES_MINERvA_CC1pi0_XSec_1DTpi_nu_GENIEv3.png`. These scripts also don't have any dependencies in the container, so can be run with a local ROOT installation, or with python locally as long as PyROOT has been enabled.

Any number of parameters can be set, as long as they can be applied to the samples included in the card file. For example, you might want to include reweightale parameters from GENIE, and also include reweighting parameters in experiment-specific reweighting packages which are designed to work with GENIE. Each dial will calculate a weight for some or all events --- when multiple dials are included, the weight that will be stored for each event (and used to fill the summary histograms we're looking at in the example), will be the product of the weights from each dial.

### Fitting parameters
NUISANCE is also able to vary specified parameters and find the values of those parameters that minimizes the chi-square test-statistic with respect to a desired set of samples. It will also calculate the postfit covariance matrix, and produce the best fit histograms, as well as storing a lot of information about the fit. The fitting itself is carried out by MINUIT by default. Again, NUISANCE isn't re-inventing the wheel!

To include parameters in the fit, they must be added to the NUISANCE card file with the format:
```
 <parameter name="MaCCRES" nominal="0" type="genie_parameter"low="-1" high="5" step="0.1" state="FREE" />
```
This adds a few more fields to the parameter line(s) of the card file with respect to previous examples:
*low: the minimum value that the parameter is allowed to take in the fit
*high: the maximum value that the parameter is allowed to take in the fit
*step: gives the fitter a hint about how large the expected variation should be, but it will re-optimize the step size itself very quickly
*state: can be "FREE", meaning it varies in the fit, or "FIX" (default) meaning it is not

Again, you can add any number of parameters and samples to the fit, but the more there are, the more complex the computational problem, and the longer the fit will take to converge. As such, it's challenging to predict how long a fit will take a priori. Note that for each iteration of the fit, NUISANCE by default will loop over all events in all of the input files required to process the samples, and calculate a weight for each. It will skip events that don't enter any of the requested samples after the first iteration for speed.

To run a fit with a desired card file, using the `nuismin` program, instead of the usual `nuiscomp` used so far. An example card file for running a simple fit with a single free GENIE parameter `MaCCRES`, and a single MINERvA data sample, is given in `fit_GENIEv3_example.card`, which can be processed with:
```
apptainer exec nuisance_nustec2026.sif nuismin -c fit_GENIEv3_example.card -o fit_GENIEv3_example.root
```

The output file `fit_GENIEv3_example.root` contains a lot of the same information we saw in the simpler NUISANCE output files, with some additions/modifications:
* `<sample_name>_MC`: as before, but it now contains the best-fit histogram!
* `fit_iterations`: a ROOT TTree containing entires for each iteration of the fit, including the chi-square for each sample included and the parameter values
* `fit_result`: a ROOT TTree with summary information describing the fit configuration and the best-fit information
* `covariance` (`covariance_free`): the covariance matrix between postfit parameters (with fixed parameters removed)
* `correlation` (`correlation_free`): the correlation matrix between postfit parameters (with fixed parameters removed)
* `decomposition` (`decomposition_free`): the Cholesky decomposition of the covariance matrix between postfit parameters (with fixed parameters removed)
* +Various summary histograms which are simple copies of information in the `fit_result` tree

where `<sample_name>` is `MINERvA_CC1pip_XSec_1DTpi_nu_2017`, in this example.
