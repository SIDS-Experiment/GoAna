//
//  ExpModel.cpp
//  BC2014Oscv0.0
//
//  Created by Nicolas Winckler on 3/9/14.
//  Copyright (c) 2014 Nicolas Winckler. All rights reserved.
//

#include "ExpModel.h"

// ---------------------------------------------------------
ExpModel::ExpModel() : BCModel("ExpModel"), fxmin(0.0), fxmax(0.0), fSampleMean(0.0)
{
    // default constructor
    DefineParameters();
}


ExpModel::ExpModel(SIDSConfig* config) : BCModel("ExpModel"), fSampleMean(0.0)
{
    fxmin=config->GetValue<double>("obs.xmin");
    fxmax=config->GetValue<double>("obs.xmax");
    DefineParameters();
}


// ---------------------------------------------------------
ExpModel::~ExpModel()
// default destructor
{
}

// ---------------------------------------------------------
void ExpModel::DefineParameters()
{
    // Add parameters to your model here.
    // You can then use them in the methods below by calling the
    // parameters.at(i) or parameters[i], where i is the index
    // of the parameter. The indices increase from 0 according to the
    // order of adding the parameters.
    
    // Allowed range for R_B is [0, 2]
    AddParameter("lambda0", 0.008, 0.020,"#lambda_{0}");
    GetParameter(0)->SetNbins(400);
    fMaxLogL = -1e99;
    use_maxLogL = false;
}

// ---------------------------------------------------------
double ExpModel::LogLikelihood(const std::vector<double> &parameters)
{
    // This methods returns the logarithm of the conditional probability
    // p(data|parameters). This is where you have to define your model.
    
    double logprob = 0.;
    
    // get parameter values
    double lambda = parameters.at(0);
    
    //calculate the normalization factor analytically
    double part0=TMath::Exp(-lambda*fxmin )-TMath::Exp(-lambda*fxmax);
    double AnalyticIntegral=part0/lambda;
    
    //logprob -= GetNDataPoints()*(log(AnalyticIntegral)+lambda*fSampleMean);
    
    for (int i = 0; i < GetNDataPoints(); ++i)
    {
        // get data
        double x = GetDataPoint(i)->GetValue(0);
        Double_t exp0=TMath::Exp(-lambda*x);
    
        logprob+=log(exp0/AnalyticIntegral);
    }
    
    if(fMaxLogL<logprob && use_maxLogL== false)
        fMaxLogL = logprob;
    
    if(use_maxLogL==true)
        logprob += -fMaxLogL;
    
    return logprob;
}


// ---------------------------------------------------------

void ExpModel::SetMyDataSet(BCDataSet* dataset, double unit)
{
    BCModel::SetDataSet(dataset);
    double sum=0.0;
    for (int i = 0; i < GetNDataPoints(); ++i)
        sum+=GetDataPoint(i)->GetValue(0);
    fSampleMean=unit*sum/((double)GetNDataPoints());
}





