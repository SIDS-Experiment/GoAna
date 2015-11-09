/* 
 * File:   EsrTree.cxx
 * Author: winckler
 * 
 * Created on August 19, 2014, 10:22 AM
 */

#include "EsrTree.h"

EsrTree::EsrTree(const string &filename, const string &treename, const string &branchname) : 
        fFileName(filename), 
        fTreeName(treename),
        fBranchName(branchname),
        fOutFile(new TFile(fFileName.c_str(),"UPDATE")), 
        fTree(NULL), fEsrData(NULL)
{
    
}

EsrTree::~EsrTree() 
{
    if(fOutFile->IsOpen())
    {
        fOutFile->Close();
        delete fOutFile;
    }
}

void EsrTree::UpdateTree(EsrInjData EsrData)
{
    fEsrData=&EsrData;
    fTree=(TTree*)fOutFile->Get(fTreeName.c_str());
    if(fEsrData)
    {
        if(fTree)
        {
            std::cout<<"[INFO] Update tree : "<< fFileName <<std::endl;
            fTree->SetBranchAddress(fBranchName.c_str(),&fEsrData);
            fTree->Fill();
        }
        else
        {
            std::cout<<"[INFO] Create new tree : "<< fFileName <<std::endl;
            fTree = new TTree(fTreeName.c_str(), "EsrTree output");
            fTree->Branch(fBranchName.c_str(),"EsrInjData", &fEsrData);
            fTree->Fill();
        }
        
        if(fTree)
        {
            fTree->Write("", TObject::kOverwrite);
            fOutFile->Close();
        }
        fEsrData=NULL;
        fTree=NULL;
    }
}



std::vector<EsrInjData> EsrTree::GetEsrData()
{
    std::vector<EsrInjData> DataList;
    if(fOutFile)
    {
        if(fOutFile->IsOpen())
        {
            fOutFile->Close();
            delete fOutFile;
            fOutFile=NULL;
            fOutFile = new TFile(fFileName.c_str(),"READ");
        }
        else
        {
            delete fOutFile;
            fOutFile=NULL;
            fOutFile = new TFile(fFileName.c_str(),"READ");
        }
    }
    else
        fOutFile = new TFile(fFileName.c_str(),"READ");
    
    if (fOutFile->IsZombie())
       std::cout << "[ERROR] Cannot open file " << fFileName.c_str() << std::endl;
    
    fTree=(TTree*)fOutFile->Get(fTreeName.c_str());
    if(fTree)
    {
        fTree->SetBranchAddress(fBranchName.c_str(),&fEsrData);
        for (Long64_t i = 0; i < fTree->GetEntries(); i++)
        {
            fTree->GetEntry(i);
            EsrInjData Data_i=*fEsrData;
            DataList.push_back(Data_i);
        }
    }
    else
        std::cout<<"[ERROR] Tree "<< fTreeName.c_str()<<" not found"<<std::endl;
    fOutFile->Close();
    return DataList;
}



#ifndef __CINT__
std::shared_ptr<EsrInjData> EsrTree::GetInjectionData(Long64_t inj)
{
    
    if(	!fOutFile->IsOpen())
        fOutFile = new TFile(fFileName.c_str(),"READ");
    
    EsrInjData Data_i;
    fTree=(TTree*)fOutFile->Get(fTreeName.c_str());
    if(fTree)
    {
        fTree->SetBranchAddress(fBranchName.c_str(),&fEsrData);
        
            fTree->GetEntry(inj);
        
    }
    fOutFile->Close();
    
    std::shared_ptr<EsrInjData> spTree(fEsrData);//spTree(std::make_shared<EsrInjData>(fEsrData));
    return spTree;
    
}

#endif //__CINT__



ClassImp(EsrTree)
