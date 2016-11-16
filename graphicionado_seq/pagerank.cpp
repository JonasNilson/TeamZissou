#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include "graphicionado.hpp"
#include <array>

/**
* processEdge(double weight, int* srcProp, int* dstProp)
* weight - the edge weight
* srcProp - pointer to source property
* dstProp - pointer to destination property
* return: VertexProperty ans 
*/
VertexProperty processEdge(double weight, VertexProperty srcProp, VertexProperty dstProp) {  
  VertexProperty v;
  v.property = srcProp.property;
  return v;
}

/**
* User-defined computation
* Right now, simple implementation taken from graphicionado paper
*/
VertexProperty reduce(VertexProperty temp, VertexProperty result) {

  temp.property = temp.property + result.property;
  return temp;
}

/**
* User-defined computation
* Right now, simple implementation taken from graphicionado paper
*/
VertexProperty apply(VertexProperty vprop, VertexProperty temp, VertexProperty vconst) {

  //TODO
  // (A + (1 - A)vprop)/V deg 
  return temp;
}
