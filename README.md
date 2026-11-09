# Interactive NUISANCE tutorials

Assuming you have `git` installed clone the repository and move into it:
```
git clone https://github.com/NUISANCEMC/tutorials.git nuisance_tutorials
cd nuisance_tutorials
```

If you don't have git, you *could* also download a zip file from the green code tab on the front page, or [here](https://github.com/NUISANCEMC/tutorials/archive/refs/heads/main.zip). But... just install git: you'll need it for a lot more than just this tutorial.

The examples are all intended to be run from within the `nuisance_tutorials` directory, and use shell syntax which _should_ be suitable for Linux, macOS Terminal, and Windows Subsystem for Linux (WSL).

## Pre-made flat-tree files
Many of the generator input files will be made within the tutorial, but we have prepared a few high-statistics files in advance for some exercises. Alternatively, if you for some reason can not or do not want to run the generators, these flat-tree files can be used for simpler analyses too.

You can find more information on getting and using these in [`scripts/README.md`](scripts/README.md).

## Getting and using the tutorial containers

For these tutorials, we have a docker *container* which is hosted on dockerhub and which contains all of the generators, NUISANCE, and all necessary library dependences (such as ROOT). The advantage of using containers is that you don't need to compile all of this software for your system, and everyone is using exactly the same software stack.

You _do_ need a container runtime environment to run the container, either on your laptop or a system you have access to. Docker, Apptainer, and SingularityCE can all run this image, and you only need one of those runtimes. Apptainer and SingularityCE are common on shared Linux computing systems. Docker Desktop is usually the more straightforward choice on macOS. If you are using a University cluster or HPC, check which runtime it provides before trying to install one yourself. There may be a home-grown container runtime (e.g., shifter at NERSC), which should also be fine, although specific instructions are not provided.

<details>
<summary><h3>Apptainer (Linux and many computing clusters)</h3></summary>

Follow your institution’s instructions if Apptainer is already provided, or see the [Apptainer user guide](https://apptainer.org/docs/user/latest/).

From the `nuisance_tutorials` directory, download the container image from dockerhub and save it as a local, 1.2 GB, `.sif` file:
```
apptainer pull nuisance_nustec2026.sif docker://wilkinsonnu/nuisancemc:tutorial
```
Note that in addition to the .sif file, this will add ~1.5 GB to the apptainer cache, which is not necessary after building the file. You can inspect the cache and clear it with:
```
apptainer cache list
apptainer cache clear
```
**Note that you can run apptainer on a Fermilab GPVM, but the typical disk quotas may be challenging to fit both the .sif file and the cache in...**

To run a program from the image:
```
apptainer exec nuisance_nustec2026.sif <program> <arguments>
```
Here, your current tutorial directory is available **inside** the container, and the program (from the container) starts there. It writes files relative to the tutorial directory on the host. Files elsewhere on your computer may need an additional bind mount before a program in the container can access them.

To explore the container interactively:
```
apptainer shell nuisance_nustec2026.sif
```
Type `exit` to leave the shell. The image itself is read-only, but **your mounted tutorial files are not**: commands in the container can change or delete them.
</details>


<details>
<summary><h3>SingularityCE (Linux and some computing clusters)</h3></summary>

If your system provides a `singularity` command rather than `apptainer`, you can use the same Docker Hub image. See the [SingularityCE user guide](https://docs.sylabs.io/guides/latest/user-guide/) if you need installation or background information.

From the `nuisance_tutorials` directory:
```
singularity pull nuisance_nustec2026.sif docker://wilkinsonnu/nuisancemc:tutorial
```
Note that, as for apptainer, in addition to the 1.2GB .sif file, this will add ~1.5 GB to the singularity cache, which is not necessary after building the file. You can inspect the cache and clear it with:
```
singularity cache list
singularity cache clear
```

Run a program, or open an interactive shell, with:
```
singularity exec nuisance_nustec2026.sif <program> <arguments>
singularity shell nuisance_nustec2026.sif
```

For the examples later in the tutorial, you can replace `apptainer` with `singularity`. As with Apptainer, the image is read-only but files in the mounted tutorial directory can be changed. Type `exit` to leave an interactive shell.
</details>


<details>
<summary><h3>Docker (macOS, Linux, or Windows with WSL)</h3></summary>

### Docker
Install [Docker Desktop](https://docs.docker.com/desktop/) on macOS or Windows. On Linux, you can also install [Docker Engine](https://docs.docker.com/engine/install/). Start Docker before running the commands below. On Windows, use a WSL shell with Docker Desktop's WSL integration enabled; the commands here are not written for PowerShell.

Docker downloads the image automatically the first time you run it. You can also download it ahead of time:
```
docker pull wilkinsonnu/nuisancemc:tutorial
```

From the `nuisance_tutorials` directory, run a program with:
```
docker run --rm -v $(pwd):/output -w /output \
    wilkinsonnu/nuisancemc:tutorial <program> <arguments>
```
The above arguments the the above command do the following:
* `--rm` stop the container after the command has executed
* `-v $(pwd):/output` mount the current working directory to `/output` within the container
* `-w /output` execute the command in `/output` within the container
An important caveat is that you don't have access to anywhere on the host machine that isn't the current working directory (but you can add other mountpoints etc if desired).

Or open a shell:
```
docker run --rm -v $(pwd):/output -w /output \
    wilkinsonnu/nuisancemc:tutorial bash
```
Type `exit` to leave the shell. When running these commands, your current working directory will be available in the container. **Changes to files in your current work directory persist on your computer**, even though `--rm` removes the temporary container when it exits. Files elsewhere on your computer are not available unless you mount them separately.

**macOS notes:**
- Docker runs Linux containers in a virtual machine. You can still edit and inspect files produced in your mounted tutorial directory using ordinary macOS applications.
- On an Apple Silicon Mac, if Docker reports that no image is available for `linux/arm64`, try adding `--platform linux/amd64` to `docker pull` and before the image name in each `docker run` command. Emulation can be slower than running a native image.
- If Docker Desktop cannot mount the tutorial directory, check its file-sharing permissions. Keeping the repository somewhere under your home directory is usually simplest.

**ROOT graphics:** 
Docker does not automatically connect graphical applications in the container to your desktop. Container commands that do not need a graphical window can still run, but exercises that open ROOT's `TBrowser` need extra display configuration or a [local ROOT installation](https://root.cern/install/). In particular, do not expect a containerised `TBrowser` window to appear automatically on macOS.

To see downloaded images, use `docker images`. If you want to remove *this tutorial image* later, use:
```
docker image rm wilkinsonnu/nuisancemc:tutorial
```
(Note that docker files will hang out indefinitely if you don't clean them up)

When a later example says:
```
apptainer exec nuisance_nustec2026.sif <program> <arguments>
```
use the following equivalent if you chose Docker:
```
docker run --rm -v $(pwd):/output -w /output \
    wilkinsonnu/nuisancemc:tutorial <program> <arguments>
```
</details>

<details>
    <summary><h3>Building containers</h3></summary>
    Although not part of this tutorial, if anybody is interested in developing generators or building on another system, our container recipes, built using docker, are all available in: https://github.com/wilkinson-nu/nuisance_containers.
</details>

## Running the event generation packages

Neutrino event generators provide executables for making neutrino events with simple starting conditions, which are sufficient for comparisons to cross-section data. More complicated applications with a full neutrino flux simulation and a realistic detector geometry require dedicated applications, but the experiments that produce data devote a lot of time and effort to account for or simply express, the effect of these, and produce cross-section data which generally does not require specific knowledge of their experiments other than the simplified flux distributions and the target material of interest.

Although some generators provide support for complex detector geometries, these are typically described using another detector simulation package, called GEANT4 (https://geant4.web.cern.ch/), which is widely used in the wider field of Particle Physics. It's also interesting to note that once an event has been simulated, and the products of a neutrino interaction have been produced by the neutrino event generator, those interaction products are handed to GEANT4 to propagate through the detector geometry and deal with all the ways non-neutrinos interact with matter. The job of the neutrino event generator is pretty specialized.

<details>

<summary><h3>Experimental fluxes</h3></summary>
A collection of experimental fluxes is available in the NUISANCE data directory, which can be found in the containers here: `/opt/generators/NUISANCE/data/flux/`, and a (relatively) thorough description of where those fluxes come is documented here: https://nuisance.hepforge.org/trac/wiki/ExperimentFlux. For the examples in this tutorial, we'll use the muon-neutrino component of the MINERvA "low energy" flux in neutrino-enhanced mode by default, which is the `numu_fhc` histogram in the file `/opt/generators/NUISANCE/data/flux/minerva_le_flux.root`. There's also a copy in `MC_inputs` which makes it easier for anybody using local versions of ROOT.

You can inspect this file with ROOT if you would like with:
```
apptainer exec nuisance_nustec2026.sif root -l MC_inputs/minerva_le_flux.root
```
or if you're using a local version of root.
```
root -l  MC_inputs/minerva_le_flux.root
```
Then open up a TBrowser with `TBrowser b` and explore the histograms contained through the GUI, i.e.
```
Your command in the shell           --> root -l MC_inputs/minerva_le_flux.root
ROOT saying it's attached the file  --> root [0] Attaching file MC_inputs/minerva_le_flux.root as _file0...
Your command in root                --> root [1] TBrowser b
```

Note that the example scripts below can be easily modified to take different input fluxes or use different targets, but also note that NUISANCE is not generating these events, the event generators are!

Fluxes you might want to play with (included in `MC_inputs` for this tutorial) are:
* MicroBooNE BNB flux: `MicroBooNE_FHC_numu_TN1031.root`
  + `numu_hist`: $\nu_{\mu}$ BNB flux
* T2K ND280 (anti)neutrino-enhanced: `t2kflux_2016_plus250kA.root` (`t2kflux_2016_minus250kA.root`)
  + `enu_nd280_numu`: $\nu_{\mu}$ flux at the ND280
  + `enu_nd280_numub`: $\bar{\nu}_{\mu}$ flux at the ND280
  + `enu_nd280_nue`: $\nu_{e}$ flux at the ND280
  + `enu_nd280_nueb`: $\bar{\nu}_{e}$ flux at the ND280  
* DUNE near detector: `DUNE_OptimizedEngineeredNov2017_REGULAR.root`
  + `numu_NDFHC_flux`: $\nu_{\mu}$ flux at the near detector in neutrino-enhanced mode
  + `numubar_NDFHC_flux`: $\bar{\nu}_{\mu}$ flux at the near detector in neutrino-enhanced mode
  + `nue_NDFHC_flux`: $\nu_{e}$ flux at the near detector in neutrino-enhanced mode
  + `nuebar_NDFHC_flux`: $\bar{\nu}_{e}$ flux at the near detector in neutrino-enhanced mode
  + Replace `FHC` for `RHC` for anti-neutrino-enhanced mode
* MINERvA LE:
  + `numu_fhc`: $\nu_{\mu}$ flux at the near detector in neutrino-enhanced mode
  + `numubar_fhc`: $\bar{\nu}_{\mu}$ flux at the near detector in neutrino-enhanced mode
  + `nue_fhc`: $\nu_{e}$ flux at the near detector in neutrino-enhanced mode
  + `nuebar_fhc`: $\bar{\nu}_{e}$ flux at the near detector in neutrino-enhanced mode
  + Replace `fhc` for `rhc` for anti-neutrino-enhanced mode
* DUNE far detector, including NuFit5.2 (NO, w/o SK): `DUNE_FHC(RHC)_NuFit5NO.root` for FHC (RHC)
  + `numu_FDFHC_flux`: unoscillated $\nu_{\mu}$ flux
  + `numu_FDFHC_flux_osc`: oscillated $\nu_{\mu} \rightarrow \nu_{\mu}$ flux
  + `nue_FDFHC_flux_osc`: oscillated $\nu_{\mu} \rightarrow \nu_{e}$ flux (other contributions neglected)
  + `numubar_FDFHC_flux`: unoscillated $\bar{\nu}_{\mu}$ flux
  + `numubar_FDFHC_flux_osc`: oscillated $`\bar{\nu}_{\mu} \rightarrow \bar{\nu}_{\mu}`$ flux
  + `nuebar_FDFHC_flux_osc`: oscillated $`\bar{\nu}_{\mu} \rightarrow \bar{\nu}_{e}`$ flux (other contributions neglected)
  + Replace `FHC` for `RHC` for the anti-neutrino-enhanced mode fluxes
* T2K/HK far detector, including NuFit5.2 (NO, w/o SK): `T2KSK_FHC(RHC)_NuFit5NO.root` for FHC (RHC)
  + `sk_numu_unosc`: unoscillated $\nu_{\mu}$ flux
  + `sk_numu_osc`: oscillated $\nu_{\mu} \rightarrow \nu_{\mu}$ flux
  + `sk_nue_osc`: oscillated $\nu_{\mu} \rightarrow \nu_{e}$ flux (other contributions neglected)
  + `sk_numubar_unosc`: unoscillated $\bar{\nu}_{\mu}$ flux
  + `sk_numubar_osc`: oscillated $`\bar{\nu}_{\mu} \rightarrow \bar{\nu}_{\mu}`$ flux
  + `sk_nuebar_osc`: oscillated $`\bar{\nu}_{\mu} \rightarrow \bar{\nu}_{e}`$ flux (other contributions neglected)
  + Fluxes have the same names for the anti-neutrino-enhanced mode flux file
Note that *some* cross-section results have custom flux tunings and don't use these "nominal" fluxes. 

Finally, there are copies of the files generated in this tutorial available here: https://portal.nersc.gov/project/nuisance/tutorial/

If you need, you can download them all from there, or with the command:
```
wget -nH -np -r --cut-dirs 3 https://portal.nersc.gov/project/nuisance/tutorial/ MC_outputs
```

</details>

### GENIE

[GENIE](http://www.genie-mc.org/) is the most widely used neutrino interaction simulation packages currently used in the field. It can simulate neutrino energies from MeV to PeV scales, and also has support for electron-nucleus scattering, photon-nucleus scattering, pion-nucleus scattering, and even provides support for simulating various dark matter models.

<details>
  
<summary><h4>GENIE event generation</h4></summary>

In this project, we will focus on neutrino interactions, for which the simplest event generation tool in GENIE is `gevgen`, which is fully documented [here](https://genie-docdb.pp.rl.ac.uk/DocDB/0000/000002/006/man.pdf) or (if the recent PHP issues mean the latter is still behind a firewall) [here](https://arxiv.org/abs/1510.05494).

An example script to generate 100k muon neutrino-hydrocarbon interactions using the MINERvA LE flux is given in `generation_GENIEv3_example.sh` using the `AR23_20i_00_000` model, which has been developed by DUNE and is now being utilized by multiple experiments. Run it with a command like (which should take 5-10 minutes):
```
apptainer exec nuisance_nustec2026.sif /bin/bash generation_GENIEv3_example.sh
```
There will be a few output files produced:
* `GENIEv3_AR23_MINERvA_LE_FHC_numu.root`, the gevgen output in GENIE's GHEP format, requires the correct libraries to read
* `GENIEv3_AR23_MINERvA_LE_FHC_numu_NUIS.root`: the output from NUISANCE's `PrepareGenie`, which adds the flux and total event rate histograms to make normalization easier
* `GENIEv3_AR23_MINERvA_LE_FHC_numu_NUISFLAT.root`: the output from NUISANCE's `nuisflat` application, which makes a ROOT flat tree with a simplified format that doesn't require any libraries to be read.
we'll use the second two of these output files later on in the tutorial.

If you look at the script, you will see that the `NU_PDG` and `TARG` variables are numeric codes, which follow the Particle Data Group's (PDG) convention for labeling particles: https://pdg.lbl.gov/2019/reviews/rpp2019-rev-monte-carlo-numbering.pdf. The PDG code for a muon neutrino is 14, so you can see that in this example, we're using an incident muon neutrino. The PDG code for a carbon nucleus is 1000060120, and for a hydrogen nucleus is 1000010010. In this example, we have `TARG=1000060120[0.9231],1000010010[0.0769]`. The actual target is C_8H_8, so 12/13 carbon and 1/13 hydrogen by the number of nucleons. All generators use the same PDG conventions for labeling particles, you get used to it.

Also, there is a `--cross-section` argument to `gevgen`, which takes a mysterious file in `MC_inputs`. This file includes pre-calculated "splines" as a function of true neutrino energy, broken down into all of the processes that have been included in the GENIE model, for a variety of targets and neutrino flavors. You can use a GENIE utility `gspl2root` to extract these splines into ROOT TGraphs, for easy plotting or inspection. An example command is:
```
apptainer exec nuisance_nustec2026.sif gspl2root -f MC_inputs/AR23_20i_00_000_splines.xml.gz -p 14,-14 -t 1000060120 -e 10 --tune AR23_20i_00_000 -o AR23_20i_00_000_splines.root
```
which can be explored with:
```
apptainer exec nuisance_nustec2026.sif root -l AR23_20i_00_000_splines.root
```
For full details on `gspl2root`, and how to make new splines (using `gmkspl`), refer to the [GENIE documentation](https://arxiv.org/abs/1510.05494) and the [GENIE website](https://genie-mc.github.io/index.html).

A few different spline files have been included for different available GENIE "tunes", which you can try to use to compare and contrast the differences:
* `AR23_20i_00_000`: `AR23_20i_00_000_splines.xml.gz`
* `CRPA21_04a_00_000`: `CRPA21_04a_00_000_splines.xml.gz`
* `G18_10a_00_000`: `G18_10a_00_000_splines.xml.gz`
* `G21_11a_00_000`: `G21_11a_00_000_splines.xml.gz`
In these files, splines are available for the targets hydrogen, carbon, oxygen and argon, for all neutrino flavors.

For full information on the physics content of the tunes, refer to https://genie-mc.github.io/tunes.html, where you can also find details on other available tunes!

</details>

### NuWro

[NuWro](https://nuwro.github.io/user-guide/) is a neutrino event generator which is developed to be flexible and include as many theory model options as possible, reflecting the main interests of the development group. As so many options are configurable, many more options need to be specified to generate events. Documentation is available on the NuWro website: https://nuwro.github.io/user-guide/getting-started/running/

<details>

<summary><h4>NuWro event generation</h4></summary>

An example script for producing 100k muon neutrino-hydrocarbon interactions for the MINERvA low energy neutrino-enhanced flux is given in `generation_NUWRO_example.sh`, which uses parameters set in `MC_inputs/MINERvA_LE_numu_NUWROv25.11.1.params`, and can be run with the following command (which should take ~1 minute):

```
apptainer exec nuisance_nustec2026.sif /bin/bash generation_NUWRO_example.sh
```

Nuwro produces a small number of files which are useful for debugging, but the two files we will use here are:
* `NUWROv25.11.1_MINERvA_LE_FHC_numu.root`, this is the nuwro output, which needs correct libraries to *fully* read, although native ROOT can probably do okay because the structure is relatively simple
* `NUWROv25.11.1_MINERvA_LE_FHC_numu_NUISFLAT.root`: the output from NUISANCE's `nuisflat` application, which makes a ROOT flat tree with a simplified format that doesn't require any libraries to be read.

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
apptainer exec  nuisance_nustec2026.sif ls -l /opt/generators/nuwro/data/target
apptainer exec  nuisance_nustec2026.sif cat /opt/generators/nuwro/data/target/CH.txt
```

</details>

### NEUT
[NEUT](https://arxiv.org/abs/2106.15809) is neutrino generator with a long pedigree, originally written in  pre-standard(!) FORTRAN in the 1980's for the Kamiokande experiment in Japan. It remains the primary generator used by the Super-Kamiokande and T2K collaborations, and has been semi-continuously developed ever since. For recent documentation, see: https://arxiv.org/abs/2106.15809

<details>
  
<summary><h4>NEUT event generation</h4></summary>

NEUT is easy to run and uses a similar input card file to NuWro with all the relevant parameters included, which for this example can be found in `MC_inputs/MINERvA_LE_numu_NEUT.card`. Generate 100k muon neutrino-hydrocarbon interactions for the MINERvA low energy neutrino-enhanced flux with the following command (which should take 5-10 minutes):
```
apptainer exec nuisance_nustec2026.sif /bin/bash generation_NEUT_example.sh
```
Note that the arguments are positional and NEUT doesn't take flags. The output of NEUT is automatically ready for NUISANCE, so no additional `Prepare` step is required.

The NEUT generation script above produces a few files, but the two files we will use here are:
* `NEUT592_MINERvA_LE_FHC_numu.root`, this is the `neutroot2` output, which needs correct libraries to read. No `PrepareX` step is necessary as NEUT already stores the histograms used for normalization in NUISANCE (which was originally developed for T2K...).
* `NEUT592_MINERvA_LE_FHC_numu_NUISFLAT.root`: the output from NUISANCE's `nuisflat` application, which makes a ROOT flat tree with a simplified format that doesn't require any libraries to be read.

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

</details>

### GiBUU
[GiBUU](https://gibuu.hepforge.org/trac/wiki) is a multi-purpose, theory-focused, event generator which can simulate neutrino-, electron-, photon- and hadron-nucleus interactions, as well as heavy ion collisions, written in Fortran. The stand-out feature of GiBUU for neutrino-interaction studies is a consistent and sophisticated treatment of the nucleus, most notably in final-state interactions, in which the evolution of all particles is calculated in discrete time steps, with every particle feeling the potential of each other, as well as the excited nuclear remnant. However, as with all generators, a few sentence summary will not do it justice, and details can be found in [recent GiBUU tutorials](https://gibuu.hepforge.org/trac/wiki/tutorial19).

<details>

  <summary><h4>GiBUU event generation</h4></summary>
  
GiBUU is fast and easy to run, but differs from other event generators in a number of ways. Like NEUT and NuWro, it uses an input (job) file which has a wealth of available options. An arbitrary flux can be specified, however, each job is only capable of producing interactions between a single incident neutrino flavour and nuclear target, and is limited to either charged- or neutral-current interactions. Multiple jobs have to be combined to overcome this, which introduces a little bit of extra bookkeeping.

An example generation script is provided, which generates $\nu_{\mu}$ interactions on both carbon and hydrogen separately, for the MINERvA low energy neutrino-enhanced flux. The output files are then combined into a single file with NUISANCE in the last step. It should take ~10 minutes to run.
```
apptainer exec nuisance_nustec2026.sif /bin/bash generation_GiBUU_example.sh
```
Several other differences are probably apparent. Unlike the other generators, GiBUU does not accept an argument specifying the number of particles to generate. Additionally, if you look at the output NUISANCE flat tree, the `InputWeights`, which are pass-through from the generator, are not 1. These are related. GiBUU does not carry out the usual accept-reject methods that the other generators use. Instead, to over-simplify, it tries to generate a certain number of events which are the `num_ensembles*A`, and for each process that occurs whilst simulating that full event, including the particle transport through the nucleus, it picks up a factor, the product of which translate to an overall event probability. Events which are blocked are simply dropped, leading to a non-fixed number of interactions for any job card.

It is interesting to look at the distribution of weights produced by GiBUU. A simple script to do so, which splits the weight into the carbon and hydrogen events, can be run with:
```
apptainer exec nuisance_nustec2026.sif python3 gibuu_weight_plotter.py MC_outputs/GiBUU_MINERvA_LE_FHC_numu_20k_NUISFLAT.root
```
The histogram produced in `gibuu_weights.pdf` shows some interesting features. One obvious feature is how broad the range of weights produced is, and how heavy-tailed the distribution is. One interesting metric is the Kish effective sample size, ESS = $(\Sigma w_{i})^2/\Sigma w_{i}^2$, which is included in the legend. However, this is a little misleading as the bins in which very large weights fall will have a much lower effective sample size. Overall the effect of these weights is to make it difficult to make smooth GiBUU predictions.

</details>

## NUISANCE

[NUISANCE](https://nuisance.hepforge.org) is a conceptually simple package that takes the output of event generators, converts them internally into a common format, and compares the generator predictions to measured data and other generators. 

The advantage is that the type of input is opaque to the user (e.g., the different generator outputs don't need to be understood in detail), and the relevant selection cuts are applied to the generated events to reflect the measurement of interest. 

The uncertainty (and covariances) from the publications are used to calculate the degree of compatibility between the data and Monte Carlo simulation, in the form of a chi-square, and the output is given in a root file, containing the data and the prediction, along with other plots which can help diagnose issues.

NUISANCE can be linked directly to the event reweighting packages provided by the generators (more on that later), if they are available. 

There are also tools for implementing simple event reweighting independent of the generators, so that the goodness of fit between data and simulation can be assessed for different parameter sets with parameters from different sources. 

Finally, NUISANCE also provides an interface to parameter fitting methods so that model parameters can be varied in a fit to data, and the best fit solution and associated uncertainties can be extracted, using any desired ensemble of datasets.

### Using the NUISANCE flat trees
[See the separate readme in the scripts directory](scripts/README.md)

<details>

<summary><h3>Making simple data-MC comparisons</h3></summary>

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

</details>

<details>
 
<summary><h3>Analyzing the NUISANCE output</h3></summary>

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

</details>

<details>

<summary><h3>Event reweighting</h3></summary>

All of the generators discussed in this tutorial have variable parameters, which can be modified when running the simulation. However, once you generate some events, it will be obvious that this is a rather slow process, and if you want to explore the parameter space in a meaningful way, it would be very computationally expensive to re-generate a large number of events at each set of parameters of interest.

Event reweighting is a way to overcome this issue in a computationally efficient way. It is widely used, but has strong limitations. The procedure is relatively simple. If you have a set of events produced with some nominal parameter set, and want to approximate a set of events with some alternative parameter set, for each event, you calculate the probability for that event to have been produced with the nominal and alternative parameter set. Then, you take the ratio of the alternative probability divided by the nominal probability and obtain a "weight", reflecting how much more or less probable that event would be with the alternative, rather than nominal, parameter set. Then, the distribution of weighted events will look the same as the distribution of events had they been produced with the alternative parameter set.

Event reweighting packages are available inside NEUT and GENIE, and are actively developed additional. Additionally, experimental collaborations often implement additional uncertainties through custom reweighting packages which may or may not be open source. NUISANCE itself also has a mechanism for implementing new  (but simple) uncertainties.

Although commonly used, a fundamental limitation of reweighting is that regions of phase space which are not occupied in the initial model cannot be reweighted into existence.

</details>

<details>

<summary><h3>Varying parameters</h3></summary>

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
</details>

<details>

<summary><h3>Fitting parameters</h3></summary>

NUISANCE is also able to vary specified parameters and find the values of those parameters that minimizes the $\chi^{2}$ test-statistic with respect to a desired set of samples. It will also calculate the postfit covariance matrix, and produce the best fit histograms, as well as storing a lot of information about the fit. The fitting itself is carried out by MINUIT by default. Again, NUISANCE isn't re-inventing the wheel!

To include parameters in the fit, they must be added to the NUISANCE card file with the format:
```
 <parameter name="MaCCRES" nominal="0" type="genie_parameter"low="-1" high="5" step="0.1" state="FREE" />
```
This adds a few more fields to the parameter line(s) of the card file with respect to previous examples:
* low: the minimum value that the parameter is allowed to take in the fit
* high: the maximum value that the parameter is allowed to take in the fit
* step: gives the fitter a hint about how large the expected variation should be, but it will re-optimize the step size itself very quickly
* state: can be "FREE", meaning it varies in the fit, or "FIX" (default) meaning it is not

Again, you can add any number of parameters and samples to the fit, but the more there are, the more complex the computational problem, and the longer the fit will take to converge. As such, it's challenging to predict how long a fit will take a priori. Note that for each iteration of the fit, NUISANCE by default will loop over all events in all of the input files required to process the samples, and calculate a weight for each. It will skip events that don't enter any of the requested samples after the first iteration for speed.

To run a fit with a desired card file, using the `nuismin` program, instead of the usual `nuiscomp` used so far. An example card file for running a simple fit with a single free GENIE parameter `MaCCRES`, and a single MINERvA data sample, is given in `fit_GENIEv3_example.card`, which can be processed with:
```
apptainer exec nuisance_nustec2026.sif nuismin -c fit_GENIEv3_example.card -o fit_GENIEv3_example.root
```

The output file `fit_GENIEv3_example.root` contains a lot of the same information we saw in the simpler NUISANCE output files, with some additions/modifications:
* `<sample_name>_MC`: as before, but it now contains the best-fit histogram!
* `fit_iterations`: a ROOT TTree containing entries for each iteration of the fit, including the chi-square for each sample included and the parameter values
* `fit_result`: a ROOT TTree with summary information describing the fit configuration and the best-fit information
* `covariance` (`covariance_free`): the covariance matrix between postfit parameters (with fixed parameters removed)
* `correlation` (`correlation_free`): the correlation matrix between postfit parameters (with fixed parameters removed)
* `decomposition` (`decomposition_free`): the Cholesky decomposition of the covariance matrix between postfit parameters (with fixed parameters removed)
* +Various summary histograms which are simple copies of information in the `fit_result` tree

where `<sample_name>` is `MINERvA_CC1pip_XSec_1DTpi_nu_2017`, in this example.

</details>
