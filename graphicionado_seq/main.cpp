#include<iostream>
#include<vector>
#inlcude<argo.hpp>
#include<graphiciando.hpp>
#include<array>

VertexProperty VTempProperty[];
VertexProperty VProperty[];
VertexProperty VConst[];
Edge Edges[];
Edge EdgeIDTable[];
Vertex ActiveVerterx[];


Graph graph;


void Process_Edge(){

}

void Reduce(){

}

void Apply(){

}


int main(int argc, char *argv[]){

  argo::init(128 * 1024 * 1024);

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

  
  
  while(1){

    // Processing Phase
    for (int i=0; i<ActiveVertexCount; i++) {
      Vertex src = ActiveVertex[i]; // Sequential Vertex Read
      int eid = EdgeIDTable[src.id]; // Edge ID Read
      Edge e = Edges[eid]; // Edge Read
      while (e.srcid == src.id) {
	dst.prop = VProperty[e.dstid]; // [OPT IONAL] Random Vertexg Read
	VertexProperty res = Process_Edge(e.weight, src.prop, dst.prop);
	VertexProperty temp = VTempProperty[e.dstid]; // Random Vertex Read
	temp = Reduce(temp, res);
	VTempProperty[e.dstid] = temp; // Random Vertex Write
	e = Edges[++eid]; // Edge Read
      }
    }
    // Reset ActiveVertex and ActiveVertexCount
    // Apply Phase
    for (int i=0; i<TotalVertexCount; i++) {
      VertexProperty vprop = VProperty[i]; // Sequential Vertex Read
      VertexProperty temp = VTempProperty[i]; // Sequential Vertex Read
      VertexProperty vconst = VConst[i];
      temp = Apply(vprop, temp, vconst);
      VProperty[i] = temp; // Sequential Vertex Write
      if(temp != vprop) {
	Vertex v;
	v.id = i;
	v.prop = temp;
	ActiveVertex[ActiveVertexCount++] = v; // Sequential Vertex Write
      }
    }

    
  }
  
  argo::finalize();
  
}
