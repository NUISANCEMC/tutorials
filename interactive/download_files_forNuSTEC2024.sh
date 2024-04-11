#!/bin/bash
# Download NUISANCE flat tree files for NuSTEC 2024 tutorial
# https://indico.fnal.gov/event/59963/contributions/288526/

# NAME is the T2K file
# NAME2 is the DUNE file

# Where files should be saved locally
save_dir="MC_outputs"
mkdir -p ${save_dir}
cd ${save_dir}
opts="-r -l1 --no-directories --no-parent https://portal.nersc.gov/project/nuisance/IOP_review_2023"

################################
GEN=GENIEv3_CRPA21_04a_00_000
NAME=T2KND_FHC_numu_C8H8_GENIEv3_CRPA21_04a_00_000_1M_0000_NUISFLAT.root
NAME2=DUNEND_FHC_numu_Ar40_GENIEv3_CRPA21_04a_00_000_1M_0000_NUISFLAT.root

wget $opts/$GEN/$NAME
wget $opts/$GEN/$NAME2

################################
GEN=GENIEv3_G18_10a_00_000
NAME=T2KND_FHC_numu_C8H8_GENIEv3_G18_10a_00_000_1M_0000_NUISFLAT.root
NAME2=DUNEND_FHC_numu_Ar40_GENIEv3_G18_10a_00_000_1M_0000_NUISFLAT.root

wget $opts/$GEN/$NAME
wget $opts/$GEN/$NAME2

################################
GEN=NEUT562
NAME=T2KND_FHC_numu_C8H8_NEUT562_1M_0000_NUISFLAT.root
NAME2=DUNEND_FHC_numu_Ar40_NEUT562_1M_0000_NUISFLAT.root

wget $opts/$GEN/$NAME
wget $opts/$GEN/$NAME2

################################
GEN=NUWRO_LFGRPA
NAME=T2KND_FHC_numu_C8H8_NUWRO_LFGRPA_1M_0000_NUISFLAT.root
NAME2=DUNEND_FHC_numu_Ar40_NUWRO_LFGRPA_1M_0000_NUISFLAT.root

wget $opts/$GEN/$NAME
wget $opts/$GEN/$NAME2

