#!/bin/sh
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Get Inputs
echo "Getting Inputs"
JOB="CRESTA"
RUNID=$1
if [[ "$2" != "" ]]
then
    export USER=$2
fi

# GET OUTDIR and GEOMETRY
echo "Getting directories"
WORKDIR=$SCRIPTDIR
OUTDIR=$WORKDIR/mcfiles/
GEOMETRY=$WORKDIR/simpletracker.geo

# make new temp scratch area
echo "Creating Temp Scratch Area"
scratchdir=$(source $SCRIPTDIR/make_temp_scratch.sh)
cd $scratchdir

# Setup the framework
echo "Setting up code"
source $WORKDIR/simulationsetup.sh

# -------------------------------------------------------------------
# SIZE TESTING
# Run Job (1 hour exposure)
echo "Running"
cosmicraysim -g $GEOMETRY -t 3600 -o cosmicraysim_${JOB}  --run ${RUNID}  -i

# Copy Outputs back to WORK Area
cp -rv ./*.root $OUTDIR/

# Remove temp directory
#rm -rv $scratchdir