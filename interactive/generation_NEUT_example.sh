#!/bin/bash

## The output file name
OUTFILE=MC_outputs/NEUT570_MINERvA_LE_FHC_numu.root

## The file that sets physics parameters for the NEUT simulation
INCARD=${PWD}/MC_inputs/MINERvA_LE_numu_NEUT.card

## This is NEUT's event generation application
echo "Starting neutroot2..."
neutroot2 ${INCARD} ${OUTFILE} &> /dev/null

## Only one NUISANCE step is required for NEUT
echo "Creating NUISANCE flat trees"
nuisflat -f GenericVectors -i NEUT:${OUTFILE} -o ${OUTFILE/.root/_NUISFLAT.root}

## Clean up the files NEUT creates when generating events
rm *_o.root
