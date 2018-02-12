/*
Project: 	IMS Cellular Automaton simulation
Authors:	Jakub Svoboda, Michael Schmid
Login:		xsvobo0z, xschmi08
Date:		2. December 2017
Course:		IMS 2017
*/

using namespace std;
namespace fs = std::experimental::filesystem;

typedef struct Parameters{
	string f;
	bool h;
	uint32_t x;
	uint32_t y;
	uint32_t a;
	uint32_t V;
	uint32_t s;
	uint32_t d;
} Parameters;

#define SEA 1
#define CITY 2
#define SPARSE 3
#define MEDIUM 4
#define DENSE 5
#define BURNED 6

#define NONE 1
#define FIELDS 3
#define BUSHES 4
#define TREES 5


void validateParameters(int argc, char* argv[]);
void createPPM(string filename);
void burn();
void setNewStates();
void calculateSteps();
void spread(uint32_t x, uint32_t y);
double pBurn(uint32_t x, uint32_t y, uint32_t nx, uint32_t ny, uint32_t angle);
bool isBurning();
void initializeStates(uint32_t startX, uint32_t startY);
