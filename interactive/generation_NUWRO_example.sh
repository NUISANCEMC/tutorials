#!/bin/bash

## The output file name
OUTFILE=MC_outputs/NUWRO_LFGRPA_MINERvA_LE_FHC_numu.root

## The file that sets all physics parameters for the nuwro simulations
INCARD=${PWD}/MC_inputs/MINERvA_LE_numu_NUWRO_LFGRPA.params

## Flux information required for normalization in the NUISANCE step
## (this information is passed to nuwro through $INCARD)
NU_PDG=14
FLUX_FILE=/opt/nuisance/data/flux/minerva_le_flux.root
FLUX_HIST=numu_fhc

## This is nuwro's event generation application
echo "Starting nuwro..."
nuwro -i ${INCARD} -o ${OUTFILE}

## The next two steps use NUISANCE applications
echo "Starting PrepareNuWroEvents..."
PrepareNuWroEvents ${OUTFILE} -F ${FLUX_FILE},${FLUX_HIST},${NU_PDG}

echo "Creating NUISANCE flat trees"
nuisflat -f GenericVectors -i NuWro:${OUTFILE} -o ${OUTFILE/.root/_NUISFLAT.root}

## Clean up the files NUWRO creates when generating events
rm random_seed q0.txt q2.txt qv.txt totals.txt T.txt ${OUTFILE}.par ${OUTFILE}.txt
