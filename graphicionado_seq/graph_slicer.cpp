
#include "graphicionado.hpp"


int add(Edge ID, unsigned int magicNum, int* next_index){
  unsigned int dst = ID.dstID;

  
  unsigned int newNum = magicNum;
  int i=0;

  while(1){
    if(dst < newNum){
      edgesArray[i][next_index[i]] = ID;
      next_index[i]++;
      return 0;
    }
    
    newNum = magicNum*(i+2);
    i++;
  }

}
void graphSlicer(){

  int next_index[partitions];
  unsigned int magicNum = numVertices / partitions;

  for(int i = 0; i < numEdges; i++){
    
    //spann av dst id sorterade på srcid numvert/port
    add(edges[i], magicNum, next_index); 
          
  }

  for(int i= 0; i< partitions; i++){
    setupEIT(numVertices,numEdges, vertices, edgeIDTableArray[i][0],edgesArray[i][0]);
  }  
  
}
