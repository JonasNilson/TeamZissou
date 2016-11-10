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
	verticies = argo::conew_array<Vertex>(numVerticies); 
	activeVertex = argo::conew_array<Vertex>(numVerticies); 
	
	edges = argo::conew_array<Edge>(numEdges); 
	edgeIDTable = argo::conew_array<unsigned int>(numVerticies); // make it of size number of verticies

	vProperty = argo::conew_array<VertexProperty>(numVerticies);
	vTempProperty = argo::conew_array<VertexProperty>(numVerticies);
	vConst = argo::conew_array<VertexProperty>(numVerticies);
}


/*
* Compare edges with srcID then dstID
*/
bool edgeCompare(edge e1, edge e2){
	// Sort by srcID
	if(e1.srcID < e2.srcID || (e1.srcID == e2.srcID && e1.dstID < e2.dstID)){
		return true;
	}
	return false;
}

/*
* Compare vertex with ID
*/
bool vertexCompare(vertex v1, vertex v2){
	if(v1.ID < v2.ID){
		return true;
	}
	return false;
}


/*
* Organize already collective allocated space in the argoDSM system 
* with data like graphicionado's model.
* numVerticies: total number of verticies in graph
* numEdges: total number of edges in graph
*/
void initializeDSM(unsigned int numVerticies, unsigned int numEdges){
	
	// Sort edges after srcID and then dstID with function edgeCompare
	std::sort(edges,edges[numEdges],edgeCompare); // if this one does not work use std::sort(edges,edges+numEdges,edgeCompare);

	// Sort verticies after ID to make sure the edge ID table correspond to correct node.
	std::sort(verticies,verticies[numVerticies],vertexCompare);

  	// TODO: Init ActiveVerticies
	// root node? where we start with. Maybe an argument what we take in

  	// Init EdgeIDTable
  	unsigned int vertex_ID = 0;
	for(unsigned int i = 0; i < numEdges; ++i){
		if(edges[i].srcID == vertex_ID){
			edgeIDTable[vertex_ID] = i;
			vertex_ID++;
		}
	}

  	// Init VProperty
	for(unsigned int i =0; i < numVerticies; ++i) {
    	vProperty[i] = verticies[i].prop;
  	}

  	// TODO: Init VTempProperty
  	//WARNING might not be needed to initialized 
  	for(unsigned int i =0; i < numVerticies; ++i) {
      	vProperty[i] = verticies[i].prop;
   	}

  	// TODO: Init VConst
   	// Get more info what this is? Do we get this from the data?

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
