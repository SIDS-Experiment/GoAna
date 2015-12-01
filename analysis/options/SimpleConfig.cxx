/* 
 * File:   SimpleConfig.cxx
 * Author: winckler
 * 
 * Created on May 6, 2015, 11:37 AM
 */

#include "SimpleConfig.h"


SimpleConfig::SimpleConfig() : SIDSProgOptions()
{
    
    
    

    
}


SimpleConfig::~SimpleConfig() {
}


int SimpleConfig::ParseAll(const int argc, char** argv, bool AllowUnregistered)
{
    InitOptionDescription();
    if(ParseCmdLine(argc,argv,fCmdLineOptions,fVarMap,AllowUnregistered))
        return 1;
    
    if(fUseConfigFile)
    {
        if(ParseCfgFile(fConfigFile.string(),fConfigFileOptions,fVarMap,AllowUnregistered))
            return 1;
    }
    PrintOptions();
    
    return 0;
}

int SimpleConfig::InitOptionDescription()
{
    fGenericDesc.add_options()
    ("version", "Version of the executable")
    ("exec", po::value<std::string>()->default_value("Hello-World"), "execute task")
    ;
    
    fFileOptions.add_options()
    //("input.file-name", po::value<std::string>(&fConfigFile)->required(), "Path to config")
    ("input.data.file.name", po::value<std::string>()->default_value("inputfile.txt"), "Path to config")
    ("input.data.name", po::value< std::string >()->default_value("mydataset"), "Name of the data set.")
    ("input.data.detector", po::value< std::string >()->default_value("detector"), "Detector type")
    ("input.data.preprocess", po::value< std::string >()->default_value("auto"), "Pre-analysis of the data")
    ("input.data.file.format", po::value< std::string >()->default_value("txt"), "Data file format e.g. : root, txt")
    ("input.data.file.tree.name", po::value< std::string >()->default_value("treename"), "Name of tree (if root format)")
    ("input.data.file.branch.name", po::value< std::string >()->default_value("branchname"), "Name of the branch (if root format)")
    ("output.directory", po::value< std::string >()->default_value(""), "Path to the ouput directory.")
    ("output.data.file.name", po::value<std::string>()->default_value("outputfile.txt"), "Path to config")
    ("output.data.file.format", po::value< std::string >()->default_value("txt"), "output Data file format e.g. : root, txt")
    ("output.data.file.tree.name", po::value< std::string >()->default_value("outputtreename"), "output Name of tree (if root format)")
    ("output.data.file.branch.name", po::value< std::string >()->default_value("outputbranchname"), "output Name of the branch (if root format)")
    ;
    
    fObservable_options.add_options()
    ("obs.name", po::value< std::string >()->default_value("Decay time"), "Name of the observable")
    ("obs.xunit", po::value< double >()->default_value(1.0), "xunit")
    ("obs.xoffset", po::value< double >()->default_value(0.0), "xoffset")
    ("obs.xmin", po::value< double >()->default_value(0.0), "xmin")
    ("obs.xmax", po::value< double >()->default_value(100.0), "xmax")
    ("obs.binning", po::value< int >()->default_value(100), "Binning of the observable")
    ("obs.XRangeOption", po::value< std::string >()->default_value("FALSE"), "FALSE //XRangeAuto")
    ;
    

    AddToCmdLineOptions(fGenericDesc);
    AddToCmdLineOptions(fObservable_options);
    AddToCmdLineOptions(fFileOptions);

    if (fUseConfigFile)
    {
        AddToCfgFileOptions(fObservable_options,false);
        AddToCfgFileOptions(fFileOptions,false);
    }
    
    return 0;
}
