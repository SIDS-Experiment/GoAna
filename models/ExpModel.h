//
//  ExpModel.h
//  BC2014Oscv0.0
//
//  Created by Nicolas Winckler on 3/9/14.
//  Copyright (c) 2014 Nicolas Winckler. All rights reserved.
//

#ifndef __BC2014Oscv0_0__ExpModel__
#define __BC2014Oscv0_0__ExpModel__

#include <iostream>

#include <BAT/BCDataPoint.h>
#include <BAT/BCParameter.h>
#include <BAT/BCMath.h>
#include <BAT/BCModel.h>

#include "TMath.h"
#include <cmath>
#include "SIDSProgOptions.h"

typedef SIDSProgOptions SIDSConfig;
class ExpModel : public BCModel
{
    
    
public:
    
    ExpModel();
    ExpModel(SIDSConfig* config);
    ~ExpModel ( );
    bool use_maxLogL;
    // Methods to overload
    void DefineParameters();
    double LogLikelihood(const std::vector<double> &parameters);
    // void MCMCIterationInterface();
    
    void SetMyDataSet(BCDataSet* dataset, double unit=1);
    double GetMaximumLogLikelihood(){return fMaxLogL;}
    
    private :
    
    double fxmin;
    double fxmax;
    double fSampleMean;
    
    double fMaxLogL;
    
};




#endif /* defined(__BC2014Oscv0_0__ExpModel__) */
