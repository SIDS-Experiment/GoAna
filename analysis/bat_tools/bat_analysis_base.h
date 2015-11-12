/* 
 * File:   bat_analysis_base.h
 * Author: winckler
 *
 * Created on March 3, 2015, 1:49 PM
 */

#ifndef BATANALYSISBASE_H
#define	BATANALYSISBASE_H

#include "BAT/BCModel.h"
#include "OscConfig.h"
class bat_analysis_base 
{
public:
    bat_analysis_base(){}
    virtual ~bat_analysis_base(){}
    void SetModelOption(BCModel* model, OscConfig& config)
    {
        double relprecision=config.GetValue<double>("bat.CubaRelPrecision");
        double absprecision=config.GetValue<double>("bat.CubaAbsPrecision");
        double niterationsmin=(double)config.GetValue<int>("bat.CubaNIterationsMin");
        double niterationsmax=(double)config.GetValue<int>("bat.CubaNIterationsMax");
        
        //model->SetNIterationsPrecisionCheck (int niterations)
        //model->SetNIterationsOutput (int niterations)

        //*
        model->SetNIterationsMin(niterationsmin);
        model->SetNIterationsMax(niterationsmax);
        
        model->SetRelativePrecision(relprecision);
        model->SetAbsolutePrecision(absprecision);
        //*/
        
        
        std::string IntegrationMethod=config.GetValue<std::string>("bat.IntegrationMethod");
        if(IntegrationMethod=="Default")    model->SetIntegrationMethod(BCIntegrate::kIntDefault); 
        if(IntegrationMethod=="Cuba")       model->SetIntegrationMethod(BCIntegrate::kIntCuba); 
        if(IntegrationMethod=="MonteCarlo") model->SetIntegrationMethod(BCIntegrate::kIntMonteCarlo); 
        if(IntegrationMethod=="Grid")       model->SetIntegrationMethod(BCIntegrate::kIntGrid); 
        
        if(IntegrationMethod=="CubaVegas")
        {
            
            BCCubaOptions::Vegas VegasOption = model->GetCubaVegasOptions(); 
            
            model->SetCubaOptions(VegasOption); 
            //model->IntegrateCuba(BCIntegrate::kCubaVegas);
        }
        
        
        std::string MarginalizationMethod=config.GetValue<std::string>("bat.MarginalizationMethod");
        if(IntegrationMethod=="Default") model->SetMarginalizationMethod(BCIntegrate::kMargDefault);
        if(IntegrationMethod=="Metropolis") model->SetMarginalizationMethod(BCIntegrate::kMargMetropolis);
        if(IntegrationMethod=="MonteCarlo") model->SetMarginalizationMethod(BCIntegrate::kMargMonteCarlo);
        if(IntegrationMethod=="Grid") model->SetMarginalizationMethod(BCIntegrate::kMargGrid);
            
        std::string MCMCPrecision=config.GetValue<std::string>("bat.MCMCPrecision");
        if(MCMCPrecision=="Low")      model->MCMCSetPrecision(BCEngineMCMC::kLow);
        if(MCMCPrecision=="Medium")   model->MCMCSetPrecision(BCEngineMCMC::kMedium);
        if(MCMCPrecision=="High")     model->MCMCSetPrecision(BCEngineMCMC::kHigh);
        if(MCMCPrecision=="VeryHigh") model->MCMCSetPrecision(BCEngineMCMC::kVeryHigh);
    }



};

#endif	/* BATANALYSISBASE_H */

