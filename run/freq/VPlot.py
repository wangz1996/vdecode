# An example python script for making a tidy plot with several lines on it
# I'm using TProfiles because that's what is in the TIDA output, but 
# the standard histogram is a "TH1F" and the exact same commands work for it.
# If you want to use graphs instead of histograms you can check out
# the other example script in here.

import ROOT
import math

# Imports ATLAS style for plotting
# You have to have set it up first (see README for instructions)
# You can run it without this but it will have an ugly stats box and so on
# that you'd have to turn off manually.
import VPStyle
ROOT.SetVPStyle()
ROOT.gROOT.SetBatch(True)

def plot_sub(name):
  # Load some histos from the example file
  # (Gaussian limits from 2016 TLA conf)
  freq = ["100","500","1k","10k","20k"]
  hist_dict = {
    "100": [],
    "500": [],
    "1k": [],
    "10k": [],
    "20k": []
  }
  infile = []
  for f in range(len(freq)):
    infile.append(ROOT.TFile.Open("7v"+freq[f]+"hz"+".root","READ"))
    for i in range(0,25):
      histo = infile[f].Get(name+"_"+str(i))
      histo.SetDirectory(0)
      if(histo.Integral()>0.):
        histo.Scale(1./histo.Integral())
      hist_dict[freq[f]].append(histo)

  mini=9999.
  maxi=0.
  # Close the input file

  # Make a canvas to put the plot on.
  # We don't want log axes for this plot, 
  # but if you do you can control them here.
  c = ROOT.TCanvas("canvas",'',0,0,1500,1500)
  c.Divide(5,5)
  c.SetLogx(False)
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
  for f in freq:
    histos = hist_dict[f]
    findex = freq.index(f)
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
      #histo.GetXaxis().SetLimits(xRange[0],xRange[1])
      histo.GetYaxis().SetRangeUser(0.001,1.0)
      histo.SetTitle(str(index))
      c.cd(index+1)
      ROOT.gPad.SetLogy(True)

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
      legend[index].AddEntry(histo,str(f))
      legend[index].Draw()

  # Save the output as a .eps, a .C, and a .root
  c.SaveAs("Ped_"+name+".png")
def plot_all():
  # Load some histos from the example file
  # (Gaussian limits from 2016 TLA conf)
  infile = ROOT.TFile.Open("hist.root","READ")
  histos = []
  legendLines = ["Main HG","Main LG","Back HG","Back LG"]
  hname = ["hpmh","hpml","hpbh","hpbl"]

  mini=9999.
  maxi=0.

  for i in hname:
    histo = infile.Get(i)
    histo.SetDirectory(0)
    histo.Scale(1./histo.Integral())
    histos.append(histo)


  # Make a canvas to put the plot on.
  # We don't want log axes for this plot, 
  # but if you do you can control them here.
  c = ROOT.TCanvas("canvas",'',0,0,1024,768)
  c.SetLogx(False)
  c.SetLogy(False)
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
  legend = ROOT.TLegend(0.6,0.72,0.92,0.92)
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

  # Draw each histogram.
  # You really shouldn't put two histograms with different
  # x axes on the same plot - I'm only doing it here
  # to show you how to draw multiple plots on the same
  # canvas.
  for histo, line in zip(histos,legendLines) :

    index = histos.index(histo)
    colour = goodColours[index]

    # Set up marker to look nice
    histo.SetMarkerColor(colour)
    histo.SetMarkerSize(1)  
    histo.SetMarkerStyle(20+index)

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
    #histo.GetXaxis().SetLimits(xRange[0],xRange[1])
    histo.GetYaxis().SetRangeUser(0.,0.5)

    if index==0 :
      histo.Draw("H") # Draw data points (you'll get error bars by default)
    else :
      histo.Draw("H SAME") # SAME means don't get rid of the previous stuff on the canvas

    # Fill entry into legend
    # "PL" means both the line and point style
    # will show up in the legend.
    legend.AddEntry(histo,line,"PL")

  # Actually draw the legend
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
  myLatex.DrawLatex(0.35,0.88,"Pedestal")

  # Update the canvas
  c.Update()

  # Save the output as a .eps, a .C, and a .root
  c.SaveAs("Plat.png")


if __name__ == "__main__":
  # plot_all()
  plot_sub("hpmh")
  plot_sub("hpml")
  plot_sub("hpbh")
  plot_sub("hpbl")