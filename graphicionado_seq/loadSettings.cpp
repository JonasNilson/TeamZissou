#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>
#include "loadSettings.hpp"

//Settings
bool isAllVerticesActive;
unsigned int maxIterations;
bool edgeDuplicates;

/**
* Get the variable name
* Input: Index of first character of variable from the buffer.
*/
void setGlobalVariables(char * buffer, int index){
	//Read variable name
	std::string vName; // String we use to store the variable name.
	char c = buffer[index];
	while(c != ':') {
		vName += c;
		c = buffer[++index];
	}
	
	//Read variable value
	index++; // to skip ":" character
	c = buffer[index];
	std::string value; // String we use to store variable value.
	while(c != ']'){
		value = c;
		c = buffer[++index];
	}

	//Set global setting variables
	if(vName == "isAllVerticesActive")
		isAllVerticesActive = (value == "y" ? true : false);
	else if(vName == "maxIterations")
		maxIterations = (unsigned int) std::stoll(value);
	else if(vName == "edgeDuplicates")
		edgeDuplicates = (value == "y" ? true : false);
	else 
		std::cout << "Something went wrong with the reading of " << vName << "\n";

	std::cout << "Setting of " << vName << " has read the option [" << value <<  "] successfully.\n";
}

/*
* length - size of buffer
* buffer - stored setting text buffer
*/
void initSettings(char * buffer, int length){
	char c;
	std::string str; // String we use to get 
	for(int i = 0; i < length; ++i){
		c = buffer[i];
		if(c == '$'){
			setGlobalVariables(buffer, i+1);
		}
	}
}

void readSettings(){

  // read a file into memory

  std::ifstream file("settings.cfg", std::ifstream::binary); // File open stream
  if (file) {
    // get length of file:
    file.seekg(0, file.end); //get end of file location
    int length = file.tellg(); // get length
    file.seekg(0, file.beg); // set to beginning again

    char * buffer = new char [length];

    std::cout << "Read setting file... \n";
    
    // read data as a block:
    file.read (buffer,length);

    if (file)
      std::cout << "Load settings... \n";
    else
      std::cout << "Error: could not read setting file. Only " << file.gcount() << " could be read";

    file.close(); //Close file stream

    //buffer contains the entire file.
    initSettings(buffer,length+1);

    delete[] buffer;
  }
}

void loadSettings(){
	std::string filename = "settings.cfg"; // filename of settings to be read.
	readSettings();
	std::cout << "Settings have been loaded.\n";
}
