#!/bin/sh
mkdir logs
mkdir mcfiles
cd logs/
for i in {0..24};
do
    condor_qsub $PWD/../runscript.sh ${i} $USER
done
cd -