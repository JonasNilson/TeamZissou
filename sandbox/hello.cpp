#include<argo.hpp> // Get access to argo function calls.
#include<iostream> // Used for output prints.
#include<vector> // Used for threads vector to store them.

struct data{
  int array[4]; // Array with 4 ints
};
  

// Function that threads run
void solve(int tID, int nID, int* mem){

  // Prints
  std::cout << "I is thread:" << tID << "\n";
  std::cout << "address from thread: " << mem << "\n"; 

  /* Switch for each node id (index) to run matching case id.
     Each case print an value *mem then write to *mem and print 
     *mem again to see if the update can be seen. */
  if(nID == 0){
    switch(tID){
    case 0:
      std::cout << "mem before w: " << *mem << "\n";
      *mem = 0;
      std::cout << "mem after w: " << *mem << "\n";
      break;
    case 1:
      std::cout << "mem before w: " << *mem << "\n";
      *mem = 1;
      std::cout << "mem after w: " << *mem << "\n";
      break;
    case 2:
      std::cout << "mem before w: " << *mem << "\n";
      *mem = 2;
      std::cout << "mem after w: " << *mem << "\n";
      break;
    case 3:
      std::cout << "mem before w: " << *mem << "\n";
      *mem = 3;
      std::cout << "mem after w: " << *mem << "\n";
      break;
    }
  }


  std::cout << "Waiting for barrier tid: " << tID << "node id: " << nID << "\n"; // Print threads that reached this barrier
  argo::barrier(4); // Barrier that wait for all nodes and 4 threads in each node to reach this point so they get synchronized
  std::cout << "done with barrier tid: " << tID << "node id: " << nID << "\n"; // Print threads after barrier

  // Node index 0's first thread 0 print *mem value after barrier.
  if(nID ==0 && tID == 0)
    std::cout << "mem vaule for t0 after barrier: " << *mem << "\n";

  // Node index 0's first thread 2 print *mem value after barrier
  if(nID ==0 && tID == 2)
    std::cout << "mem vaule for t2 after barrier: " << *mem << "\n";

  // Node with id 1 (index 1) each threads will print their corresponding value from the data
  if(nID == 1){
    switch(tID){
    case 0:
      std::cout << "thread ID : " << tID << " mem: "  << *mem << "\n";
      break;
    case 1:
      std::cout << "thread ID : " << tID << " mem: "  << *mem << "\n";
      break;
    case 2:
      std::cout << "thread ID : " << tID << " mem: "  << *mem << "\n";
      break;
    case 3:
      std::cout << "thread ID : " << tID << " mem: "  << *mem << "\n";
      break;
    }
    std::cout << "thread: " << tID << " is done \n";
  }
}


//main function
int main(int argc, char *argv[]){

  /* Init the total space that is shared between all nodes.
    Also set up the argo enviroment, caches and global memory. */
  argo::init(128 * 1024 * 1024);


  int id = argo::node_id(); // get this node uniqe index number 
  int nodes = argo::number_of_nodes(); // return the number of nodes in the argo system.
  int threads = 4; // Number of threads that we want to run.

  /*int* array[4];
    array[0] = argo::conew_<int>();
    array[1] = argo::conew_<int>();
    array[2] = argo::conew_<int>();
    array[3] = argo::conew_<int>();
  */

  data* dataX = argo::conew_<data>(); // Collective allocation that every node need to do. The allocation size is the size of the data type <data> (struct in our case). 
  
  // prints
  std::cout << "address: " << dataX<< "\n";
  std::cout << "address 2: " << &dataX->array[0] << "\n";
  
  // Init the a struct data with 5
  for(int i= 0; i< 2; ++i){
    dataX->array[i] = 5;
  }

  // synchronization of all argoDSM Nodes to reach this barrier and make writebacks and update everything.
  argo::barrier(); 


  std::vector<std::thread> threads_vector(threads); // Create a vector of size threads (works like an array)
  
  // Create threads and give them function solve to run. And set them in the thread array vector.
  for(int i= 0; i < threads; ++i){
    threads_vector[i] = std::thread(solve,i,id,&dataX->array[i]);
  }

  // Wait for all threads to finish their task function before this main function can continue.
  for(int i=0; i < threads;++i){
    threads_vector[i].join();
  }
  
  
  argo::finalize(); // Cleanup that the tells argoDSM that the program is done.

  return 0;

}
