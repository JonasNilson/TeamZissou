#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include "algorithms.hpp"
// #include "graphicionado.hpp" // Data structures for graph problems
#include <array>

/* User defined prop is void pointers that gets casted*/
// User defined computation
/**
* processEdge(double weight, int* srcProp, int* dstProp)
* weight - the edge weight
* srcProp - pointer to source property
* dstProp - pointer to destination property
* return: VertexProperty an 
*/
VertexProperty processEdge(double weight, int* srcProp, int* dstProp) {
	//TODO: Add user defined computation

	/* BFS Implementation */
	VertexProperty v;
	v.property = 0;
	return v; // BFS process edge implementation
	/* END OF BFS Implementation */
}

// User defined computation
VertexProperty reduce(VertexProperty temp, VertexProperty result) {
	//TODO: Add user defined computation
  VertexProperty v;
	/* BFS Implementation */
	//Result is currently null since we do not return anything from process edge phase. Should send IterCount to result if result is null in main loop?
	 v.property = std::min(temp.property,result.property); // Not correct but something to see and understand is this same as: min(Vtemp, IterCount)
	 return v;
	 /* END OF BFS Implementation */
}

// User defined computation
VertexProperty apply(VertexProperty vprop, VertexProperty temp, VertexProperty vconst) {
	//TODO: Add user defined computation

	// BFS Implementation
	return temp;
	// END OF BFS Implementation
}
