import sys
#from subprocess import subprocess
import os.path
from math import *
from subprocess import call, check_call

lowlat=float(sys.argv[1])
lowlng=float(sys.argv[2])
higlat=float(sys.argv[3])
higlng=float(sys.argv[4])

if (higlat < lowlat):
    temp = higlat
    higlat = lowlat
    lowlat = temp

if (higlng < lowlng):
    temp = higlng
    higlng = lowlng
    lowlng = temp

lowlat=floor( lowlat )
lowlng=float( lowlng )
higlat=ceil(  higlat ) + 1
higlng=ceil(  higlng ) + 1

lat = lowlat
lng = lowlng
tilename = ""
tilelist = []

blacklist = []
with open("../hgt_files/blacklist.txt","r") as f:
    for line in f:
        blacklist.append(line.strip())

print "Downloading files between ", lowlat, higlat, lowlng, higlng

while(lat < higlat):
    lng = lowlng
    while (lng < higlng):
        
        if (lat >= 0): tilename = "N"
        else: tilename = "S"
        latname = str(int(floor(fabs(lat))))
        if (len(latname) == 1): latname = "0" + latname
        tilename += latname

        if (lng>= 0): tilename += "E"
        else: tilename += "W"
        lngname= str(int(floor(fabs(lng))))
        if (len(lngname) < 2): lngname = "0" + lngname
        if (len(lngname) < 3): lngname = "0" + lngname
        tilename += lngname
        
        tilename += ".hgt"
        tilepath = "../hgt_files/" + tilename

        if os.path.isfile(tilepath):
            print "Already got : ", tilepath
        if (tilename not in tilelist and not os.path.isfile(tilepath) and tilename not in blacklist): 
            tilelist.append([tilename,tilepath])

        lng += 1
    lat += 1

for name in tilelist:
    print "Downloading : ", name
    if os.path.isfile(name[0] + ".zip"): call(["rm",name[0] + ".zip"])
    wgetresult = call(["wget","https://dds.cr.usgs.gov/srtm/version2_1/SRTM3/Eurasia/" + name[0] + ".zip"])

    if wgetresult == 8:
        print "--> Cannot find file on server. Adding to blacklist!"
        with open("../hgt_files/blacklist.txt","a") as f:
            f.write(name[0] + "\n")
    else:
        check_call(["unzip",name[0] + ".zip"])
        check_call(["mv",name[0],name[1]])
        check_call(["rm",name[0] + ".zip"])


print "Complete!"
