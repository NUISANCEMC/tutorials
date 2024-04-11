#!/bin/bash

## The random seed used to generate events. This will use the local time.
## Note that if you generate a large number of events split into many jobs,
## more complex seeding may be required to ensure there are no duplicates
THIS_SEED=0

## The PDG code of the incoming neutrino
NU_PDG=14

## The flux histograms to use
FLUX_FILE=/opt/nuisance/data/flux/minerva_le_flux.root
FLUX_HIST=numu_fhc

## The target material
TARG=1000060120[0.9231],1000010010[0.0769]

## The number of events to generate
NEVENTS=100000

## The maximum and minimum neutrino energies to simulate
E_MIN=0.1
E_MAX=50

## The physics model "tune" to generate events with, and a set of 
## pre-calculated cross-section "splines" used to massively reduce
## The necessary computational time required.
TUNE=AR23_20i_00_000
XSEC_SPLINES=MC_inputs/${TUNE}_v340_splines.xml.gz

## The output file name
OUTFILE=MC_outputs/GENIEv3_AR23_MINERvA_LE_FHC_numu.root

## This uses GENIE's gevgen application
echo "Starting gevgen..."
gevgen -n ${NEVENTS} -t ${TARG} -p ${NU_PDG} \
       --cross-sections ${XSEC_SPLINES} \
       --tune ${TUNE} --seed ${THIS_SEED} \
       -f ${FLUX_FILE},${FLUX_HIST} -e ${E_MIN},${E_MAX} -o ${OUTFILE} &> /dev/null

## The next two steps use NUISANCE applications
echo "Starting PrepareGENIE..."
PrepareGENIE -i $OUTFILE -f ${FLUX_FILE},${FLUX_HIST} \
	     -t $TARG -o ${OUTFILE/.root/_NUIS.root}

echo "Creating NUISANCE flat trees"
nuisflat -f GenericVectors -i GENIE:${OUTFILE/.root/_NUIS.root} -o ${OUTFILE/.root/_NUISFLAT.root}


