/* 
 * File:   RooECAnalysis.h
 * Author: winckler
 *
 * Created on March 2, 2015, 11:21 AM
 */

#ifndef ROOECANALYSIS_H
#define	ROOECANALYSIS_H

// std
#include <string>
#include <vector>
#include <cmath>

// ROOT
#include "TMath.h"
#include "TFile.h"
#include "TROOT.h"
#include "TF2.h"
#include "TPaveText.h"

// RooFit
#include "RooRealVar.h" 
#include "RooDataSet.h"
#include "RooPlot.h"
#include "RooGenericPdf.h"
#include "RooMinuit.h"
#include "RooWorkspace.h"
#include "RooRandom.h"

#include "RooUniform.h"
#include "RooGaussian.h"
#include "RooStats/ModelConfig.h"
#include "RooNumIntConfig.h"
#include "RooRealProxy.h"
#include "RooAbsPdf.h"
#include "RooAbsData.h"

// My stuff
//#include "RooMyPdfDict.h"

#include "OscConfig.h"
#include "RootOutFileManager.h"
#include "EsrInjData.h"

using namespace RooFit;
using namespace RooStats;

class RooECAnalysis 
{
public:

    RooECAnalysis(OscConfig* config);
    virtual ~RooECAnalysis();

    void UnbinnedLikelihoodFit(bool Draw=true);
    void LoadData(std::vector<EsrInjData>& Data);
    

private:
    
    void initAttributes(OscConfig* config);
    
    // progoptions
    OscConfig* fConfig;

    // roofit
    RooDataSet* fECdata;
    RooFitResult* fFitResultH0;
    
    RooRealVar* fx;
    RooRealVar* fx_cool;
    RooRealVar* fx_err;
    
    RooRealVar* fLambda0;

    
    RooRealVar* fmeancool;
    RooRealVar* fsigmacool;
    RooRealVar* fmeanerr;
    RooRealVar* fsigmaerr;
    
    RooGenericPdf* fPdf_H0;

    RooGaussian* fCoolingDistribution;
    RooGaussian* fErrorDistribution;
    
    int fBinning;
};

#endif	/* ROOECANALYSIS_H */

