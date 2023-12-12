#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <cmath> 
#include <string>
#include <vector>
#include <playsoundapi.h>
#include <thread>
#include <chrono>



#define M_PI 3.14159265358979323846

int count;

int WIDTH = 1280;
int HEIGHT = 720;


GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1000;

int gameLevel = 1;

int maleZombiehits = 0;
int femaleZombiehits = 0;
int greenAlienHits = 0;
int grayAlienHits = 0;
bool showDamageIndicator = false;
bool shakeCamera = false;
int shakeTimer = 0;




class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(10, 10, 40);
Vector At(10, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_brain;
Model_3DS model_zombieMale;
Model_3DS model_zombieMale2;
Model_3DS model_zombieFemale;
Model_3DS model_hazmat;
Model_3DS model_alienGray;
Model_3DS model_alienGreen;
Model_3DS model_star;
Model_3DS model_spaceship;
//Model_3DS model_hazmat;

// Textures
GLTexture tex_ground;
GLTexture tex_zombieMale;

//model positions
float playerPos[3] = { 7, 0, 10 }; int playerRot = 0.0;
float zombieMalePos[3] = { 15, 4, 15 };
float zombieFemalePos[3] = { 15, 3, 10 };
float alienGrayPos[3] = { 5, 3, 15 };
float alienGreenPos[3] = { 5, 3, 20 };
float spaceshipPos[3] = { 17, 1, 25 };
float housePos[3] = { 40,0,10 };
float shootingPosition[2] = {WIDTH / 2, HEIGHT/2};

float starsPos[3][3] = { { 7, 1, 20 },
						 { 6, 1, 15 },
						 { 5,1,14 } };

float brainsPos[3][3] = { { 13,2,10 },
						 { 13,2,13 },
						 { 13,2,15 } };

//controls
int score = 0, cameraMode = 2 ;
float playerHealth = 100.0, maxHealth = 100.0; // Full health
bool stars[3] = { false,false,false };
bool brains[3] = { false,false,false };
bool gameOver, gameResult, starsFound, brainsFound;
bool gameOverSoundPlayed = false;
bool isJumping = false;
float jumpVelocity = 0.0f;
float gravity = 0.98f; // adjust as needed


GLfloat lightIntensity[] = { 0.2, 0.2, 0.2, 1.0f };
GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };

GLfloat playerLightIntensity[] = { 0.7, 0.7, 0.7, 1.0f }; // Adjust as needed
GLfloat playerLightPosition[] = { playerPos[0], playerPos[1] + 2.0, playerPos[2], 1.0f }; // Positioned above the player
GLfloat spotDirection[] = { 0.0, -1.0, 0.0 }; // Spotlight direction (downwards)



//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

bool checkIntersect(float playerPostions[], float balloon[]) {
	float x1 = playerPostions[0] - 1, x2 = playerPostions[0] + 1;
	float z1 = playerPostions[2] - 1, z2 = playerPostions[2] + 1;
	return (balloon[0] >= x1 && balloon[0] <= x2 && balloon[2] >= z1 && balloon[2] <= z2);
}

void InitPlayerLight() {

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, playerLightIntensity);
	glLightfv(GL_LIGHT1, GL_POSITION, playerLightPosition);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0); // Set the cutoff angle
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5.0); // Set the focus of the spotlight
}



//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	// Initialize additional light for damage indicator
	GLfloat defaultLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat defaultLightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, defaultLight);
	glLightfv(GL_LIGHT2, GL_POSITION, defaultLightPosition);
	glDisable(GL_LIGHT2);

	InitMaterial();

	InitPlayerLight();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing
	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-200, 0, -200);
	glTexCoord2f(5, 0);
	glVertex3f(200, 0, -200);
	glTexCoord2f(5, 5);
	glVertex3f(200, 0, 200);
	glTexCoord2f(0, 5);
	glVertex3f(-200, 0, 200);
	glEnd();
	glPopMatrix();

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

//void drawHazmat() {

//	glPushMatrix();
//	glTranslatef(17, 3, 17);
//	glScalef(2.4, 2.4, 2.4);
//	glRotatef(90.f, 1, 0, 0);
//	model_hazmat.Draw();

//	glPopMatrix();

//}

void drawPlayer() {

	glPushMatrix();

	glTranslatef(playerPos[0], playerPos[1], playerPos[2]);
	glScalef(2.4, 2.4, 2.4);
	glRotatef(playerRot, 0, 1, 0);
	model_zombieMale2.Draw();

	glPopMatrix();

}

void drawZombieMale() {

	tex_zombieMale.Use();

	glPushMatrix();
	glTranslatef(zombieMalePos[0], zombieMalePos[1], zombieMalePos[2]);
	glScalef(0.15, 0.15, 0.15);
	glRotatef(90.f, 1, 0, 0);
	model_zombieMale.Draw();

	glPopMatrix();


}

void drawZombieFemale() {


	glPushMatrix();
	glTranslatef(zombieFemalePos[0], zombieFemalePos[1], zombieFemalePos[2]);
	glScalef(2.4, 2.4, 2.4);
	glRotatef(90.f, 1, 0, 0);
	model_zombieFemale.Draw();

	glPopMatrix();


}

void drawAlienGray() {
	//zombie-female model


	glPushMatrix();
	glTranslatef(alienGrayPos[0], alienGrayPos[1], alienGrayPos[2]);
	glScalef(2.4, 2.4, 2.4);
	glRotatef(90.f, 1, 0, 0);
	glColor3f(0.18, 0.18, 0.18);
	model_alienGray.Draw();

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

}

void drawAlienGreen() {
	//zombie-female model

	glPushMatrix();
	glTranslatef(alienGreenPos[0], alienGreenPos[1], alienGreenPos[2]);
	glScalef(2.4, 2.4, 2.4);
	glRotatef(90.f, 1, 0, 0);
	glColor3f(0.18, 0.22, 0.16);
	model_alienGreen.Draw();

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

}

void drawStar() {
	//zombie-female model
	
	glColor3f(1.0f, 1.0f, 0.0f);

	for (int i = 0; i < 3; i++) {
		if (!stars[i]) {
			glPushMatrix();
			glTranslatef(starsPos[i][0], starsPos[i][1], starsPos[i][2]);
			glScalef(0.4, 0.4, 1);
			glRotatef(90.f, 1, 0, 1);
			model_star.Draw();

			glPopMatrix();
		}
	}

	glColor3f(1, 1, 1);
}

void drawSpaceship() {
	//zombie-female model

	glPushMatrix();
	glTranslatef(spaceshipPos[0], spaceshipPos[1], spaceshipPos[2]);
	glScalef(4,4,4);
	glRotatef(90.f, 1, 0, 0);
	glColor3f(0.18, 0.18, 0.18);
	model_spaceship.Draw();

	glPopMatrix();
	glColor3f(1,1,1);
}

void drawBrain() {
	//brain model
	glColor3f(0.9686f, 0.5686f, 0.5255f);

	for (int i = 0; i < 3; i++) {
		if (!brains[i]) {
			glPushMatrix();
			glTranslatef(brainsPos[i][0], brainsPos[i][1], brainsPos[i][2]);
			glScalef(0.3, 0.3, 0.3);
			model_brain.Draw();
			glPopMatrix();
		}
	}

	glColor3f(1, 1, 1);
}

void drawHouse() {
	// Draw house Model
	glPushMatrix();
	glTranslatef(housePos[0], housePos[1], housePos[2]);
	glScalef(3,3,3);
	glRotatef(90.f, 1, 0, 0);
	model_house.Draw();
	glPopMatrix();

}


void DrawHealthBar() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Draw the health bar background
	glColor3f(0.0f, 1.0f, 0.0f); // Green background
	glBegin(GL_QUADS);
	glVertex2f(10, HEIGHT - 30);
	glVertex2f(10 + 200 * (playerHealth / 100), HEIGHT - 30); // Width based on health
	glVertex2f(10 + 200 * (playerHealth / 100), HEIGHT - 10);
	glVertex2f(10, HEIGHT - 10);
	glEnd();

	// Draw the health bar
	glColor3f(0.3f, 0.3f, 0.3f); // Grey health
	glBegin(GL_QUADS);
	glVertex2f(10, HEIGHT - 30);
	glVertex2f(210, HEIGHT - 30);
	glVertex2f(210, HEIGHT - 10);
	glVertex2f(10, HEIGHT - 10);
	glEnd();

	// Set the text color (white)
	glColor3f(0.0, 0.0, 0.0);

	// Set the position to display ""
	float xPos = 300;  // Adjust the X position
	float yPos = HEIGHT - 30;   // Adjust the Y position

	glRasterPos2f(xPos, yPos);
	const char* text = ("Score: ");
	std::string fullText = text + std::to_string(score);
	const char* displayText = fullText.c_str();
	int length = strlen(displayText);
	for (int i = 0; i < length; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, displayText[i]);
	}

	// Set the position to display ""
	float xPos1 = 500;  // Adjust the X position
	float yPos1 = HEIGHT - 30;   // Adjust the Y position

	glRasterPos2f(xPos1, yPos1);
	const char* text1 = "Level: ";
	std::string fullText1 = text1 + std::to_string(gameLevel);
	const char* displayText1 = fullText1.c_str();
	int length1 = strlen(displayText1);
	for (int i = 0; i < length1; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, displayText1[i]);
	}

	// Set the position to display ""
	float xPos2 = 700;  // Adjust the X position
	float yPos2 = HEIGHT - 30;   // Adjust the Y position

	glRasterPos2f(xPos2, yPos2);
	const char* text2 = "Time: ";
	std::string fullText2 = text2 + std::to_string(count);
	const char* displayText2 = fullText2.c_str();
	int length2 = strlen(displayText2);
	for (int i = 0; i < length2; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, displayText2[i]);
	}

	// Restore the original matrices
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


	glColor3f(1.0, 1.0, 1.0);
}


void drawCrosshair() {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST); // Temporarily disable depth test
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(50.0f);

	glBegin(GL_LINES);
	glVertex2f(WIDTH / 2 - 10, HEIGHT / 2); // Horizontal line
	glVertex2f(WIDTH / 2 + 10, HEIGHT / 2);

	glVertex2f(WIDTH / 2, HEIGHT / 2 - 10); // Vertical line
	glVertex2f(WIDTH / 2, HEIGHT / 2 + 10);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}


//=======================================================================
// Bullet Code 
//=======================================================================


bool maleZombieAlive = true;
bool femaleZombieAlive = true;
bool greenAlienAlive = true;
bool grayAlienAlive = true;

struct Bullet {
	float posX, posY, posZ; // Position of the bullet
	float velX, velY, velZ; // Velocity of the bullet
	int cycle = 0;
	int direction;  // 1 for right, 2 for left, 3 for front, 4 for back

	bool operator==(const Bullet& other) const {
		return (posX == other.posX && posY == other.posY && posZ == other.posZ &&
			velX == other.velX && velY == other.velY && velZ == other.velZ);
	}

	Bullet(float x, float y, float z, float vx, float vy, float vz, int direction) : posX(x), posY(y), posZ(z), velX(vx), velY(vy), velZ(vz), cycle(0), direction(direction) {}
};

std::vector<Bullet> bullets; // Vector to store all the bullets

void updateBullets(int value) {
	// Update the position of each bullet in the vector
	for (auto& bullet : bullets) {

		float bulletPos[3] = { bullet.posX, bullet.posY, bullet.posZ };

		//code to check if a zombie or alien died
		if (gameLevel == 1) {
			//check collisions with zombies

			if (maleZombieAlive && checkIntersect(bulletPos, zombieMalePos)) {
				score += 50;
				maleZombiehits++;
				if (maleZombiehits == 2) {
					maleZombieAlive = false;
					sndPlaySound(TEXT("sounds/zombieDeath.wav"), SND_FILENAME | SND_ASYNC);
					std::this_thread::sleep_for(std::chrono::seconds(2));
					sndPlaySound(TEXT("sounds/gameBackground.wav"), SND_FILENAME | SND_ASYNC);
				}
				else {
					for (int i = 0; i < 3; ++i) {
						if (i % 2 == 0) {
							if (zombieMalePos[i] > playerPos[i]) {
								zombieMalePos[i] += 0.3f;
							}
							else {
								zombieMalePos[i] -= 0.3f;
							}
							
						}
					}
				}

			}
			if (femaleZombieAlive && checkIntersect(bulletPos, zombieFemalePos)) {
				score += 50;
				femaleZombiehits++;
				if (femaleZombiehits == 2) {
					femaleZombieAlive = false;
					sndPlaySound(TEXT("sounds/zombieDeath.wav"), SND_FILENAME | SND_ASYNC);
					std::this_thread::sleep_for(std::chrono::seconds(2));
					sndPlaySound(TEXT("sounds/gameBackground.wav"), SND_FILENAME | SND_ASYNC);
				}
				else {
					for (int i = 0; i < 3; ++i) {
						if (i % 2 == 0) {
							if (zombieFemalePos[i] > playerPos[i]) {
								zombieFemalePos[i] += 0.3f;
							}
							else {
								zombieFemalePos[i] -= 0.3f;
							}

						}
					}
				}
			}
		}
		else {
			//check collisions with aliens

			if (greenAlienAlive && checkIntersect(bulletPos, alienGreenPos)) {
				score += 100;
				greenAlienHits++;
				if (greenAlienHits == 2) {
					greenAlienAlive = false;
					sndPlaySound(TEXT("sounds/alienDeath.wav"), SND_FILENAME | SND_ASYNC);
					std::this_thread::sleep_for(std::chrono::seconds(3));
					sndPlaySound(TEXT("sounds/gameBackground.wav"), SND_FILENAME | SND_ASYNC);
				}
				else {
					for (int i = 0; i < 3; ++i) {
						if (i % 2 == 0) {
							if (alienGreenPos[i] > playerPos[i]) {
								alienGreenPos[i] += 0.3f;
							}
							else {
								alienGreenPos[i] -= 0.3f;
							}

						}
					}

				}
			}
			if (grayAlienAlive && checkIntersect(bulletPos, alienGrayPos)) {
				score += 100;
				grayAlienHits++;
				if (grayAlienHits == 2) {
					grayAlienAlive = false;
					sndPlaySound(TEXT("sounds/alienDeath.wav"), SND_FILENAME | SND_ASYNC);
					std::this_thread::sleep_for(std::chrono::seconds(3));
					sndPlaySound(TEXT("sounds/gameBackground.wav"), SND_FILENAME | SND_ASYNC);
				}
				else {
					for (int i = 0; i < 3; ++i) {
						if (i % 2 == 0) {
							if (alienGrayPos[i] > playerPos[i]) {
								alienGrayPos[i] += 0.3f;
							}
							else {
								alienGrayPos[i] -= 0.3f;
							}

						}
					}

				}
			}
		}



		if(bullet.direction == 1)
			bullet.posX += bullet.velX;
		if(bullet.direction == 2)
			bullet.posX -= bullet.velX;
		if(bullet.direction == 3)
			bullet.posZ += bullet.velY;
		if(bullet.direction == 4)
			bullet.posZ -= bullet.velY;

		bullet.cycle++;

		if (bullet.cycle == 10) {
			// Find the iterator pointing to the specific bullet in the vector
			auto it = std::find(bullets.begin(), bullets.end(), bullet);

			// Check if the bullet was found
			if (it != bullets.end()) {
				// Erase the bullet from the vector
				bullets.erase(it);
			}
		}

	}

	glutPostRedisplay();

	// Schedule the next update after 0.5 seconds (500 milliseconds)
	glutTimerFunc(500, updateBullets, 0);
}

void drawBullet(float posX, float posY, float posZ) {

	int bulletsize = 0.1;

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f); // Red color for bullets (you can change the color)
	glTranslatef(posX, posY, posZ); // Translate to the bullet's position
	glutSolidSphere(0.2, 10, 10); // Draw a small sphere as a bullet
	glPopMatrix();
}

// Function to draw the win screen
void drawWinScreen() {
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glColor3f(0.0, 1.0, 0.0);  // Green color
	glRasterPos2f(WIDTH / 2 - 50, HEIGHT / 2 + 20);
	const char* winText = "You Win!";
	int winLength = strlen(winText);
	for (int i = 0; i < winLength; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, winText[i]);
	}
}

// Function to draw the lose screen
void drawLoseScreen() {
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glColor3f(1.0, 0.0, 0.0);  // Red color
	glRasterPos2f(WIDTH / 2 - 70, HEIGHT / 2 + 20);
	const char* loseText = "Game Over!";
	int loseLength = strlen(loseText);
	for (int i = 0; i < loseLength; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, loseText[i]);
	}
}


// Function to check collision between player and house from any side
bool checkHouseCollision() {
	float x1 = housePos[0] - 5;  // Adjust the collision bounds as needed
	float x2 = housePos[0] + 5;
	float z1 = housePos[2] - 5;
	float z2 = housePos[2] + 5;

	return (playerPos[0] >= x1 && playerPos[0] <= x2 && playerPos[2] >= z1 && playerPos[2] <= z2);
}

bool checkCollisionWithSpaceship() {
	if (gameOver) {
		// Game is over, no collision check needed
		return false;
	}

	// Assuming the spaceship is a rectangular object
	float spaceshipMinX = spaceshipPos[0] - 2;  // Adjust based on spaceship dimensions
	float spaceshipMaxX = spaceshipPos[0] + 2;
	float spaceshipMinZ = spaceshipPos[2] - 2;  // Adjust based on spaceship dimensions
	float spaceshipMaxZ = spaceshipPos[2] + 2;

	// Check if the player's position is within the spaceship boundaries
	return (playerPos[0] >= spaceshipMinX && playerPos[0] <= spaceshipMaxX &&
		playerPos[2] >= spaceshipMinZ && playerPos[2] <= spaceshipMaxZ);
}
 

void fireBullet() {

	int bulletPosx;
	int bulletPosy;
	int bulletPosz;

	if (cameraMode == 1) {
		//shoot front from the middle of the screen 
		bulletPosx = Eye.x;
		bulletPosy = Eye.y;
		bulletPosz = Eye.z;
		
	}
	else {
		bulletPosx = playerPos[0];
		bulletPosy = playerPos[1] + 3.0f;
		bulletPosz = playerPos[2];

	}

	sndPlaySound(TEXT("sounds/shoot.wav"), SND_FILENAME | SND_ASYNC);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	sndPlaySound(TEXT("sounds/gameBackground.wav"), SND_FILENAME | SND_ASYNC);

	int bulletDirection;
	if (playerRot == 0.0f)
		bulletDirection = 3;
	else if (playerRot == 180.0f)
		bulletDirection = 4;
	else if (playerRot == 90.0)
		bulletDirection = 1;
	else
		bulletDirection = 2;


	bullets.emplace_back(bulletPosx, bulletPosy, bulletPosz, 1.0f, 1.0f, 1.0f, bulletDirection);
}


//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Draw Ground
	RenderGround();

	if (shakeCamera && shakeTimer < 2000) {
		glPushMatrix(); // Save the current matrix state

		// Shake the camera horizontally
		double offsetX = 2.0 * sin(shakeTimer * 0.01); // Adjust the multiplier for intensity
		glTranslated(offsetX, 0.0, 0.0);
		shakeTimer += 16; // Adjust as needed

		glPopMatrix(); // Restore the saved matrix state
	}

	if (showDamageIndicator) {
		// Render red light at player's position
		glEnable(GL_LIGHT2);
		GLfloat redLight[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		GLfloat redLightPosition[] = { playerPos[0], playerPos[1] + 2.0f, playerPos[2], 1.0f };
		glLightfv(GL_LIGHT2, GL_DIFFUSE, redLight);
		glLightfv(GL_LIGHT2, GL_POSITION, redLightPosition);
	}
	else {
		glDisable(GL_LIGHT2); // Turn off the additional light source
	}


	if (gameLevel == 1) {
		drawHouse();

		drawBrain();

		if (maleZombieAlive) {
			drawZombieMale();
		}

		if (femaleZombieAlive) {
			drawZombieFemale();
		}
	}
	else {

		if (grayAlienAlive) {
			drawAlienGray();
		}

		if (greenAlienAlive) {
			drawAlienGreen();
		}

		drawStar();

		drawSpaceship();
	}

	playerLightPosition[0] = playerPos[0];
	playerLightPosition[1] = playerPos[1] + 2.0;
	playerLightPosition[2] = playerPos[2];
	playerLightPosition[3] = 1.0f; // Positioned above the player
	glLightfv(GL_LIGHT1, GL_POSITION, playerLightPosition);


	drawPlayer();

	//drawHazmat();

	 // Draw the win or lose screen based on gameResult
	if (gameOver) {
		if (gameResult) {
			drawWinScreen();
		}
		else {
			drawLoseScreen();
		}
	}

	

	//sky box
	glPushMatrix();

	glClearColor(0.3f, 0.1f, 0.1f, 1.0f);

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);

	glPopMatrix();

	DrawHealthBar();

	if(cameraMode == 1)
	drawCrosshair();

	for (const auto& bullet : bullets) {
		drawBullet(bullet.posX, bullet.posY, bullet.posZ);
	}

	glutSwapBuffers();
}


//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	const GLdouble radiansPerDegree = M_PI / 180.0; // Convert degrees to radians

	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			cameraMode = 1;
			Eye.x = playerPos[0] + cos(playerRot * M_PI / 180.0);
			Eye.y = playerPos[1] + 5;
			Eye.z = playerPos[2] + sin(playerRot * M_PI / 180.0);
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			// Change to camera mode 3
			cameraMode = 3;
			Eye.x = playerPos[0] + cos(playerRot * M_PI / 180.0);
			Eye.y = playerPos[1] + 8;
			Eye.z = playerPos[2] + sin(playerRot * M_PI / 180.0) - 10;
			
		}

		// Calculate the direction the player is looking
		GLdouble lookX = sin(playerRot * radiansPerDegree);
		GLdouble lookZ = cos(playerRot * radiansPerDegree);

		if (cameraMode == 1) {
			// Update the Eye position to be at the player's position
			Eye.x = playerPos[0];
			Eye.y = playerPos[1] + 5; // Adjust height if needed
			Eye.z = playerPos[2];

			// Update the At vector to be in the direction the player is facing
			At.x = Eye.x + lookX;
			At.y = Eye.y;
			At.z = Eye.z + lookZ;
		}

		if (cameraMode == 3) {

			Eye.x = playerPos[0] - lookX * 10; // Adjust the multiplier for distance behind player
			Eye.y = playerPos[1] + 5; // Height offset
			Eye.z = playerPos[2] - lookZ * 10; // Adjust the multiplier for distance behind player

			// For camera mode 3, update the Eye position to stay behind the player's head

			At.x = Eye.x + lookX;
			At.y = Eye.y;
			At.z = Eye.z + lookZ;
		}

		// Update camera view
		glLoadIdentity();
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
		glutPostRedisplay();
	}
}


void myKeyboard(unsigned char button, int x, int y)
{
	const GLdouble cameraSpeed = 0.5; // Adjust as needed
	const GLdouble upDownSpeed = 0.1;
	const GLdouble movement = 0.1;
	const GLdouble radiansPerDegree = M_PI / 180.0; // Convert degrees to radians

	if (gameOver) {
		// Game is over, don't process keyboard input
		return;
	}


	switch (button)
	{
	case 's': // Move forward
		Eye.z += cameraSpeed;
		break;
	case 'a': // Move backward
		Eye.x -= cameraSpeed;
		break;
	case 'd': // Move left
		Eye.x += cameraSpeed;
		break;
	case 'w': // Move right
		Eye.z -= cameraSpeed;
		break;

	case 'k': // Move player forward
		if (playerPos[2] + movement <= 100) // Check boundary along Z-axis
		{
			playerRot = 0.0;
			playerPos[2] += movement;
		}
		break;

	case 'i': // Move player backward
		if (playerPos[2] - movement >= 0) // Check boundary along Z-axis
		{
			playerRot = 180.0;
			playerPos[2] -= movement;
		}
		break;

	case 'l': // Move player right
		if (playerPos[0] + movement <= 100) // Check boundary along X-axis
		{
			playerRot = 90.0;
			playerPos[0] += movement;
		}
		break;

	case 'j': // Move player left
		if (playerPos[0] - movement >= 0) // Check boundary along X-axis
		{
			playerRot = -90.0;
			playerPos[0] -= movement;
		}

		break;
	case 'e':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;

	case '9': // Move UP vector upwards
		Eye.y += upDownSpeed;
		break;
	case '0': // Move UP vector downwards
		Eye.y -= upDownSpeed;
		break;

	case '1':
		cameraMode = 1;
		Eye.x = playerPos[0] + cos(playerRot * M_PI / 180.0);
		Eye.y = playerPos[1] + 5;
		Eye.z = playerPos[2] + sin(playerRot * M_PI / 180.0);
		break;
	case '2':
		cameraMode = 2;
		Eye.x = 10;
		Eye.y = 10;
		Eye.z = 40;
		break;
	case '3':
		cameraMode = 3;
		Eye.x = playerPos[0] + cos(playerRot * M_PI / 180.0);
		Eye.y = playerPos[1] + 8;
		Eye.z = playerPos[2] + sin(playerRot * M_PI / 180.0)-10;
		break;
	case 27: // ESC key
		exit(0);
		break;
	case 32: // ASCII code for Spacebar
		if (!isJumping) {
			isJumping = true;
			jumpVelocity = 1.0f; // initial jump velocity, adjust as needed
		}
		break;
	case 13: // ASCII code for Enter Key
		fireBullet();
		break;

	default:
		break;
	}

	GLdouble lookX = sin(playerRot * radiansPerDegree);
	GLdouble lookZ = cos(playerRot * radiansPerDegree);

	if (cameraMode == 1) {
		// Update the Eye position to be at the player's position
		Eye.x = playerPos[0];
		Eye.y = playerPos[1] + 5; // Adjust height if needed
		Eye.z = playerPos[2];

		// Update the At vector to be in the direction the player is facing
		At.x = Eye.x + lookX;
		At.y = Eye.y;
		At.z = Eye.z + lookZ;
	}

	if (cameraMode == 3) {
		Eye.x = playerPos[0] - lookX * 10; // Adjust the multiplier for distance behind player
		Eye.y = playerPos[1] + 5; // Height offset
		Eye.z = playerPos[2] - lookZ * 10; // Adjust the multiplier for distance behind player

		// For camera mode 3, update the Eye position to stay behind the player's head

		At.x = Eye.x + lookX;
		At.y = Eye.y;
		At.z = Eye.z + lookZ;
	}

	// Update camera
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	glutPostRedisplay();
}
//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}



//===============================================
//Enemies movement
//==============================================



void moveEnemies() {

	if (gameOver) {
		// Game is over, stop enemy movement
		return;
	}

	if (gameLevel == 1) {
		// Move zombies towards the player
		float zombieMovement = 0.2f;
		for (int i = 0; i < 3; ++i) {
			if (i % 2 == 0) {
				if (zombieMalePos[i] > playerPos[i]) {
					zombieMalePos[i] -= zombieMovement;
				}
				else {
					zombieMalePos[i] += zombieMovement;
				}
				if (zombieFemalePos[i] > playerPos[i]) {
					zombieFemalePos[i] -= zombieMovement;
				}
				else {
					zombieFemalePos[i] += zombieMovement;
				}
			}
		}
	}
	else if (gameLevel == 2) {
		// Move aliens towards the player
		float alienMovement = 0.3f;
		for (int i = 0; i < 3; ++i) {
			if (i % 2 == 0) {
				if (alienGrayPos[i] > playerPos[i]) {
					alienGrayPos[i] -= alienMovement;
				}
				else {
					alienGrayPos[i] += alienMovement;
				}
				if (alienGreenPos[i] > playerPos[i]) {
					alienGreenPos[i] -= alienMovement;
				}
				else {
					alienGreenPos[i] += alienMovement;
				}
			}
		}
	}

}


//=======================================================================
// Special Keys Function
//=======================================================================
void mySpecialKeys(int key, int x, int y)
{
	const GLdouble rotationSpeed = 1; // Adjust as needed

	switch (key)
	{
	case GLUT_KEY_UP: // Rotate up
		At.y += rotationSpeed;
		break;
	case GLUT_KEY_DOWN: // Rotate down
		At.y -= rotationSpeed;
		break;
	case GLUT_KEY_LEFT: // Rotate left
		At.x -= rotationSpeed;
		break;
	case GLUT_KEY_RIGHT: // Rotate right
		At.x += rotationSpeed;
		break;
	}

	// Update camera
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	glutPostRedisplay();
}


//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_brain.Load("Models/brain/brain.3ds");
	model_zombieMale.Load("Models/Zombie-male/zombieMale.3ds");
	model_zombieMale2.Load("Models/Player/sci_fi5.3ds");
	model_zombieFemale.Load("Models/Zombie-female/YX7E6SXK5QENDHI2C2SDRHFV9.3ds");
	model_alienGray.Load("Models/Alien-gray/RHF0I8IA4NR339RPGEX2E5UU4.3ds");
	model_alienGreen.Load("Models/Alien-green/K0Y3926GODW8EXPFE835EUWG7.3ds");
	model_star.Load("Models/Star/5ebea14b8ef94f57b1f37ac18211f70e.3ds");
	model_spaceship.Load("Models/Spaceship/IPFJ80NKQ01QATOPYPW2HQKAD.3ds");
	

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	tex_zombieMale.Load("models/Zombie-male/t0046_1.bmp");
}

void damageIndicatorTimer(int value) {
	showDamageIndicator = false; // Turn off damage indicator
	glutPostRedisplay();
}
void shakeCameraTimer(int value) {
	shakeTimer = 0;
	shakeCamera = false; // Turn off camera shake
	glutPostRedisplay();
}

void decrementHealth(int damage) { // 0-100 damage
	if (playerHealth >= damage) {
		playerHealth -= damage;
		showDamageIndicator = true; // Activate damage indicator
		shakeCamera = true; // Activate camera shake
		glutTimerFunc(2000, damageIndicatorTimer, 0); // Start damage indicator timer
		glutTimerFunc(2000, shakeCameraTimer, 0); // Start camera shake timer		sndPlaySound(TEXT("sounds/playerDamage.wav"), SND_FILENAME | SND_ASYNC);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		sndPlaySound(TEXT("sounds/gameBackground.wav"), SND_FILENAME | SND_ASYNC);
	}
	else {
		playerHealth = 0;
		gameOver = true;
		gameResult = false;
	}
	if (playerHealth <= maxHealth / 2) {
		lightIntensity[0] -= 0.02;
		lightIntensity[1] -= 0.02;
		lightIntensity[2] -= 0.02;
	}
}

void incrementHealth(int heal) {
	if (playerHealth+heal <= maxHealth) {
		playerHealth += heal;
	}
	else {
		playerHealth = maxHealth;
	}
}


void Timer(int value) {
	count++;

	moveEnemies();

	if (gameLevel == 1) {
		// Set light color to greenish for level 1
		GLfloat greenishLight[] = { 0.1, 0.5, 0.1, 1.0 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, greenishLight);
	}
	else if (gameLevel == 2) {
		// Set light color to purple-ish for level 2
		GLfloat purpleishLight[] = { 0.5, 0.0, 0.6, 1.0 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, purpleishLight);
	}

	if (gameOver) {
		// Stop player movement and enemy movement
		return;
	}

	if (gameLevel == 1) {

		// Check collision with the house
		if (checkHouseCollision()) {
			gameLevel = 2;
			count = 0;
		}

		if ( maleZombieAlive &&  checkIntersect(playerPos, zombieMalePos)) {
			decrementHealth(10);
		}

		if ( femaleZombieAlive &&  checkIntersect(playerPos, zombieFemalePos)) {
			decrementHealth(10);
		}

		if (showDamageIndicator) {
			// Start damage indicator timer
			glutTimerFunc(2000, damageIndicatorTimer, 0);
		}

		if (shakeCamera && shakeTimer < 2000) {
			shakeTimer += 16; // Adjust as needed
		}

		//Zombies game (Level 1)
		if (count == 100 && !gameOver ) {
			brains[0] = brains[1] = brains[2] = true;
			gameOver = true;
			gameResult = false;
		}

		bool f2 = true;
		bool f = true;
		for (int i = 0; i < 3 && !gameOver; i++) {
			if (brains[i] == false && checkIntersect(playerPos, brainsPos[i])) {
				brains[i] = true;
				score += 5;
				sndPlaySound(TEXT("sounds/collect.wav"), SND_FILENAME | SND_ASYNC);
				std::this_thread::sleep_for(std::chrono::seconds(2));
				sndPlaySound(TEXT("sounds/gameBackground.wav"), SND_FILENAME | SND_ASYNC);
			}
			if (!brainsFound) {
				f = f && brains[i];
			}
		}

		if (!brainsFound && f && !gameOver) {
			brainsFound = true;
		}

	}
	else {

		// Check collision with spaceship
		if (checkCollisionWithSpaceship()) {
			gameResult = true;
			gameOver = true;
		}


		if ( greenAlienAlive&&  checkIntersect(playerPos, alienGreenPos)) {
			decrementHealth(10);
		}

		if (grayAlienAlive &&  checkIntersect(playerPos, alienGrayPos)) {
			decrementHealth(10);
		}
		

		if (count == 100 && !gameOver ) {
			stars[0] = stars[1] = stars[2] = true;
			gameOver = true;
			gameResult = false;
		}

		//Alien game (Level 2)
		bool f = true;
		for (int i = 0; i < 3 && !gameOver; i++) {
			if (stars[i] == false && checkIntersect(playerPos, starsPos[i])) {
				stars[i] = true;
				score += 5;
				sndPlaySound(TEXT("sounds/collect.wav"), SND_FILENAME | SND_ASYNC);
				std::this_thread::sleep_for(std::chrono::seconds(2));
				sndPlaySound(TEXT("sounds/gameBackground.wav"), SND_FILENAME | SND_ASYNC);
			}
			if (!starsFound) {
				f = f && stars[i];
			}
		}

		if (!starsFound && f && !gameOver) {
			starsFound = true;
		}

	}
	

	// Check if the time is up
	if (count >= 90) {
		gameResult = false;
		gameOver = true;
	}

	if (gameOver && !gameOverSoundPlayed) {
		if (gameResult) {
			sndPlaySound(TEXT("sounds/surviveWin.wav"), SND_FILENAME | SND_ASYNC);
			gameOverSoundPlayed = true;
		}
		else {
			sndPlaySound(TEXT("sounds/gameOver.wav"), SND_FILENAME | SND_ASYNC);
			gameOverSoundPlayed = true;
		}
	}

	if (isJumping) {
		playerPos[1] += jumpVelocity; // Update player's Y position
		jumpVelocity -= gravity; // Apply gravity

		// Check for landing
		if (playerPos[1] <= 0.0f) {
			playerPos[1] = 0.0f; // Reset to ground level
			isJumping = false;
		}
	}

	lightIntensity[0] -= 0.01;
	lightIntensity[1] -= 0.01;
	lightIntensity[2] -= 0.01;

	if (count == 0) {
		lightIntensity[0] = 0.4;
		lightIntensity[1] = 0.4;
		lightIntensity[2] = 0.4;
	}


	glutPostRedisplay();
	glutTimerFunc(1000, Timer, 0);

}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutSpecialFunc(mySpecialKeys);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);


	glutTimerFunc(500, updateBullets, 0);
	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glutTimerFunc(0, Timer, 0);
	glShadeModel(GL_SMOOTH);

	sndPlaySound(TEXT("sounds/gameBackground.wav"), SND_FILENAME | SND_ASYNC);

	glutMainLoop();
}