// Compile using: g++ test_read.cpp -std=c++11

#include <iostream> // Used for output prints.
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

void readTextFileLineByLine(const char * filename) {
	std::cout << "___readTextFileLineByLine___" << std::endl;

	// Local variable declaration
	std::ifstream file;
	std::string line;
	unsigned int numVertices;
	unsigned int numEdges;

	file.open(filename); // Open file with filename.

	getline(file,line); // get first line in file.
	numVertices = std::stoll(line);
	std::cout << "numVertices: \t" << numVertices << std::endl;
	getline(file,line); // get second line in file.
	numEdges = std::stoll(line);
	std::cout << "numEdges: \t" << numEdges << std::endl;

	for(unsigned int i = 0; i < numVertices; ++i){
		getline(file,line); // Saves the line in line.
		std::cout << "Vertex ID: \t" << line << std::endl;
		getline(file,line);
		std::cout << "Vertex prop: \t" << line << std::endl;
	}

	for(unsigned int i = 0; i < numEdges; ++i){
		getline(file,line); // Saves the line in line.
		std::cout << "Edge srcID: \t" << line << std::endl;
		getline(file,line);
		std::cout << "Edge dstID: \t" << line << std::endl;
		getline(file,line);
		std::cout << "Edge weight: \t" << line << std::endl;
	}

	file.close(); // Closes file 
}

void readTextFileWithParsing(const char* filename) {
	std::cout << "___readTextFileWithParsing___" << std::endl;

	// Local variable declaration
	std::ifstream file;
	std::string line;
	std::string item;
	char delimiter = ',';
	unsigned int numVertices;
	unsigned int numEdges;

	file.open(filename);

	// Read the number of vertices 
	getline(file, line);
	numVertices = std::stoll(line);
	std::cout << "numVertices: \t" << numVertices << std::endl;

	// Read the number of edges
	getline(file, line);
	numEdges = std::stoll(line);
	std::cout << "numEdges: \t" << numEdges << std::endl;

	// Read all the vertices
	for(unsigned int i = 0; i < numVertices; ++i){
		getline(file,line); // Saves the line in line.
	    
		std::stringstream ss; // Create a new string stream
		ss.str(line); // Stream the current line
		std::getline(ss, item, delimiter); // Read the vertex ID
		std::cout << "Vertex ID: \t" << item << std::endl;
		std::getline(ss, item, delimiter); // Read the vertex property
		std::cout << "Vertex prop: \t" << item << std::endl;
	}

	// Read all the edges
	for(unsigned int i = 0; i < numEdges; ++i){
		getline(file,line); // Saves the line in line.

		std::stringstream ss; // Create a new string stream
		ss.str(line); // Stream the current line
		std::getline(ss, item, delimiter); // Read the edge srcID
		std::cout << "Edge srcID: \t" << item << std::endl;
		std::getline(ss, item, delimiter); // Read the edge dstID
		std::cout << "Edge dstID: \t" << item << std::endl;
		std::getline(ss, item, delimiter); // Read the edge weight
		std::cout << "Edge weight: \t" << item << std::endl;
	}

	// Close the file
	file.close();
}

int main(int argc, char *argv[]) {
	// void readTextFile(char * filename, Vertex* vertices, Edge* edges)
	const char* filename;
	filename = "test_graph.txt";
	readTextFileLineByLine(filename);
	filename = "test_graph_2.txt";
	readTextFileWithParsing(filename);
}
