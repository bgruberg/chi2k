// I N C L U D E S
// -----------------------------------------------------------------------------

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include "TBranch.h"

// roofit includes
#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooStats/SPlot.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooPlot.h"
#include "RooChebychev.h"
#include "RooDataHist.h"
#include "RooArgList.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooAddition.h"
#include "RooProduct.h"
#include "TCanvas.h"
#include "RooAbsPdf.h"
#include "RooFit.h"
#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooConstVar.h"

// C++ includes
#include <iostream>

using std::cout;
using namespace RooFit;
using namespace RooStats;

// include personal header files
#include "plotHelper.h"

void processData()
{
  const char *fileName = "/data/lhcb/users/gruberg/chi/grid/chicToKK/2015/run06_magUpAddEtprev/data/chicToKK2015All.root";
  const char *treeName = "tupleChiC2KK/DecayTree";
  const char *outputName = "analysisOutput.root";


  // L O A D   D A T A
  //----------------------------------------------------------------------------

  cout << " --> L O A D I N G   D A T A : " << endl << endl;

  TFile outputFile( outputName, "recreate" );
  cout << " --> Created new output file: " << outputName << endl << endl;

  TChain myChain( treeName );

  cout << " --> TChain Created." << endl << endl;

  myChain.Add( fileName );

  cout << "     --> Files added to chain: " << fileName << endl << endl;

  myChain.GetEntry( 0 );

  cout << "     --> Cloning a subset of TTree " << treeName << endl << endl;

  #include "branches.h"  // List of Branches to remove from TTree

  TTree *myTree = ( TTree* )myChain.GetTree()->CloneTree( 0 ); // Clone activated branch structure in empty new TTree

  // myTree->Print(); // Print Tree structure and content

  TTreeReader myReader( &myChain );


  // T T R E E   V A R I A B L E S
  //----------------------------------------------------------------------------
  #include "variables.h" // Long list of all TTree Branches available for use

  // Define Histograms
  TH1D *fChic_PT = new TH1D( "f Chic_PT", "Transverse Momentum", 128, 0, 5000 );
  TH1D *fMassChicKaon = new TH1D( "fMassChicKaon", "Invariant Mass", 128, 3200, 3600 );
  TH1D *fMassChicKaonUnzoom = new TH1D( "fMassChicKaonUnzoom", "Invariant Mass", 128, 3100, 3800 );
  TH1D *fMassChicPionHypothesis = new TH1D( "fMassChicPionHypothesis", "#chi_{c} Invariant Mass Pion Hypothesis", 256, 2500, 3600 );
  TH1D *fMassChicProtonHypothesis = new TH1D( "fMassChicProtonHypothesis", "#chi_{c} Invariant Mass Proton Hypothesis", 256, 3400, 4500 );
  TH1D *fKaon1_PIDK = new TH1D( "fKaon1_PIDK", "Kaon PID", 128, -10, 200 );
  TH1D *fL0HadronEt = new TH1D( "fL0HadronEt", "L0 Hadron E_{T}", 128, 0, 300 );
  TH1D *fL0Data_Sum_Et = new TH1D( "fL0Data_Sum_Et", "L0 Sum E_{T} Prev", 128, 0, 300 );
  TH1D *fSpd_After = new TH1D("fSpd_After", "nSpd After Multiplicity and PID Cuts ", 50, 0, 50);



  // L o o p   O v e r   E v e n t s
  //----------------------------------------------------------------------------

  int barWidth = 85;

  cout << " --> A N A L Y Z I N G   D A T A : " << endl;

  // for ( int nCuts = 1; nCuts < 9; nCuts++ )
  for ( int nCuts = 1; nCuts < 2; nCuts++ )
  {
    int nEvents = 0;    // Number of Events
    double progress = 0.0;
    double pos = 0.0;

    // Empty Histograms to prevent double counting
    fMassChicKaon->Reset();
    fSpd_After-> Reset();

    myReader.SetEntry( 0 ); // Restart Entry loop.

    // Loop Over All Events
    while  ( myReader.Next() )
    {
      myChain.GetEntry( nEvents );


      // Track cuts
      if ( *nUpstream == 0 && *nDownstream == 0 && *nVelo == 0 && *nLong == 2 && *NumberOfStdLooseAllPhotons < 2 )
      {
        // Mother Cuts
        if ( pow( *chic_PT / 1000, 2 ) < 1 && *chic_M > 3200 && *chic_M < 3600)
        {
          // Daughter Cuts
          if ( *kaon1_PIDK > 20 && *kaon2_PIDK > 20 )
          {

            if ( *L0Data_Hadron_Et > 25 && *L0Data_Sum_Et_Prev1 < 250 && *nSpd < ( nCuts * 5 ) ) { //*L0Data_Sum_Et_Prev1 < ( nCuts * 250 ) // && *nSpd > 6

              fMassChicKaon->Fill( *chic_M );
              myTree->Fill(); // Fill reduced TTree

            }
          }
        }
      }

      nEvents++;
      // Proces bar
      if ( nEvents % 10000 == 0 )
      {
        progress = nEvents / 2578527.0;
        cout << "[";
        pos = barWidth * progress;
        for ( int i = 0; i < barWidth; ++i )
        {
          if ( i < pos ) cout << "|";
          else cout << " ";
        }
        cout << "] " << int(progress * 100.0) << " %  |  Events: " << nEvents << "\r";
        cout.flush();
      }
    }
    cout << "\n --> E V E N T S   A N A L Y Z E D : " << nEvents <<  endl;



    // R O O F I T
    //--------------------------------------------------------------------------

    RooRealVar massRange( "chic_M", "Mass (MeV/c^{2})", 3200, 3600 );
    // massRange.setBins(128);

    RooPlot *massKaonFrame = massRange.frame();

    massKaonFrame->SetTitle( "K^{+}K^{-} Invariant Mass - 2015" );

    // Get data from histogram - Bined
    // RooDataHist dataMassKaon( "dataMassKaon", "Chi_c Invariant Mass Dataset", massRange, fMassChicKaon );

    // Get Data From Dataset - Unbined
    RooArgSet myArgSet( massRange );
    RooDataSet dataMassKaon("dataMassKaon", "myDataSet", myTree, myArgSet);

    // Signal
    RooRealVar gaussian_mean_massKaon( "gaussian_mean_massKaon", "mean of gaussian for signal peak", 3414.0, 3400.0, 3420.0, "MeV" );
    // gaussian_mean_massKaon.setVal( 3400.28 );    // fixing mean
    // gaussian_mean_massKaon.setConstant();

    RooRealVar gaussian_sigma_massKaon( "gaussian_sigma_massKaon", "width of gaussian for signal peak", 10.0, 0.2, 20, "MeV" );
    // gaussian_sigma_massKaon.setVal( 13.8524 );  // fixing sigma
    // gaussian_sigma_massKaon.setConstant();

    // RooRealVar gaussian_mean_massKaon("gaussian_mean_massKaon", "mean of gaussian for signal peak", 3402.2, 3402.2, 3402.2, "MeV");
    // RooRealVar gaussian_sigma_massKaon("gaussian_sigma_massKaon", "width of gaussian for signal peak", 11.7281, 11.7281, 11.7281, "MeV");

    RooGaussian peak_gaussian_massKaon( "peak_gaussian_massKaon", "gaussian for signal peak", massRange, gaussian_mean_massKaon, gaussian_sigma_massKaon );
    RooRealVar peak_yield_massKaon( "peak_yield_massKaon", "yield signal peak", 1000, 0, 1000000 );

    // Background
    RooRealVar lambda_massKaon( "lambda_massKaon", "slope of exponential", -0.1, -5.0, 0.0 );
    RooExponential expo_massKaon( "expo_massKaon", "exponential for background", massRange, lambda_massKaon );
    RooRealVar expo_yield_massKaon( "expo_yield_massKaon", "yield background", 1000, 0, 1000000 );


    // Total PDF
    RooArgList shapes_massKaon;
    shapes_massKaon.add( expo_massKaon );
    shapes_massKaon.add( peak_gaussian_massKaon );

    RooArgList yields_massKaon;
    yields_massKaon.add( expo_yield_massKaon );
    yields_massKaon.add( peak_yield_massKaon );

    RooAddPdf totalPdfMassKaon( "totalPdf", "sum of signal and background PDF's", shapes_massKaon, yields_massKaon );

    totalPdfMassKaon.fitTo( dataMassKaon, Extended() );
    dataMassKaon.plotOn( massKaonFrame, RooFit::Name( "dataMass" ) );

    totalPdfMassKaon.plotOn( massKaonFrame, Components(peak_gaussian_massKaon ), LineStyle( kDashed ), LineColor( kGreen+1 ), RooFit::Name( "dataFitGaus" ) );
    totalPdfMassKaon.plotOn( massKaonFrame, Components( expo_massKaon ), LineStyle( kDashed ), LineColor( kRed ), RooFit::Name( "dataFitExp" ) );
    totalPdfMassKaon.plotOn( massKaonFrame, RooFit::Name( "dataFit" ) );

    // Construct a histogram with the residuals of the data w.r.t. the curve
    RooHist *fResidualMassKaon = massKaonFrame->pullHist() ;

    // Create a new frame to draw the residual distribution and add the distribution to the frame
    RooPlot *massKaonFramePull = massRange.frame( Title( "	" ) );
    massKaonFramePull->addPlotable( fResidualMassKaon, "BEX0" );
    massKaonFramePull->SetLabelSize ( 0.10, "X" );

    cout<< "\n ***** Yield: "<< peak_yield_massKaon.getVal() << endl << endl;



    // S A V E
    //----------------------------------------------------------------------------

    plot_Helper(massKaonFrame, massKaonFramePull, 3200, 3600, peak_yield_massKaon.getVal(), peak_yield_massKaon.getError());

    TCanvas* canvas = new TCanvas( "canvas" );

    cout << "\n --> S A V I N G   P L O T S ..." << endl;

    fMassChicKaon->Draw();
    fMassChicKaon->Write();
    canvas->Print( "plots.pdf)" );

    // TFile newFile("reducednTupleFile.root", "recreate");
    //TFile ButDst0pi_D0tkpi_File("/data/lhcb/users/rollings/ButoDst0X_DATA_1/2011_MagUp/ButDst0pi_D0tkpi_2011_MagUp.root", "recreate");

    myTree->AutoSave();

    cout << "Done!" << endl;
  }
}



// }

// M A I N
void read()
{
  processData();
  // other( myChain1 );
}
