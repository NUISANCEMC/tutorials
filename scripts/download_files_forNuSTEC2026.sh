#!/bin/bash

# Download NUISANCE flat-tree files for NuSTEC school 2026 tutorials

# Wednesday (main flat-tree session): https://indico.fnal.gov/event/74282/contributions/351699/
# Tuesday: https://indico.fnal.gov/event/74282/contributions/351695/
# Thursday: https://indico.fnal.gov/event/74282/contributions/351703/

# Where files should be saved locally
save_dir="MC_outputs"
mkdir -p ${save_dir}
cd ${save_dir}
opts="-r -l1 --continue --no-directories --no-parent https://portal.nersc.gov/project/nuisance/MC_IOP_review/experiment"

# The base name of the experiments we want (specifying flux, target and species)
EXPS="T2KND_FHC_numu_C8H8 DUNEND_FHC_numu_Ar40"
# The generator predictions that we want
GENS="GENIEv3_CRPA21_04a_00_000 GENIEv3_G18_10a_00_000 NEUTDCC NUWROv25.3.1 GiBUU"

# Loop over the generator and model combinations
for GEN in ${GENS}; do
  # Loop over the experiments
  for EXP in ${EXPS}; do
    # Only download the first sub-file
    FILE=${EXP}_${GEN}_1M_0000_NUISFLAT.root
    echo "Downloading ${FILE}..."
    wget $opts/$GEN/${FILE}
  done
done

