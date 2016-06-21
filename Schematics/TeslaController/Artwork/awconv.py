#! python

import os
import sys

#DCConv-F_Cu.gbr
#DCConv-F_Mask.gbr
#DCConv-F_SilkS.gbr
#DCConv-B_Cu.gbr
#DCConv-B_Mask.gbr
#DCConv-B_SilkS.gbr
#DCConv.drl
#DCConv-Edge_Cuts.gbr

command = 'pcbrender.py --cutout=Gerbers/TeslaController-Edge.Cuts.gbr --drill=Gerbers/TeslaController.drl -o Bitmaps -i "copper|silk" --drillsize 0.3'
command += ' --fcopper=Gerbers/TeslaController-F.Cu.gbr --fmask=Gerbers/TeslaController-F.Mask.gbr --fsilk=Gerbers/TeslaController-F.SilkS.gbr'
command += ' --bcopper=Gerbers/TeslaController-B.Cu.gbr --bmask=Gerbers/TeslaController-B.Mask.gbr --bsilk=Gerbers/TeslaController-B.SilkS.gbr'
command += " " + " ".join(sys.argv[1:])

os.system(command)

os.chdir("Bitmaps")

print "Tile Signal Front"
os.system("pcbtiler.py -A4 -x 2 -y 2 -m FrontCopper.png -m FrontCopper.png BackCopper.png BackCopper.png -o TiledSignal.png")

#print "Tile Signal Back"
#os.system("pcbtiler.py -A4 -x 1 -y 2 BackCopper.png BackCopper.png -o TiledSignalBack.png")

print "Tile Solder Mask Front"
os.system("pcbtiler.py -A4 -x 2 -y 2 -m FrontSolderMask.png -m FrontSolderMask.png BackSolderMask.png BackSolderMask.png -o TiledSolderMask.png")

#print "Tile Solder Mask Back"
#os.system("pcbtiler.py -A4 -x 1 -y 2 -m BackSolderMask.png BackSolderMask.png -o TiledSolderMaskBack.png")

#print "Tile Silk Front"
#os.system("pcbtiler.py -A4 -x 1 -y 2 -m FrontSilk.png FrontSilk.png -o TiledSilkFront.png")

#print "Tile Silk Back"
#os.system("pcbtiler.py -A4 -x 1 -y 2 -m BackSilk.png BackSilk.png -o TiledSilkBack.png")
