/* 
 * File:   runPlot.cxx
 * Author: winckler
 *
 * Created on May 20, 2015, 5:47 PM
 */

// std
#include <cstdlib>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <sstream>
#include <iomanip>
// root
#include "TApplication.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLegend.h"

// sids/goana lib
#include "SIDSLog.h"
#include "EsrInjData.h"
#include "EsrTree.h"
#include "SimpleConfig.h"
#include "SIDSFileManager.h"


#include "RooRealVar.h" 
#include "RooDataSet.h"
#include "RooPlot.h"



/*
 * 
 */

int main(int argc, char** argv) 
{
    SET_LOG_LEVEL(DEBUG);
    
    SimpleConfig* config = nullptr; 
    SIDSFileManager* man = nullptr;
    
    std::vector<EsrInjData> DataContainer;
    
    try
    {
        
        config = new SimpleConfig();
        config->UseConfigFile();
        po::options_description decayNbDesc("requested decay number");
        decayNbDesc.add_options() 
            ("NEC-dist1", po::value<int>()->default_value(1), "requested maximum number of decay per file for distribution 1")
            ("NEC-dist2", po::value<int>()->default_value(2), "requested maximum number of decay per file for distribution 2")
            ("sim", po::value<bool>()->default_value(false), "sim")
            
            ;
        config->AddToCmdLineOptions(decayNbDesc);
        
        if (config->ParseAll(argc,argv,true))
            return 0;
        
        
        std::string filename=config->GetValue<std::string>("input.data.file.name");
        std::string treename=config->GetValue<std::string>("input.data.file.tree.name");
        std::string branchname=config->GetValue<std::string>("input.data.file.branch.name");

        man = new SIDSFileManager(filename,treename,branchname);
        
        
        DataContainer=man->GetAllData(true);
        // true = pre-sorted 
        // false = direct from tree
        
    }
    catch (std::exception& e)
    {
        SLOG(ERROR) << e.what();
        return 1;
    }
    

    /// plot data
    std::string execute=config->GetValue<std::string>("exec");
    bool simulation=config->GetValue<bool>("sim");
    if(execute=="plot")
    {
        double xmin=config->GetValue<double>("obs.xmin");
        double xmax=config->GetValue<double>("obs.xmax");
        int binning=config->GetValue<int>("obs.binning");
        


        int NECDist1=config->GetValue<int>("NEC-dist1");
        int NECDist2=config->GetValue<int>("NEC-dist2");
        // Create and fill histogram

        std::string h1title="EC-decay histogram Nb EC/ injection = " + std::to_string(NECDist1);
        std::string h2title="EC-decay histogram Nb EC/ injection = " + std::to_string(NECDist2);
        TH1D* Histo1 = new TH1D("dist1",h1title.c_str(),binning,xmin,xmax);
        TH1D* Histo2 = new TH1D("dist2",h2title.c_str(),binning,xmin,xmax);

        std::string cdftitle="empirical distribution function ";
        cdftitle+=std::to_string(NECDist1);
        cdftitle+=" EC vs "+std::to_string(NECDist2) +" EC";

        TH1D* CDF1 = new TH1D("cdf1",cdftitle.c_str(),binning,xmin,xmax);
        TH1D* CDF2 = new TH1D("cdf2","empirical distribution function 2",binning,xmin,xmax);
        TH1D* Distance = new TH1D("distance","Kolmogorov distance",binning,xmin,xmax);

        CDF1->SetStats(kFALSE);
        CDF2->SetStats(kFALSE);
        Distance->SetStats(kFALSE);

        
        if(simulation)
        {
            TF1* exp1 = new TF1("exp1","expo",xmin,xmax);
            TF1* exp2 = new TF1("exp2","expo",xmin,xmax);
            exp1->SetParameters(1.0,-0.01);
            exp2->SetParameters(1.0,-0.1);
            Histo1->FillRandom("exp1",100);
            Histo2->FillRandom("exp2",100);

        }
        else
        {
            for(auto& file : DataContainer)
            {
                std::vector<EsrDecayEvent> eventlist = file.GetECData();
                if(eventlist.size() == NECDist1)
                    for(auto& event : eventlist)
                    {
                        double t_ec=(double)event.GetDecayTime();
                        if(t_ec>=xmin && t_ec<=xmax)
                            Histo1->Fill(t_ec);
                    }

                if(eventlist.size() == NECDist2)
                    for(auto& event : eventlist)
                    {
                        double t_ec=(double)event.GetDecayTime();
                        if(t_ec>=xmin && t_ec<=xmax)
                            Histo2->Fill(t_ec);
                    }
            }
        }



        double Norm1 = Histo1->GetEntries();
        double Norm2 = Histo2->GetEntries();
        double cdf1=0.;
        double cdf2=0.;
        double distance=0.;

        std::map<double,double> C_Alpha;
        std::vector<TLine*> RejectionRegion;

        C_Alpha[0.1] = 1.22;
        C_Alpha[0.05] = 1.36;
        C_Alpha[0.025] = 1.48;
        C_Alpha[0.01] = 1.63;
        C_Alpha[0.005] = 1.73;
        C_Alpha[0.001] = 1.95;

        double max_distance=0.;


        for(int i(0); i<binning; i++)
        {
            cdf1+=Histo1->GetBinContent(i+1)/Norm1;
            cdf2+=Histo2->GetBinContent(i+1)/Norm2;
            distance=fabs(cdf1-cdf2);
            if(distance>max_distance)
                max_distance=distance;
            CDF1->SetBinContent(i+1,cdf1);
            CDF2->SetBinContent(i+1,cdf2);
            Distance->SetBinContent(i+1,distance);
        }

        LOG(INFO)<<"Distribution 1 : NEC="<<NECDist1;
        LOG(INFO)<<"Distribution 2 : NEC="<<NECDist2;
        
        LOG(INFO)<<"Maximum Kolmogorov distance = "<<max_distance;
        double max_height=max_distance;

        TLegend* leg = new TLegend(0.1,0.7,0.48,0.9);
        leg->SetHeader("Kolmogorov test rejection region at given alpha");
        //leg->AddEntry(h1,"Histogram filled with random numbers","f");
        //leg->AddEntry("f1","Function abs(#frac{sin(x)}{x})","l");
        //leg->AddEntry("gr","Graph with error bars","lep");
        //leg->Draw();

        int color=1;

        for(const auto& p :  C_Alpha)
        {
            double arg=(Norm1+Norm2)/(Norm1*Norm2);
            double test=p.second*std::sqrt(arg);
            LOG(INFO)<<"";
            LOG(INFO)<<"Rejection point = "<<test;
            std::string results_str;

            std::stringstream alpha;
            alpha<<std::setprecision(3)<<p.first;

            if(max_distance>test)
            {
                results_str="#alpha = " + alpha.str();//std::to_string(p.first);
                results_str+=" (rejected)";
                LOG(INFO)<<"H0 rejected at alpha = " << alpha.str();
                
            }
            else
            {
                results_str="#alpha = " + alpha.str();//std::to_string(p.first);
                LOG(INFO)<<"H0 not rejected at alpha = " << alpha.str();
            }


            TLine* li = new TLine(xmin,test,xmax,test);
            li->SetLineColor(color);
            color++;
            leg->AddEntry(li,results_str.c_str(),"l");
            RejectionRegion.push_back(li);

            

            if(test>max_height)
                max_height=test;


        }


        max_height=max_height+0.1*max_height;


        std::string outputPdfFile;

        if(!simulation)
        {
            outputPdfFile="Kolmogorov_RV2014_";
            outputPdfFile+=std::to_string(NECDist1);
            outputPdfFile+="VS"+std::to_string(NECDist2);
        }
        else
        {
            outputPdfFile="Kolmogorov_simulation_";
        }
        outputPdfFile+=".pdf";
        
        std::string canvatitle="Two samples Kolmogorov test ";
        

        if(!simulation)
        {
            canvatitle+=std::to_string(NECDist1);
            canvatitle+=" EC vs "+std::to_string(NECDist2) +" EC";
        }
        else
        {
            canvatitle+=" (simulation)";
        }
        // Plot data
        TApplication app("App", nullptr, nullptr);
        TCanvas *Canvas_compare = new  TCanvas(canvatitle.c_str(),canvatitle.c_str(),1000,800);
        Canvas_compare->Divide(2,2);
        Canvas_compare->cd(1);
        Histo1->Draw();
        Canvas_compare->cd(2);
        Histo2->Draw();
        
        Canvas_compare->cd(3);
        CDF1->Draw();
        CDF2->SetLineColor(2);
        CDF2->Draw("SAME");

        Canvas_compare->cd(4);
        Distance->GetYaxis()->SetRangeUser(0.,max_height);
        Distance->Draw();
        
        for(auto& p : RejectionRegion)
            p->Draw("SAME");
        leg->Draw();
        
        Canvas_compare->SaveAs(outputPdfFile.c_str());


        app.Run();
        
        if(Canvas_compare)
            delete Canvas_compare;
        if(Histo1)
            delete Histo1;
    }
    
    if(config)
        delete config;
    
    if(man)
        delete man;
    
    return 0;
}


