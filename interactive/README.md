# Interactive NUISANCE tutorials

This is intended to be a repository to facilitate research projects using NUISANCE.

Assuming you have git installed get a local copy of these files with:
```
git clone https://github.com/wilkinson-nu/nuisance_project.git
```
If you don't have git, you ***could*** also download a zip file from the green code tab on the front page. But... just install git.

## Singularity

Instructions for installing singularity on linux can be found here: https://sylabs.io/guides/3.5/admin-guide/installation.html

All build recipes have been tested with singularity version 3.5.3, although any recent version should be fine.

**Note that all singularity commands below may need to be tweaked for Mac and (especially) Windows, the exact syntax is likely to be similar, but I come from a Linux background.**

```
singularity pull nuisance_nuint2024.sif docker://nuisancemc/tutorial:nuint2024
```


### Running singularity containers
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
Note that you need to be root to do this, and again, you can also modify your host system as root, which is more dangerous. In general, you shouldn't need to use the sandbox functionality at all this during the project.

## Running the event generation packages
Neutrino event generators provide executables for making neutrino events with simple starting conditions, which are sufficient for comparisons to cross-section data. More complicated applications with a full neutrino flux simulation and a realistic detector geometry require dedicated applications, but the experiments that produce data devote a lot of time and effort to account for or simply express, the effect of these, and produce cross-section data which does not require specific knowledge of their experiments other than the simplified flux distributions and the target material of interest.

A collection of experimental fluxes is available in the NUISANCE data directory, which can be found in the containers here: `/opt/nuisance/data/flux/`, and a thorough description of where those fluxes come is documented here: https://nuisance.hepforge.org/trac/wiki/ExperimentFlux 

Although some generators provide support for complex detector geometries, these are typically described using another detector simulation package, called GEANT4 (https://geant4.web.cern.ch/), which is widely used in the wider field of Particle Physics. It's also interesting to note that once an event has been simulated, and the products of a neutrino interaction have been produced by the neutrino event enerator, those interaction products are handed to GEANT4 to propagate through the detector geometry and deal with all the ways non-neutrinos interact with matter. The job of the neutrino event generator is therefore very specialized.

### GENIE
GENIE is one of the most widely used neutrino interaction simulation packages currently used in the field. It can simulate neutrino energies from MeV to PeV scales, and also has support for electron-nucleus scattering, photon-nucleus scattering, pion-nucleus scattering, and even provides support for simulating various dark matter models. 

In this project, we will focus on neutrino interactions, for which the simplest event generation tool in GENIE is `gevgen`, which is fully documented here: https://genie-docdb.pp.rl.ac.uk/DocDB/0000/000002/006/man.pdf

Example scripts to generate 100,000 muon neutrino-deuterium interactions using the Argonne Bubble Chamber flux are given in:
* GENIE v2.12.10: ANL_numu_D2_GENIE_v2.12.10.sh
* GENIE v3.00.06: ANL_numu_D2_GENIE_v3.00.06.sh

Either file can be run with singularity:
```
singularity exec images/genie_v2.12.10.sif /bin/sh ANL_numu_D2_GENIE_v2.12.10.sh
```
Or equivalently with shifter on NERSC:
```
shifter -V ${PWD}:/output --entrypoint --image=docker:wilkinsonnu/nuisance_project:genie_v2.12.10 /bin/sh ANL_numu_D2_GENIE_v2.12.10.sh
```
There will be a few output files produced, but the important one is given by the `outFileName` variable in the script. This script can be used as a basis for generating events on different targets, with different fluxes, and even with different GENIE models.

Note that as well as calling `gevgen`, the scripts contain a second step using `PrepareGENIE`, which is actually a NUISANCE utility to make the output file ready for use in NUISANCE, it simply calculates the total cross section (required for normalization) before running any comparison tools. This factorization is done for for speed. A second relevant output file, which has a similar name to the first, but will end in `_NUIS.root` is ready for NUISANCE!

If you look at the script, you may notice that the `nuType` and `TARG` variables are numeric codes, which follow the Particle Data Group's (PDG) convention for labeling particles: https://pdg.lbl.gov/2019/reviews/rpp2019-rev-monte-carlo-numbering.pdf. The PDG code for a muon neutrino is 14, so you can see that in this example, we're using an incident muon neutrino. The PDG code for a deuterium nucleus is 1000010020, so you can see we're using a deuterium target in this example. All generators use the same PDG conventions for labeling particles, so it's a bit confusing at first, but you get used to it.

### NUWRO
NuWro is a neutrino event generator which is developed to be flexible and include as many theory model options as possible, reflecting the main interests of the development group. As so many options are configurable, many more options need to be specified to generate events. Documentation is available on the NuWro website: https://nuwro.github.io/user-guide/getting-started/running/

A relevant example input file for producing 100,000 muon neutrino-deuterium interactions using the Argonne Bubble Chamber flux is given in ANL_numu_D2_nuwro.params, which can be run with the following command:
```
singularity exec images/nuwro_19.02.2.sif nuwro -i ANL_numu_D2_nuwro.params -o ANL_numu_D2_nuwro.root
```
Nuwro produces a small number of files for monitoring purposes, but the important output file is specified by the `-o` option, which can be renamed as you wish.

Although the NuWro input file contains a lot of options, the important ones for your purposes are:
* `number_of_events`: how many events to generate!
* `beam_particle`: the PDG code of the incident particle
* `beam_inputroot`: the file containing the desired flux spectrum
* `beam_inputroot_flux`: the name of the histogram in the flux file

And the target type, which is the most confusing part of the NuWro input. For deuterium, there are 4 relevant parameters:
* `target_type`
* `nucleus_p`
* `nucleus_n`
* `nucleus_target`
For more complex targets, the correct settings are predefined in the nuwro/data directory, the options can be found with (for example):
```
singularity exec images/nuwro_19.02.2.sif ls /opt/nuwro/data/target
```
And the desired option included in the input file with (for example): `@target/CH2.txt`. Note that this is **instead** of the 4 parameters defined for deuterium.

Finally, in order to make the NuWro output file ready for NUISANCE, a quick additional step is needed, as was the case for GENIE:
```
singularity exec images/nuwro_19.02.2.sif PrepareNuwro ANL_numu_D2_nuwro.root -F /opt/nuisance/data/flux/ANL_1977_2horn_rescan.root,numu_flux
```
This command uses the input histograms used to generate the events, defined in the NuWro input file. It's possible but not necessary to save the output to a different file, rather than modifying the existing NuWro output file with the `-o` option.

### NEUT
NEUT is an old neutrino generator originally written in the 1980's for the Kamiokande experiment in Japan. It remains the primary generator used by the Super-Kamiokande and T2K collaborations, and has been semi-continuously developed ever since, but is an old piece of code originally written in pre-standard(!) FORTRAN. There is some documentation, which isn't strictly speaking public, but if you need to know more details, I can provide it. There is no website, and the code is not public, so in order to recreate the singularity container from the definition file provided, you need a couple of additional files which I have provided in this repository.

NEUT is easy to run and uses a similar input card file to NuWro with all the relevant parameters included. Generate events with:
```
singularity exec images/neut_5.4.1.sif neutroot2 input.card output.root
```
Note that the arguments are positional and NEUT doesn't take flags. The output of NEUT is automatically ready for NUISANCE, so no addiitonal `Prepare` step is required. Two example card files are provided, the first, `ANL_numu_D2_NEUT.card`, generates 100,000 muon neutrino-deuterium interactions using the Argonne Bubble Chamber flux as in the other examples. The second, `MiniBooNE_numu_CH2_NEUT.card` generates 100,00 muon neutrino-CH2 interactions using the MiniBooNE experiment flux. Note that CH_2 represents the MiniBooNE mineral oil detector material. Both cards have been provided because a number of settings are required to turn off nuclear effects for deuterium targets in NEUT, which wasn't really designed for such light targets.

Important parameters you will need to control in the NEUT card files:
* `EVCT-NEVT`: sets the number of events to generate
* `EVCT-IDPT`: the incoming neutrino PDG code
* `EVCT-FILENM`: the filename to take the incoming flux from
* `EVCT-HISTNM`: the name of the relevant histogram in the flux file
* `NEUT-NUMBNDN`: number of bound neutrons (e.g., 6 for carbon)
* `NEUT-NUMBNDP`: number of bound protons (e.g., 6 for carbon)
* `NEUT-NUMFREP`: number of free protons to include
* `NEUT-NUMATOM`: total number of nucleons (it's redundant but has to be set)
NEUT is not able to simulate complex molecular targets, just single elements with additional hydrogen components, but that's sufficient for the common targets, and for our purposes.

### Event reweighting
All of the generators discussed above have variable parameters, which can be modified when running the simulation. However, once you generate some events, it will be obvious that this is a rather slow process, and if you want to explore the parameter space in a meaningful way, it would be very computationally expensive to re-generate a large number of events at each set of parameters of interest.

Event reweighting is a way to overcome this issue in a computationally efficient way. It is very widely used, but can have limitations. The procedure is relatively simple. If you have a set of events produced with some nominal parameter set, and want to approximate a set of events with some alternative parameter set, for each event, you calculate the probability for that event to have been produced with the nominal and alternative parameter set. Then, you take the ratio of the alternative probability divided by the nominal probability and obtain a "weight", reflecting how much more or less probable that event would be with the alternative, rather than nominal, parameter set. Then, the distribution of weighted events will look the same as the distribution of events had they been produced with the alternative parameter set.

## NUISANCE
NUISANCE is a conceptually simple package that takes the output of event generators, and compares it to measured data. The advantage is that the type of input is opaque to the user (e.g., the different generator outputs don't need to be understood in detail), and the relevant selection cuts are applied to the generated events to reflect the measurement of interest. Then the published uncertainty (and covariance) are used to calculate the degree of compatibility between the data and Monte Carlo simulation.

NUISANCE also links directly to the event reweighting packages provided by the generators, and provided tools for implementing simple event reweighting independent of the generators, so that the goodness of fit between data and simulation can be assessed for different parameter sets. Finally, it provides an interface to parameter fitting methods so that model parameters can be varied in a fit to data, and the best fit solution and associated uncertainties can be extracted, using any desired ensemble of datasets.

### Making simple data-MC comparisons
There are a number of NUISANCE utilities for different purposes, but most of what will be needed in this project can be accomplished with `nuiscomp`, which essentially takes the output from the generators and makes an appropriate comparison to published data. Extensive documentation can be found here: https://nuisance.hepforge.org/tutorials/nuiscomp.html, and you will need to refer to it for more complex uses (e.g., fitting variable parameters).

A simple example card file input to `nuiscomp` is given by `NUISANCE_ANL_example.card`, which can be run with:
```
singularity exec images/genie_v3.00.06.sif nuiscomp -c NUISANCE_ANL_example_with_GENIEv3.00.06.card -o NUISANCE_ANL_example_with_GENIEv3.00.06.root
```
Note that although all containers have NUISANCE in, it's only built against a single generator in each, so won't work unless you run it with the correct container.

In this example, data and GENIE are compared for three ANL measurements, you can include as many different measurements as you want in each NUISANCE card file **as long as all use the same input generator**. You just have to ensure that you use the correctly generated event file for each experiment! To see all the measurements available, you can run the `nuissamples` executable, and it will tell you all the measurements it knows about. The source code for NUISANCE can be found here: https://nuisance.hepforge.org/tutorials/nuiscomp.html for future reference.

### Analyzing the NUISANCE output
The output of NUISANCE executables contains a lot of information in a ROOT data format. You can open up the file and look at the contents through the ROOT interactive command prompt with:
```
singularity exec images/genie_v3.00.06.sif root -l <file_name.root>
```
ROOT interactive sessions use an approximation of C++, but you don't need to use them extensively. You can explore the file with the ROOT TBrowser GUI with:
```
TBrowser b
```
***Note that this opens up a GUI, and will be extremely slow if you do it on NERSC. It would work, but would be incredibly frustrating, so I don't recommend it***

Actually any of the singularity containers will do as they all contain ROOT. Alternatively, you could install ROOT on your local machine and use it to analyze the output files if you prefer. The NUISANCE output has no dependencies at all, you can open the files and explore them on any machine which has ROOT installed.

I've included two example scripts for producing a simple plot comparing data and simulation for one for the datasets analyzed using the example NUISANCE card file above. The scripts produce identical plots, but one is written in python (2.7), the other is in C++, although note that the latter is actually interpreted but a C++ interpreter ROOT users (from ROOT v6, it is actually compiled with a just in time compiler). These can be run with:
```
singularity exec images/root_v5.34.36_pythia6.sif python simple_NUISANCE_plotter.py
```
Or:
```
singularity exec images/root_v5.34.36_pythia6.sif root -q -l -b simple_NUISANCE_plotter.C
```
The output image in both cases is simply a png file: `ANL_CC1ppip_XSec_1DEnu_nu_GENIEv3.png`.

### Fitting parameters
NUISANCE is also able to vary parameters for you, and find the value of the set of parameters you provide in the card file that minimizes the chi-square test-statistic with respect to the samples you provide in the card file. It will also calculate the postfit covariance matrix, and produce the best fit histograms.

To include parameters in the fit, they must be added to the NUISANCE card file with the format:
```
  <parameter name="NXSec_CA5RES" nominal="-0.04951" type="t2k_parameter" low="-0.5" high="0.5" step="0.1" state="FREE" />
```
The low/high variables give the fit range that should be allowed for that parameter. The step variable gives the fitter a hint about how large the expected variation should be, but it will re-optimize the step size itself very quickly. The state parameter can be "FREE", meaning the parameter is included in the fit, or "FIX" meaning that it is not.

You can add any number of parameters to the fit, but the more there are, the more complex the computational problem, and the longer the fit will take to converge. As such, it's challenging to predict how long a fit will take a priori.

To run a fit with a desired card file, using the `nuismin` program, instead of the usual `nuiscomp` used so far.

