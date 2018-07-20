from ROOT import *
infile = TFile("trackfit.discriminator.root","READ")
hist = infile.Get("hist50")
cont = infile.Get("histNS")
n = hist.GetNbinsZ()

maxs = cont.Clone()
maxs.Divide(hist)
maxv = maxs.GetMaximum()
minv = maxs.GetMinimum()

templ = hist.Project3D("yx");
templ.Reset()
for i in range(n):
    templ.Reset()
    for j in range(hist.GetNbinsX()):
        for k in range(hist.GetNbinsY()):
            ninbin = cont.GetBinContent(j+1,k+1,i+1)
            
#            if (ninbin < 10): continue
            if (hist.GetBinContent(j+1,k+1,i+1) <= 0.0): continue
            val = (hist.GetBinContent(j+1,k+1,i+1))*ninbin

            templ.SetBinContent(j+1,templ.GetNbinsY()-(k+1),  val)

    templ.Draw("COLZ")
#    templ.GetZaxis().SetRangeUser(minv, minv + (maxv-minv)*0.3)
    gPad.Update()
    if i == 0: gPad.Print("smeared.pdf(")
    elif i == n-1: gPad.Print("smeared.pdf)")
    else: gPad.Print("smeared.pdf")
