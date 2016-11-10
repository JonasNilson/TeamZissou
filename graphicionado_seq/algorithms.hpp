#ifndef algorithms_hpp
#define algorithms_hpp algorithms_hpp

#include "graphicionado.hpp"

VertexProperty processEdge(double weight, int* srcProp, int* dstProp);

VertexProperty reduce(VertexProperty temp, VertexProperty result);

VertexProperty apply(VertexProperty vprop, VertexProperty temp, VertexProperty vconst);

#endif
