/* 
 * File:   RooECAnalysis.cxx
 * Author: winckler
 * 
 * Created on March 2, 2015, 11:21 AM
 */

#include "RooECAnalysis.h"


//////////////////////////////////////////////////
////////////////////// Constructor/destructor


RooECAnalysis::RooECAnalysis(OscConfig* config) :
    fConfig(nullptr),// memory handled outside
    
    fECdata(nullptr),
    fFitResultH0(nullptr),
    
    fx(nullptr),
    fx_cool(nullptr),
    fx_err(nullptr),
    
    fLambda0(nullptr),// model 0 (null)
    
    fmeancool(nullptr),
    fsigmacool(nullptr),
    fmeanerr(nullptr),
    fsigmaerr(nullptr),
    
    fPdf_H0(nullptr),

    fCoolingDistribution(nullptr),
    fErrorDistribution(nullptr),
    fBinning(100)
{
    fConfig=config;
    initAttributes(config);

    
}


RooECAnalysis::~RooECAnalysis() 
{
    if(fConfig)
        delete fConfig;// memory handled outside
    
    if(fECdata)
        delete fECdata;
    if(fFitResultH0)
        delete fFitResultH0;
    if(fx)
        delete fx;
    if(fx_cool)
        delete fx_cool;
    if(fx_err)
        delete fx_err;
    if(fLambda0)
        delete fLambda0;// model 0 (null)
    if(fmeancool)
        delete fmeancool;
    if(fsigmacool)
        delete fsigmacool;
    if(fmeanerr)
        delete fmeanerr;
    if(fsigmaerr)
        delete fsigmaerr;
    if(fPdf_H0)
        delete fPdf_H0;
    if(fCoolingDistribution)
        delete fCoolingDistribution;
    if(fErrorDistribution)
        delete fErrorDistribution;

}


void RooECAnalysis::LoadData(std::vector<EsrInjData>& Data)
    {
        int requested_decaynumber_perfile=fConfig->GetValue<int>("MaxNbDecayPerFile");

        for(auto& file : DataContainer)
        {
            std::vector<EsrDecayEvent> eventlist = file.GetECData();
            if(eventlist.size() == requested_decaynumber_perfile)
                for(auto& event : eventlist)
                {
                    double t_ec=(double)event.GetDecayTime();
                    histox->Fill(t_ec);
                    *fx=t_ec;
                    fECdata->add(*fx);
                }
        }

    }
//////////////////////////////////////////////////
////////////////////// Init data members


void RooECAnalysis::UnbinnedLikelihoodFit(bool Draw)
{
    //RooDataSet* ReducedDataSet = (RooDataSet*) fECdata->reduce(*fx,CutRange.c_str()) ; 
    fFitResultH0=fpdfH0->fitTo(*fECdata,RooFit::Save());
    if(Draw)
    {
        
        //fFitResultH1=fpdfH1->fitTo(*ReducedDataSet,RooFit::Save());
        RooPlot* xframe0 = fx->frame(RooFit::Title("Binned data and fit functions with their 1-sigma error bands"));
        fECdata->plotOn(xframe0,RooFit::Binning(fBinning));
        fpdfH0->plotOn(xframe0,RooFit::LineColor(kRed));
        //fpdfH1->plotOn(xframe0,RooFit::LineColor(kBlue));
        RooMsgService::instance().setGlobalKillBelow(RooFit::ERROR);
        fpdfH0->plotOn(xframe0,RooFit::VisualizeError(*fFitResultH0,1),RooFit::FillColor(kOrange));
        //fpdfH1->plotOn(xframe0,RooFit::VisualizeError(*fFitResultH1,1),RooFit::FillColor(kOrange));
        fpdfH0->plotOn(xframe0,RooFit::VisualizeError(*fFitResultH0,1,kFALSE),RooFit::DrawOption("L"),RooFit::LineWidth(1),RooFit::LineColor(kRed),RooFit::LineStyle(kDashed));
        //fpdfH1->plotOn(xframe0,RooFit::VisualizeError(*fFitResultH1,1,kFALSE),RooFit::DrawOption("L"),RooFit::LineWidth(1),RooFit::LineColor(kBlue),RooFit::LineStyle(kDashed));
        RooMsgService::instance().setGlobalKillBelow(RooFit::INFO);
        xframe0->Draw();
    }
}

void RooECAnalysis::initAttributes(OscConfig* config)
{
    /// Import parameters from configfile
    // Range of the Analysis

    double fxmin=config->GetValue<double>("obs.xmin");
    double fxmax=config->GetValue<double>("obs.xmax");
    double flambdamin=config->GetValue<double>("par.lambda.min");
    double flambdamax=config->GetValue<double>("par.lambda.max");
    // Model parameters init
    double LambdaInit=config->GetValue<double>("par.lambda.init");

    /// //////////  Define random variable and parameters ////////////
    //Define observable (rdv)
    fx = new RooRealVar("x","x",fxmin,fxmax);

    //Define null parameters
    fLambda0 = new RooRealVar("#lambda_{0}","#lambda_{0}",LambdaInit,flambdamin,flambdamax);

    /// //////////  Define PDF ////////////
    //Define null model H0
    fPdf_H0 = new RooGenericPdf("H0","#lambda_{0}*exp(-#lambda_{0}*x)",RooArgSet(*fx,*fLambda0));

    fmeancool = new RooRealVar("#mu_{cool}","#mu_{cool}",0.565);
    fsigmacool = new RooRealVar("#sigma_{cool}","#sigma_{cool}",0.174);
    fmeanerr = new RooRealVar("#mu_{err}","#mu_{err}",0.0);
    fsigmaerr = new RooRealVar("#sigma_{err}","#sigma_{err}",0.064);

    fx_cool = new RooRealVar("xcool","xcool",0.0,3.0);
    fx_err =new RooRealVar("xerr","xerr",-1,1);

    //RooGaussian gauss("gauss","gaussian PDF",x,mean,sigma);
    fCoolingDistribution = new RooGaussian("CoolDist","CoolDist",*fx_cool,*fmeancool,*fsigmacool);
    fErrorDistribution = new RooGaussian("ErrDist","ErrDist",*fx_err,*fmeanerr,*fsigmaerr);
    fECdata= new RooDataSet("DataSet","DataSet",*fx);
}

