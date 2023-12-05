#include <dai/alldai.h> 
#include <dai/factorgraph.h>
#include "cxxopts.hpp"
#include <dai/map.h>
#include <chrono>
#include <ctime>
#include <filesystem>


using namespace std;
using namespace dai;


// comment for production mode, uncomment for debug messages
#define DEBUGMODE

#ifdef DEBUGMODE
	#define DEBUG(a) a;
#else
	#define DEBUG(a) ;
#endif	

std::string testdir = "TestsResults";
std::string inputfile = "./alarm.fg";
std::string outputfile = "./results";
std::vector<unsigned int> hypothesisVars;
std::vector<unsigned int> evidenceVars;
std::vector<unsigned int> evidenceValues;

bool mapComputation = false;
bool veMapComputation = false;

cxxopts::ParseResult parse(int argc, char* argv[])
{
    const char *shortdes = "MAP, MFE, and Annealed MAP experimental simulation";
    try
    {
        cxxopts::Options options(argv[0], shortdes);
        options.add_options()
            ("i,input", "factor graph to run simulations on", cxxopts::value<std::string>())
            ("o,output", "output file for simulation results", cxxopts::value<std::string>())
            ("H,hypothesis-variables", "hypothesis variables", cxxopts::value<std::vector<unsigned int>>())
            ("E,evidence-variables", "evidence variables", cxxopts::value<std::vector<unsigned int>>())
            ("e,evidence-values", "values of the evidence variables", cxxopts::value<std::vector<unsigned int>>())
            ("M,map", "run exact MAP computation")
            ("V,vemap", "run exact MAP using variable elimination")
        ;

        if (argc == 1)
        {
          std::cout << shortdes << std::endl;
          exit(0);
        }
    
        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
          std::cout << options.help({"", "Group"}) << std::endl;
          exit(0);
        }

        if (result.count("map"))
        {
            mapComputation = true;  
            DEBUG(std::cout << "Exact computation using MAP" << std::endl)
        }

        if (result.count("vemap")){
            veMapComputation = true;
            DEBUG(std::cout << "Exact computation using VE MAP" << std::endl)
        }

        

        if (result.count("input"))
        {
            inputfile = result["input"].as<std::string>();
            DEBUG(std::cout << "Input file: " << inputfile << std::endl)
        }

        if (result.count("output"))
        {
            outputfile = result["output"].as<std::string>();
            DEBUG(std::cout << "Output file: " << outputfile << std::endl)
        }

        if (result.count("hypothesis-variables"))
        {  
            hypothesisVars = result["hypothesis-variables"].as<std::vector<unsigned int>>();
            DEBUG(
                std::cout << "Hypothesis variables: ";
                for (auto i = hypothesisVars.begin(); i != hypothesisVars.end(); ++i) std::cout << *i << ' ';
                std::cout << std::endl;
                 )
        }

        if (result.count("evidence-variables"))
        {  
            evidenceVars = result["evidence-variables"].as<std::vector<unsigned int>>();
            DEBUG(
                std::cout << "Evidence variables: ";
                for (auto i = evidenceVars.begin(); i != evidenceVars.end(); ++i) std::cout << *i << ' ';
                std::cout << std::endl;
                 )
        }

        if (result.count("evidence-values"))
        {  
            evidenceValues = result["evidence-values"].as<std::vector<unsigned int>>();
            DEBUG(
                std::cout << "Evidence values: ";
                for (auto i = evidenceValues.begin(); i != evidenceValues.end(); ++i) std::cout << *i << ' ';
                std::cout << std::endl;
                 )
        }

        return result;
    } 
    catch (const cxxopts::OptionException& e)
    {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }
}


int main( int argc, char *argv[] ) {
    namespace fs = std::filesystem;
    
    auto result = parse(argc, argv);
    auto arguments = result.arguments();

    time_t now = time(0);
   	dai::FactorGraph fg;
   	fg.ReadFromFile(inputfile.c_str());

	std::ofstream ofs;

    std::string filepath = testdir + "/" + outputfile;

    if(!fs::exists(testdir)){

        if(!fs::create_directory(testdir)){
            std::cerr << "Error creating direcotyr: " << testdir << std::endl;
        }
    }
	ofs.open (filepath.c_str(), std::ofstream::out | std::ofstream::app);

	ofs << std::endl << "command: ";
    for (int i = 0; i < argc; i++)
        ofs << argv[i] << " ";
    ofs << std::endl;

	ofs << inputfile << " simulation results " << ctime(&now) << std::endl;
	ofs << "hypothesis vars " << hypothesisVars << std::endl;
	ofs << "evidence vars " << evidenceVars << " values " << evidenceValues << std::endl;

    if(mapComputation){

        // compute exact MAP
        if (mapComputation)
        {
            ofs << std::endl << "[MAP] MAP explanation of the hypotheses given the evidence is: ";
            auto start = std::chrono::steady_clock::now();
            dai::Factor MAP = get_map(fg, hypothesisVars, evidenceVars, evidenceValues, false);
            auto end = std::chrono::steady_clock::now();
            ofs << MAP.p() << std::endl;

            // for (const auto& myMap : MAP.i()){
            //     std::cout << myMap << std::endl;
            //     ofs << myMap << std::endl;
            // }
            ofs << "[MAP] Computation took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns" << std::endl;
        }
    }

    ofs << std::endl;
	ofs.close();
}