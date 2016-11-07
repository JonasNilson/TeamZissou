/**
* Parallel implementation of graphicionado
*/

#include<argo.hpp> // Get access to Argo function calls.
#include<iostream> // Used for output prints.


// Global variable declaration
int THREADS = 4; // Set number of threads

int main(int argc, char *argv[]){
	/* 
	 Set up the argo environment, caches and global memory. 
	 Init the total space that is shared between all nodes. 
	*/
	argo::init(128 * 1024 * 1024); 

	// Local variable declaration
	int id = argo::node_id(); // get this node unique index number starting from 0
	int nodes = argo::number_of_nodes(); // return the total number of nodes in the Argo system.

	//TODO: Implement 



	argo::finalize(); // Cleanup for this node when program has finished.

	return 0;
}