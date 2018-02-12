/*
Project: 	IMS Cellular Automaton simulation
Authors:	Jakub Svoboda, Michael Schmid
Login:		xsvobo0z, xschmi08
Date:		2. December 2017
Course:		IMS 2017
*/

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <ctype.h>
#include <cstdlib>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <fcntl.h>
#include <experimental/filesystem>
#include <climits>
#include <cmath>
#include "main.h"

using namespace std;
namespace fs = std::experimental::filesystem;

Parameters params;
 

class Cell{
	private:
		uint32_t x;
		uint32_t y;
		uint32_t type;				//density, see or city
		double altitude;
		uint32_t fuelType;			//tree, bushes, fields, city, none
		uint32_t state;
		uint32_t nextState;

	public:
		Cell(){
		}

		void setCell(uint32_t x, uint32_t y, uint32_t type, double altitude, uint32_t fuelType){
			this->altitude = altitude;
			this->type = type;
			this->x = x;
			this->y = y;
			this->fuelType = fuelType;
		}

		double getAltitude(){
			return altitude;
		}
		uint32_t getType(){
			return type;
		}
		uint32_t getX(){
			return x;
		}
		uint32_t getY(){
			return y;
		}
		uint32_t getFuelType(){
			return fuelType;
		}
		uint32_t getState(){
			return state;
		}
		uint32_t getNextState(){
			return nextState;
		}

		void setAltitude(double altitude){
			this->altitude = altitude;
		}
		void setType(double altitude){
			this->type = type;
		}
		void setX(uint32_t x){
			this->x = x;
		}
		void setY(uint32_t y){
			this->y = y;
		}
		void setFuelType(uint32_t fuelType){
			this->fuelType = fuelType;
		}
		void setState(uint32_t state){
			this->state = state;
		}
		void setNextState(uint32_t nextState){
			this->nextState = nextState;
		}

};

class Map{
	public:
		uint32_t width;
		uint32_t height;
		vector<vector<Cell>> array;
		
		Map(uint32_t width, uint32_t height, string path){
			this->width=width;
			this->height=height;
			std::ifstream file;
			file.open(path);				
			string line;
			array.resize(height,vector<Cell>(width));

			for(uint32_t x = 0; x <height; x++){
				for(uint32_t y = 0; y <width; y++){
					if(std::getline(file, line)){
						std::istringstream iss(line);
						vector<string> v;
						for(string word; iss >> word; )
							v.push_back(word);
						uint32_t type,type2;
						if(v[2]=="sea"){
							type = SEA;
						}else if(v[2]=="city"){
							type = CITY;
						}else if(v[2]=="sparse"){
							type = SPARSE;
						}else if(v[2]=="normal"){
							type = MEDIUM;
						}else if(v[2]=="dense"){
							type = DENSE;
						}else{
							cerr<<"unknown cell type: " <<v[2]<<endl;
							exit(-1);
						}
						if(v[4]=="none"){
							type2 = NONE;
						}else if(v[4]=="field"){
							type2 = FIELDS;
						}else if(v[4]=="bushes"){
							type2 = BUSHES;
						}else if(v[4]=="trees"){
							type2 = TREES;
						}else if(v[4]=="city"){
							type2 = CITY;	
						}else{
							cerr<<"unknown cell type: " <<v[4]<<endl;
							exit(-1);
						}
						array[x][y].setCell(stoul(v[0]),stoul(v[1]),type,stod(v[3]),type2);
						
					}else{
						cerr<<"Not enough data in the data file for a map of this size."<< x <<" "<< y <<endl;
						exit(0);
					}
				}
			}
		}
		uint32_t getWidth(){
			return this->width;
		}

		uint32_t getHeight(){
			return this->height;
		}
};
Map* lattice;

//Sets parameters' values to Parameters structure and checks whether correct values were passed or not.
void validateParameters(int argc, char* argv[]){
	params = {"", false, UINT_MAX, UINT_MAX};		//default values
	int opt;
	while ((opt = getopt(argc, argv, "hf:x:y:a:V:s:d:")) != -1) {		//parse with getopt()
		switch (opt) {
			case 'h':											//help parameter
				params.h = true;
				cout << ("./ims [-x STARTX] [-y STARTY] [-h] [-f INPUTFILE] [-a ANGLE] [-V WINDVELOCITY] [-s SAMPLING] [-d DIMENSIONS]")<<endl;
				exit(0);
				break;
			case 'f':											//file with data
				params.f.assign(optarg);
				if(!fs::exists(params.f)){
					cerr<<"File with cells does not exist: "<<params.f<<"."<<endl;
					cerr<<"If you don't have this file, download it from the link in the README file or use \"make download\"."<<endl;
					exit(-1);
				}
				break;
			case 'x':											//x coord of fire start
				if(atoi(optarg) < 0){
					cerr<<"Negative X"<<endl;
					exit(-1);
				}
				params.x = stoul(optarg);
				break;
			case 'y':											//y coord of fire start
				if(atoi(optarg) < 0){
					cerr<<"Negative Y"<<endl;
					exit(-1);
				}
				params.y = stoul(optarg);
				break;
			case 'a':											//angle of wind
				if(atoi(optarg) < 0){
					cerr<<"Negative a"<<endl;
					exit(-1);
				}
				params.a = stoul(optarg);
				break;
			case 's':											//sampling rate
				if(atoi(optarg) <= 0){
					cerr<<"Negative s"<<endl;
					exit(-1);
				}
				params.s = stoul(optarg);
				break;	
			case 'V':											//Velocity of wind
				if(atoi(optarg) < 0){
					cerr<<"Negative V"<<endl;
					exit(-1);
				}
				params.V = stoul(optarg);
				break;	
			case 'd':											//dimensions of map
				if(atoi(optarg) <= 0){
					cerr<<"Negative d"<<endl;
					exit(-1);
				}
				params.d = stoul(optarg);
				break;		
			default:
				cerr << ("./ims [-x STARTX] [-y STARTY] [-h] [-f INPUTFILE] [-a ANGLE] [-V WINDVELOCITY] [-s SAMPLING] [-d DIMENSIONS]")<<endl;
				exit(-1);
		}
	}
	if(params.x == UINT_MAX || params.y == UINT_MAX){
		cerr<< "Starting point not assigned. Use -x <startX> -y <startY>"<<endl;
		exit(-1);
	}
	
}

//Sets fuel to cells where there is vegetation. Then the fire is started at X/Y point.
void initializeStates(uint32_t startX, uint32_t startY){
	for(uint32_t y =0; y < lattice->getWidth(); y++){
		for(uint32_t x=0; x< lattice->getHeight(); x++){
			if(lattice->array[x][y].getFuelType() == NONE ){			//city or see
				lattice->array[x][y].setState(1);
			}else{
				lattice->array[x][y].setState(2);	
			}	
			lattice->array[x][y].setNextState(lattice->array[x][y].getState());
		}
	}
	if(lattice->array[startX][startY].getState() == 2){			//set burning if has fuel
		lattice->array[startX][startY].setState(3);
	}else{
		cerr<<"Cannot start fire in a place with no fuel: "<< startX << " "<< startY<<endl;
		exit(-1);
	}
}

//Returns true when there is an active fire anywhere on the map.
bool isBurning(){
	for(uint32_t y =0; y < lattice->getWidth(); y++){
		for(uint32_t x=0; x< lattice->getHeight(); x++){
			if(lattice->array[x][y].getState() == 3 ){			//burning
				return true;
			}
		}
	}
	return false;
}

//Calculates the pBurn spread probability from cell x/y to cell nx/ny.
double pBurn(uint32_t x, uint32_t y, uint32_t nx, uint32_t ny, uint32_t angle){
	bool corner = false;			//True if not a "side by side" neighbour cell
	if(x==nx || y==ny){
		corner = true;
	}
	if(lattice->array[nx][ny].getState() != 2){			//fire cannot spread when there is no fuel
		return 0;
	}
	
	double pDen,pVeg;
	if(lattice->array[nx][ny].getFuelType() == NONE){				//vegetation type
		pVeg = -1;	
	}else if (lattice->array[nx][ny].getFuelType() == CITY){
		pVeg = -0.3;
	}else if (lattice->array[nx][ny].getFuelType() == FIELDS){
		pVeg = -0.3;
	}else if (lattice->array[nx][ny].getFuelType() == BUSHES){
		pVeg = 0;
	}else if (lattice->array[nx][ny].getFuelType() == TREES){
		pVeg = 0.4;
	}else{
		cerr<<"Unknown density "<< 	lattice->array[nx][ny].getType()<<endl;
		exit(-1);
	}
	
	if(lattice->array[nx][ny].getFuelType() == SEA){				//density
		pDen = -1;	
	}else if (lattice->array[nx][ny].getType() == CITY){
		pDen = 0;
	}else if (lattice->array[nx][ny].getType() == SPARSE){
		pDen = -0.4;
	}else if (lattice->array[nx][ny].getType() == MEDIUM){
		pDen = 0;
	}else if (lattice->array[nx][ny].getType() == DENSE){
		pDen = 0.3;
	}else{
		cerr<<"Unknown density "<< 	lattice->array[nx][ny].getType()<<endl;		//data passed to program were incorrect
		exit(-1);
	}	
	uint32_t distance = (uint32_t)abs((int)params.a-(int)angle) % 360;			
	distance = distance>180 ? 360-distance : distance;							//dinsace in degrees between wind direction and spread direction
	double pw = exp(0.045*params.V)*exp(params.V*0.131*(cos((distance*3.1415926)/180)-1)); 
	double ps;
	double E1 = lattice->array[x][y].getAltitude();
	double E2 = lattice->array[nx][ny].getAltitude();
	if (corner){	//a=0.078	
		ps = exp(atan((E1-E2)/(5*sqrt(2))));		
	}else{
		ps = exp(atan((E1-E2)/5));
	}
	
	return 0.58*(1+pVeg)*(1+pDen)*pw*ps;
}

//Spreads fire with given probability in Moores neighbourhood
void spread(uint32_t x, uint32_t y){
	if(((double)rand()/(RAND_MAX))<pBurn(x,y,x-1,y-1,315) && x>0 && y>0 && lattice->array[x-1][y-1].getState()==2){				//<^
		lattice->array[x-1][y-1].setNextState(3);
	}
	if(((double)rand()/(RAND_MAX))<pBurn(x,y,x,y-1,0) && y>0 && lattice->array[x][y-1].getState()==2){					//^
		lattice->array[x][y-1].setNextState(3);
	}
	if(((double)rand()/(RAND_MAX))<pBurn(x,y,x+1,y-1,45) && y>0 &&lattice->array[x+1][y-1].getState()==2){				//>^
		lattice->array[x+1][y-1].setNextState(3);
	}
	if(((double)rand()/(RAND_MAX))<pBurn(x,y,x+1,y,90) && lattice->array[x+1][y].getState()==2){					//>
		lattice->array[x+1][y].setNextState(3);
	}
	if(((double)rand()/(RAND_MAX))<pBurn(x,y,x+1,y+1,135) && lattice->array[x+1][y+1].getState()==2){					//v>
		lattice->array[x+1][y+1].setNextState(3);
	}
	if(((double)rand()/(RAND_MAX))<pBurn(x,y,x,y+1,180) && lattice->array[x][y+1].getState()==2){				//v
		lattice->array[x][y+1].setNextState(3);
	}
	if(((double)rand()/(RAND_MAX))<pBurn(x,y,x-1,y+1,225) && lattice->array[x-1][y+1].getState()==2){					//<v
		lattice->array[x-1][y+1].setNextState(3);
	}
	if(((double)rand()/(RAND_MAX))<pBurn(x,y,x-1,y,270) && x>0 &&lattice->array[x-1][y].getState()==2){				//<
		lattice->array[x-1][y].setNextState(3);
	}
}

//Sets the next state of burning cells to 4 (burned area)
void calculateSteps(){
	for(uint32_t y =0; y < lattice->getWidth(); y++){
		for(uint32_t x=0; x< lattice->getHeight(); x++){
			if (lattice->array[x][y].getState() == 3){		//Rule #2 if burning then burned
				lattice->array[x][y].setNextState(4);
				spread(x,y);
			}
		}
	}
}

//Sets new states for each cell in the map. All states are updated at the same time.
void setNewStates(){
	for(uint32_t y =0; y < lattice->getWidth(); y++){
		for(uint32_t x=0; x< lattice->getHeight(); x++){
			lattice->array[x][y].setState(lattice->array[x][y].getNextState());	
		}
	}	
}

//Loops until the fire has ended completely. Creates an image at the sampling rate -s.
void burn(){
	uint32_t stepNum = 0;				
	while(isBurning()){
		calculateSteps();
		setNewStates();
		cout<<"\rStep #"<<stepNum<<flush;
		string str = to_string(stepNum);
		if(stepNum%params.s ==0)			//If sampling is required this step
			createPPM("output/output" + str +".ppm");		
		stepNum++;
	}
	createPPM("output/output"+ to_string(stepNum) +".ppm");
	cout<<"\nFire has ended."<<endl;
	
}

//Creates a .ppm bitmap file of the current state of the fire on the island.
void createPPM(string filename){
	FILE *fp = fopen(filename.c_str(), "wb");
	(void) fprintf(fp, "P6\n%d %d\n255\n", lattice->getWidth(), lattice->getHeight());	//ppm header
	for(uint32_t y =0; y < lattice->getWidth(); y++){
		for(uint32_t x=0; x< lattice->getHeight(); x++){
			static unsigned char color[3];
			if(lattice->array[x][y].getType() == SEA){		//Paint see blue
				color[0] = color[1] =0;
				color[2] = 255;
			}else if (lattice->array[x][y].getState() == 2){		//Paint cells with fuel green, shade depends on multiple variables.
				color[0] = color[2] = lattice->array[x][y].getAltitude();
				color[1] = 25*(lattice->array[x][y].getType()+lattice->array[x][y].getFuelType() );
			}else if (lattice->array[x][y].getState() == 3){	//Active fire gets painted red.
				color[1] = color[2] =0;
				color[0] = 255;
			}else{												//Burned cells get painted black.
				color[0] = color[1]=color[2] =  0;
			}
			(void) fwrite(color, 1, 3, fp);	
		}
	}	
	(void) fclose(fp);
}

int main(int argc, char* argv[]){
    srand(time(0));
	validateParameters(argc, argv);
	lattice = new Map(params.d,params.d, params.f);
	initializeStates( params.x, params.y);
	burn();
	return 0;
}



























