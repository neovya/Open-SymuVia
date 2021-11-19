#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

const int SUCCESS = 0;
const int HELP_PRINTED = 1;
const int ERROR_IN_COMMAND_LINE = 2;
const int FAILURE = 3;

using namespace std;

namespace po = boost::program_options;

#define SYMUBRUIT_EXPORT
#define SYMUBRUIT_CDECL

SYMUBRUIT_EXPORT bool SYMUBRUIT_CDECL SymLoadNetwork(std::string sTmpXmlDataFile, std::string sScenarioID = "", std::string sOutdir = "");
SYMUBRUIT_EXPORT bool SYMUBRUIT_CDECL SymRunNextStep(std::string &sXmlFluxInstant, bool bTrace, bool &bNEnd);
SYMUBRUIT_EXPORT int SYMUBRUIT_CDECL SymQuit();

SYMUBRUIT_EXPORT int SYMUBRUIT_CDECL SymCreateVehicle( // Create a vehicle
    std::string sType,                                 // sType : type of the vehicle
    std::string sOrigin,                               // sOrigin: ID of the node origin
    std::string sDestination,                          // sDestination : ID of the node destination
    int nLane,                                         // nLane :  lane number where the vehicle is created (1 is the most right lane)
    double dbt                                         // Time of creation during the time step
);

SYMUBRUIT_EXPORT double SYMUBRUIT_CDECL SymDriveVehicle( // Drive a vehicle
    int nID,                                             // nID : vehicle ID
    std::string sLink,                                   // SLink : Link ID where locate vehicle (at the end of current time step)
    int nVoie,                                           // nLane : lane number where locate vehicle (at the end of current time step)
    double dbPos,
    bool bForce);

SYMUBRUIT_EXPORT int SYMUBRUIT_CDECL SymCreateVehicle( // Create a vehicle
    std::string sType,                                 // sType : type of the vehicle
    std::string sTroncon,
    int nLane,                                         // nLane :  lane number where the vehicle is created (1 is the most right lane)
    double dbPos,
    double dbSpeed,
    double dbAcc
);

SYMUBRUIT_EXPORT double SYMUBRUIT_CDECL SymDriveVehicle( // Drive a vehicle
    int nID,                                             // nID : vehicle ID
    std::string sLink,                                   // SLink : Link ID where locate vehicle (at the end of current time step)
    int nVoie,                                           // nLane : lane number where locate vehicle (at the end of current time step)
    double dbPos,
    double dbSpeed,
    double dbAcc);

int main(int argc, char *argv[])
{
    int nStep = 0;
    bool bLoad, bEnd = 0;
    std::string sInputSymuVia;
    std::string sflow;

    int returnCode;

    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")("symuvia-config-file,I", po::value<std::string>()->required(), "SymuVia XML configuration file");

    po::positional_options_description p;
    p.add("symuvia-config-file", -1);

    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

        if (vm.count("help") || vm.empty()
            // the input XML file must be the only parameter if present, to avoid handling of priorities of parameters between the config file and the command line parameters
            || !((vm.count("symuvia-config-file") == 1 && vm.size() == 1) || (vm.count("symuvia-config-file") == 0 && vm.count("symuvia") == 1)))
        {
            std::cout << "usage : SymuViaLauncher symuvia-config-file.xml" << std::endl;
            std::cout << desc << std::endl;
            return HELP_PRINTED;
        }

        po::notify(vm);
    }
    catch (boost::program_options::required_option &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl
                  << std::endl;
        return ERROR_IN_COMMAND_LINE;
    }
    catch (boost::program_options::error &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl
                  << std::endl;
        return ERROR_IN_COMMAND_LINE;
    }

    if (!SymLoadNetwork(vm.at("symuvia-config-file").as<std::string>()))
    {
        return FAILURE;
    }

    int createdVehicleId;
    while (!bEnd)
    {
        // Time step computation
        SymRunNextStep(sflow, 1, bEnd);
        nStep = nStep + 1;
        if (bEnd == 0)
            std::cout << nStep << std::endl;
        else
            std::cout << "Microscopic simulation completed" << std::endl;

        if (nStep == 10)
        {
            //createdVehicleId = SymCreateVehicle("VL", "E1", "S1", 1, 0);
            createdVehicleId = SymCreateVehicle("VL", "T1", 1, 35, 6.66, 6.66);
        }
        /*else if (nStep == 16)
        {
            std::cout << "SymDriveVehicle : " << SymDriveVehicle(createdVehicleId, "T3", 1, 20, true) << std::endl;
        }*/
        else if (nStep == 20)
        {
            std::cout << "SymDriveVehicle : " << SymDriveVehicle(createdVehicleId, "CAF1_D0_T1_CAF1_C0_T3_1_1", 1, 5, 6.66, 6.66) << std::endl;
        }

        if (nStep == 100)
        {
            SymCreateVehicle("VL", "CAF1_D0_T1_CAF1_C0_T3_1_1", 1, 35, 6.66, 6.66);
        }
    }

    // Terminate properly
    SymQuit();

    return SUCCESS;
}