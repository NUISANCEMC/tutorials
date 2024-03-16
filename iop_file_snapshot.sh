#!/bin/bash
MODEL=$1
GEN=$2
DATALIST=$(curl --silent https://portal.nersc.gov/project/nuisance/IOP_review_2023/$1/ | grep -o 'href=".*">' | sed 's/href="//;s/\/">//' | sed 's/old//' | sed 's@/project/nuisance@@g' | sed 's@">@@g' | grep 0000_NUISFLAT )

DATAFILE=$(echo $DATALIST | grep $GEN | tr '/' '\n')
echo "Requesting following datafiles:"
echo $DATAFILE
DATAFILE=$(echo "$DATAFILE" | tr '\n' ';')
DATAFILE=${DATAFILE//;/\ }

export IFS=";"
VAR="$DATAFILE"
for DATA in ${VAR//;/ }; do
DATAPATH=https://portal.nersc.gov/project/nuisance/IOP_review_2023/$1/$DATA
DATA=$(echo $DATA | tr -d ' ')
DATAPATH=$(echo $DATAPATH | tr -d ' ')
echo "Request : ($DATA) ($DATAPATH)"


root -l <<EOF 
{ 
int nevtf = 100000; 
std::string filenameb="$DATAPATH"; 
std::string filenamef="$DATA";

std::cout<<"Snapshotting : " << filenameb << " " << filenamef << std::endl; 
TFile* f1 = TFile::Open(filenameb.c_str()); 
std::cout<<"             : -> Grabbing File Headers " << filenameb << std::endl; 
TTree* t = f1->Get<TTree>("FlatTree_VARS"); 
int nevtb = t->GetEntries(); 
ROOT::RDataFrame d( *t ); 
//ROOT::RDF::Experimental::AddProgressBar(d); 
auto d3  = d.Redefine("fScaleFactor", Form("return fScaleFactor*%d/%d", nevtb, nevtf)); 
auto df = d3.Range(nevtf); 
std::cout<<"             : -> Starting pipe to : " << filenamef << std::endl; 
df.Snapshot("FlatTree_VARS", filenamef.c_str()); 
auto f2 = TFile::Open(filenamef.c_str(),"UPDATE"); 
f2->cd(); 
f1->Get<TH1D>("FlatTree_FLUX")->Write(); 
f1->Get<TH1D>("FlatTree_EVT")->Write(); 
f2->Close(); 
} 
EOF

done
