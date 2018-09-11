#!/bin/sh
cosmicraysim -g simpletracker.geo -n 100000
python analysis.py