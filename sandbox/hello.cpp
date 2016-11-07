#include<argo.hpp>
#include<iostream>
#include<vector>

struct data{
  int array[4];
};
  

void solve(int tID, int nID, int* mem){
  std::cout << "I is thread:" << tID << "\n";
  std::cout << "address from thread: " << mem << "\n"; 
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
  std::cout << "Waiting for barrier tid: " << tID << "node id: " << nID << "\n";
  argo::barrier(4);
  std::cout << "done with barrier tid: " << tID << "node id: " << nID << "\n";

  if(nID ==0 && tID == 0)
    std::cout << "mem vaule for t0 after barrier: " << *mem << "\n";

  if(nID ==0 && tID == 2)
    std::cout << "mem vaule for t2 after barrier: " << *mem << "\n";

  
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

int main(int argc, char *argv[]){

  argo::init(128 * 1024 * 1024);


  int id = argo::node_id();
  //int nodes = argo::number_of_nodes();
  int threads = 4;

  /*int* array[4];
    array[0] = argo::conew_<int>();
    array[1] = argo::conew_<int>();
    array[2] = argo::conew_<int>();
    array[3] = argo::conew_<int>();
  */

  data* dataX = argo::conew_<data>(); 
  
  std::cout << "address: " << dataX<< "\n";
  std::cout << "address 2: " << &dataX->array[0] << "\n";
  
  for(int i= 0; i< 2; ++i){
    dataX->array[i] = 5;
  }
  argo::barrier();
  std::vector<std::thread> threads_vector(threads);
  for(int i= 0; i < threads; ++i){
    threads_vector[i] = std::thread(solve,i,id,&dataX->array[i]);
  }
  for(int i=0; i < threads;++i){
    threads_vector[i].join();
  }
  
  
  argo::finalize();

  return 0;

}
