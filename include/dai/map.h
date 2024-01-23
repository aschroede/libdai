#ifndef __defined_libdai_map_h
#define __defined_libdai_map_h


#include <set>
#include <vector>
#include <dai/factorgraph.h>

namespace dai {

using namespace std;

template<class EliminationChoice>
vector<size_t> getConstrainedElimOrder(const FactorGraph &fg, EliminationChoice f, std::vector<unsigned int> map_vars);

dai::Factor get_map_ve(dai::FactorGraph fg, std::vector<unsigned int> map_vars, std::vector<unsigned int> evidence_vars,
        std::vector<unsigned int> evidence_values, bool mapList);

std::vector<unsigned long int> get_map(dai::FactorGraph fg, std::vector<unsigned int> hypothesis_vars, std::vector<unsigned int> evidence_vars,
	std::vector<unsigned int> evidence_values, bool mapList);

} 

#endif