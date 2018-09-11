from ROOT import *

infile = TFile("cosmicoutput.0.0.root","READ")
infile.ls()
infile.Get("detectorevents")
detectorevents.Show(0)
detectorevents.Draw("tracker_E")
gPad.Update()
raw_input("WAIT")
