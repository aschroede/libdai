#include <dai/alldai.h> 
#include <dai/factorgraph.h>
#include <dai/map.h>

using namespace std;
using namespace dai;

// int main( int argc, char *argv[] ) {


//     if ( argc != 2 && argc != 3 ) {
//         cout << "Usage: " << argv[0] << " <filename.fg> [maxstates]" << endl << endl;
//         cout << "Reads factor graph <filename.fg> and runs MAP on it." << endl;
//         return 1;

//     } else {
    
//         // Read FactorGraph from the file specified by the first command line argument
//         FactorGraph fg;
//         std::cout << "Factor graph path: " << argv[1] << std::endl;
//         fg.ReadFromFile(argv[1]);;


//         // Example from page 260 of Modeling and Reasoning with Bayesian Networks
//         // MAP variables = {I, J} = {0, 1}
//         // Evidence: O = true. O is variable number 4
//         // Constrained variable order = O, Y, X, I, J = 4, 2, 3, 0, 1

//         std::vector<unsigned int> ex_evidenceVars =        { 4 };
// 	    std::vector<unsigned int> ex_evidenceValues =      { 1};
// 	    std::vector<unsigned int> ex_mapVars =             { 0, 1};
//         std::vector<unsigned int> constrainedElimOrder =   { 4, 2, 3, 0, 1 };


//         dai::Factor MAP = get_map(fg, ex_mapVars, ex_evidenceVars, ex_evidenceValues, false);

//         cout << "Map probability: " << MAP.p() << endl;

//         cout << "Map instantiation: ";
//         for (const auto& myMap : MAP.i()) {
//             std::cout << myMap << endl;
//         }

//         //cout << "Map Instantiation: " << MAP.i() << endl;
//         // std::vector<std::pair<Var, dai::Real>> instantiation = MAP.getInstantiation();
        
//         // for (const auto& pair : instantiation){
//         //     std::cout << "Var: " << pair.first << " Value: " << pair.second << endl;
//         // }
        
//     }
// }

int main( int argc, char *argv[] ) {


    if ( argc != 2 && argc != 3 ) {
        cout << "Usage: " << argv[0] << " <filename.fg> [maxstates]" << endl << endl;
        cout << "Reads factor graph <filename.fg> and runs MAP on it." << endl;
        return 1;

    } else {
    
        // Read FactorGraph from the file specified by the first command line argument
        FactorGraph fg;
        std::cout << "Factor graph path: " << argv[1] << std::endl;
        fg.ReadFromFile(argv[1]);;




        std::vector<unsigned int> ex_evidenceVars =        { 27,54,55,56,59,120,122,124,126,127,128,137,148,182,183,188,190,203,204,206,220,222 };
	    std::vector<unsigned int> ex_evidenceValues =      { 1,0,1,1,1,1,0,0,1,1,0,0,0,1,1,1,1,1,0,0,1,0 };
	    std::vector<unsigned int> ex_mapVars =             { 0,1,2,3,4 };
        //std::vector<unsigned int> constrainedElimOrder =   { 4, 2, 3, 0, 1 };


        dai::Factor MAP = get_map(fg, ex_mapVars, ex_evidenceVars, ex_evidenceValues, false);

        cout << "Map probability: " << MAP.p() << endl;

        cout << "Map instantiation: ";
        for (const auto& myMap : MAP.i()) {
            std::cout << myMap << endl;
        }

        //cout << "Map Instantiation: " << MAP.i() << endl;
        // std::vector<std::pair<Var, dai::Real>> instantiation = MAP.getInstantiation();
        
        // for (const auto& pair : instantiation){
        //     std::cout << "Var: " << pair.first << " Value: " << pair.second << endl;
        // }
    }
}