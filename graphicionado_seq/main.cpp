/**
* Sequential implementation of graphicionado
*/

#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include "graphicionado.hpp" // Data structures for graph problems
#include "algorithms.hpp"
#include "read_graph.hpp"
#include <array>

// Global variable declaration
int THREADS = 4; // Set number of threads

//Graph graph; // Not being used at this moment.

Vertex* verticies; // All verticies in the graph
Vertex* activeVertex; 

Edge* edges; // All edges in the graph
Edge* edgeIDTable;

VertexProperty* vProperty;
VertexProperty* vTempProperty;
VertexProperty* vConst;

/*
* Collective allocations for the argoDSM system.
* numVerticies: total number of verticies in graph
* numEdges: total number of edges in graph
*/
void setupDSM(unsigned int numVerticies, unsigned int numEdges){
	verticies = argo::conew_array<Vertex>(numVerticies); // TODO: Check if this allocation with data times numVerticies actually allocate that size. 
	activeVertex = argo::conew_array<Vertex>(numVerticies); // TODO: Check if this allocation actually allocate that wanted size.
	
	edges = argo::conew_array<Edge>(numEdges); // TODO: Check if this allocation actually allocate that wanted size. 
	edgeIDTable = argo::conew_array<Edge>(numEdges); // TODO: Check if this allocation actually allocate that wanted size.

	vProperty = argo::conew_array<VertexProperty>(numVerticies);// TODO: Check if this allocation actually allocate that wanted size.
	vTempProperty = argo::conew_array<VertexProperty>(numVerticies);// TODO: Check if this allocation actually allocate that wanted size.
	vConst = argo::conew_array<VertexProperty>(numVerticies);// TODO: Check if this allocation actually allocate that wanted size.
}

/*
* Organize already collective allocated space in the argoDSM system 
* with data like graphicionado's model.
* numVerticies: total number of verticies in graph
* numEdges: total number of edges in graph
*/
void initializeDSM(unsigned int numVerticies, unsigned int numEdges){
	
  	// TODO: Init ActiveVerticies

  	// Init EdgeIDTable
  	// FIXME SHOULD BE SHORTED BY SRC FIRST AND THEN DEST SECOND
	unsigned int edgeIndex=0;
	for(unsigned int i= 0; i < numEdges; ++i){
    	edgeIDTable[i] = edges[edgeIndex];
    	unsigned int j = 0;
    	while(verticies[i].ID == edges[j].srcID){
      		j++;
      		edgeIndex++;
    	}
  	}

  	// Init VProperty
	for(unsigned int i =0; i < numVerticies; ++i) {
    	vProperty[i] = verticies[i].prop;
  	}

  	// TODO: Init VTempProperty
  	
  	// TODO: Init VConst
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

	// Local variable declaration
	//	int id = argo::node_id(); // get this node unique index number starting from 0
	//int nodes = argo::number_of_nodes(); // return the total number of nodes in the Argo system.

	/* TODO: Implement section */

	/*
	// START SUDO CODE from graphicionado
	for (int i=0; i<activeVertexCount; i++) {
		Vertex src = activeVertex[i]; // Sequential Vertex Read
		int eid = edgeIDTable[src.ID]; // Edge ID Read
		Edge e = edges[eID]; // Edge Read
		
		while (e.srcID == src.ID) {
			dst.prop = vProperty[e.dstID]; // [OPT IONAL] Random Vertex Read
			VertexProperty res = processEdge(e.weight, src.prop, dst.prop);
			VertexProperty temp = vTempProperty[e.dstID]; // Random Vertex Read
			temp = reduce(temp, res);
	 		vTempProperty[e.dstID] = temp; // Random Vertex Write
	 		e = edges[++eID] // Edge Read
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
		if(temp != vprop) {
			Vertex v;
			v.ID = i;
			v.prop = temp;
			activeVertex[activeVertexCount++] = v; // Sequential Vertex Write
		}
	}

	*/
	//END OF SUDO CODE

	argo::finalize(); // Cleanup for this node when program has finished.

	return 0;
}
