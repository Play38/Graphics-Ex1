#pragma warning(disable:4996)
#include "Pixel.h"
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

/*#define FREEGLUT_STATIC
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0*/


class Point {
public:
	float x, y;
	void setxy(float x2, float y2)
	{
		x = x2; y = y2;
	}
	//operator overloading for '=' sign
	const Point& operator=(const Point& rPoint)
	{
		x = rPoint.x;
		y = rPoint.y;
		return *this;
	}

};

int factorial(int n)
{
	if (n <= 1)
		return(1);
	else
		n = n * factorial(n - 1);
	return n;
}

float binomial_coff(float n, float k)
{
	float ans;
	ans = factorial(n) / (factorial(k) * factorial(n - k));
	return ans;
}

using namespace std;

float red = 0.0, green = 0.0, blue = 0.0; // initiate it to black
int tmpx, tmpy; // store the first point when shape is line, or circle
int tmpcx1, tmpcy1, tmpcx2, tmpcy2, tmpcx3, tmpcy3, tmpcx4, tmpcy4;
bool isSecond = false;
float window_w = 500;
float window_h = 500;

Point abc[20];
int curvePoints = 0;
int clicks = 4;

int shape = 1; // 1:line, 2:circle, 3:curve

std::vector<Pixel> pixels;		// store all pixels

void display(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(2);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < pixels.size(); i++)
	{
		glColor3f(pixels[i].getR(), pixels[i].getG(), pixels[i].getB());
		glVertex2i(pixels[i].getX(), pixels[i].getY());
	}
	glEnd();
	glutSwapBuffers();
}



void quit()
{
	std::cout << "Goodbye" << std::endl;
	exit(0);
}

void drawPixel(int mousex, int mousey)
{
	Pixel newPixel(mousex, window_h - mousey, red, green, blue);
	pixels.push_back(newPixel);
}



void myLine(int x1, int y1, int x2, int y2)
{
	bool changed = false;
	if (abs(x2 - x1) < abs(y2 - y1))
	{
		int tmp1 = x1;
		x1 = y1;
		y1 = tmp1;
		int tmp2 = x2;
		x2 = y2;
		y2 = tmp2;
		changed = true;
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	int xi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}
	int d = 2 * dy - dx;
	int incrE = dy * 2;
	int incrNE = 2 * dy - 2 * dx;

	int x = x1, y = y1;
	if (changed)
		drawPixel(y, x);
	else
		drawPixel(x, y);
	while (x != x2)
	{
		if (d <= 0)
			d += incrE;
		else
		{
			d += incrNE;
			y += yi;
		}
		x += xi;
		if (changed)
			drawPixel(y, x);
		else
			drawPixel(x, y);
	}
}

/**
 * Midpoint circle algorithm
 */
void myCircle(int x1, int y1, int x2, int y2)
{
	int r = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	double d;
	int x, y;

	x = 0;
	y = r;
	d = 1.25 - r;

	while (x <= y)
	{
		drawPixel(x1 + x, y1 + y);
		drawPixel(x1 - x, y1 + y);
		drawPixel(x1 + x, y1 - y);
		drawPixel(x1 - x, y1 - y);
		drawPixel(x1 + y, y1 + x);
		drawPixel(x1 - y, y1 + x);
		drawPixel(x1 + y, y1 - x);
		drawPixel(x1 - y, y1 - x);
		x++;
		if (d < 0)
		{
			d += (double)2 * x + 3;
		}
		else
		{
			y--;
			d += (double)2 * (x - y) + 5;
		}
	}
}

//Calculate the bezier point
Point drawBezier(Point PT[], double t) {
	Point P;
	P.x = pow((1 - t), 3) * PT[0].x + 3 * t * pow((1 - t), 2) * PT[1].x + 3 * (1 - t) * pow(t, 2) * PT[2].x + pow(t, 3) * PT[3].x;
	P.y = pow((1 - t), 3) * PT[0].y + 3 * t * pow((1 - t), 2) * PT[1].y + 3 * (1 - t) * pow(t, 2) * PT[2].y + pow(t, 3) * PT[3].y;

	return P;
}

Point drawBezierGeneralized(Point PT[], double t) {
	Point P;
	P.x = 0; P.y = 0;
	for (int i = 0; i < clicks; i++)
	{
		P.x = P.x + binomial_coff((float)(clicks - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (clicks - 1 - i)) * PT[i].x;
		P.y = P.y + binomial_coff((float)(clicks - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (clicks - 1 - i)) * PT[i].y;
	}
	//cout<<P.x<<endl<<P.y;
	//cout<<endl<<endl;
	return P;
}

void keyboard(unsigned char key, int xIn, int yIn)
{
	isSecond = false;
	switch (key)
	{
	case 'q':
	case 27: // 27 is the esc key
		quit();
		break;
	
	}
}

void mouse(int bin, int state, int x, int y)
{
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		
			{
				if (!isSecond)
				{
					tmpx = x;
					tmpy = y;
					isSecond = true;
				}
				else
				{
					if (shape == 1)
						myLine(tmpx, tmpy, x, y);
					else if (shape == 2)
						myCircle(tmpx, tmpy, x, y);
					isSecond = false;
				}
				if (shape == 3)
				{
					abc[curvePoints].setxy((float)x, (float)(y));
					curvePoints++;

					if (curvePoints == clicks)
					{
						Point p1 = abc[0];
						/* Draw each segment of the curve.Make t increment in smaller amounts for a more detailed curve.*/
						for (double t = 0.0; t <= 1.0; t += 0.005)
						{
							Point p2 = drawBezierGeneralized(abc, t);
							myLine(p1.x, p1.y, p2.x, p2.y);
							p1 = p2;
						}
						curvePoints = 0;
					}
				}
			}
		}
	
}



void reshape(int w, int h)
{
	window_w = w;
	window_h = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void processMainMenu(int value)
{
	switch (value)
	{
	case 0:
		quit();
	
	}
}



void processShapeMenu(int value)
{
	shape = value;
	isSecond = false;

	switch (shape)
	{
	case 1:
	case 2:
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		break;
	}
}




void createOurMenu()
{

	int shapeMenu = glutCreateMenu(processShapeMenu);
	glutAddMenuEntry("Line", 1);
	glutAddMenuEntry("Circle", 2);
	glutAddMenuEntry("Curve", 3);



	int main_id = glutCreateMenu(processMainMenu);
	glutAddSubMenu("Shapes", shapeMenu);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void init(void)
{
	/* background color */
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(red, green, blue);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, window_w, 0.0, window_h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void FPS(int val)
{
	glutPostRedisplay();
	glutTimerFunc(0, FPS, 0);
}

void callbackInit()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutTimerFunc(17, FPS, 0);
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(window_w, window_h);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Paint");
	callbackInit();
	init();
	createOurMenu();
	glutMainLoop();
	return (0);
}