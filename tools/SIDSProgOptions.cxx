/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/*
 * File:   SIDSProgOptions.cxx
 * Author: winckler
 * 
 * Created on March 11, 2015, 5:38 PM
 */

#include "SIDSProgOptions.h"

using namespace std;

/// //////////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor
SIDSProgOptions::SIDSProgOptions() : 
                        fGenericDesc("Generic options description"),
                        fConfigDesc("Configuration options description"),
                        fHiddenDesc("Hidden options description"),
                        fEnvironmentDesc("Environment Variables"),
                        fCmdLineOptions("Command line options"),
                        fConfigFileOptions("Configuration file options"),
                        fVisibleOptions("Visible options"),
                        fVerboseLvl("INFO"), fUseConfigFile(false), fConfigFile()
{
    // define generic options
    fGenericDesc.add_options()
        ("help,h", "produce help")
        ("version,v", "print version")
        ("verbose", po::value<std::string>(&fVerboseLvl)->default_value("DEBUG"), "Verbosity level : \n"
                                                                    "  TRACE \n"
                                                                    "  DEBUG \n"
                                                                    "  RESULTS \n"
                                                                    "  INFO \n"
                                                                    "  WARN \n"
                                                                    "  ERROR \n"
                                                                    "  STATE \n"
                                                                    "  NOLOG"
            )
        ;

    fSeverityMap["TRACE"]              = sids::severity_level::TRACE;
    fSeverityMap["DEBUG"]              = sids::severity_level::DEBUG;
    fSeverityMap["RESULTS"]            = sids::severity_level::RESULTS;
    fSeverityMap["INFO"]               = sids::severity_level::INFO;
    fSeverityMap["WARN"]               = sids::severity_level::WARN;
    fSeverityMap["ERROR"]              = sids::severity_level::ERROR;
    fSeverityMap["STATE"]              = sids::severity_level::STATE;
    fSeverityMap["NOLOG"]              = sids::severity_level::NOLOG;
}

/// Destructor
SIDSProgOptions::~SIDSProgOptions()
{
}

/// //////////////////////////////////////////////////////////////////////////////////////////////////////
/// Add option descriptions
int SIDSProgOptions::AddToCmdLineOptions(const po::options_description& optDesc, bool visible)
{
    fCmdLineOptions.add(optDesc);
    if (visible)
    {
        fVisibleOptions.add(optDesc);
    }
    return 0;
}

int SIDSProgOptions::AddToCfgFileOptions(const po::options_description& optDesc, bool visible)
{
    //if UseConfigFile() not yet called, then enable it with required file name to be provided by command line
    if (!fUseConfigFile)
    {
        UseConfigFile();
    }

    fConfigFileOptions.add(optDesc);
    if (visible)
    {
        fVisibleOptions.add(optDesc);
    }
    return 0;
}

int SIDSProgOptions::AddToEnvironmentOptions(const po::options_description& optDesc)
{
    fEnvironmentDesc.add(optDesc);
    return 0;
}

void SIDSProgOptions::UseConfigFile(const string& filename)
{
        fUseConfigFile = true;
        if (filename.empty())
        {
            fConfigDesc.add_options()
                ("config,c", po::value<boost::filesystem::path>(&fConfigFile)->required(), "Path to configuration file (required argument)");
            AddToCmdLineOptions(fConfigDesc);
        }
        else
        {
            fConfigFile = filename;
        }
}

/// //////////////////////////////////////////////////////////////////////////////////////////////////////
/// Parser

int SIDSProgOptions::ParseCmdLine(const int argc, char** argv, const po::options_description& desc, po::variables_map& varmap, bool allowUnregistered)
{
    // get options from cmd line and store in variable map
    // here we use command_line_parser instead of parse_command_line, to allow unregistered and positional options
    if (allowUnregistered)
    {
        po::command_line_parser parser{argc, argv};
        parser.options(desc).allow_unregistered();
        po::parsed_options parsedOptions = parser.run();
        po::store(parsedOptions, varmap);
    }
    else
    {
        po::store(po::parse_command_line(argc, argv, desc), varmap);
    }

    // call the virtual NotifySwitchOption method to handle switch options like e.g. "--help" or "--version"
    // return 1 if switch options found in varmap
    if (NotifySwitchOption())
    {
        return 1;
    }

    po::notify(varmap);
    return 0;
}

int SIDSProgOptions::ParseCmdLine(const int argc, char** argv, const po::options_description& desc, bool allowUnregistered)
{
    return ParseCmdLine(argc,argv,desc,fVarMap,allowUnregistered);
}

int SIDSProgOptions::ParseCfgFile(ifstream& ifs, const po::options_description& desc, po::variables_map& varmap, bool allowUnregistered)
{
    if (!ifs)
    {
        cout << "can not open configuration file \n";
        return -1;
    }
    else
    {
        po:store(parse_config_file(ifs, desc, allowUnregistered), varmap);
        po::notify(varmap);
    }
    return 0;
}

int SIDSProgOptions::ParseCfgFile(const string& filename, const po::options_description& desc, po::variables_map& varmap, bool allowUnregistered)
{
    ifstream ifs(filename.c_str());
    if (!ifs)
    {
        cout << "can not open configuration file: " << filename << "\n";
        return -1;
    }
    else
    {
        po:store(parse_config_file(ifs, desc, allowUnregistered), varmap);
        po::notify(varmap);
    }
    return 0;
}

int SIDSProgOptions::ParseCfgFile(const string& filename, const po::options_description& desc, bool allowUnregistered)
{
    return ParseCfgFile(filename,desc,fVarMap,allowUnregistered);
}

int SIDSProgOptions::ParseCfgFile(ifstream& ifs, const po::options_description& desc, bool allowUnregistered)
{
    return ParseCfgFile(ifs,desc,fVarMap,allowUnregistered);
}

int SIDSProgOptions::ParseEnvironment(const function<string(string)>& environmentMapper)
{
    po::store(po::parse_environment(fEnvironmentDesc, environmentMapper), fVarMap);
    po::notify(fVarMap);

    return 0;
}

// Given a key, convert the variable value to string
string SIDSProgOptions::GetStringValue(const string& key)
{
    string valueStr;
    try
    {
        if (fVarMap.count(key))
        {
            valueStr=sids::ConvertVariableValue<sids::ToString>().Run(fVarMap.at(key));
        }
    }
    catch(exception& e)
    {
        LOG(ERROR) << "Exception thrown for the key '" << key << "'";
        LOG(ERROR) << e.what();
    }

    return valueStr;
}

/// //////////////////////////////////////////////////////////////////////////////////////////////////////
/// Print/notify options
int SIDSProgOptions::PrintHelp()  const
{
    cout << fVisibleOptions << "\n";
    return 0;
}

int SIDSProgOptions::PrintOptions()
{
    // //////////////////////////////////
    // Method to overload.
    // -> loop over variable map and print its content
    // -> In this example the following types are supported:
    // string, int, float, double, short, boost::filesystem::path
    // vector<string>, vector<int>, vector<float>, vector<double>, vector<short>

    MapVarValInfo_t mapinfo;

    // get string length for formatting and convert varmap values into string
    int maxLength1st = 0;
    int maxLength2nd = 0;
    int maxLengthTypeInfo = 0;
    int maxLengthDefault = 0;
    int maxLengthEmpty = 0;
    int totalLength = 0;
    for (const auto& m : fVarMap)
    {
        Max(maxLength1st, m.first.length());

        VarValInfo_t valinfo = GetVariableValueInfo(m.second);
        mapinfo[m.first] = valinfo;
        string valueStr;
        string typeInfoStr;
        string defaultStr;
        string emptyStr;
        tie(valueStr, typeInfoStr, defaultStr, emptyStr) = valinfo;

        Max(maxLength2nd, valueStr.length());
        Max(maxLengthTypeInfo, typeInfoStr.length());
        Max(maxLengthDefault, defaultStr.length());
        Max(maxLengthEmpty, emptyStr.length());
    }

    // TODO : limit the value length field in a better way
    if (maxLength2nd > 100)
    {
        maxLength2nd = 100;
    }
    totalLength = maxLength1st + maxLength2nd + maxLengthTypeInfo + maxLengthDefault + maxLengthEmpty;

    // maxLength2nd = 200;

    // formatting and printing

    LOG(INFO) << setfill ('*') << setw (totalLength + 3) << "*";// +3 because of string " = "
    string PrintOptionsTitle = "     Program options found     ";

    int leftSpaceLength = 0;
    int rightSpaceLength = 0;
    int leftTitleShiftLength = 0;
    int rightTitleShiftLength = 0;

    leftTitleShiftLength = PrintOptionsTitle.length() / 2;

    if ((PrintOptionsTitle.length()) % 2)
        rightTitleShiftLength = leftTitleShiftLength + 1;
    else
        rightTitleShiftLength = leftTitleShiftLength;

    leftSpaceLength = (totalLength + 3) / 2 - leftTitleShiftLength;
    if ((totalLength + 3) % 2)
    {
        rightSpaceLength = (totalLength + 3) / 2 - rightTitleShiftLength + 1;
    }
    else
    {
        rightSpaceLength = (totalLength + 3) / 2 - rightTitleShiftLength;
    }

    LOG(INFO) << setfill ('*') << setw(leftSpaceLength) << "*"
                << setw(PrintOptionsTitle.length()) << PrintOptionsTitle
                << setfill ('*') << setw(rightSpaceLength) << "*";

    LOG(INFO) << setfill ('*') << setw (totalLength+3) << "*";

    for (const auto& p : mapinfo)
    {
        string keyStr;
        string valueStr;
        string typeInfoStr;
        string defaultStr;
        string emptyStr;
        keyStr = p.first;
        tie(valueStr, typeInfoStr, defaultStr, emptyStr) = p.second;
        LOG(INFO) << std::setfill(' ')
                    << setw(maxLength1st) << left
                    << p.first << " = "
                    << setw(maxLength2nd)
                    << valueStr
                    << setw(maxLengthTypeInfo)
                    << typeInfoStr
                    << setw(maxLengthDefault)
                    << defaultStr
                    << setw(maxLengthEmpty)
                    << emptyStr;
    }
    LOG(INFO) << setfill ('*') << setw (totalLength + 3) << "*";// +3 for " = "

    return 0;
}

int SIDSProgOptions::NotifySwitchOption()
{
    // Method to overload.
    if (fVarMap.count("help"))
    {
        cout << "***** FAIR Program Options ***** \n" << fVisibleOptions;
        return 1;
    }

    if (fVarMap.count("version"))
    {
        cout << "alpha version 0.0\n";
        return 1;
    }

    return 0;
}

SIDSProgOptions::VarValInfo_t SIDSProgOptions::GetVariableValueInfo(const po::variable_value& varValue)
{
    return sids::ConvertVariableValue<sids::ToVarInfo>().Run(varValue);
}
