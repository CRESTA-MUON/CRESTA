TIME="`date +%Y%m%d%H%M%S`"
RAND=$RANDOM
JOBID=$$
ID="${USER}_temp_${TIME}_${RAND}_${JOBID}"

scratchdir="/scratch/$USER/"

# If we have scratch access make dir
if [ -d "$scratchdir" ]
then
  mkdir $scratchdir/$ID
fi

# If scratch dir made then return it, otherwise make temp in current dir
if [ -d "$scratchdir/$ID" ]
then
  echo $scratchdir/$ID
else
  mkdir $PWD/$ID
  echo $PWD/$ID
fi