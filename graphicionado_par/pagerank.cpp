#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include "graphicionado.hpp"
#include <array>

std::string graphAlgorithm = "PR"; // Set algorithm used
double dampenFactor = 0.85; // If to low it overshoot, if to high it might get dumped

/**
* processEdge(double weight, int* srcProp, int* dstProp)
* weight - the edge weight
* srcProp - pointer to source property
* dstProp - pointer to destination property
* return: VertexProperty ans 
*/
VertexProperty processEdge(double weight, VertexProperty srcProp, VertexProperty dstProp) {  
  VertexProperty v;
  std::cout << "PAGERANK: processEdge: srcProp.property: " << srcProp.property << std::endl;
  
  v.property = srcProp.property;
  
  return v;
}

/**
* User-defined computation
* Right now, simple implementation taken from graphicionado paper
*/
VertexProperty reduce(VertexProperty temp, VertexProperty result) {
  std::cout << "PAGERANK: reduce: temp.property: " << temp.property << "; result.property: " << result.property << std::endl;
  
  temp.property = temp.property + result.property;
  return temp;
}

/**
* User-defined computation
* Right now, simple implementation taken from graphicionado paper
*/
VertexProperty apply(VertexProperty vprop, VertexProperty temp, VertexProperty vconst) {
	VertexProperty v;
	if(vconst.property == 0) {
		// If no outgoing edges it give 0 property to that vertex in page rank.
		v.property = 0;
		return v;
	}
	
	std::cout << "PAGERANK: apply: temp.property: " << temp.property << "; vconst.property: " << vconst.property << std::endl;
	
	v.property = (dampenFactor + (1 - dampenFactor) * temp.property) / vconst.property;
	
	std::cout << "PAGERANK: apply: v.property: " << v.property << std::endl;
	
 	return v;
//                     (A is value between 0 - 1   used 0.85 is common dampen value)   v deg is outgoing edges 
  //return temp;
}
