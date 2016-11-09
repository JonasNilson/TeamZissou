/**
* Sequential implementation of graphicionado
*/

#include<argo.hpp> // Get access to Argo function calls.
#include<iostream> // Used for output prints.
#include<graphicionado.hpp> // Data structures for graph problems
#include<array>


// Global variable declaration
template <typename VertexProperty>;
int THREADS = 4; // Set number of threads
VertexProperty VTempProperty[];
VertexProperty VProperty[];
VertexProperty VConst[];
Edge Edges[];
Edge EdgeIDTable[];
Vertex ActiveVerterx[];

Graph graph;

/**
Information about graphicionado
* EdgeIDTable - is constructed and stored in memory. It is an array that store edge id of the first edge of each vertex. Sorted by srcID and then dstID.
* VProperty - is an array that store vertex property (Unknown for now)
* VTempProperty - same as VProperty but updated from the function reduce.
* Edges - array of all edges. Is indexed (sorted) by edge ID E.g of an edge: (srcID, dstID, weight) (NOT SURE VISULISE TOMORROW)
*/



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
	return; // BFS process edge implementation
	/* END OF BFS Implementation */
}

// User defined computation
VertexProperty reduce(VertexProperty temp, VertexProperty result) {
	//TODO: Add user defined computation

	/* BFS Implementation */
	//Result is currently null since we do not return anything from process edge phase. Should send IterCount to result if result is null in main loop?
	return std::min(temp,result); // Not correct but something to see and understand is this same as: min(Vtemp, IterCount)
	/* END OF BFS Implementation */
}

// User defined computation
VertexProperty apply(VertexProperty vprop, VertexProperty temp, VertexProperty vconst) {
	//TODO: Add user defined computation

	// BFS Implementation
	return temp;
	// END OF BFS Implementation
}




int main(int argc, char *argv[]){
	/* 
	 Set up the argo environment, caches and global memory. 
	 Init the total space that is shared between all nodes. 
	*/
	argo::init(128 * 1024 * 1024); 

	// Local variable declaration
	int id = argo::node_id(); // get this node unique index number starting from 0
	int nodes = argo::number_of_nodes(); // return the total number of nodes in the Argo system.


	/*LINUS*/
	Edges = graph.EdgeList;

	for(int i =0; i< graph.VertexList.size()){
    	VProperty[i] = graph.VertexList[i].VertexProperty;
  	}
  
	int edgeIndex=0;
  
	for(int i= 0; i< graph->VertexList.size(); ++i){
    	EdgeIDTable[i] = Edges[edgeIndex];
    	int j = 0;
    	while(graph->Vertex[i].id == graph->Edges[j].srcid){
      		j++;
      		edgeIndex++;
    	}
  	}
	ActiveVertex[0] = Vertex[0];

	/*END OF LINUS*/



	/* TODO: Implement section */
	// START SUDO CODE from graphicionado
	for (int i=0; i<ActiveVertexCount; i++) {
		Vertex src = ActiveVertex[i]; // Sequential Vertex Read
		int eid = EdgeIDTable[src.id]; // Edge ID Read
		Edge e = Edges[eid]; // Edge Read
		
		while (e.srcid == src.id) {
			dst.prop = VProperty[e.dstid]; // [OPT IONAL] Random Vertex Read
			VertexProperty res = processEdge(e.weight, src.prop, dst.prop);
			VertexProperty temp = VTempProperty[e.dstid]; // Random Vertex Read
			temp = reduce(temp, res);
	 		VTempProperty[e.dstid] = temp; // Random Vertex Write
	 		e = Edges[++eid] // Edge Read
	 	}
	}
	// Reset ActiveVertex and ActiveVertexCount
	
	//B Apply Phase
	for (int i=0; i<TotalVertexCount; i++) {
		VertexProperty vprop = VProperty[i]; // Sequential Vertex Read
		VertexProperty temp = VTempProperty[i]; // Sequential Vertex Read
		VertexProperty vconst = VConst[i];
		temp = apply(vprop, temp, vconst);
		VProperty[i] = temp; // Sequential Vertex Write
		if(temp != vprop) {
			Vertex v;
			v.id = i;
			v.prop = temp;
			ActiveVertex[ActiveVertexCount++] = v; // Sequential Vertex Write
		}
	}

	//END OF SUDO CODE



	argo::finalize(); // Cleanup for this node when program has finished.

	return 0;
}