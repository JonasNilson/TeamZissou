#include "graphicionado.hpp"
#include "loadSettings.hpp"
#include "preprocess.hpp"
#include <iostream>

int add(Edge ID, unsigned int magicNum, int* next_index){

  // std::cout << "segfault check \n";

  unsigned int dst = ID.dstID;

  
  unsigned int newNum = magicNum;
  int i=0;

  while(1){
    if(dst < newNum){
      //	  std::cout <<"dst: "  << dst << "\n";
      //	  std::cout << "segfault check 3 \n";
      edgesArray[i][next_index[i]] = ID;
      next_index[i]++;
      return 0;
    }
    
    newNum = magicNum*(i+2);
    i++;
    //	std::cout << "segfault check 2 \n";
  }

}
void graphSlicer(){

  //  std::cout << "graph slicer initiated \n";
  
  int next_index[partitions];
  for(unsigned int i = 0; i<partitions; i++){
    next_index[i]=0;
  }
  
  unsigned int magicNum = numVertices / partitions;

  for(unsigned int i = 0; i < numEdges; i++){
    
    //spann av dst id sorterade på srcid numvert/port
    add(edges[i], magicNum, next_index); 
          
  }

  for(unsigned int i= 0; i< partitions; i++){
    setupEIT(numVertices,numEdges, vertices, &edgeIDTableArray[i][0],&edgesArray[i][0]);
  }  
  
}
