#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include "graphicionado.hpp" // Data structures for graph problems
#include <array>

// User defined computation
/**
* processEdge(double weight, int* srcProp, int* dstProp)
* weight - the edge weight
* srcProp - pointer to source property
* dstProp - pointer to destination property
* return: VertexProperty v 
*/
VertexProperty processEdge(double weight, VertexProperty srcProp, VertexProperty dstProp) {	
	VertexProperty v;
	v.property = srcProp.property + 1; // BFS: the vertex property is the distance/depth from the starting node (IterCount)
	return v; // BFS process edge implementation
}

// User defined computation
VertexProperty reduce(VertexProperty temp, VertexProperty result) {
	VertexProperty v;
	//Result is currently null since we do not return anything from process edge phase. Should send IterCount to result if result is null in main loop?
	 v.property = std::min(temp.property,result.property); // Not correct but something to see and understand is this same as: min(Vtemp, IterCount)
	 return v;
}

// User defined computation
VertexProperty apply(VertexProperty vprop, VertexProperty temp, VertexProperty vconst) {
	return temp;
}
