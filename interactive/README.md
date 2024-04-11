# Interactive NUISANCE tutorials

Assuming you have git installed get a local copy of the files in this directory:
```
git clone https://github.com/NUISANCEMC/tutorials.git nuisance_tutorials
```
If you don't have git, you *could* also download a zip file from the green code tab on the front page. But... just install git.

All of the work will be done within `nuisance_tutorials/interactive`.

## Pre-made example files:
Many of the generator input files will be made within the tutorial, but we have prepared a few high-statistics files in advance for some exercises. These can be obtained by sourcing the script:
```
source download_files_forNuSTEC2024.sh
```
This will download ~1.6 GB of files to `MC_outputs` inside the git repo.

## Getting and using the tutorial containers

For these tutorials, we have a docker *container* which is hosted on dockerhub and which contains all of the generators, NUISANCE, and all necessary library dependences (such as ROOT). The advantage of using containers is that you don't need to compile all of this software for your system, and everyone is using exactly the same software stack.

Although the containers are built using docker, most container runtime environments are able to run them. For security reasons, although it is a standard for industry use of containers, most HEP computing systems we use do not support docker. There isn't really a standard, and there are several home-grown container ***runtime*** environments for running containers supported by different major HEP computing sites. For this tutorial, we're using singularity as the default option, as it is probably the most common for High Performance Computing sites used in HEP. However, it will not run natively on Mac or Windows machines, so we've provided docker examples as well.

You can also use any other container runtime environment you're familiar with that supports docker images. Good luck.


### Singularity
Instructions for installing singularity on linux can be found here: https://sylabs.io/guides/3.10/admin-guide/installation.html

You can obtain the container needed for this tutorial with the command:
```
singularity pull nuisance_nuint2024.sif docker://nuisancemc/tutorial:nuint2024
```
which will get the container from dockerhub, and convert it to a singularity (.sif) image locally. This step needs an internet connection, and will save a ~1 GB file locally on your system.

Singularity containers can be used in a few different ways, and it can get a bit confusing to remember what is in the container, and what is on the host machine. When you run it, the container has access to your home directory (and all subdirectories) and your current working directory, so it can run software from within the container on files from your host machine. Example commands can look like:
```
singularity exec <my_container.sif> do_something.exe /path/to/input <arguments>
```
where the program to be run is contained within the container and accessible through the path.

Additionally, you can open up the container and poke around in it with:
```
singularity shell <my_container.sif>
```
This will open a new shell inside the container. Note that you cannot modify the contents of the container, **but** you can modify your host system! So be careful what you try to delete. Exit the shell with `exit`.

You can also modify the containers interactively by making a sandbox:
```
sudo singularity build --sandbox <my_sandbox_dir> <my_container.sif>
```
Then you can play around with the sandbox interactively:
```
sudo singularity shell --writable <my_sandbox_dir>
```
Note that you need to be root to do this, and again, you can also modify your host system as root, which is more dangerous. You won't need the sandbox functionality at all durin gthis tutorial.

### docker
Instructions for installing docker on different systems can be found here: https://docs.docker.com/desktop/

(Note that Linux users can install the lighter-weight docker engine here: https://docs.docker.com/engine/install/, but they can also just use singularity...)

The first time you try and use the docker container, it will pull it down from dockerhub and then cache it locally. You can check what has been cached with:
```
sudo docker images
```
And you can delete all local docker images once you're finished with this tutorial using:
```
sudo docker system prune -f --all
```
docker files will hang out in unexpected places indefinitely if you don't. I assume if you don't want to remove all docker images, you're familiar enough with docker to do that.

The design usage for docker is a little different to singularity. Most obviously, everything is run as root. But the intended use case is to have persistent images which you enter and then modify the contents of independent of the host system, with explicit copies between container and host whenever you need to transfer something. The usage examples here entirely break that model, and are definitely not a general "how to" for docker.

The equivalent docker command to
```
singularity exec <my_container.sif> do_something.exe /path/to/input <arguments>
```
is
```
sudo docker run --rm -v $(pwd):/output -w /output <container> do_something.exe /path/to/input <arguments>
```
with some caveats.

The above arguments the the above command do the following:
* `--rm` stop the container after the command has executed
* `-v $(pwd):/output` mount the current working directory to `/output` within the container
* `-w /output` execute the command in `/output` within the container
An important caveat is that you don't have access to anywhere on the host machine that isn't the current working directory (but you can add other mountpoints etc if desired).

Another caveat is that docker does not set up X11 forwarding from the container to the host, so opening GUI's from the container will report issues. This is solveable, but is system dependent, so I won't offer a general solution here. Instead, please ensure that you have a version of ROOT installed locally, for which you can instructions here: https://root.cern/install/

The rest of the tutorial will use singularity for the examples. If you're using docker, you can replace any instance of:
```
singularity exec nuisance_nuint2024.sif <...>
```
with
```
sudo docker run --rm -v $(pwd):/output -w /output nuisancemc/tutorial:nuint2024
```
When examples suggest opening files with ROOT and browsing with ROOT's GUI `TBrowser`, you will need to use your local copy of ROOT instead... or be very confident that you know how to sort out X11 forwarding between the container and your host machine.

## Running the event generation packages
Neutrino event generators provide executables for making neutrino events with simple starting conditions, which are sufficient for comparisons to cross-section data. More complicated applications with a full neutrino flux simulation and a realistic detector geometry require dedicated applications, but the experiments that produce data devote a lot of time and effort to account for or simply express, the effect of these, and produce cross-section data which generally does not require specific knowledge of their experiments other than the simplified flux distributions and the target material of interest.

Although some generators provide support for complex detector geometries, these are typically described using another detector simulation package, called GEANT4 (https://geant4.web.cern.ch/), which is widely used in the wider field of Particle Physics. It's also interesting to note that once an event has been simulated, and the products of a neutrino interaction have been produced by the neutrino event enerator, those interaction products are handed to GEANT4 to propagate through the detector geometry and deal with all the ways non-neutrinos interact with matter. The job of the neutrino event generator is pretty specialized.

A collection of experimental fluxes is available in the NUISANCE data directory, which can be found in the containers here: `/opt/nuisance/data/flux/`, and a thorough description of where those fluxes come is documented here: https://nuisance.hepforge.org/trac/wiki/ExperimentFlux. For all the examples in this tutorial, we'll use the muon-neutrino component of the MINERvA ``low energy'' flux in neutrino-enhanced mode, which is the `numu_fhc` histogram in the file `/opt/nuisance/data/flux/minerva_le_flux.root`. You can inspect this file with ROOT if you would like with:
```
singularity exec nuisance_nuint2024.sif root -l /opt/nuisance/data/flux/minerva_le_flux.root
```
Then open up a TBrowser with `TBrowser b` and explore the histograms contained through the GUI.

Note that the example scripts below can be easily modified to take different input fluxes or use different targets, but also note that NUISANCE is not generating these events, the event generators are!

Finally, there are copies of the files generated in this tutorial available here: https://portal.nersc.gov/project/nuisance/tutorial/

If you need, you can download them all from there, or with the command:
```
wget -nH -np -r --cut-dirs 3 https://portal.nersc.gov/project/nuisance/tutorial/
```

### GENIE
[GENIE](http://www.genie-mc.org/) is the most widely used neutrino interaction simulation packages currently used in the field. It can simulate neutrino energies from MeV to PeV scales, and also has support for electron-nucleus scattering, photon-nucleus scattering, pion-nucleus scattering, and even provides support for simulating various dark matter models.

In this project, we will focus on neutrino interactions, for which the simplest event generation tool in GENIE is `gevgen`, which is fully documented here: https://genie-docdb.pp.rl.ac.uk/DocDB/0000/000002/006/man.pdf

An example script to generate 100k muon neutrino-hydrocarbon interactions using the MINERvA LE flux is given in `generation_GENIEv3_example.sh` using the `AR23_20i_00_000` model, which has been developed by DUNE and is now being utilized by multiple experiments. Run it with a command like (which should take 5-10 minutes):
```
singularity exec nuisance_nuint2024.sif /bin/bash generation_GENIEv3_example.sh
```
There will be a few output files produced:
* `GENIEv3_AR23_MINERvA_LE_FHC_numu.root`, the gevgen output in GENIE's GHEP format, requires the correct libraries to read
* `GENIEv3_AR23_MINERvA_LE_FHC_numu_NUIS.root`: the output from NUISANCE's `PrepareGenie`, which adds the flux and total event rate histograms to make normalization easier
* `GENIEv3_AR23_MINERvA_LE_FHC_numu_NUISFLAT.root`: the output from NUISANCE's `nuisflat` application, which makes a ROOT flat tree with a simplified format that doesn't require any libraries to be read.
we'll use the second two of these output files later on in the tutorial.

If you look at the script, you will see that the `NU_PDG` and `TARG` variables are numeric codes, which follow the Particle Data Group's (PDG) convention for labeling particles: https://pdg.lbl.gov/2019/reviews/rpp2019-rev-monte-carlo-numbering.pdf. The PDG code for a muon neutrino is 14, so you can see that in this example, we're using an incident muon neutrino. The PDG code for a carbon nucleus is 11000060120, and for a hydrogen nucleus is 1000010010. In this example, we have `TARG=1000060120[0.9231],1000010010[0.0769]`. The actual target is C_8H_8, so 12/13 carbon and 1/13 hydrogen by the number of nucleons. All generators use the same PDG conventions for labeling particles, you get used to it.

### NUWRO
[NuWro](https://nuwro.github.io/user-guide/) is a neutrino event generator which is developed to be flexible and include as many theory model options as possible, reflecting the main interests of the development group. As so many options are configurable, many more options need to be specified to generate events. Documentation is available on the NuWro website: https://nuwro.github.io/user-guide/getting-started/running/

An example script for producing 100k muon neutrino-hydrocarbon interactions for the MINERvA low energy neutrino-enhanced flux is given in `generation_NUWRO_example.sh`, which uses parameters set in `MC_inputs/MINERvA_LE_numu_NUWRO_LFGRPA.params`, and can be run with the following command (which should take ~1 minute):
```
singularity exec nuisance_nuint2024.sif /bin/bash generation_NUWRO_example.sh
```
Nuwro produces a small number of files which are useful for debugging, but the two files we will use here are:
*`NUWRO_LFGRPA_MINERvA_LE_FHC_numu.root`, this is the nuwro output, which needs correct libraries to ***fully*** read, although native ROOT can probably do okay because the structure is relatively simple
*`NUWRO_LFGRPA_MINERvA_LE_FHC_numu_NUISFLAT.root`: the output from NUISANCE's `nuisflat` application, which makes a ROOT flat tree with a simplified format that doesn't require any libraries to be read.

Note that most of the parameters that govern NuWro's execution are specified in 
Although the NuWro input file contains a lot of options, the important ones for your purposes are:
* `number_of_test_events`: how many *test* events to generate. This calculates the cross section before generating the events that will be saved, and serves a similar purpose to the GENIE input splines. The larger this number is, the more accurate the results will be. As a rule of thumb (citation definitely needed) >2x the `number_of_events` parameter is used.
* `number_of_events`: how many *unweighted* events to generate
* `beam_particle`: the PDG code of the incident particle
* `beam_inputroot`: the file containing the desired flux spectrum
* `beam_inputroot_flux`: the name of the histogram in the flux file
* `@target/CH.txt`: this isn't really a parameter, but rather includes a file with multiple parameter that describe the target material. 

To explore the targets available, and the settings, you can look at the nuwro source code, for example:
```
singularity exec  nuisance_nuint2024.sif cat /opt/nuwro/data/target/CH.txt
```

### NEUT
[NEUT](https://arxiv.org/abs/2106.15809) is neutrino generator with a long pedigree, originally written in  pre-standard(!) FORTRAN in the 1980's for the Kamiokande experiment in Japan. It remains the primary generator used by the Super-Kamiokande and T2K collaborations, and has been semi-continuously developed ever since. For recent documentation, see: https://arxiv.org/abs/2106.15809

NEUT is easy to run and uses a similar input card file to NuWro with all the relevant parameters included, which for this example can be found in `MC_inputs/MINERvA_LE_numu_NEUT.card`. Generate events 100k muon neutrino-hydrocarbon interactions for the MINERvA low energy neutrino-enhanced flux with the following command (which should take 5-10 minutes):
```
singularity exec nuisance_nuint2024.sif /bin/bash generation_NEUT_example.sh
```
Note that the arguments are positional and NEUT doesn't take flags. The output of NEUT is automatically ready for NUISANCE, so no additional `Prepare` step is required.

The NEUT generation script above produces a few files, but the two files we will use here are:
*`NEUT570_MINERvA_LE_FHC_numu.root`, this is the `neutroot2` output, which needs correct libraries to read. No `PrepareX` step is necessary as NEUT already stores the histograms used for normalization in NUISANCE (which was originally developed for T2K...).
*`NEUT570_LFGRPA_MINERvA_LE_FHC_numu_NUISFLAT.root`: the output from NUISANCE's `nuisflat` application, which makes a ROOT flat tree with a simplified format that doesn't require any libraries to be read.

Important parameters you will need to control in the NEUT card files:
* `EVCT-NEVT`: sets the number of events to generate
* `EVCT-IDPT`: the incoming neutrino PDG code
* `EVCT-FILENM`: the filename to take the incoming flux from
* `EVCT-HISTNM`: the name of the relevant histogram in the flux file
* `NEUT-NUMBNDN`: number of bound neutrons (e.g., 6 for carbon)
* `NEUT-NUMBNDP`: number of bound protons (e.g., 6 for carbon)
* `NEUT-NUMFREP`: number of free protons to include
* `NEUT-NUMATOM`: total number of nucleons (it's redundant but has to be set)
`neutroot2` is not able to simulate complex molecular targets, just single elements with additional hydrogen components, but that's sufficient for most purposes. Alternative event generation applications are provided to generate events with more complex targets and geometries.

### Event reweighting
All of the generators discussed above have variable parameters, which can be modified when running the simulation. However, once you generate some events, it will be obvious that this is a rather slow process, and if you want to explore the parameter space in a meaningful way, it would be very computationally expensive to re-generate a large number of events at each set of parameters of interest.

Event reweighting is a way to overcome this issue in a computationally efficient way. It is very widely used, but can have limitations. The procedure is relatively simple. If you have a set of events produced with some nominal parameter set, and want to approximate a set of events with some alternative parameter set, for each event, you calculate the probability for that event to have been produced with the nominal and alternative parameter set. Then, you take the ratio of the alternative probability divided by the nominal probability and obtain a "weight", reflecting how much more or less probable that event would be with the alternative, rather than nominal, parameter set. Then, the distribution of weighted events will look the same as the distribution of events had they been produced with the alternative parameter set.

## NUISANCE
[NUISANCE](https://nuisance.hepforge.org) is a conceptually simple package that takes the output of event generators, and compares it to measured data. The advantage is that the type of input is opaque to the user (e.g., the different generator outputs don't need to be understood in detail), and the relevant selection cuts are applied to the generated events to reflect the measurement of interest. Then the published uncertainty (and covariance) are used to calculate the degree of compatibility between the data and Monte Carlo simulation.

NUISANCE also links directly to the event reweighting packages provided by the generators, and provided tools for implementing simple event reweighting independent of the generators, so that the goodness of fit between data and simulation can be assessed for different parameter sets. Finally, it provides an interface to parameter fitting methods so that model parameters can be varied in a fit to data, and the best fit solution and associated uncertainties can be extracted, using any desired ensemble of datasets.

### Using the NUISANCE flat trees
***Clarence, please drop your old example into some scripts and then describe them here***

### Making simple data-MC comparisons
There are a number of NUISANCE utilities for different purposes, but most of what will be needed in this project can be accomplished with `nuiscomp`, which essentially takes the output from the generators and makes an appropriate comparison to published data. Extensive documentation can be found here: https://nuisance.hepforge.org/tutorials/nuiscomp.html, and you will need to refer to it for more complex uses (e.g., fitting variable parameters).

A simple example XML card file input to `nuiscomp` is given by `nominal_GENIEv3_example.sh`, which can be run with:
```
singularity exec nuisance_nuint2024.sif nuiscomp -c nominal_GENIEv3_example.card -o nominal_GENIEv3_example.root
```
In this example, data and the GENIEv3 files generated above are compared for three MINERvA low energy measurements, you can include as many different measurements as you want in each NUISANCE card file **as long as all use the same input generator**. You just have to ensure that you use the correctly generated event file for each flux--target pair!

You can update the card to use NEUT or NuWro by updating each line from, e.g.:
```
<sample name="MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"  input="GENIE:MC_outputs/GENIEv3_AR23_MINERvA_LE_FHC_numu_NUIS.root"/>
```
to either:
```
<sample name="MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"  input="NuWro:MC_outputs/NUWRO_LFGRPA_MINERvA_LE_FHC_numu.root"/>
```
or:
```
<sample name="MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"  input="NEUT:MC_outputs/NEUT570_MINERvA_LE_FHC_numu.root"/>
```

To see all the measurements available, you can run the `nuissamples` executable, and it will tell you all the measurements it knows about. The source code for NUISANCE can be found here: https://nuisance.hepforge.org/tutorials/nuiscomp.html for future reference.

Other tags can be added to the card file to change the NUISANCE behaviour. One has been added in this case, the rather mysterious:
```
<config UseSVDInverse="1" />
```
If run without this line, *for these measurement* NUISANCE will refuse to run because the covariance matrix provided with the data is not invertible. This line forces NUISANCE to *approximate* the inversion using Singular Value Decomposition. This might seem drastic, and maybe is, but is extremely common. Most matrices provided by experiments are not invertible!

### Analyzing the NUISANCE output
The output of NUISANCE executables contains a lot of information in a ROOT data format. You can open up the file and look at the contents through the ROOT interactive command prompt with:
```
singularity exec nuisance_nuint2024.sif root -l <file_name.root>
```
And then explore the file with the ROOT TBrowser GUI with:
```
TBrowser b
```
**Note that this opens up a GUI, and will be extremely slow if you do it over a remote connection. This will work seamlessly for singularity, but possibly not for docker where X11 forwarding is... less seamless**

Alternatively, you could install ROOT on your local machine and use it to analyze the output files if you prefer. The NUISANCE output has no dependencies at all, you can open the files and explore them on any machine which has ROOT installed.

There are two example scripts for producing a simple plot comparing data and simulation for one for the datasets analyzed using the example NUISANCE card file above. The scripts produce identical plots, but one is written for python, the other is in C++, although note that the latter is actually interpreted but a C++ interpreter ROOT users (from ROOT v6, it is actually compiled with a just in time compiler). These can be run with:
```
singularity exec  nuisance_nuint2024.sif python3 nominal_plotter.py
```
Or:
```
singularity exec  nuisance_nuint2024.sif root -q -l -b nominal_plotter.C
```
The output image in both cases is a png image for each sample we we asked for, e.g. `nominal_MINERvA_CC1pi0_XSec_1DTpi_nu_GENIEv3.png`. These scripts also don't have any dependencies in the container, so can be run with a local ROOT installation, or with python locally as long as PyROOT has been enabled.

### Varying parameters
NUISANCE can link to various reweighting packages in order to calculate reweighting factors event by event for a given set of parameter variations. These reweighting packages could be from the event generators themselves, or additional reweighting packages such as those produced by many experiments. It is also possible to implement simple reweighting parameters in NUISANCE, although historically this has mostly been done for rapid development of a dial destined for another package. Again, NUISANCE is not (or at least rarely) doing the heavy lifting, it is a high-level tool that builds on top of the existing event generators!

In this case, the parameter variations can be carried out by the generators, or NUISANCE, or another package which specifies how to reweight events, or all of the above.

To include a parameter variation, a line can be added to the NUISANCE card file with a format like:
```
  <parameter name="MaCCRES" nominal="-0.5" type="genie_parameter"/>
```
where this changes the value of a "genie_parameter" (meaning [GENIEReWeight]{https://github.com/GENIE-MC/Reweight}) called "MaCCRES" (the axial mass value for the CC-resonant axial form factor), to -0.5, where the meaning of -0.5 is interpreted by the reweighting package.

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
singularity exec nuisance_nuint2024.sif nuiscomp -c dialvar_negMaCCRES_GENIEv3_example.card -o dialvar_negMaCCRES_GENIEv3_example.root
singularity exec nuisance_nuint2024.sif nuiscomp -c dialvar_posMaCCRES_GENIEv3_example.card -o dialvar_posMaCCRES_GENIEv3_example.root
```

And then the scripts `dial_validation_plotter.C` or `dial_validation_plotter.py` both make validation plots showing the impact of those shifts on the predictions for the MINERvA samples of interest. Run these with either:
```
singularity exec  nuisance_nuint2024.sif python3 dial_validation_plotter.py
```
Or:
```
singularity exec  nuisance_nuint2024.sif root -q -l -b dial_validation_plotter.C
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
*high: the maxmimum value that the parameter is allowed to take in the fit
*step: gives the fitter a hint about how large the expected variation should be, but it will re-optimize the step size itself very quickly
*state: can be "FREE", meaning it varies in the fit, or "FIX" (default) meaning it is not

Again, you can add any number of parameters and samples to the fit, but the more there are, the more complex the computational problem, and the longer the fit will take to converge. As such, it's challenging to predict how long a fit will take a priori. Note that for each iteration of the fit, NUISANCE by default will loop over all events in all of the input files required to process the samples, and calculate a weight for each. It will skip events that don't enter any of the requested samples after the first iteration for speed.

To run a fit with a desired card file, using the `nuismin` program, instead of the usual `nuiscomp` used so far. An example card file for running a simple fit with a single free GENIE parameter `MaCCRES`, and a single MINERvA data sample, is given in `fit_GENIEv3_example.card`, which can be processed with:
```
singularity exec nuisance_nuint2024.sif nuismin -c fit_GENIEv3_example.card -o fit_GENIEv3_example.root
```

The output file `fit_GENIEv3_example.root` contains a lot of the same information we saw in the simpler NUISANCE output files, with some additions:
*<sample_name>_MC: as before, but it now contains the bestfit histogram!
*<sample_name>_data: the data provided for this sample
*<sample_name>_MODES_*: the best fit prediction broken up into interaction modes
*<

where <sample_name> is `MINERvA_CC1pip_XSec_1DTpi_nu_2017`, in this example.
