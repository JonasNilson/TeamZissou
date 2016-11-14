/**
 * Sequential implementation of graphicionado
 */
#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include "graphicionado.hpp" // Data structures for graph problems
#include "algorithms.hpp"
#include "preprocess.hpp"
#include <array>
//#include <algorithm>

// Global variable declaration
//int THREADS = 4; // Set number of threads

//Graph graph; // Not being used at this moment.

/**
   Information about graphicionado
   * EdgeIDTable - is constructed and stored in memory. It is an array that store edge id of the first edge of each vertex. Sorted by srcID and then dstID.
   * VProperty - is an array that store vertex property (Unknown for now)
   * VTempProperty - same as VProperty but updated from the function reduce.
   * Edges - array of all edges. Is indexed (sorted) by edge ID E.g of an edge: (srcID, dstID, weight) (NOT SURE VISULISE TOMORROW)
   */
int main(int argc, char *argv[]){
  /* 
     Set up the argo environment, caches and global memory. 
     Init the total space that is shared between all nodes. 
  */
  argo::init(128 * 1024 * 1024); 

  // Local variable declaration
  //	int id = argo::node_id(); // get this node unique index number starting from 0
  //int nodes = argo::number_of_nodes(); // return the total number of nodes in the Argo system.

  readTextFile("filename.txt");
  
  /* TODO: Implement section */
  int activeVertexCount = 2; // Number of active nodes.
  Vertex dst;
  	
  // START SUDO CODE from graphicionado
  for (int i=0; i<activeVertexCount; i++) {
    Vertex src = activeVertex[i]; // Sequential Vertex Read
    int eID = edgeIDTable[src.ID]; // Edge ID Read
    Edge e = edges[eID]; // Edge Read
    
    while (e.srcID == src.ID) {
      dst.prop = vProperty[e.dstID]; // [OPT IONAL] Random Vertex Read
      VertexProperty res = processEdge(e.weight, src.prop, dst.prop);
      VertexProperty temp = vTempProperty[e.dstID]; // Random Vertex Read
      temp = reduce(temp, res);
      vTempProperty[e.dstID] = temp; // Random Vertex Write
      e = edges[++eID]; // Edge Read
    }
  }
  // Reset ActiveVertex and ActiveVertexCount
  
  //B Apply Phase
  for (int i=0; i<totalVertexCount; i++) {
    VertexProperty vprop = vProperty[i]; // Sequential Vertex Read
    VertexProperty temp = vTempProperty[i]; // Sequential Vertex Read
    VertexProperty vconst = vConst[i];
    temp = apply(vprop, temp, vconst);
    vProperty[i] = temp; // Sequential Vertex Write
    if(temp.property != vprop.property) {
      Vertex v;
      v.ID = i;
      v.prop = temp;
      activeVertex[activeVertexCount++] = v; // Sequential Vertex Write
    }
  }
	
  //END OF SUDO CODE

  argo::finalize(); // Cleanup for this node when program has finished.

  return 0;
}
