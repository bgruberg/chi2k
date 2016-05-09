// I N C L U D E S
// -----------------------------------------------------------------------------

// ROOT includes
#include "TCanvas.h"
#include "TLine.h"
#include "TLegend.h"
#include "TAxis.h"
#include "TColor.h"
#include "TStyle.h"

// roofit includes
#include "RooPlot.h"

// C++ includes
#include <iostream>

#include "plotHelper.h"


// High Granulatirty TH2
void set_plot_style()
{
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;

  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
}

//Invariant Mass Plot With Pull
void plot_Helper(RooPlot *frame1, RooPlot *frame2, double min, double max, double yield_value, double yield_error)
{

  TCanvas *c1 = new TCanvas("c1","multipads",900,700);

  TPad *pad1 = new TPad("pad1", "The pad 80% of the height", 0.0, 0.2, 1.0, 1.0, 21);
  TPad *pad2 = new TPad("pad2", "The pad 20% of the height", 0.0, 0.05, 1.0, 0.304, 22);

  pad1->SetFillColor(kWhite);
  pad1->SetFillStyle(4000);

  pad2->SetFillColor(kWhite);
  pad2->SetFillStyle(4000);
  pad2->SetBottomMargin(.3);

  pad1->Draw();
  pad2->Draw();

  pad1->cd();

  frame1->SetXTitle("");
  frame1->SetLabelOffset(10, "X");
  frame1->Draw();


  TLegend *legendMassKaon = new TLegend(0.15, 0.20, 0.45, 0.6);
  // legendMassKaon->SetHeader("The Legend Title");
  legendMassKaon->AddEntry((TObject*)0, "#int Ldt = 307 #pm 15 pb^{-1}", "");
  legendMassKaon->AddEntry((TObject*)0, Form("Yield: %.0f #pm %.0f events", yield_value, yield_error), "");
  legendMassKaon->AddEntry((TObject*)0, "Rate: 6.7 #pm 0.7 events/pb^{-1}", "");
  legendMassKaon->AddEntry(frame1->findObject("dataMass"), "Data", "p");
  legendMassKaon->AddEntry(frame1->findObject("dataFit"), "Fit", "l");
  legendMassKaon->AddEntry(frame1->findObject("dataFitGaus"), "Signal", "l");
  legendMassKaon->AddEntry(frame1->findObject("dataFitExp"), "Background", "l");

  legendMassKaon->SetLineColor(kWhite);
  legendMassKaon->Draw();

  pad2->cd();
  frame2->SetTitle("");
  frame2->SetYTitle("");
  frame2->SetAxisRange(-5.5, 5.5, "Y");
  frame2->SetLabelSize(0.12, "X");
  frame2->SetLabelSize(0.12, "Y");
  frame2->SetTickLength(0.11, "X");
  frame2->SetTitleSize(0.11, "X");
  frame2->Draw();

  TLine *line1 = new TLine(min, 2, max, 2); line1->SetLineColor(kRed); line1->Draw();
  TLine *line2 = new TLine(min, -2, max, -2); line2->SetLineColor(kRed); line2->Draw();

  c1->Print("plots.pdf(");
  c1->Write();
}
