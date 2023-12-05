#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>

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
float playerPos[3] = { 7, 3, 10 }; int playerRot = -90.0;
float zombieMalePos[3] = { 15, 3, 15 };
float zombieFemalePos[3] = { 15, 3, 10 };
float alienGrayPos[3] = { 5, 3, 15 };
float alienGreenPos[3] = { 5, 3, 20 };
float spaceshipPos[3] = { 17, 1, 25 };
float housePos[3] = { 5,0,5 };

float starsPos[3][3] = { { 7, 1, 20 },
						 { 6, 1, 15 },
						 { 5,1,14 } };

float brainsPos[3][3] = { { 13,2,10 },
						 { 13,2,13 },
						 { 13,2,15 } };

//controls
int level = 1, score = 0;
float playerHealth = 1.0; // Full health
bool stars[3] = { false,false,false };
bool brains[3] = { false,false,false };
bool gameOver, gameResult, starsFound, brainsFound;

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

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

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

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

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
	glColor3f(0.3, 0, 0.18);

	glTranslatef(playerPos[0], playerPos[1], playerPos[2]);
	glScalef(2.4, 2.4, 2.4);
	glRotatef(90.f, 1, 0, 0);
	glRotatef(playerRot, 0, 0, 1);
	model_zombieMale2.Draw();

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

}

void drawZombieMale() {
	//zombie-male model

	tex_zombieMale.Use();
	//glBindTexture(GL_TEXTURE_2D, tex_zombieMale.texture[1]);
	//glBindTexture(GL_TEXTURE_2D, tex_zombieMale.texture[2]);
	//glBindTexture(GL_TEXTURE_2D, tex_zombieMale.texture[3]);
	//glBindTexture(GL_TEXTURE_2D, tex_zombieMale.texture[4]);

	glPushMatrix();
	glTranslatef(zombieMalePos[0], zombieMalePos[1], zombieMalePos[2]);
	glScalef(2.4, 2.4, 2.4);
	glRotatef(90.f, 1, 0, 0);
	model_zombieMale.Draw();

	glPopMatrix();


}

void drawZombieFemale() {
	//zombie-female model

	tex_zombieMale.Use();

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
	glColor3f(0.3f, 0.3f, 0.3f); // Grey background
	glBegin(GL_QUADS);
	glVertex2f(10, HEIGHT - 30);
	glVertex2f(210 - 200 * playerHealth, HEIGHT - 30);
	glVertex2f(210 - 200 * playerHealth, HEIGHT - 10);
	glVertex2f(10, HEIGHT - 10);
	glEnd();

	// Draw the health bar
	glColor3f(0.0f, 1.0f, 0.0f); // Green health
	glBegin(GL_QUADS);
	glVertex2f(10, HEIGHT - 30);
	glVertex2f(10 + 200 * playerHealth, HEIGHT - 30); // Width based on health
	glVertex2f(10 + 200 * playerHealth, HEIGHT - 10);
	glVertex2f(10, HEIGHT - 10);
	glEnd();

	// Set the text color (white)
	glColor3f(0.0, 0.0, 0.0);

	// Set the position to display ""
	float xPos = 300;  // Adjust the X position
	float yPos = HEIGHT - 30;   // Adjust the Y position

	glRasterPos2f(xPos, yPos);
	const char* text = ("Score: ");
	int length = strlen(text);
	for (int i = 0; i < length; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}

	// Set the position to display ""
	float xPos1 = 500;  // Adjust the X position
	float yPos1 = HEIGHT - 30;   // Adjust the Y position

	glRasterPos2f(xPos1, yPos1);
	const char* text1 = "Level: ";
	int length1 = strlen(text1);
	for (int i = 0; i < length1; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text1[i]);
	}

	// Set the position to display ""
	float xPos2 = 700;  // Adjust the X position
	float yPos2 = HEIGHT - 30;   // Adjust the Y position

	glRasterPos2f(xPos2, yPos2);
	const char* text2 = "Time: ";
	int length2 = strlen(text2);
	for (int i = 0; i < length2; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text2[i]);
	}

	// Restore the original matrices
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


	glColor3f(1.0, 1.0, 1.0);
}



void drawTree() {
	// Draw Tree Model
	glPushMatrix();
	glTranslatef(10, 0, 0);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Draw Ground
	RenderGround();

	drawTree();

	drawHouse();

	drawBrain();

	drawZombieMale();
	
	drawZombieFemale();
	
	drawAlienGray();
	
	drawAlienGreen();
	
	drawStar();

	drawSpaceship();

	drawPlayer();

	//drawHazmat();

	//sky box
	glPushMatrix();

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

	glutSwapBuffers();
}

void myKeyboard(unsigned char button, int x, int y)
{
	const GLdouble cameraSpeed = 0.5; // Adjust as needed
	const GLdouble upDownSpeed = 0.1;
	const GLdouble movement = 0.1;

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
			playerRot = -90.0;
			playerPos[2] += movement;
		}
		break;

	case 'i': // Move player backward
		if (playerPos[2] - movement >= 0) // Check boundary along Z-axis
		{
			playerRot = 90.0;
			playerPos[2] -= movement;
		}
		break;

	case 'l': // Move player right
		if (playerPos[0] + movement <= 100) // Check boundary along X-axis
		{
			playerRot = 180.0;
			playerPos[0] += movement;
		}
		break;

	case 'j': // Move player left
		if (playerPos[0] - movement >= 0) // Check boundary along X-axis
		{
			playerRot = 0.0;
			playerPos[0] -= movement;
		}
		break;

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
	case 27: // ESC key
		exit(0);
		break;
	default:
		break;
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
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
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
	model_zombieMale.Load("Models/Zombie-male/W6VXUFDDZEWHLDMF1TDFEHLSQ.3ds");
	model_zombieMale2.Load("Models/Zombie-male/W6VXUFDDZEWHLDMF1TDFEHLSQ.3ds");
	model_zombieFemale.Load("Models/Zombie-female/YX7E6SXK5QENDHI2C2SDRHFV9.3ds");
	model_alienGray.Load("Models/Alien-gray/RHF0I8IA4NR339RPGEX2E5UU4.3ds");
	model_alienGreen.Load("Models/Alien-green/K0Y3926GODW8EXPFE835EUWG7.3ds");
	model_star.Load("Models/Star/5ebea14b8ef94f57b1f37ac18211f70e.3ds");
	model_spaceship.Load("Models/Spaceship/IPFJ80NKQ01QATOPYPW2HQKAD.3ds");
	//model_hazmat.Load("Models/Hazmat/IPFJ80NKQ01QATOPYPW2HQKAD.3ds");
	

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	tex_zombieMale.Load("models/Zombie-male/t0045_0.bmp");
	//tex_zombieMale.Load("Models/Zombie-male/t0045_2.bmp");
	//tex_zombieMale.Load("Models/Zombie-male/t0046_1.bmp");
	//tex_zombieMale.Load("Models/Zombie-male/t0047_0.bmp");
	//tex_zombieMale.Load("Models/Zombie-male/t0071_1.bmp");
}

void Timer(int value) {
	count++;

	if (count == 100 && !gameOver && !starsFound) {
		stars[0] = stars[1] = stars[2] = true;
		gameOver = true;
		gameResult = false;
	}


	bool f = true;
	for (int i = 0; i < 3 && !gameOver; i++) {
		if (stars[i] == false && checkIntersect(playerPos, starsPos[i])) {
			stars[i] = true;
			score += 5;
		}
		if (!starsFound) {
			f = f && stars[i];
		}
	}

	if (!starsFound && f && !gameOver) {
		starsFound = true;
		gameOver = true;
		gameResult = true;
	}

	if (count == 100 && !gameOver && !brainsFound) {
		brains[0] = brains[1] = brains[2] = true;
		gameOver = true;
		gameResult = false;
	}

	bool f2 = true;
	for (int i = 0; i < 3 && !gameOver; i++) {
		if (brains[i] == false && checkIntersect(playerPos, brainsPos[i])) {
			brains[i] = true;
			score += 5;
		}
		if (!brainsFound) {
			f = f && brains[i];
		}
	}

	if (!brainsFound && f && !gameOver) {
		brainsFound = true;
		gameOver = true;
		gameResult = true;
	}

	glutTimerFunc(500, Timer, 0);
	glutPostRedisplay();
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

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glutTimerFunc(0, Timer, 0);
	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}