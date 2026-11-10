#!/bin/bash

## GiBUU can cause RAM issues... lower if you have an issue
NUM_ENSEMBLES=20000
OUTFILE=MC_outputs/GiBUU_MINERvA_LE_FHC_numu_20k.root

## Make a flux histogram GiBUU can process...
FLUX_FILE=MC_inputs/minerva_le_flux.root
FLUX_HIST=numu_fhc
python3 MC_inputs/make_gibuu_flux.py ${FLUX_FILE} ${FLUX_HIST} gibuu_flux.dat

## Sort the carbon card
cp MC_inputs/MINERvA_LE_numu_GiBUU.job MINERvA_LE_numu_GiBUU_C12.job
sed -i "s/_PROTONS_/6/g" MINERvA_LE_numu_GiBUU_C12.job
sed -i "s/_NUCLEONS_/12/g" MINERvA_LE_numu_GiBUU_C12.job
sed -i "s/_NUM_ENSEMBLES_/${NUM_ENSEMBLES}/g" MINERvA_LE_numu_GiBUU_C12.job

## Actually make the GiBUU events on carbon
echo "Generating C12 events..."
/opt/generators/GiBUU/GiBUU.x < MINERvA_LE_numu_GiBUU_C12.job &> /dev/null 
PrepareGiBUU -i EventOutput.Pert.00000001.root -f gibuu_flux.dat -o ${OUTFILE/.root/_C12.root}

## Sort the hydrogen card
cp MC_inputs/MINERvA_LE_numu_GiBUU.job MINERvA_LE_numu_GiBUU_H1.job
sed -i "s/_PROTONS_/1/g" MINERvA_LE_numu_GiBUU_H1.job
sed -i "s/_NUCLEONS_/1/g" MINERvA_LE_numu_GiBUU_H1.job
sed -i "s/_NUM_ENSEMBLES_/${NUM_ENSEMBLES}/g" MINERvA_LE_numu_GiBUU_H1.job

## Actually make the GiBUU events on hydrogen
echo "Generating H1 events..."
/opt/generators/GiBUU/GiBUU.x < MINERvA_LE_numu_GiBUU_H1.job &> /dev/null
PrepareGiBUU -i EventOutput.Pert.00000001.root -f gibuu_flux.dat -o ${OUTFILE/.root/_H1.root}

## Need to set the input here depending on whether the hydrogen sample needs to be included...
nuisflat -f GenericVectors -i GiBUU:${OUTFILE/.root/_C12.root},${OUTFILE/.root/_H1.root} -o ${OUTFILE/.root/_NUISFLAT.root} -q "nuisflat_SaveSignalFlags=false"

## Clean up
rm main.run \
   DensTab_target.dat \
   gibuu_flux.dat \
   GiBUU_database.tex \
   GiBUU_database_decayChannels.txt \
   massAssStatus.dat \
   Multiplicity_Ave.dat \
   Multiplicity_Distr.dat \
   neutrino_info.dat \
   neutrino_initialized_energyFlux.dat \
   NucleonVacuumMass.dat \
   PYR.RG \
   sigma.dat \
   MINERvA_LE_numu_GiBUU_C12.job \
   MINERvA_LE_numu_GiBUU_H1.job \
   ReAdjust.PlotPot.010.dat \
   massass_nBody.dat \
   EventOutput.Pert.00000001.root
