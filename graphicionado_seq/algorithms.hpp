#ifndef algorithms_hpp
#define algorithms_hpp algorithms_hpp

#include "graphicionado.hpp"

VertexProperty processEdge(double weight, VertexProperty srcProp, VertexProperty dstProp);

VertexProperty reduce(VertexProperty temp, VertexProperty result);

VertexProperty apply(VertexProperty vprop, VertexProperty temp, VertexProperty vconst);

#endif
