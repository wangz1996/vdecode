# An example python script for making a tidy plot with several lines on it
# I'm using TProfiles because that's what is in the TIDA output, but 
# the standard histogram is a "TH1F" and the exact same commands work for it.
# If you want to use graphs instead of histograms you can check out
# the other example script in here.

import ROOT
import math
import sys
args = sys.argv

# Imports ATLAS style for plotting
# You have to have set it up first (see README for instructions)
# You can run it without this but it will have an ugly stats box and so on
# that you'd have to turn off manually.
import VPStyle
ROOT.SetVPStyle()
ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError

def plot_sub(fname):
  # Load some histos from the example file
  # (Gaussian limits from 2016 TLA conf)
  name = fname.rsplit('_',2)[0].split('-',1)[1]
  mode = ["hmh","hml","hbh","hbl"]
  leg_dist = {
    "hmh":"Main HG",
    "hml":"Main LG",
    "hbh":"Back HG",
    "hbl":"Back LG"
  }
  mode_hist = {
    "hmh":[],
    "hml":[],
    "hbh":[],
    "hbl":[]
  }
  infile = ROOT.TFile.Open(fname,"READ")
  for i in range(0,25):
    for m in mode:
      histo = infile.Get(m+"_"+str(i))
      histo.SetDirectory(0)
      # if(histo.Integral()>0.):
      #   histo.Scale(1./histo.Integral())
      mode_hist[m].append(histo)

  mini=9999.
  maxi=0.
  # Close the input file

  # Make a canvas to put the plot on.
  # We don't want log axes for this plot, 
  # but if you do you can control them here.
  c = ROOT.TCanvas("canvas",'',0,0,1500,1500)
  c.Divide(5,5)
  c.SetLogx(True)
  c.SetLogy(True)
  c.SetGridx(0)
  c.SetGridy(0)

  # Decide what x and y range to use in the display.
  xRange = [-3.2,3.2]
  yRange = [0.9,1.1]

  # Decide what colours to use.
  # These ones look decent, but obviously use
  # whatever you like best.
  goodColours = [ROOT.kCyan+2,ROOT.kBlue+1,ROOT.kMagenta+1,ROOT.kOrange,ROOT.kBlack]

  # Make a legend.
  # These are the locations of the left side, bottom side, right
  # side, and top, as fractions of the canvas.
  legend = []

  # Draw each histogram.
  # You really shouldn't put two histograms with different
  # x axes on the same plot - I'm only doing it here
  # to show you how to draw multiple plots on the same
  # canvas.
  for m in mode:
    histos = mode_hist[m]
    findex = mode.index(m)
    for histo in histos :

      index = histos.index(histo)
      colour = goodColours[findex]

      # Set up marker to look nice
      histo.SetMarkerColor(colour)
      histo.SetMarkerSize(0)  
      histo.SetMarkerStyle(0)

      # Set up line to look nice
      histo.SetLineColor(colour)
      histo.SetLineWidth(3)
      histo.SetLineStyle(1)

      # Make sure we don't get a fill
      histo.SetFillColor(0)

      # Label my axes!!
      histo.GetXaxis().SetTitle("ADC")
      histo.GetYaxis().SetTitle("Fraction")
      # Move the label around if you want
      histo.GetYaxis().SetTitleOffset(1.5)

      # Set the limit for the axes
      histo.GetXaxis().SetRangeUser(0.1,1e8)
      histo.GetYaxis().SetRangeUser(0.1,1e4)
      histo.SetTitle(str(index))
      c.cd(index+1)
      ROOT.gPad.SetLogy(True)
      ROOT.gPad.SetLogx(True)
      if findex==0 :
        histo.Draw("H") # Draw data points (you'll get error bars by default)
      else :
        histo.Draw("H same") # SAME means don't get rid of the previous stuff on the canvas
      legend.append(ROOT.TLegend(0.6,0.52,0.92,0.92))
      # Make the text a nice fond, and big enough
      legend[index].SetTextFont(42)
      legend[index].SetTextSize(0.05)
      # A few more formatting things .....
      legend[index].SetBorderSize(0)
      legend[index].SetLineColor(0)
      legend[index].SetLineStyle(1)
      legend[index].SetLineWidth(3)
      legend[index].SetFillColor(0)
      legend[index].SetFillStyle(0)
      legend[index].AddEntry(histo,leg_dist[m])
      legend[index].Draw()

  # Save the output as a .eps, a .C, and a .root
  c.SaveAs("Mag_"+name+".png")
def plot_all(fname):
  # Load some histos from the example file
  # (Gaussian limits from 2016 TLA conf)
  name = fname.rsplit('_',2)[0].split('-',1)[1]
  infile = ROOT.TFile.Open(fname,"READ")
  #infile = ROOT.TFile.Open("LED-4.0V-100Hz-500ns-FEE3-4_20250413_1846.root","READ")
  histos = []
  legendLines = []
  hname = []

  mini=9999.
  maxi=0.
  histo1 = infile.Get("hmh")
  histo1.SetDirectory(0)
  # histo1.scale(1./histo1.Integral())
  histos.append(histo1)
  legendLines.append("Main HG")

  histo2 = infile.Get("hml")
  histo2.SetDirectory(0)
  # histo2.scale(1./histo2.Integral())
  histos.append(histo2)
  legendLines.append("Main LG")

  histo3 = infile.Get("hbh")
  histo3.SetDirectory(0)
  # histo3.scale(1./histo3.Integral())
  histos.append(histo3)
  legendLines.append("Back HG")

  histo4 = infile.Get("hbl")
  histo4.SetDirectory(0)
  # histo4.scale(1./histo4.Integral())
  histos.append(histo4)
  legendLines.append("Back LG")
  # Close the input file
  infile.Close()

  # Make a canvas to put the plot on.
  # We don't want log axes for this plot, 
  # but if you do you can control them here.
  c = ROOT.TCanvas("canvas",'',0,0,1024,768)
  c.SetLogx(True)
  c.SetLogy(True)
  c.SetGridx(0)
  c.SetGridy(0)

  # Decide what x and y range to use in the display.
  xRange = [-3.2,3.2]
  yRange = [0.9,1.1]

  # Decide what colours to use.
  # These ones look decent, but obviously use
  # whatever you like best.
  goodColours = [ROOT.kCyan+2,ROOT.kBlue+1,ROOT.kMagenta+1,ROOT.kOrange,ROOT.kBlack]
  legend = ROOT.TLegend(0.2,0.52,0.52,0.82)
  # Make the text a nice fond, and big enough
  legend.SetTextFont(42)
  legend.SetTextSize(0.04)
  # A few more formatting things .....
  legend.SetBorderSize(0)
  legend.SetLineColor(0)
  legend.SetLineStyle(1)
  legend.SetLineWidth(3)
  legend.SetFillColor(0)
  legend.SetFillStyle(0)
  # Make a legend.
  # These are the locations of the left side, bottom side, right
  # side, and top, as fractions of the canvas.

  # Draw each histogram.
  # You really shouldn't put two histograms with different
  # x axes on the same plot - I'm only doing it here
  # to show you how to draw multiple plots on the same
  # canvas.
  for histo,line in zip(histos,legendLines):

    index = histos.index(histo)
    colour = goodColours[index]

    # Set up marker to look nice
    histo.SetMarkerColor(colour)
    histo.SetMarkerSize(0)  
    histo.SetMarkerStyle(0)

    # Set up line to look nice
    histo.SetLineColor(colour)
    histo.SetLineWidth(3)
    histo.SetLineStyle(1)

    # Make sure we don't get a fill
    histo.SetFillColor(0)

    # Label my axes!!
    histo.GetXaxis().SetTitle("ADC")
    histo.GetYaxis().SetTitle("Events/bin")
    # Move the label around if you want
    histo.GetYaxis().SetTitleOffset(1.5)

    # Set the limit for the axes
    histo.GetXaxis().SetRangeUser(1e0,1e8)
    histo.GetYaxis().SetRangeUser(1e-1,1e4)

    if index==0 :
      histo.Draw("H") # Draw data points (you'll get error bars by default)
    else :
      histo.Draw("H same") # SAME means don't get rid of the previous stuff on the canvas
    # Fill entry into legend
    # "PL" means both the line and point style
    # will show up in the legend.
    legend.AddEntry(histo,line,"PL")

  # # Actually draw the legend
  legend.Draw()

  # This is one way to draw text on the plot
  myLatex = ROOT.TLatex()
  myLatex.SetTextColor(ROOT.kBlack)
  myLatex.SetNDC()

  # Put an VLAST-P Internal label
  # I think it has to be Helvetica
  myLatex.SetTextSize(0.05)
  myLatex.SetTextFont(72)
  # These are the x and y coordinates of the bottom left corner of the text
  # as fractions of the canvas
  myLatex.DrawLatex(0.18,0.88,"VLAST-P")
  # Now we switch back to normal font for the "Internal"
  myLatex.SetTextFont(42)
  myLatex.DrawLatex(0.35,0.88,name)

  # Update the canvas
  c.Update()

  # Save the output as a .eps, a .C, and a .root
  c.SaveAs("MagAll_"+name+".png")

def plot_hl(fname):
  # Load some histos from the example file
  # (Gaussian limits from 2016 TLA conf)
  name = fname.rsplit('_',2)[0].split('-',1)[1]
  infile = ROOT.TFile.Open(fname,"READ")
  #infile = ROOT.TFile.Open("LED-4.0V-100Hz-500ns-FEE3-4_20250413_1846.root","READ")
  histos = []
  legendLines = []
  hname = []

  mini=9999.
  maxi=0.
  histo1 = infile.Get("hh")
  histo1.SetDirectory(0)
  # histo1.scale(1./histo1.Integral())
  histos.append(histo1)
  legendLines.append("HG")

  histo2 = infile.Get("hl")
  histo2.SetDirectory(0)
  # histo2.scale(1./histo2.Integral())
  histos.append(histo2)
  legendLines.append("LG")
  infile.Close()

  # Make a canvas to put the plot on.
  # We don't want log axes for this plot, 
  # but if you do you can control them here.
  c = ROOT.TCanvas("canvas",'',0,0,1024,768)
  c.SetLogx(True)
  c.SetLogy(True)
  c.SetGridx(0)
  c.SetGridy(0)

  # Decide what x and y range to use in the display.
  xRange = [-3.2,3.2]
  yRange = [0.9,1.1]

  # Decide what colours to use.
  # These ones look decent, but obviously use
  # whatever you like best.
  goodColours = [ROOT.kCyan+2,ROOT.kBlue+1,ROOT.kMagenta+1,ROOT.kOrange,ROOT.kBlack]
  legend = ROOT.TLegend(0.2,0.52,0.52,0.82)
  # Make the text a nice fond, and big enough
  legend.SetTextFont(42)
  legend.SetTextSize(0.04)
  # A few more formatting things .....
  legend.SetBorderSize(0)
  legend.SetLineColor(0)
  legend.SetLineStyle(1)
  legend.SetLineWidth(3)
  legend.SetFillColor(0)
  legend.SetFillStyle(0)
  # Make a legend.
  # These are the locations of the left side, bottom side, right
  # side, and top, as fractions of the canvas.

  # Draw each histogram.
  # You really shouldn't put two histograms with different
  # x axes on the same plot - I'm only doing it here
  # to show you how to draw multiple plots on the same
  # canvas.
  for histo,line in zip(histos,legendLines):

    index = histos.index(histo)
    colour = goodColours[index]

    # Set up marker to look nice
    histo.SetMarkerColor(colour)
    histo.SetMarkerSize(0)  
    histo.SetMarkerStyle(0)

    # Set up line to look nice
    histo.SetLineColor(colour)
    histo.SetLineWidth(3)
    histo.SetLineStyle(1)

    # Make sure we don't get a fill
    histo.SetFillColor(0)

    # Label my axes!!
    histo.GetXaxis().SetTitle("ADC")
    histo.GetYaxis().SetTitle("Events/bin")
    # Move the label around if you want
    histo.GetYaxis().SetTitleOffset(1.5)

    # Set the limit for the axes
    histo.GetXaxis().SetRangeUser(1e0,1e8)
    histo.GetYaxis().SetRangeUser(1e-1,1e4)

    if index==0 :
      histo.Draw("H") # Draw data points (you'll get error bars by default)
    else :
      histo.Draw("H same") # SAME means don't get rid of the previous stuff on the canvas
    # Fill entry into legend
    # "PL" means both the line and point style
    # will show up in the legend.
    legend.AddEntry(histo,line,"PL")

  # # Actually draw the legend
  legend.Draw()

  # This is one way to draw text on the plot
  myLatex = ROOT.TLatex()
  myLatex.SetTextColor(ROOT.kBlack)
  myLatex.SetNDC()

  # Put an VLAST-P Internal label
  # I think it has to be Helvetica
  myLatex.SetTextSize(0.05)
  myLatex.SetTextFont(72)
  # These are the x and y coordinates of the bottom left corner of the text
  # as fractions of the canvas
  myLatex.DrawLatex(0.18,0.88,"VLAST-P")
  # Now we switch back to normal font for the "Internal"
  myLatex.SetTextFont(42)
  myLatex.DrawLatex(0.35,0.88,name)

  # Update the canvas
  c.Update()

  # Save the output as a .eps, a .C, and a .root
  c.SaveAs("MagHL_"+name+".png")

if __name__ == "__main__":
  plot_all(args[1])
  plot_sub(args[1])
  plot_hl(args[1])
