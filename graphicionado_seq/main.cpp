/**
 * Sequential implementation of graphicionado
 */
#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include <array>
#include "graphicionado.hpp" // Data structures for graph problems
#include "preprocess.hpp"
#include "loadSettings.hpp"

// Global variable declaration
//int THREADS = 4; // Set number of threads

Vertex* vertices; // All vertices in the graph
Vertex* activeVertex; 

Edge* edges; // All edges in the graph
unsigned int* edgeIDTable;

VertexProperty* vProperty; // property of nodes.
VertexProperty* vTempProperty; // new vProperty that been changed
VertexProperty* vConst; 

unsigned int totalVertexCount; // Number of nodes in the system.
unsigned int activeVertexCount; // Number of active nodes in the system.

// Make all cleanups needed before closing the program.
void terminateProgram(){
    std::cout << "Shutting down program! \n";
    argo::finalize(); // Cleanup for this node when program has finished.
}

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

  // Load the configuration settings from file (settings.cfg)
  loadSettings();
 
  // Local variable declaration
  //int id = argo::node_id(); // get this node unique index number starting from 0
  //int nodes = argo::number_of_nodes(); // return the total number of nodes in the Argo system.
 
  if(argc>1) {
      std::cout << "Reading graph from textfile: " << argv[1] << std::endl;
      //readGTgraphFile(argv[1]);
	   readTextFile(argv[1]);
	  // readTextFileWithLineParsing(argv[1]);
  }
  else {
    // Error no argument with filename
    std::cout << "Missing argument: graph file. \n";
    terminateProgram();
    return 1;
  }
  
  /* TODO: Implement section */
  Vertex dst; // WHat is this and what should it do?

  while(activeVertexCount != 0) {

    // START SUDO CODE from graphicionado
    //A Process edge Phase
    for (unsigned int i=0; i<activeVertexCount; i++) {
      Vertex src = activeVertex[i]; // Sequential Vertex Read
      unsigned int eID = edgeIDTable[src.ID]; // Edge ID Read
      if(eID == 0) continue; // If the index for the vertex is 0, it has no outgoing edges.
      eID--; // Edges index is shifted by 1 because if the index is 0 it indicates that there are no outgoing edges from this vertex.
      Edge e = edges[eID]; // Edge Read
      
      while (e.srcID == src.ID) {
		// dst.prop = vProperty[e.dstID]; // [OPT IONAL] Random Vertex Read
        VertexProperty res = processEdge(e.weight, src.prop, dst.prop);
        VertexProperty temp = vTempProperty[e.dstID]; // Random Vertex Read
        temp = reduce(temp, res);
        vTempProperty[e.dstID] = temp; // Random Vertex Write
        e = edges[++eID]; // Edge Read
      }
    }

    // Reset ActiveVertex and ActiveVertexCount
    activeVertexCount = 0; // reset activeVertexCount & active vertices      


    //B Apply Phase
    for (unsigned int i=0; i<totalVertexCount; i++) {
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

    //Settings check. If isAllVerticesActive = true then all vertices should be active over all iterations.
    if(isAllVerticesActive){
      activeVertexCount = totalVertexCount;//Set active Vertex count to be the total number of vertices.
    }

    //Settings check if we should use max iteration implementation or not
    if(maxIterations != 0){ //If setting is set to 0 it will use infinity iteration possibility
      maxIterations--;
      if(maxIterations == 0){
		  std::cout << "All iterations are done!" << std::endl;
        //Only enter this stage if it reached the maximum iteration count
        break; // break the entire loop
      }
    }

  }
	
  //END OF SUDO CODE

  terminateProgram(); // Cleanup for this node when program has finished.
 
  return 0;
}
