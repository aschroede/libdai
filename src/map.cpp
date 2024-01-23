/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */

#include <iostream>
//#include <map>
#include <dai/alldai.h>  // Include main libDAI header file
#include <chrono>
#include <stack>
#include <statsutil.h>

// comment for production mode, uncomment for debug messages
#define DEBUGMODE

#ifdef DEBUGMODE
	#define DEBUG(a) a;
#else
	#define DEBUG(a) ;
#endif	

namespace dai {

using namespace std;

template<class EliminationChoice>
vector<size_t> getConstrainedElimOrder(const FactorGraph &fg, EliminationChoice f, std::vector<unsigned int> map_vars, std::vector<unsigned int> evidence_vars  ){

    // Create cluster graph from factor graph
    ClusterGraph cl( fg, true );

    // Obtain elimination sequence
    vector<VarSet> ElimVec = cl.VarElim( greedyVariableElimination( f )).eraseNonMaximal().clusters();

    // Calculate unconstrainted treewidth. This is the best we can do for now. 
    size_t treewidth = 0;
    BigInt nrstates = 0.0;
    for( size_t i = 0; i < ElimVec.size(); i++ ) {
        if( ElimVec[i].size() > treewidth )
            treewidth = ElimVec[i].size();
        BigInt s = ElimVec[i].nrStates();
        if( s > nrstates )
            nrstates = s;
    }

    std::cout << "Unconstrained tree width: " << treewidth << std::endl;
    std::cout << "Unconstrainted state number: " << nrstates << std::endl;
    std::cout << "Assuming doubles worth 8 bytes on each row. Max mem usage for state number: " << nrstates.get_d()*8*0.000000001 << " GB" << std::endl;

    // Now get constrained tree width
    // Construct set of variable indices for non-Map vars
    std::set<size_t> nonMapVarindices;
    std::set<size_t> MapVarindices;

    for( size_t i = 0; i < cl.vars().size(); ++i ){\

        auto it = std::find(evidence_vars.begin(), evidence_vars.end(), i);

        // Only add non-evidence variables
        if(it == evidence_vars.end()){

            auto it = std::find(map_vars.begin(), map_vars.end(), i);

            // If not in map variables add it
            if (it == map_vars.end()) {
                nonMapVarindices.insert( i );
            }
            else{
                MapVarindices.insert( i );
            }
        }
    }

    vector<size_t> elimOrder;

    // Load up non map vars first
    while( !nonMapVarindices.empty() ) {
        size_t i = f( cl, nonMapVarindices );
        elimOrder.push_back(i);
        nonMapVarindices.erase( i );
    }

    // Then load map vars
    while( !MapVarindices.empty() ) {
        size_t i = f( cl, MapVarindices );
        elimOrder.push_back(i);
        MapVarindices.erase( i );
    }

    return elimOrder;

}


void calculateEliminationWidth(dai::FactorGraph fg, vector<size_t> elimOrder){

    // Get list of factors in this form ([A], [B], [A, B, C], [D], [E, B], [F, C, E, D])


    // keep track of the largest factor constructed
    std::uint16_t maxVars = 0;
    dai::BigInt maxStates = 0;

    // Perform Variable Elimination
    std::vector<dai::Factor> factors = fg.factors();

    std::vector<dai::VarSet> factorList;

    for(int j=0; j<factors.size(); j++){
                
        dai::VarSet vars = (factors[j].vars());

        factorList.push_back(vars);
    }

    for (int i=0; i < elimOrder.size(); i++){

        std::cout << "Eliminate: " << elimOrder[i] << endl;

        // Find all factors that contain the variable to be eliminated

        std::vector<dai::VarSet> toMultiply;
        for(int j=0; j<factorList.size(); j++){
            
            dai::VarSet vars = factorList[j];
            for (auto it = vars.begin(); it != vars.end(); ++it){

                if(it->label() == elimOrder[i]){
                    
                    toMultiply.push_back(factorList[j]);
                }
            }
        }

        // "Multiply" the factors by taking the union of the variables in toMultiply
        dai::VarSet newFactor = toMultiply[0];
        if(toMultiply.size() > 1){

            for (int i = 1; i<toMultiply.size(); i++){
                newFactor.operator|=(toMultiply[i]);
            }
        }

        // Check the size of the new factor that was created by multiplying all the other factors
        if(newFactor.nrStates() > maxStates){
            maxStates = newFactor.nrStates();
        }
        if(newFactor.size() > maxVars){
            maxVars = newFactor.size();
        }



        // "Sum out"/"Maximise Out" the variable to be eliminated by doing set subtraction
        // The operator/=() takes a Var as an argument, not an index
        // How can I get the elimination order in terms of variables rather than indices?
        // Is there an index to var function somewhere? 
        // There is the indices to var function
        // Need to remove the variable to be eliminated specified by elimOrder[i]. But elimOrder[i] is just a number
        // and operator/=(const Var &t) takes a variable. Not sure how to get a variable from the variable number

        dai::Var varToRemove = fg.var(elimOrder[i]);
        newFactor.operator/=(varToRemove);

        // Now put the newFactor in the list of factors and remove the old factors that were multiplied together.

        for (auto it = toMultiply.begin(); it != toMultiply.end(); ++it){
                factorList.erase(std::find_if(factorList.begin(), factorList.end(), [&](VarSet const& f){ return f == *it; }));
            }

        factorList.push_back(newFactor);
        
    }

    // After all variables have been eliminated, then multiply remaining factors together

    // Multiply remaining factors
    dai::VarSet newFactor = factorList[0];
    if(factorList.size() > 1){

        for (int i = 1; i<factorList.size(); i++){
            newFactor.operator|=(factorList[i]);
        }
    }

    // Check the size of the new factor that was created by multiplying all the other factors
    if(newFactor.nrStates() > maxStates){
        maxStates = newFactor.nrStates();
    }
    if(newFactor.size() > maxVars){
        maxVars = newFactor.size();
    }

    std::cout<<"Elimination Order: " << elimOrder << std::endl;
    std::cout<<"Maximum Variables in one Factor: " << maxVars << std::endl;
    std::cout<<"Maximum States in one Factor: " << maxStates << std::endl;

}


dai::Factor get_map_ve(dai::FactorGraph fg, std::vector<unsigned int> map_vars, std::vector<unsigned int> evidence_vars,
        std::vector<unsigned int> evidence_values, bool mapList){
        
    try{

        std::cout << "This is a another test" << std::endl;
        // TODO: PruneNetwork

        // Clamp evidence
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < evidence_vars.size(); i++){
            fg.clampReduce(evidence_vars[i], evidence_values[i]);
        }
        auto end = std::chrono::steady_clock::now();
        std::cout << "Clamping evidence " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns" << std::endl;

        double num_states = 1;
        double num_hyp_states = 1;
        for (Var var : fg.vars()){
            
            if(std::find(evidence_vars.begin(), evidence_vars.end(), var.label()) == evidence_vars.end()){

                num_states *= var.states();
            }

            if(std::find(map_vars.begin(), map_vars.end(), var.label()) != map_vars.end()){

                num_hyp_states *= var.states();
            }

        }

        std::cout << "Num joint states: " << num_states << std::endl; 
        std::cout << "Num hypothesis joint states: " << num_hyp_states << std::endl; 

        // Generate constrained variable elimination order. Don't include evidence variables
        vector<size_t> constrainedElimOrder = getConstrainedElimOrder(fg, greedyVariableElimination( eliminationCost_MinNeighbors), map_vars, evidence_vars);
        calculateEliminationWidth(fg, constrainedElimOrder);

        std::cout << "Elimination Order: " << constrainedElimOrder << endl;
        std::cout << "Number of vars: " << constrainedElimOrder.size() << endl;
        
        int eliminationCount = 0;

        // Perform Variable Elimination
        std::vector<dai::Factor> factors = fg.factors();

        // for (dai::Factor& factor : factors) {
        //     std::cout << factor.p().size() << " " << factor.i().size() << endl;
        // }

        for (int i=0; i < constrainedElimOrder.size(); i++){

            std::cout << "Eliminate: " << constrainedElimOrder[i] << endl;

            // Find all factors fk that mention variable pi[i] 
            // f <- Then multiply those factors together 

            // Could also use findFactor and findVars in factorgraph
            std::vector<dai::Factor> toMultiply;

            for(int j=0; j<factors.size(); j++){
                
                dai::VarSet vars = (factors[j].vars());

                for (auto it = vars.begin(); it != vars.end(); ++it){

                    if(it->label() == constrainedElimOrder[i]){
                        
                        toMultiply.push_back(factors[j]);
                    }
                }
            }

            // If more than one factor found with the variable to be eliminated,
            // then multiply together the factors
            dai::Factor newFactor = toMultiply[0];
            if(toMultiply.size() > 1){

                for (int i = 1; i<toMultiply.size(); i++){

                    newFactor *= toMultiply[i];
                }
            }

            // Check if variable to eliminate is a MAP variable
            if (std::find(map_vars.begin(), map_vars.end(), constrainedElimOrder[i]) != map_vars.end()){
                
                // If variable pi(i) is a map variable then
                // fi <- max out pi(i) from f
                dai::VarSet vars = newFactor.vars();

                dai::VarSet varsToKeep;
                for (auto it = vars.begin(); it != vars.end(); ++it){

                    if(it->label() == constrainedElimOrder[i]){
                        continue;
                    }
                    else{
                        varsToKeep.insert(*it);
                    }
                }
                newFactor = newFactor.maxMarginalTransparent(varsToKeep,  false);
            }

            // Else fi <- sum out pi(i) from f
            else{
                
                dai::VarSet vars = newFactor.vars();

                dai::VarSet varsToKeep;
                for (auto it = vars.begin(); it != vars.end(); ++it){

                    if(it->label() == constrainedElimOrder[i]){
                        continue;
                    }
                    else{
                        varsToKeep.insert(*it);
                    }
                }
                newFactor = newFactor.marginal(varsToKeep, false);
            }

            // Replace all factors  fk in the set of factor S by factor fi
            // Remove factors to multiply and replace with newFactor
            for (auto it = toMultiply.begin(); it != toMultiply.end(); ++it){
                factors.erase(std::find_if(factors.begin(), factors.end(), [&](Factor const& f){ return f == *it; }));
            }

            //printAllMemStats();

            factors.push_back(newFactor);

            std::cout << "Eliminated " << ++eliminationCount << "/" << constrainedElimOrder.size() << endl;

            std::cout << sizeof(factors) << std::endl;
            

        }

        // Multiply remaining factors
        dai::Factor newFactor = factors[0];
        if(factors.size() > 1){
            for (int i = 1; i<factors.size(); i++){

                newFactor *= factors[i];
            }
        }
        std::cout << "Returning last factor" << std::endl;
        return newFactor;
    }
    
    catch( Exception &e ) {
        // Save a copy of /proc/self/maps to a file
        std::ofstream mapsFile("proc_self_maps_copy.txt");
        if (mapsFile.is_open()) {
            std::ifstream maps("/proc/self/maps");
            if (maps.is_open()) {
                mapsFile << maps.rdbuf();
                maps.close();
            } else {
                std::cerr << "Failed to open /proc/self/maps for reading." << std::endl;
            }

            mapsFile.close();
        } else {
            std::cerr << "Failed to open proc_self_maps_copy.txt for writing." << std::endl;
        }
    }
    
}


std::vector<unsigned long int> get_map(dai::FactorGraph fg, std::vector<unsigned int> hypothesis_vars, std::vector<unsigned int> evidence_vars,
	std::vector<unsigned int> evidence_values, bool mapList)
{
	// returns the map, the joint value assignment to the hypothesis vars that has maximum posterior probability given the evidence
	// while marginalizing over the (relevant) intermediate variables. As libDAI has no MAP function we just compute the distribution
	// over the MAP variables and select the state with maximum value from the posterior, which is the MAP assignment

	// when used in MFE function, the evidence is the actual 'real' evidence plus the sampled irrelevant intermediate nodes

    std::vector<unsigned long int> map;
    std::vector<unsigned long int> h_vars(begin(hypothesis_vars), end(hypothesis_vars));    // needs cast to long

	dai::VarSet hypSet = fg.inds2vars(h_vars);

	auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < evidence_vars.size(); i++)
    {
        fg.clamp(evidence_vars[i], evidence_values[i], false);
    }
	auto end = std::chrono::steady_clock::now();
	DEBUG(std::cout << "Clamping evidence " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns" << std::endl;)

	start = std::chrono::steady_clock::now();
    dai::PropertySet opts;
    dai::JTree jt = dai::JTree(fg, opts("updates",std::string("HUGIN"))("inference",std::string("SUMPROD")));
    jt.init();
    jt.run();
	end = std::chrono::steady_clock::now();
	DEBUG(std::cout << "JT run " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns" << std::endl;)

	start = std::chrono::steady_clock::now();
	dai::Factor hypFact = jt.calcMarginal(hypSet);
	end = std::chrono::steady_clock::now();
	DEBUG(std::cout << "Marginal time " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns" << std::endl;)
    
	// find element with maximum value ( = MAP explanation)
	start = std::chrono::steady_clock::now();
	double max = 0.0;
	int entry = 0; 
    for (int i = 0; i < hypFact.nrStates(); i++)
    {
        if (mapList)
        {
            std::cout << "entry ";
            for (auto const& j: dai::calcState(hypFact.vars(), i))
                std::cout << j.second;
            std::cout << " has probability " << hypFact.p()[i] << std::endl;
        }
		if (hypFact.p()[i] > max)
		{
    	    max = hypFact.p()[i];
			entry = i;
		}
    }

	// transform index to map of <Var, value> pairs
	std::map<dai::Var, size_t> mapValues = dai::calcState(hypFact.vars(), entry);
	end = std::chrono::steady_clock::now();
	DEBUG(std::cout << "MAP time " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns" << std::endl;)

	// now set map accordingly to the values in hypothesis_vars
	for (auto const& i: mapValues)
	{
		map.push_back(i.second);
	}
    DEBUG(std::cout << "map " << map << " has probability " << max << std::endl;)

	return map;
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
        fg.ReadFromFile(argv[1]);;


        // Example from page 260 of Modeling and Reasoning with Bayesian Networks
        // MAP variables = {I, J} = {0, 1}
        // Evidence: O = true. O is variable number 4
        // Constrained variable order = O, Y, X, I, J = 4, 2, 3, 0, 1

        std::vector<unsigned int> ex_evidenceVars =        { 4 };
	    std::vector<unsigned int> ex_evidenceValues =      { 1};
	    std::vector<unsigned int> ex_mapVars =             { 0, 1};
        std::vector<unsigned int> constrainedElimOrder =   { 4, 2, 3, 0, 1 };


        // dai::Factor MAP = get_m(fg, ex_mapVars, ex_evidenceVars, ex_evidenceValues, false);

        // cout << "Map probability: " << MAP.p() << endl;

        // cout << "Map instantiation: ";
        // for (const auto& myMap : MAP.i()) {
        //     std::cout << myMap << endl;
        // }    

        //cout << "Map Instantiation: " << MAP.i() << endl;
        // std::vector<std::pair<Var, dai::Real>> instantiation = MAP.getInstantiation();
        
        // for (const auto& pair : instantiation){
        //     std::cout << "Var: " << pair.first << " Value: " << pair.second << endl;
        // }
    }
}

}



