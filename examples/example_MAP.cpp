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

using namespace dai;
using namespace std;

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


        // Bound treewidth for junctiontree
        bool do_jt = true;
        try {
            boundTreewidth(fg, &eliminationCost_MinFill, maxstates );
        } catch( Exception &e ) {
            if( e.getCode() == Exception::OUT_OF_MEMORY ) {
                do_jt = false;
                cout << "Skipping junction tree (need more than " << maxstates << " states)." << endl;
            }
            else
                throw;
        }


    }




}