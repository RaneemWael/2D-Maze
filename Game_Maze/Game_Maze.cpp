//Includes
#include <stdio.h>
#include <math.h>
#include <random>
#include <glut.h>
#include <string>
#include <iostream>
#include <ctime>
#include <cmath>
using namespace std;
//-----------------

//Methods Signatures
void drawRect(int x, int y, int w, int h);
void drawCircle(int x, int y, float r);
void Key(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void Timer(int value);
void displayText(int x, int y, float r, float g, float b, char* string);
void drawPiggyBank();
void drawCoin(int x, int y);
void drawPower1(int x, int y);
void drawPower2(int x, int y);
void drawGoal();
void Anim();
void TimeNow(int value);
void Display();
//-----------------

//Global Variables
int windowX = 800;
int windowY = 800;
const int lanes = 6;
const int coins = 4;
int score = 0;
int timeLeft = 120;
int bridgesX[lanes-1];
int coinsLane[coins];
int coinsX[coins];
int selectedChick = 0; 
int pigY = 0;       
int pigX = 0;       
double coinRotation = 0;
int power1X;
int power1Lane;
int power2X;
int power2Lane;
int bordersY[lanes*2];
int pigDimY[2];
int pigDimX[2];
int currentLane = lanes;
bool inBridge = false;
bool p1Used = false;
bool p2Used = false;
bool goalTaken = false;
bool up = false;
//-----------------

void main(int argc, char** argr) {
	glutInit(&argc, argr);

	//exceptions
	if (lanes < 5) {
		throw "Number of lanes cannot be less than 5";
	}
	if (coins < 4) {
		throw "Number of coins cannot be less than 4";
	}
	//if lane height greater than character height
	if (((windowY - 140) / lanes) < 66) {
		throw "Please decrease number of lanes as lane height is smaller than character height";
	}

	//intialize pig dimensions
	pigDimX[0] = 55;
	pigDimX[1] = 105;
	pigDimY[0] = windowY-155;
	pigDimY[1] = windowY-220;

	//intialize coinsLane array with random values between 1 and max number of lanes
	srand((unsigned)time(0));
	for (int x = 1; x <= coins; x++) {
		int randX = (rand() % (lanes-1)) + 1;
		coinsLane[x - 1] = randX;
	}

	//intialize coinsX array with random values between 35 and windowX-20-15-35
	srand((unsigned)time(0));
	for (int x = 1; x <= coins; x++) {
		int randCoinX;
		bool final = false; 

		if (coinsLane[x - 1] == (lanes - 1)) {

			//to prevent overlapping of coins
			int rejects[coins+2];
			int rejectsSpace = 0;
			for (int i = 0; i < coins; i++) {
				if (x - 1 != i) {
					if (coinsLane[i] == coinsLane[x])
						rejects[rejectsSpace++] = coinsX[i];
				}
			}

			while (!final) {
				randCoinX = (rand() % (windowX - 150) + 1) + 115;
				for (int i = 0; i <= 15; i++) {
					if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
						break;
				}
				for (int i = 0; i <= 15; i++) {
					if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
						break;
				}
				final = true;
			}
		}
		else {

			//to prevent overlapping of coins
			int rejects[coins+2];
			int rejectsSpace = 0;
			for (int i = 0; i < coins; i++) {
				if (x - 1 != i) {
					if (coinsLane[i] == coinsLane[x])
						rejects[rejectsSpace++] = coinsX[i];
				}
			}
			while (!final) {
				randCoinX = (rand() % (windowX - 70) + 1) + 35;
				for (int i = 0; i <= 15; i++) {
					if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
						break;
				}
				for (int i = 0; i <= 15; i++) {
					if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
						break;
				}
				final = true;
			}
		}

		coinsX[x - 1] = randCoinX;
	}
	//-----------------------------------------------------------------------------------------

	//intiallize bordersY
	int lane = 0;
	for (int i = 0; i < lanes * 2; i+=2) {
		if (lane == 0) {
			bordersY[i] = 10;
			bordersY[i + 1] = ((windowY - 140) / lanes) - 5;
		}
		else if (lane == lanes-1) {
			bordersY[i] = ((windowY - 140) / lanes) * lane + 5;
			bordersY[i + 1] = ((windowY - 140) / lanes) * lane + ((windowY - 140) / lanes);
		}
		else {
			bordersY[i] = ((windowY - 140) / lanes) * lane + 5;
			bordersY[i+1] = ((windowY - 140) / lanes) * lane - 5 + ((windowY - 140) / lanes);
		}
		lane++;
	}

	glutInitWindowSize(windowX, windowY);
	glutInitWindowPosition(300, 50);

	glutCreateWindow("Piggy Bank");
	glutDisplayFunc(Display);
	glutIdleFunc(Anim);
	glutKeyboardFunc(Key);
	glutKeyboardUpFunc(KeyUp);
	glutTimerFunc(0, TimeNow, 0);
	glutTimerFunc(0, Timer, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(1, 1, 1, 0);
	glPointSize(2.0);
	gluOrtho2D(0, windowX, 0, windowY);

	glutMainLoop();
}

//draws rectangles using the (x,y) of the bottom left vertex, width (w) and height (h)
void drawRect(int x, int y, int w, int h) {
	glBegin(GL_POLYGON);
		
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

//draws a circle using OpenGL's gluDisk, given (x,y) of its center and its radius
void drawCircle(int x, int y, float r) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	GLUquadric* quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();
}

//Keyboard handler function
//key: the key pressed on the keyboard
void Key(unsigned char key, int x, int y) {

	//if the key 'w' is pressed, move up until it reaches limit
	if (key == 'w') {

		up = true;

		//collisions
		//in bridge to change currentLane
		if (pigDimY[0] > bordersY[currentLane * 2 - 1]) {
			pigY += 3;
			pigDimY[0] += 3;
			pigDimY[1] += 3;
			if (pigDimY[1] >= bordersY[(currentLane + 1) * 2 - 2]) {
				currentLane++;
				inBridge = false;
			}
		}
		//not in bridge
		else if (pigDimY[0] + 1 == bordersY[currentLane * 2 - 1]) {
			if (currentLane != lanes) {
				if (pigDimX[0] >= bridgesX[currentLane - 1] && pigDimX[1] <= bridgesX[currentLane - 1] + 70) {
					pigY += 3;
					pigDimY[0] += 3;
					pigDimY[1] += 3;
					inBridge = true;
				}
			}
			else
				return;
		}
		else if (pigDimY[0] + 2 == bordersY[currentLane * 2 - 1]) {
			if (currentLane != lanes) {
				if (pigDimX[0] >= bridgesX[currentLane - 1] && pigDimX[1] <= bridgesX[currentLane - 1] + 70) {
					pigY += 3;
					pigDimY[0] += 3;
					pigDimY[1] += 3;
					inBridge = true;
				}
			}
			else {
				pigY += 1;
				pigDimY[0] += 1;
				pigDimY[1] += 1;
			}
		}
		else if (pigDimY[0] + 3 == bordersY[currentLane * 2 - 1]) {
			if (currentLane != lanes) {
				if (pigDimX[0] >= bridgesX[currentLane - 1] && pigDimX[1] <= bridgesX[currentLane - 1] + 70) {
					pigY += 3;
					pigDimY[0] += 3;
					pigDimY[1] += 3;
					inBridge = true;
				}
			}
			else {
				pigY += 2;
				pigDimY[0] += 2;
				pigDimY[1] += 2;
			}
		}
		else {
			pigY += 3;
			pigDimY[0] += 3;
			pigDimY[1] += 3;
		}

		//to catch coins
		if (pigDimY[0] <= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15 || pigDimY[1]-1 <= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15 || pigDimY[1]-3 <= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15) {
			for (int j = 0; j < coins; j++) {
				if (coinsLane[j] == currentLane-1) {
					if (pigDimX[1] >= (coinsX[j] - 15) && pigDimX[1] <= (coinsX[j] + 15)) {
						int posY = ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15;
						if (posY == pigDimY[0] || posY == pigDimY[0] - 1 || posY == pigDimY[0] - 2) {
							coinsX[j] = 0;
							score += 10;
							break;
						}
					}
				}
			}
		}

		//to catch p1
		if (!p1Used) {
			if (pigDimY[0] <= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15 || pigDimY[1] - 1 <= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15 || pigDimY[1] - 3 <= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15) {
				if (power1Lane == currentLane - 1) {
					if (pigDimX[1] >= (power1X - 15) && pigDimX[1] <= (power1X + 15)) {
						int posY = ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15;
						if (posY == pigDimY[0] || posY == pigDimY[0] - 1 || posY == pigDimY[0] - 2) {
							p1Used = true;
							if (power1X - 35 < 20)
								bridgesX[currentLane - 2] = power1X;
							else if (power1X + 35 < 20)
								bridgesX[currentLane - 2] = power1X - 70;
							else
								bridgesX[currentLane - 2] = power1X - 35;
						}
					}
				}
			}
		}
		//to catch p2
		if (!p2Used) {
			if (pigDimY[0] <= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15 || pigDimY[1] - 1 <= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15 || pigDimY[1] - 3 <= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15) {
				if (power2Lane == currentLane - 1) {
					if (pigDimX[1] >= (power2X - 15) && pigDimX[1] <= (power2X + 15)) {
						int posY = ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) - 15;
						if (posY == pigDimY[0] || posY == pigDimY[0] - 1 || posY == pigDimY[0] - 2) {
							p2Used = true;
							timeLeft += 20;
						}
					}
				}
			}
		}
	}
	//if the key 's' is pressed, move down until it reaches limit
	if (key == 's') {

		up = false;

		//collisions
		//in bridge to change currentLane
		if (pigDimY[1] < bordersY[currentLane * 2 - 2]) {
			pigY -= 3;
			pigDimY[0] -= 3;
			pigDimY[1] -= 3;
			if (pigDimY[0] <= bordersY[(currentLane - 1) * 2 - 1]) {
				currentLane--;
				inBridge = false;
			}
		}
		//not in bridge
		else if (pigDimY[1] - 1 == bordersY[currentLane * 2 - 2]) {
			if (currentLane != 1) {
				if (pigDimX[0] >= bridgesX[currentLane - 2] && pigDimX[1] <= bridgesX[currentLane - 2] + 70) {
					pigY -= 3;
					pigDimY[0] -= 3;
					pigDimY[1] -= 3;
					inBridge = true;
				}
			}
			else
				return;
		}
		else if (pigDimY[1] - 2 == bordersY[currentLane * 2 - 2]) {
			if (currentLane != 1) {
				if (pigDimX[0] >= bridgesX[currentLane - 2] && pigDimX[1] <= bridgesX[currentLane - 2] + 70) {
					pigY -= 3;
					pigDimY[0] -= 3;
					pigDimY[1] -= 3;
					inBridge = true;
				}
			}
			else {
				pigY -= 1;
				pigDimY[0] -= 1;
				pigDimY[1] -= 1;
			}
		}
		else if (pigDimY[1] - 3 == bordersY[currentLane * 2 - 2]) {
			if (currentLane != 1) {
				if (pigDimX[0] >= bridgesX[currentLane - 2] && pigDimX[1] <= bridgesX[currentLane - 2] + 70) {
					pigY -= 3;
					pigDimY[0] -= 3;
					pigDimY[1] -= 3;
					inBridge = true;
				}
			}
			else {
				pigY -= 2;
				pigDimY[0] -= 2;
				pigDimY[1] -= 2;
			}
		}
		else {
			pigY -= 3;
			pigDimY[0] -= 3;
			pigDimY[1] -= 3;
		}

		//to catch coins
		if (pigDimY[1] >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15 || pigDimY[1] + 1 >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15 || pigDimY[1] + 2 >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15) {
			for (int j = 0; j < coins; j++) {
				if (coinsLane[j] == currentLane-1) {
					if (pigDimX[1] >= (coinsX[j] - 15) && pigDimX[1] <= (coinsX[j] + 15)) {
						int posY = ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15;
						if (posY == pigDimY[1] || posY == pigDimY[1] + 1 || posY == pigDimY[1] + 2) {
							coinsX[j] = 0;
							score += 10;
							break;
						}
					}
				}
			}
		}

		//to catch p1
		if (!p1Used) {
			if (pigDimY[1] >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15 || pigDimY[1] + 1 >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15 || pigDimY[1] + 2 >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15) {
				if (power1Lane == currentLane - 1) {
					if (pigDimX[1] >= (power1X - 15) && pigDimX[1] <= (power1X + 15)) {
						int posY = ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15;
						if (posY == pigDimY[1] || posY == pigDimY[1] + 1 || posY == pigDimY[1] + 2) {
							p1Used = true;
							if (power1X - 35 < 20)
								bridgesX[currentLane - 2] = power1X;
							else if (power1X + 35 < 20)
								bridgesX[currentLane - 2] = power1X - 70;
							else
								bridgesX[currentLane - 2] = power1X - 35;
						}
					}
				}
			}
		}
		//to catch p2
		if (!p2Used) {
			if (pigDimY[1] >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15 || pigDimY[1] + 1 >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15 || pigDimY[1] + 2 >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15) {
				if (power2Lane == currentLane - 1) {
					if (pigDimX[1] >= (power2X - 15) && pigDimX[1] <= (power2X + 15)) {
						int posY = ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15;
						if (posY == pigDimY[1] || posY == pigDimY[1] + 1 || posY == pigDimY[1] + 2) {
							p2Used = true;
							timeLeft += 20;
						}
					}
				}
			}
		}
		//to catch goal
		if (currentLane == 1) {
			if (pigDimY[1] >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15 || pigDimY[1] + 1 >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15 || pigDimY[1] + 2 >= ((windowY - 140) / lanes) * currentLane + 15 + ((windowY - 140) / lanes / 2) + 15) {
				if (pigDimX[1] >= (windowX-70) && pigDimX[1] <= (windowX-30)) {
					if (80 == pigDimY[1] || 80 == pigDimY[1] + 1 || 80 == pigDimY[1] + 2)
						goalTaken = true;
				}
			}
		}
	}
	//if the key 'a' is pressed, move left until it reaches limit
	if (key == 'a') {

		//collisions
		//in bridge
		if (inBridge) {
			if (pigDimX[0] - 1 == bridgesX[currentLane-2]) {
				return;
			}
			else if (pigDimX[0] - 2 == bridgesX[currentLane - 2]) {
				pigX -= 1;
				pigDimX[0] -= 1;
				pigDimX[1] -= 1;
			}
			else if (pigDimX[0] - 3 == bridgesX[currentLane - 2]) {
				pigX -= 2;
				pigDimX[0] -= 2;
				pigDimX[1] -= 2;
			}
			else {
				pigX -= 3;
				pigDimX[0] -= 3;
				pigDimX[1] -= 3;
			}
		}
		//not in bridge
		else {
			if (pigDimX[0] - 1 == 20) {
				return;
			}
			else if (pigDimX[0] - 2 == 20) {
				pigX -= 1;
				pigDimX[0] -= 1;
				pigDimX[1] -= 1;
			}
			else if (pigDimX[0] - 3 == 20) {
				pigX -= 2;
				pigDimX[0] -= 2;
				pigDimX[1] -= 2;
			}
			else {
				pigX -= 3;
				pigDimX[0] -= 3;
				pigDimX[1] -= 3;
			}
		}

		//to catch coins
		for (int j = 0; j < coins; j++) {
			if (coinsLane[j] == currentLane-1) {
				for (int i = 0; i < 3; i++) {
					if ((coinsX[j] + 15) == (pigDimX[0] + i) || (coinsX[j] - 15) == (pigDimX[0] + i)) {
						coinsX[j] = 0;
						score += 10;
						break;
					}
				}
			}
		}

		//to catch P1
		if (!p1Used) {
			if (power1Lane == currentLane - 1) {
				for (int i = 0; i < 3; i++) {
					if ((power1X + 15) == (pigDimX[0] + i) || (power1X - 15) == (pigDimX[0] + i)) {
						p1Used = true;
						if (power1X - 35 < 20)
							bridgesX[currentLane - 2] = power1X;
						else if (power1X + 35 < 20)
							bridgesX[currentLane - 2] = power1X - 70;
						else
							bridgesX[currentLane - 2] = power1X - 35;
					}
				}
			}
		}
		//to catch P2
		if (!p2Used) {
			if (power2Lane == currentLane - 1) {
				for (int i = 0; i < 3; i++) {
					if ((power2X + 15) == (pigDimX[0] + i) || (power2X - 15) == (pigDimX[0] + i)) {
						p2Used = true;
						timeLeft += 20;
					}
				}
			}
		}
	}
	//if the key 'd' is pressed, move right until it reaches limit
	if (key == 'd') {

		//collisions
		//in bridge
		if (inBridge) {
			if (pigDimX[1] + 1 == bridgesX[currentLane - 2] + 70) {
				return;
			}
			else if (pigDimX[1] + 2 == bridgesX[currentLane - 2] + 70) {
				pigX += 1;
				pigDimX[0] += 1;
				pigDimX[1] += 1;
			}
			else if (pigDimX[1] + 3 == bridgesX[currentLane - 2] + 70) {
				pigX += 2;
				pigDimX[0] += 2;
				pigDimX[1] += 2;
			}
			else {
				pigX += 3;
				pigDimX[0] += 3;
				pigDimX[1] += 3;
			}
		}
		//not in bridge
		else {
			if (pigDimX[1] + 1 == windowX - 20) {
				return;
			}
			else if (pigDimX[1] + 2 == windowX - 20) {
				pigX += 1;
				pigDimX[0] += 1;
				pigDimX[1] += 1;
			}
			else if (pigDimX[1] + 3 == windowX - 20) {
				pigX += 2;
				pigDimX[0] += 2;
				pigDimX[1] += 2;
			}
			else {
				pigX += 3;
				pigDimX[0] += 3;
				pigDimX[1] += 3;
			}
		}

		//to catch coins
		for (int j = 0; j < coins; j++) {
			if (coinsLane[j] == currentLane - 1) {
				for (int i = 0; i < 3; i++) {
					if ((coinsX[j] + 15) == (pigDimX[1] - i) || (coinsX[j] - 15) == (pigDimX[1] - i)) {
						coinsX[j] = 0;
						score += 10;
						break;
					}
				}
			}
		}

		//to catch P1
		if (!p1Used) {
			if (power1Lane == currentLane - 1) {
				for (int i = 0; i < 3; i++) {
					if ((power1X + 15) == (pigDimX[1] - i) || (power1X - 15) == (pigDimX[1] - i)) {
						p1Used = true;
						if (power1X - 35 < 20)
							bridgesX[currentLane - 2] = power1X;
						else if (power1X + 35 < 20)
							bridgesX[currentLane - 2] = power1X - 70;
						else
							bridgesX[currentLane - 2] = power1X - 35;
					}
				}
			}
		}
		//to catch P2
		if (!p2Used) {
			if (power2Lane == currentLane - 1) {
				for (int i = 0; i < 3; i++) {
					if ((power2X + 15) == (pigDimX[1] - i) || (power2X - 15) == (pigDimX[1] - i)) {
						p2Used = true;
						timeLeft += 20;
					}
				}
			}
		}
		//to catch goal
		if (currentLane == 1) {
			for (int i = 0; i < 3; i++) {
				if ((windowX-70) == (pigDimX[1] - i) || (windowX-70) == (pigDimX[1] - i))
					goalTaken = true;
			}
		}
	}

	//ask OpenGL to recall the display function to reflect the changes on the window
	glutPostRedisplay();
}

//KeyboardUp handler function
//called only when the key is released
void KeyUp(unsigned char key, int x, int y) {
	//reset the selectedChicken value to 0 to unselect the selected bar
	selectedChick = 0;

	//ask OpenGL to recall the display function to reflect the changes on the window
	glutPostRedisplay();
}

void Timer(int value) {
	//set bridges' start point to a random number between 20 and windowX-20-70-20
	srand((unsigned)time(0));
	for (int x = 1; x < lanes; x++) {
		int randX = (rand() % (windowX - 110)+1) + 20;
		bridgesX[x - 1] = randX;
	}

	//set power1Lane and power2Lane with random values between 1 and max number of lanes
	srand((unsigned)time(0));
	power1Lane = (rand() % (lanes - 1)) + 1;
	power2Lane = (rand() % (lanes - 1)) + 1;

	//set power1X and power2X with random values between 35 and windowX-20-15-35
	srand((unsigned)time(0));
	int randPower1X;
	int randPower2X;
	int rejects1[coins + 1];
	int rejects2[coins + 1];
	int rejectsSpace1 = 0;
	int rejectsSpace2 = 0;
	bool final1 = false;
	bool final2 = false;

	for (int i = 0; i < coins; i++) {
		if (power1Lane == coinsLane[i])
			rejects1[rejectsSpace1++] = coinsX[i];
	}

	while (!final1) {
		if (power1Lane == lanes-1)
			randPower1X = (rand() % (windowX - 150) + 1) + 115;
		else
			randPower1X = (rand() % (windowX - 70) + 1) + 35;
		for (int i = 0; i <= 15; i++) {
			if ((find(begin(rejects1), end(rejects1), randPower1X - i)) != end(rejects1))
				i = 16;
		}
		for (int i = 0; i <= 15; i++) {
			if ((find(begin(rejects1), end(rejects1), randPower1X + i)) != end(rejects1))
				i = 16;
		}
		final1 = true;
	}
	power1X = randPower1X;
	rejects1[rejectsSpace1++] = randPower1X;

	if (power2Lane == power1Lane)
		for (int i = 0; i < coins + 1; i++)
			rejects2[i] = rejects1[i];
	else {
		for (int i = 0; i < coins; i++) {
			if (power2Lane == coinsLane[i])
				rejects2[rejectsSpace2++] = coinsX[i];
		}
	}
	while (!final2) {
		if (power2Lane == lanes-1)
			randPower2X = (rand() % (windowX - 150) + 1) + 115;
		else
			randPower2X = (rand() % (windowX - 70) + 1) + 35;
		for (int i = 0; i <= 15; i++) {
			if ((find(begin(rejects2), end(rejects2), randPower2X - i)) != end(rejects2))
				i = 16;
		}
		for (int i = 0; i <= 15; i++) {
			if ((find(begin(rejects2), end(rejects2), randPower2X + i)) != end(rejects2))
				i = 16;
		}
		final2 = true;
	}
	power2X = randPower2X;
	//----------------------------------------------------------------------
	
	if (inBridge) {
		if (up) {
			pigY += (bordersY[(currentLane+1) * 2 - 2] - pigDimY[1]);
			pigDimY[1] = bordersY[(currentLane+1) * 2 - 2];
			pigDimY[0] = pigDimY[1] + 65;
			currentLane++;
		}
		else {
			pigY -= (pigDimY[0] - bordersY[(currentLane - 1) * 2 - 1]);
			pigDimY[0] = bordersY[(currentLane-1) * 2 - 1];
			pigDimY[1] = pigDimY[0] - 65;
			currentLane--;
		}
		inBridge = false;
	}

	//ask OpenGL to recall the display function to reflect the changes on the window
	glutPostRedisplay();

	//recall the Timer function after 20 seconds (20,000 milliseconds)
	glutTimerFunc(20 * 1000, Timer, 0);
}

void displayText(int x, int y, float r, float g, float b, char* string)
{
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

void drawPiggyBank() //(5 primitive types; circle + polygon + lines + points + line_strip)
{
	//length is 65 (away from top 15)
	//width is 50 (away from side 35)

	glColor3f(1.0f, 0.7f, 0.8f);
	//head
	drawCircle(90, windowY - 150, 15);
	//body
	drawCircle(75, windowY - 175, 20);
	glColor3f(0.0f, 0.0f, 0.0f);
	//slot
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex3f(60.0f, windowY - 175, 0.0f);
	glVertex3f(65.0f, windowY - 164, 0.0f);
	glEnd();
	//legs
	drawRect(65, windowY - 200, 5, 10);
	drawRect(80, windowY - 200, 5, 10);
	//eyes
	glBegin(GL_POINTS);
	glVertex3f(85.0f, windowY - 145, 0.0f);
	glVertex3f(95.0f, windowY - 145, 0.0f);
	glEnd();
	//mouth
	glLineWidth(2.0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(85.0f, windowY - 153, 0.0f);
	glVertex3f(90.0f, windowY - 157, 0.0f);
	glVertex3f(95.0f, windowY - 153, 0.0f);
	glEnd();
}

void drawCoin(int x, int y) //(2 primitive types; circle+lines)
{
	glColor3f(0.9, 0.9, 0.2);
	drawCircle(x, y, 15);

	//to show rotation
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glLineWidth(6.0);
	glVertex3f(x - 8, y, 0.0f);
	glVertex3f(x + 5, y, 0.0f);
	glVertex3f(x + 5, y + 10, 0.0f);
	glVertex3f(x + 5, y - 10, 0.0f);
	glEnd();
}

void drawPower1(int x, int y) //(3 primitive types; circle+polygon+lines) OPENS BRIDGE ABOVE IT
{
	glColor3f(0.6, 0.2, 0.7);
	drawCircle(x, y, 15);

	glColor3f(0.5, 0.3, 1.0);
	drawRect(x - 10, y - 8, 20, 15);

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	glLineWidth(8.0);
	glVertex3f(x - 5, y, 0.0f);
	glVertex3f(x + 5, y, 0.0f);
	glEnd();
}

void drawPower2(int x, int y) //(3 primitive types; circle+polygon+triangles) GIVES YOU 20 MORE SECONDS
{
	glColor3f(0.6, 0.2, 0.7);
	drawCircle(x, y, 15);

	glColor3f(0.5, 0.3, 1.0);
	drawRect(x - 10, y - 8, 20, 15);

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex3f(x, y+8, 0.0f);
	glVertex3f(x+10, y-8, 0.0f);
	glVertex3f(x-10, y-8, 0.0f);
	glEnd();
}

void drawGoal() //(2 primitive types; rectangle+polygon)
{
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	glVertex3f(windowX - 70, 60.0f, 0.0f);
	glVertex3f(windowX - 30, 60.0f, 0.0f);
	glVertex3f(windowX - 50, 80.0f, 0.0f);
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	drawRect(windowX - 70, 30, 40, 30);
}

void Anim() {
	coinRotation++;
	for (int i = 0; i < 6000000; i++);
	glutPostRedisplay();
}

void TimeNow(int value) {
	timeLeft--;
	glutPostRedisplay();

	//recall the Timer function after 1 seconds (1,000 milliseconds)
	glutTimerFunc(1 * 1000, TimeNow, 0);
}

void Display() {
	glClear(GL_COLOR_BUFFER_BIT);

	if (goalTaken) {
		//display you won + score
		char* p0s1[20];
		sprintf((char*)p0s1, "YOU WON!!");
		displayText(windowX / 3, windowY - (windowY / 3), 0.0, 0.0, 0.0, (char*)p0s1);

		char* p0s[20];
		sprintf((char*)p0s, "Score: %d", score);
		displayText(windowX / 2, windowY - (windowY / 2), 0.0, 0.0, 0.0, (char*)p0s);
	}
	else if (timeLeft == 0 && !goalTaken) {
		//display you lost + score
		char* p0s1[20];
		sprintf((char*)p0s1, "Times up, you lost :(");
		displayText(windowX / 3, windowY - (windowY / 3), 0.0, 0.0, 0.0, (char*)p0s1);

		char* p0s[20];
		sprintf((char*)p0s, "Score: %d", score);
		displayText(windowX / 2, windowY - (windowY / 2), 0.0, 0.0, 0.0, (char*)p0s);
	}
	else {
		//display score
		char* p0s[20];
		sprintf((char*)p0s, "Score: %d", score);
		displayText(windowX / 3, windowY - 60, 0.0, 0.0, 0.0, (char*)p0s);
		//display time
		char* p0s1[20];
		sprintf((char*)p0s, "Time left (seconds): %d", timeLeft);
		displayText(windowX / 2, windowY - 60, 0.0, 0.0, 0.0, (char*)p0s1);

		//draw border (2 primitive types; quads+line_loop)
		glBegin(GL_QUADS);
		glColor3f(0.0f, 0.3f, 0.4f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, windowY - 100, 0.0f);
		glVertex3f(20.0f, windowY - 120, 0.0f);
		glVertex3f(20.0f, 20.0f, 0.0f);
		glVertex3f(0.0f, windowY - 100, 0.0f);
		glVertex3f(windowX, windowY - 100, 0.0f);
		glVertex3f(windowX - 20, windowY - 120, 0.0f);
		glVertex3f(20.0f, windowY - 120, 0.0f);
		glVertex3f(windowX, windowY - 100, 0.0f);
		glVertex3f(windowX, 0.0f, 0.0f);
		glVertex3f(windowX - 20, 20.0f, 0.0f);
		glVertex3f(windowX - 20, windowY - 120, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(windowX, 0.0f, 0.0f);
		glVertex3f(windowX - 20, 20.0f, 0.0f);
		glVertex3f(20.0f, 20.0f, 0.0f);
		glEnd();

		glLineWidth(8.0);
		glBegin(GL_LINE_LOOP);
		glColor3f(0.0f, 0.4f, 0.3f);
		glVertex3f(0.0f, windowY - 100, 0.0f);
		glVertex3f(windowX, windowY - 100, 0.0f);
		glVertex3f(windowX, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();
		//-------------------------------------

		//draw laneBorders (2 primitive types; polygon+line)
		for (int x = 1; x < lanes; x++) {
			int pos = ((windowY - 140) / lanes) * x + 15;
			glColor3f(0.8f, 0.7f, 0.0f);
			drawRect(20, pos, windowX - 40, 10);

			glLineWidth(1.0);
			glColor3f(0.0f, 0.0f, 0.0f);
			glBegin(GL_LINES);
			glVertex3f(20.0f, pos + 5, 0.0f);
			glVertex3f(windowX - 20, pos + 5, 0.0f);
			glEnd();
		}
		//-------------------------------------

		//draw bridges
		srand((unsigned)time(0));
		for (int x = 1; x < lanes; x++) {
			int pos = ((windowY - 140) / lanes) * x + 15;
			glColor3f(1.0f, 1.0f, 1.0f);
			drawRect(bridgesX[x - 1], pos, 70, 10);
		}
		//-------------------------------------

		//draw piggy bank (5 primitive types; circle+polygon+lines+points+line_strip)
		glPushMatrix();
		glTranslatef(pigX, pigY, 0);
		drawPiggyBank();
		glPopMatrix();
		//----------------------------------------

		//draw coins (3 primitive types; circle+lines+points)
		for (int x = 1; x <= coins; x++) {
			if (coinsX[x - 1] != 0) {
				glPushMatrix();
				int posY = ((windowY - 140) / lanes) * coinsLane[x - 1] + 15 + ((windowY - 140) / lanes / 2);
				glTranslatef(coinsX[x - 1], posY, 0);
				glRotatef(coinRotation, 0, 1, 0);
				glTranslatef(-coinsX[x - 1], -posY, 0);
				drawCoin(coinsX[x - 1], posY);
				glPopMatrix();
			}
		}
		//---------------------------------------

		//draw powerups
		if (!p1Used)
			drawPower1(power1X, ((windowY - 140) / lanes) * power1Lane + 15 + ((windowY - 140) / lanes / 2));
		if (!p2Used)
		drawPower2(power2X, ((windowY - 140) / lanes) * power2Lane + 15 + ((windowY - 140) / lanes / 2));

		//draw goal
		drawGoal();
	}
	
	glFlush();
}