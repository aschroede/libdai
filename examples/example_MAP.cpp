/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */

#include <iostream>
#include <map>
#include <dai/alldai.h>  // Include main libDAI header file
#include <dai/jtree.h>
#include <dai/bp.h>
#include <dai/decmap.h>
#include <chrono>

// comment for production mode, uncomment for debug messages
#define DEBUGMODE

#ifdef DEBUGMODE
	#define DEBUG(a) a;
#else
	#define DEBUG(a) ;
#endif	

using namespace dai;
using namespace std;

std::vector<unsigned long int> get_map(dai::FactorGraph fg, std::vector<unsigned int> map_vars, std::vector<unsigned int> evidence_vars,
        std::vector<unsigned int> evidence_values, std::vector<unsigned int> constrainedElimOrder, bool mapList){

        // PruneNetwork

        // Generate constrained variable elimination order (pi)

        // Clamp evidence

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < evidence_vars.size(); i++){
            fg.clamp(evidence_vars[i], evidence_values[i], false);
        }
        auto end = std::chrono::steady_clock::now();
        std::cout << "Clamping evidence " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns" << std::endl;


        for (int i=0; i < constrainedElimOrder.size(); i++){

            // Find all factors fk that mention variable pi[i] 
            // f <- Then multiply those factors together 

            std::vector<dai::Factor> factors = fg.factors();

            std::vector<dai::Factor> toMultiply;

            for(int j=0; j<factors.size(); j++){
                
                dai::VarSet vars = (factors[j].vars());

                for (auto it = vars.begin(); it != vars.end(); ++it){

                    if(it->label() == constrainedElimOrder[i]){
                        
                        toMultiply.push_back(factors[j]);

                        cout << "Found something!" << endl;
                    }
                }
                //vars.contains(constrainedElimOrder[i])
                //contains())
            }

            
            // If variable pi(i) is a map variable then
            //      fi <- max out pi(i) from f
            
    
            // Else
            //      fi <- sum out pi(i) from f


            // Replace all factors  fk in the set of factor S by factor fi
        }

        // Return trivial factor
        
    }

int main( int argc, char *argv[] ) {


    if ( argc != 2 && argc != 3 ) {
        cout << "Usage: " << argv[0] << " <filename.fg> [maxstates]" << endl << endl;
        cout << "Reads factor graph <filename.fg> and runs MAP on it." << endl;
        return 1;

    } else {
    
        // Read FactorGraph from the file specified by the first command line argument
        FactorGraph fg;
        std::cout << "Factor graph path: " << argv[1] << std::endl;
        fg.ReadFromFile(argv[1]);
        size_t maxstates = 1000000;


        // Example from page 260 of Modeling and Reasoning with Bayesian Networks
        // MAP variables = {I, J} = {0, 1}
        // Evidence: O = true. O is variable number 4
        // Constrained variable order = O, Y, X, I, J = 4, 2, 3, 0, 1

        std::vector<unsigned int> ex_evidenceVars =        { 4 };
	    std::vector<unsigned int> ex_evidenceValues =      { 1};
	    std::vector<unsigned int> ex_mapVars =             { 0, 1};
        std::vector<unsigned int> constrainedElimOrder =   { 4, 2, 3, 0, 1 };


        std::vector<unsigned long int> MAP = get_map(fg, ex_evidenceVars, ex_evidenceValues, ex_mapVars, constrainedElimOrder, false);
    }
}



