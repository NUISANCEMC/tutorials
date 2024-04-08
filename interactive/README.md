# Interactive NUISANCE tutorials

Assuming you have git installed get a local copy of the files in this directory:
```
git clone https://github.com/NUISANCEMC/tutorials.git nuisance_tutorials
```
If you don't have git, you ***could*** also download a zip file from the green code tab on the front page. But... just install git.

## Getting and using the tutorial containers

For these tutorials, we have a docker ***container*** which is hosted on dockerhub and which contains all of the generators, NUISANCE, and all necessary library dependences (such as ROOT). The advantage of using containers is that you don't need to compile all of this software for your system, and everyone is using exactly the same software stack.

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

Another caveat is that docker does not set up X11 forwarding from the container to the host, so opening GUI's from the container will report issues. This is solveable, but is system dependent, so I won't offer a general solution here...

The rest of the tutorial will use singularity for the examples. If you're using docker, you can replace any instance of:
```
singularity exec nuisance_nuint2024.sif <...>
```
with
```
sudo docker run --rm -v $(pwd):/output -w /output nuisancemc/tutorial:nuint2024
```

## Running the event generation packages
Neutrino event generators provide executables for making neutrino events with simple starting conditions, which are sufficient for comparisons to cross-section data. More complicated applications with a full neutrino flux simulation and a realistic detector geometry require dedicated applications, but the experiments that produce data devote a lot of time and effort to account for or simply express, the effect of these, and produce cross-section data which generally does not require specific knowledge of their experiments other than the simplified flux distributions and the target material of interest.

Although some generators provide support for complex detector geometries, these are typically described using another detector simulation package, called GEANT4 (https://geant4.web.cern.ch/), which is widely used in the wider field of Particle Physics. It's also interesting to note that once an event has been simulated, and the products of a neutrino interaction have been produced by the neutrino event enerator, those interaction products are handed to GEANT4 to propagate through the detector geometry and deal with all the ways non-neutrinos interact with matter. The job of the neutrino event generator is pretty specialized.

A collection of experimental fluxes is available in the NUISANCE data directory, which can be found in the containers here: `/opt/nuisance/data/flux/`, and a thorough description of where those fluxes come is documented here: https://nuisance.hepforge.org/trac/wiki/ExperimentFlux. For all the examples in this tutorial, we'll use the muon-neutrino component of the MINERvA ``low energy'' flux in neutrino-enhanced mode, which is the `numu_fhc` histogram in the file `/opt/nuisance/data/flux/minerva_le_flux.root`. You can inspect this file with ROOT if you would like with:
```
singularity exec nuisance_nuint2024.sif root -l /opt/nuisance/data/flux/minerva_le_flux.root
```
Then open up a TBrowser with `TBrowser b` and explore the histograms contained through the GUI.

Note that the example scripts below can be easily modified to take different input fluxes or use different targets, but also note that NUISANCE is not generating these events, the event generators are!

### GENIE
GENIE is one of the most widely used neutrino interaction simulation packages currently used in the field. It can simulate neutrino energies from MeV to PeV scales, and also has support for electron-nucleus scattering, photon-nucleus scattering, pion-nucleus scattering, and even provides support for simulating various dark matter models.

In this project, we will focus on neutrino interactions, for which the simplest event generation tool in GENIE is `gevgen`, which is fully documented here: https://genie-docdb.pp.rl.ac.uk/DocDB/0000/000002/006/man.pdf

An example script to generate 100k muon neutrino-hydrocarbon interactions using the MINERvA LE flux is given in `GENIEv3_MINERvA_example.sh` using the `AR23_20i_00_000` model, which has been developed by DUNE and is now being utilized by multiple experiments. Run it with a command like (which should take 5-10 minutes):
```
singularity exec nuisance_nuint2024.sif /bin/bash GENIEv3_MINERvA_example.sh
```
There will be a few output files produced:
* `GENIEv3_AR23_MINERvA_LE_FHC_numu.root`, the gevgen output in GENIE's GHEP format, requires the correct libraries to read
* `GENIEv3_AR23_MINERvA_LE_FHC_numu_NUIS.root`: the output from NUISANCE's `PrepareGenie`, which adds the flux and total event rate histograms to make normalization easier
* `GENIEv3_AR23_MINERvA_LE_FHC_numu_NUISFLAT.root`: the output from NUISANCE's `nuisflat` application, which makes a ROOT flat tree with a simplified format that doesn't require any libraries to be read.
we'll use the second two of these output files later on in the tutorial.

If you look at the script, you will see that the `NU_PDG` and `TARG` variables are numeric codes, which follow the Particle Data Group's (PDG) convention for labeling particles: https://pdg.lbl.gov/2019/reviews/rpp2019-rev-monte-carlo-numbering.pdf. The PDG code for a muon neutrino is 14, so you can see that in this example, we're using an incident muon neutrino. The PDG code for a carbon nucleus is 11000060120, and for a hydrogen nucleus is 1000010010. In this example, we have `TARG=1000060120[0.9231],1000010010[0.0769]`. The actual target is C_8H_8, so 12/13 carbon and 1/13 hydrogen by the number of nucleons. All generators use the same PDG conventions for labeling particles, you get used to it.

### NUWRO
NuWro is a neutrino event generator which is developed to be flexible and include as many theory model options as possible, reflecting the main interests of the development group. As so many options are configurable, many more options need to be specified to generate events. Documentation is available on the NuWro website: https://nuwro.github.io/user-guide/getting-started/running/

An example script for producing 100k muon neutrino-hydrocarbon interactions for the MINERvA low energy neutrino-enhanced flux is given in `NUWRO_MINERvA_example.sh`, which uses parameters set in `MC_inputs/MINERvA_LE_numu_NUWRO_LFGRPA.params`, and can be run with the following command (which should take ~1 minute):
```
singularity exec nuisance_nuint2024.sif /bin/bash NUWRO_MINERvA_example.sh
```
Nuwro produces a small number of files which are useful for debugging, but the two files we will use here are:
*`NUWRO_LFGRPA_MINERvA_LE_FHC_numu.root`, this is the nuwro output, which needs correct libraries to ***fully*** read, although native ROOT can probably do okay because the structure is relatively simple
*`NUWRO_LFGRPA_MINERvA_LE_FHC_numu_NUISFLAT.root`: the output from NUISANCE's `nuisflat` application, which makes a ROOT flat tree with a simplified format that doesn't require any libraries to be read.

Note that most of the parameters that govern NuWro's execution are specified in 
Although the NuWro input file contains a lot of options, the important ones for your purposes are:
* `number_of_test_events`: how many ***test*** events to generate. This calculates the cross section before generating the events that will be saved, and serves a similar purpose to the GENIE input splines. The larger this number is, the more accurate the results will be. As a rule of thumb (citation definitely needed) >2x the `number_of_events` parameter is used.
* `number_of_events`: how many ***unweighted*** events to generate
* `beam_particle`: the PDG code of the incident particle
* `beam_inputroot`: the file containing the desired flux spectrum
* `beam_inputroot_flux`: the name of the histogram in the flux file
* `@target/CH.txt`: this isn't really a parameter, but rather includes a file with multiple parameter that describe the target material. 

To explore the targets available, and the settings, you can look at the nuwro source code, for example:
```
singularity exec  nuisance_nuint2024.sif cat /opt/nuwro/data/target/CH.txt
```

### NEUT
NEUT is neutrino generator with a long pedigree, originally written in  pre-standard(!) FORTRAN in the 1980's for the Kamiokande experiment in Japan. It remains the primary generator used by the Super-Kamiokande and T2K collaborations, and has been semi-continuously developed ever since. For recent documentation, see: https://arxiv.org/abs/2106.15809

NEUT is easy to run and uses a similar input card file to NuWro with all the relevant parameters included, which for this example can be found in `MC_inputs/MINERvA_LE_numu_NEUT.card`. Generate events 100k muon neutrino-hydrocarbon interactions for the MINERvA low energy neutrino-enhanced flux with the following command (which should take 5-10 minutes):
```
singularity exec nuisance_nuint2024.sif /bin/bash NEUT_MINERvA_example.sh
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
NUISANCE is a conceptually simple package that takes the output of event generators, and compares it to measured data. The advantage is that the type of input is opaque to the user (e.g., the different generator outputs don't need to be understood in detail), and the relevant selection cuts are applied to the generated events to reflect the measurement of interest. Then the published uncertainty (and covariance) are used to calculate the degree of compatibility between the data and Monte Carlo simulation.

NUISANCE also links directly to the event reweighting packages provided by the generators, and provided tools for implementing simple event reweighting independent of the generators, so that the goodness of fit between data and simulation can be assessed for different parameter sets. Finally, it provides an interface to parameter fitting methods so that model parameters can be varied in a fit to data, and the best fit solution and associated uncertainties can be extracted, using any desired ensemble of datasets.

### Making simple data-MC comparisons
There are a number of NUISANCE utilities for different purposes, but most of what will be needed in this project can be accomplished with `nuiscomp`, which essentially takes the output from the generators and makes an appropriate comparison to published data. Extensive documentation can be found here: https://nuisance.hepforge.org/tutorials/nuiscomp.html, and you will need to refer to it for more complex uses (e.g., fitting variable parameters).

A simple example XML card file input to `nuiscomp` is given by `NUISANCE_example_with_GENIEv3.card`, which can be run with:
```
singularity exec nuisance_nuint2024.sif nuiscomp -c NUISANCE_example_with_GENIEv3.card -o NUISANCE_example_with_GENIEv3.root
```
In this example, data and the GENIEv3 files generated above are compared for three MINERvA low energy measurements, you can include as many different measurements as you want in each NUISANCE card file **as long as all use the same input generator**. You just have to ensure that you use the correctly generated event file for each flux--target pair!

You can update the card to use NEUT or NuWro by updating each line from, e.g.:
```
<sample name="MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"  input="GENIE:GENIEv3_AR23_MINERvA_LE_FHC_numu_NUIS.root"/>
```
to either:
```
<sample name="MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"  input="NuWro:NUWRO_LFGRPA_MINERvA_LE_FHC_numu.root"/>
```
or:
```
<sample name="MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"  input="NEUT:NEUT570_MINERvA_LE_FHC_numu.root"/>
```

To see all the measurements available, you can run the `nuissamples` executable, and it will tell you all the measurements it knows about. The source code for NUISANCE can be found here: https://nuisance.hepforge.org/tutorials/nuiscomp.html for future reference.

Other tags can be added to the card file to change the NUISANCE behaviour. One has been added in this case, the rather mysterious:
```
<config UseSVDInverse="1" />
```
If run without this line, ***for these measurement*** NUISANCE will refuse to run because the covariance matrix provided with the data is not invertible. This line forces NUISANCE to **approximate** the inversion using Singular Value Decomposition. This might seem drastic, and maybe is, but is extremely common. Most matrices provided by experiments are not invertible!

### Analyzing the NUISANCE output
The output of NUISANCE executables contains a lot of information in a ROOT data format. You can open up the file and look at the contents through the ROOT interactive command prompt with:
```
singularity exec nuisance_nuint2024.sif root -l <file_name.root>
```
And then explore the file with the ROOT TBrowser GUI with:
```
TBrowser b
```
***Note that this opens up a GUI, and will be extremely slow if you do it over a remote connection. This will work seamlessly for singularity, but possibly not for docker where X11 forwading is... less seamless***

Alternatively, you could install ROOT on your local machine and use it to analyze the output files if you prefer. The NUISANCE output has no dependencies at all, you can open the files and explore them on any machine which has ROOT installed.

There are two example scripts for producing a simple plot comparing data and simulation for one for the datasets analyzed using the example NUISANCE card file above. The scripts produce identical plots, but one is written for python, the other is in C++, although note that the latter is actually interpreted but a C++ interpreter ROOT users (from ROOT v6, it is actually compiled with a just in time compiler). These can be run with:
```
singularity exec  nuisance_nuint2024.sif python3 simple_NUISANCE_plotter.py
```
Or:
```
singularity exec  nuisance_nuint2024.sif root -q -l -b simple_NUISANCE_plotter.C
```
The output image in both cases is simply a png file: `MINERvA_CC1pi0_XSec_1DTpi_nu_GENIEv3.png`. These scripts also don't have any dependencies in the container, so can be run with a local ROOT installation, or with python locally as long as PyROOT has been enabled.

### Fitting parameters
NUISANCE is also able to vary parameters for you, and find the value of the set of parameters you provide in the card file that minimizes the chi-square test-statistic with respect to the samples you provide in the card file. It will also calculate the postfit covariance matrix, and produce the best fit histograms. This was one of the primary motivations for producing NUISANCE in the first place!

It is possible to implement simple reweighting parameters in NUISANCE for development, but it primarily interfaces with the reweighting packages provided by the generators themselves. E.g., it's not reinventing the wheel, it's a high-level tool which builds on top of the generators, which do all of the heavy lifting!

To include parameters in the fit, they must be added to the NUISANCE card file with the format:
```
  <parameter name="NXSec_CA5RES" nominal="-0.04951" type="t2k_parameter" low="-0.5" high="0.5" step="0.1" state="FREE" />
```
The low/high variables give the fit range that should be allowed for that parameter. The step variable gives the fitter a hint about how large the expected variation should be, but it will re-optimize the step size itself very quickly. The state parameter can be "FREE", meaning the parameter is included in the fit, or "FIX" meaning that it is not.

You can add any number of parameters and samples to the fit, but the more there are, the more complex the computational problem, and the longer the fit will take to converge. As such, it's challenging to predict how long a fit will take a priori.

To run a fit with a desired card file, using the `nuismin` program, instead of the usual `nuiscomp` used so far.

