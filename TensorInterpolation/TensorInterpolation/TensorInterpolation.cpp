// TensorInterpolation.cpp : 定义控制台应用程序的入口点。
// 三边的特征向量插值 横竖

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "f2c.h"
#include <cmath>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <conio.h>
#include <vector>
#include "matrix.h"

#define X 32
#define Y 32
#define N 3
#define NUM_OF_LINE 1000


using namespace std;

//=============================================================
void cal(int i, int j);
void cal2(double curX, double curY, gsl_matrix* curDirection, int lineNumber);
void init();
void display(void);
void reshape(int w, int h);
void myIdle(void);
void trace();
void keyboard(unsigned char key, int x, int y);
bool isBiggerThan90(gsl_matrix* matrixA, gsl_matrix* matrixB);
bool isBiggerThan90(double x1, double y1, double x2, double y2);
bool dist(int numLine, double x, double y);
void fix(double curX, double curY, gsl_matrix* curDirection);
void seedPoint(double i, double j);
void mouse(int key, int state, int x, int y);
//=============================================================


const double eps = 1e-6;
int Density = 9;
double interval = 0.04;

double eigenvalue[X][Y][N];

double cl[X][Y];//glyph的颜色
double cp[X][Y];
double cs[X][Y];

gsl_matrix* eigenvector[X][Y];
gsl_matrix* negEigenvector[X][Y];
gsl_matrix* fix_eigenvector[2][2];//三近边调整后的特征向量
gsl_matrix* eigenvector_case[4][2][2];

float mtrl_ambient[4];
float mtrl_diffuse[4];
float mtrl_specular[4];
float one_mat[] = { 1.0,1.0,1.0,1 };
float zero_mat[] = { 0,0,0,1.0 };
float light0_position[] = { 15, 15,  50,  -1 };
float light0_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
float light0_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
float light0_specular[] = { 0.7, 0.7, 0.7, 1.0 };

float light1_position[] = { 30, 10,  20,  1 };
float light1_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
float light1_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
float light1_specular[] = { 0.5, 0.5, 0.5, 1.0 };

int up_ang = 0;
int left_ang = 0;
int print_mode = 1;

gsl_matrix* Ainv = gsl_matrix_calloc(N, N);
gsl_matrix* Cinv = gsl_matrix_calloc(N, N);
gsl_matrix* Binv = gsl_matrix_calloc(N, N);
gsl_matrix* negAinv = gsl_matrix_calloc(N, N);
gsl_matrix* negCinv = gsl_matrix_calloc(N, N);
gsl_matrix* negBinv = gsl_matrix_calloc(N, N);
gsl_matrix* Ainv_case[4];
gsl_matrix* Cinv_case[4];
gsl_matrix* Binv_case[4];
gsl_matrix* fix_Ainv;
gsl_matrix* fix_Cinv;
gsl_matrix* fix_Binv;

gsl_matrix* curDir = gsl_matrix_calloc(N, N);

gsl_matrix* uptmp1 = gsl_matrix_calloc(N, N);
gsl_matrix* uptmp2 = gsl_matrix_calloc(N, N);
gsl_matrix* uptmp3 = gsl_matrix_calloc(N, N);
gsl_matrix* downtmp1 = gsl_matrix_calloc(N, N);
gsl_matrix* downtmp2 = gsl_matrix_calloc(N, N);
gsl_matrix* downtmp3 = gsl_matrix_calloc(N, N);
gsl_matrix* tmp4 = gsl_matrix_calloc(N, N);
gsl_matrix* tmp5 = gsl_matrix_calloc(N, N);
gsl_matrix* tmp6 = gsl_matrix_calloc(N, N);

vector<double> lineX[NUM_OF_LINE];
vector<double> lineY[NUM_OF_LINE];
vector<double> lineCl[NUM_OF_LINE];
vector<double> lineCp[NUM_OF_LINE];
vector<double> lineCs[NUM_OF_LINE];
int numLine = 0;
double winW = 1.0;
double winH = 1.0;

void cal(int i, int j)//glyph主特征向量和颜色
{
	double a = eigenvalue[i][j][0], b = eigenvalue[i][j][1], c = eigenvalue[i][j][2];
	double tmp1, tmp2;
	if (a < b)
	{
		swap(a, b);
		swap(eigenvalue[i][j][0], eigenvalue[i][j][1]);
		for (int k = 0; k < N; k++)
		{
			tmp1 = gsl_matrix_get(eigenvector[i][j], 0, k);
			tmp2 = gsl_matrix_get(eigenvector[i][j], 1, k);
			gsl_matrix_set(eigenvector[i][j], 0, k, tmp2);
			gsl_matrix_set(eigenvector[i][j], 1, k, tmp1);
		}
	}
	if (a < c)
	{
		swap(a, c);
		swap(eigenvalue[i][j][0], eigenvalue[i][j][2]);
		for (int k = 0; k < N; k++)
		{
			tmp1 = gsl_matrix_get(eigenvector[i][j], 0, k);
			tmp2 = gsl_matrix_get(eigenvector[i][j], 2, k);
			gsl_matrix_set(eigenvector[i][j], 0, k, tmp2);
			gsl_matrix_set(eigenvector[i][j], 2, k, tmp1);
		}
	}
	if (b < c)
	{
		swap(b, c);
		swap(eigenvalue[i][j][1], eigenvalue[i][j][2]);
		for (int k = 0; k < N; k++)
		{
			tmp1 = gsl_matrix_get(eigenvector[i][j], 1, k);
			tmp2 = gsl_matrix_get(eigenvector[i][j], 2, k);
			gsl_matrix_set(eigenvector[i][j], 1, k, tmp2);
			gsl_matrix_set(eigenvector[i][j], 2, k, tmp1);
		}
	}
	cl[i][j] = (a - b) / (a + b + c);
	cp[i][j] = 2.0 * (b - c) / (a + b + c);
	cs[i][j] = 3.0 * c / (a + b + c);
}

void cal2(double curX, double curY, gsl_matrix* curDirection, int lineNumber)//流线主特征向量和颜色
{
	double tmpEigenvalue[3];
	for (int k = 0; k < N; k++)
	{
		tmpEigenvalue[k] = 0.0;
		tmpEigenvalue[k] += (double)(X - curX - 1) * (Y - curY - 1) * eigenvalue[0][0][k] / (X - 1) / (Y - 1);
		tmpEigenvalue[k] += (double)(X - curX - 1) * (curY)* eigenvalue[0][Y - 1][k] / (X - 1) / (Y - 1);
		tmpEigenvalue[k] += (double)(curX) * (Y - curY - 1) * eigenvalue[X - 1][0][k] / (X - 1) / (Y - 1);
		tmpEigenvalue[k] += (double)(curX) * (curY)* eigenvalue[X - 1][Y - 1][k] / (X - 1) / (Y - 1);
	}
	double a = tmpEigenvalue[0], b = tmpEigenvalue[1], c = tmpEigenvalue[2];
	double tmp1, tmp2;
	if (a < b)
	{
		swap(a, b);
		for (int k = 0; k < N; k++)
		{
			tmp1 = gsl_matrix_get(curDirection, 0, k);
			tmp2 = gsl_matrix_get(curDirection, 1, k);
			gsl_matrix_set(curDirection, 0, k, tmp2);
			gsl_matrix_set(curDirection, 1, k, tmp1);
		}
	}
	if (a < c)
	{
		swap(a, c);
		for (int k = 0; k < N; k++)
		{
			tmp1 = gsl_matrix_get(curDirection, 0, k);
			tmp2 = gsl_matrix_get(curDirection, 2, k);
			gsl_matrix_set(curDirection, 0, k, tmp2);
			gsl_matrix_set(curDirection, 2, k, tmp1);
		}
	}
	if (b < c)
	{
		swap(b, c);
		for (int k = 0; k < N; k++)
		{
			tmp1 = gsl_matrix_get(curDirection, 1, k);
			tmp2 = gsl_matrix_get(curDirection, 2, k);
			gsl_matrix_set(curDirection, 1, k, tmp2);
			gsl_matrix_set(curDirection, 2, k, tmp1);
		}
	}
	lineCl[lineNumber].push_back((a - b) / (a + b + c));
	lineCp[lineNumber].push_back(2.0 * (b - c) / (a + b + c));
	lineCs[lineNumber].push_back(3.0 * c / (a + b + c));
}
void init()
{
	glClearColor(0, 0, 0, 1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_NORMALIZE);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glLoadIdentity();

	glRotatef(up_ang, 1, 0, 0);
	glRotatef(left_ang, 0, 1, 0);
	if (print_mode & 2)
	{
		glColor3d(1, 1, 1);
		glLineWidth(1);
		for (int i = 0; i < numLine; i++)
		{
			glBegin(GL_LINE_STRIP);
			{
				int len = lineX[i].size();
				for (int j = 0; j < len; j++)
				{
					mtrl_ambient[0] = lineCl[i][j], mtrl_ambient[1] = lineCp[i][j], mtrl_ambient[2] = lineCs[i][j], mtrl_ambient[3] = 0.0;
					mtrl_diffuse[0] = lineCl[i][j], mtrl_diffuse[1] = lineCp[i][j], mtrl_diffuse[2] = lineCs[i][j], mtrl_diffuse[3] = 0.0;
					mtrl_specular[0] = lineCl[i][j], mtrl_specular[1] = lineCp[i][j], mtrl_specular[2] = lineCs[i][j], mtrl_specular[3] = 0.0;
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mtrl_ambient);
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mtrl_diffuse);
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mtrl_specular);
					glVertex3f(lineX[i][j], lineY[i][j], -1);
				}
			}
			glEnd();
		}
	}
	if (print_mode & 1)
	{
		for (int i = 1; i < X; i += 4)
		{
			for (int j = 1; j < Y; j += 4)
			{
				double a = eigenvalue[i][j][0], b = eigenvalue[i][j][1], c = eigenvalue[i][j][2];
				double frobenius = sqrt(a*a + b*b + c*c);

				a /= frobenius;
				b /= frobenius;
				c /= frobenius;
				double tmp[9];
				double rotate[16];
				for (int ii = 0; ii < N*N; ii++)
				{

					tmp[ii] = gsl_matrix_get(eigenvector[i][j], ii / N, ii%N);
					//把每个特征向量模变为1
					if (ii % N == 2)
					{
						double tot = sqrt(tmp[ii - 2] * tmp[ii - 2] + tmp[ii - 1] * tmp[ii - 1] + tmp[ii] * tmp[ii]);
						tmp[ii - 2] /= (tot)*0.4;
						tmp[ii - 1] /= (tot)*0.4;
						tmp[ii] /= (tot)*0.4;
					}
				}
				//========================
				rotate[0] = tmp[0];
				rotate[1] = tmp[1];
				rotate[2] = tmp[2];
				rotate[4] = tmp[3];
				rotate[5] = tmp[4];
				rotate[6] = tmp[5];
				rotate[8] = tmp[6];
				rotate[9] = tmp[7];
				rotate[10] = tmp[8];
				rotate[3] = rotate[7] = rotate[11] = rotate[12] = rotate[13] = rotate[14] = 0.0;
				rotate[15] = 1.0;
				//=======================
				mtrl_ambient[0] = cl[i][j], mtrl_ambient[1] = cp[i][j], mtrl_ambient[2] = cs[i][j], mtrl_ambient[3] = 0.0;
				mtrl_diffuse[0] = cl[i][j], mtrl_diffuse[1] = cp[i][j], mtrl_diffuse[2] = cs[i][j], mtrl_diffuse[3] = 0.0;
				mtrl_specular[0] = cl[i][j], mtrl_specular[1] = cp[i][j], mtrl_specular[2] = cs[i][j], mtrl_specular[3] = 0.0;
				glMaterialfv(GL_FRONT, GL_AMBIENT, mtrl_ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, mtrl_diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, mtrl_specular);
				glPushMatrix();
				glPushAttrib(GL_LIGHTING_BIT);
				glTranslated(i*1.0, j*1.0, 0);
				glMultMatrixd(rotate);
				glScaled(a, b, c);
				glutSolidSphere(0.3, 20, 20);
				glPopAttrib();
				glPopMatrix();

			}
		}
	}
	glFlush();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	winW = w*1.0;
	winH = h*1.0;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(90, w / h, 0, 60);
	glOrtho(-1, X, -1, Y, -60, 60);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glutPostRedisplay();
}

void myIdle(void)
{
	glutPostRedisplay();
}

void seedPoint(double i, double j)
{
	int cnt = 0;
	double curX = 1.0*i;
	double curY = 1.0*j;
	double preX = 0.0;//之前的方向
	double preY = 0.0;
	lineX[numLine].clear();
	lineY[numLine].clear();
	while (cnt < 500 && curX < X - 1 && (curX > 0.1 && (curY < Y - 1 && curY > 0.1)))
	{
		cnt++;
		lineX[numLine].push_back(curX);
		lineY[numLine].push_back(curY);
		fix(curX, curY, curDir);
		cal2(curX, curY, curDir, numLine);
		double xx = gsl_matrix_get(curDir, 0, 0);
		double xy = gsl_matrix_get(curDir, 0, 1);
		double norm = sqrt(xx*xx + xy*xy) * 5;
		xx /= norm;
		xy /= norm;
		if ((preX > eps) && isBiggerThan90(preX, preY, xx, xy)) break;
		preX = xx;
		preY = xy;
		curX += xx;
		curY += xy;
		if (dist(numLine, curX, curY)) break;
	}
	curX = 1.0*i;
	curY = 1.0*j;
	preX = 0.0;
	preY = 0.0;
	cnt = 0;
	numLine++;
	lineX[numLine].clear();
	lineY[numLine].clear();
	while (cnt < 500 && curX < X - 1 && (curX > 0.1 && (curY < Y - 1 && curY > 0.1)))
	{
		cnt++;
		lineX[numLine].push_back(curX);
		lineY[numLine].push_back(curY);
		fix(curX, curY, curDir);
		cal2(curX, curY, curDir, numLine);
		double xx = gsl_matrix_get(curDir, 0, 0);
		double xy = gsl_matrix_get(curDir, 0, 1);
		double norm = sqrt(xx*xx + xy*xy) * 5;
		xx /= norm;
		xy /= norm;
		if ((preX > eps) && isBiggerThan90(preX, preY, -xx, -xy)) break;
		preX = -xx;
		preY = -xy;
		curX -= xx;
		curY -= xy;
		if (dist(numLine - 1, curX, curY)) break;
	}
	numLine++;
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'w')
	{
		up_ang++;
	}
	else if (key == 's')
	{
		up_ang--;
	}
	else if (key == 'a')
	{
		left_ang++;
	}
	else if (key == 'd')
	{
		left_ang--;
	}
	else if (key == '1')
	{
		print_mode = 1;
	}
	else if (key == '2')
	{
		print_mode = 2;
	}
	else if (key == '3')
	{
		print_mode = 3;
	}
	else if (key == '+')
	{
		Density++;
		printf("Waiting...\n");
		trace();
		printf("Success Density = %d\n\n", Density);
	}
	else if (key == '-')
	{
		Density--;
		printf("Waiting...\n");
		trace();
		printf("Success Density = %d\n\n", Density);
	}
	else if (key == 'i')
	{
		printf("Input the interval, it is %lf now: ", interval);
		cin >> interval;
		printf("Waiting...\n");
		trace();
		printf("Success interval = %lf\n\n", interval);
	}
	glutPostRedisplay();
}

bool isBiggerThan90(gsl_matrix* matrixA, gsl_matrix* matrixB)
{
	double x1 = gsl_matrix_get(matrixA, 0, 0);
	double y1 = gsl_matrix_get(matrixA, 0, 1);
	double x2 = gsl_matrix_get(matrixB, 0, 0);
	double y2 = gsl_matrix_get(matrixB, 0, 1);
	double a = (x2 - x1)*(x2 - x1) + (y2 - y1) * (y2 - y1);
	double b = x1*x1 + y1*y1;
	double c = x2*x2 + y2*y2;
	double cos = (b + c - a) / (2 * sqrt(b)*sqrt(c));
	if (cos < -eps) return true;
	else return false;
}

bool isBiggerThan90(double x1, double y1, double x2, double y2)
{
	double a = (x2 - x1)*(x2 - x1) + (y2 - y1) * (y2 - y1);
	double b = x1*x1 + y1*y1;
	double c = x2*x2 + y2*y2;
	double cos = (b + c - a) / (2 * sqrt(b)*sqrt(c));
	if (cos < -eps) return true;
	else return false;
}

bool dist(int numLine, double x, double y)//是否有距离比较接近的线(曼哈顿距离)
{
	for (int i = 0; i < numLine; i++)
	{
		int len = lineX[i].size();
		for (int j = 0; j < len; j++)
		{
			double xx = lineX[i][j];
			double yy = lineY[i][j];
			if ((x - xx)*(x - xx) + (y - yy)*(y - yy) < 0.04) return true;
		}
	}
	return false;
}

void fix(double curX, double curY, gsl_matrix* curDirection)//最远边调整
{
	int ca;
	if (curX >= X - curX - 1.0 && curX >= curY && curX >= Y - curY - 1.0)//case 0
	{
		//cout << "case0" << endl;
		ca = 0;
		fix_eigenvector[0][0] = eigenvector_case[0][0][0];
		fix_eigenvector[0][1] = eigenvector_case[0][0][1];
		fix_eigenvector[1][0] = eigenvector_case[0][1][0];
		fix_eigenvector[1][1] = eigenvector_case[0][1][1];
		fix_Ainv = Ainv_case[0];
		fix_Binv = Binv_case[0];
		fix_Cinv = Cinv_case[0];
	}
	else if (curY >= Y - curY - 1.0 && curY >= curX && curY >= X - curX - 1.0)//case 1
	{
		//cout << "case1" << endl;
		ca = 1;
		fix_eigenvector[0][0] = eigenvector_case[1][0][0];
		fix_eigenvector[0][1] = eigenvector_case[1][0][1];
		fix_eigenvector[1][0] = eigenvector_case[1][1][0];
		fix_eigenvector[1][1] = eigenvector_case[1][1][1];
		fix_Ainv = Ainv_case[1];
		fix_Binv = Binv_case[1];
		fix_Cinv = Cinv_case[1];
	}
	else if (X - curX - 1.0 >= curX && X - curX - 1.0 >= curY && X - curX - 1.0 >= Y - curY - 1.0)//case 2
	{
		//cout << "case2" << endl;
		ca = 2;
		fix_eigenvector[0][0] = eigenvector_case[2][0][0];
		fix_eigenvector[0][1] = eigenvector_case[2][0][1];
		fix_eigenvector[1][0] = eigenvector_case[2][1][0];
		fix_eigenvector[1][1] = eigenvector_case[2][1][1];
		fix_Ainv = Ainv_case[2];
		fix_Binv = Binv_case[2];
		fix_Cinv = Cinv_case[2];
	}
	else//case 3
	{
		//cout << "case3" << endl;
		ca = 3;
		fix_eigenvector[0][0] = eigenvector_case[3][0][0];
		fix_eigenvector[0][1] = eigenvector_case[3][0][1];
		fix_eigenvector[1][0] = eigenvector_case[3][1][0];
		fix_eigenvector[1][1] = eigenvector_case[3][1][1];
		fix_Ainv = Ainv_case[3];
		fix_Binv = Binv_case[3];
		fix_Cinv = Cinv_case[3];
	}
	double t1 = (Y - 1 - curY)*1.0 / (Y - 1.0);
	double t2 = curX*1.0 / (X - 1.0);
	if (ca & 1)
	{
		gsl_matrix_mul(fix_Ainv, fix_eigenvector[0][0], uptmp1);
		gsl_matrix_mul(fix_Binv, fix_eigenvector[1][0], downtmp1);
		interpolateMatrix(uptmp2, uptmp1, t1);
		interpolateMatrix(downtmp2, downtmp1, t1);
		gsl_matrix_mul(fix_eigenvector[0][1], uptmp2, uptmp3);
		gsl_matrix_mul(fix_eigenvector[1][1], downtmp2, downtmp3);
		invertMatrix(tmp4, uptmp3);
		gsl_matrix_mul(tmp4, downtmp3, tmp5);
		interpolateMatrix(tmp6, tmp5, t2);
		gsl_matrix_mul(uptmp3, tmp6, curDirection);
	}
	else
	{
		double t1 = curX*1.0 / (X - 1.0);
		double t2 = (Y - 1 - curY)*1.0 / (Y - 1.0);
		gsl_matrix_mul(fix_Ainv, fix_eigenvector[1][1], uptmp1);
		gsl_matrix_mul(fix_Cinv, fix_eigenvector[1][0], downtmp1);
		interpolateMatrix(uptmp2, uptmp1, t1);
		interpolateMatrix(downtmp2, downtmp1, t1);
		gsl_matrix_mul(fix_eigenvector[0][1], uptmp2, uptmp3);
		gsl_matrix_mul(fix_eigenvector[0][0], downtmp2, downtmp3);
		invertMatrix(tmp4, uptmp3);
		gsl_matrix_mul(tmp4, downtmp3, tmp5);
		interpolateMatrix(tmp6, tmp5, t2);
		gsl_matrix_mul(uptmp3, tmp6, curDirection);
	}
}

void trace()
{
	numLine = 0;
	//for (int i = X / Density; i < X; i += X / Density)
	//{
	//	for (int j = Y / Density; j < Y; j += Y / Density)
	//	{
	//		seedPoint(i, j);
	//	}

	//}
}

void mouse(int key, int state, int x, int y)
{
	if (key == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		double curX = (x*1.0 - winW / (X + 1.0)) / ((X - 1.0)*winW / (X + 1.0)) * (X - 1.0);
		double curY = (winH - y*1.0 - winH / (Y + 1.0)) / ((Y - 1.0)*winH / (Y + 1.0)) * (Y - 1.0);
		//cout << curX << ' ' << curY << endl;
		if (curX > 0 && curY > 0 && curX < X - 1 && curY < Y - 1)
		{
			printf("tracing %lf %lf...\n", curX, curY);
			seedPoint(curX, curY);
			printf("Success\n");
		}
	}
}


int main(int argc, char** argv)
{
	ifstream fin("data.txt", ios::in);
	if (!fin)
	{
		printf("File open fail!");
	}
	else
	{
		double readin[N];

		//======================================read=============
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				//fix_eigenvector[i][j] = gsl_matrix_calloc(N, N);
				int ii = (i == 0) ? 0 : X - 1;
				int jj = (j == 0) ? 0 : Y - 1;
				//read eigenvalues
				for (int k = 0; k < N; k++)
				{
					fin >> eigenvalue[ii][jj][k];
				}
				eigenvector[ii][jj] = gsl_matrix_calloc(N, N);
				negEigenvector[ii][jj] = gsl_matrix_calloc(N, N);
				//read eigenvectors
				for (int m = 0; m < N; m++)
				{
					int n;
					for (n = 0; n < N; n++)
					{
						fin >> readin[n];
					}
					//把每个特征向量模变为1
					double norm = sqrt(readin[0] * readin[0] + readin[1] * readin[1] + readin[2] * readin[2]);
					for (n = 0; n < N - 1; n++)
					{
						gsl_matrix_set(eigenvector[ii][jj], m, n, readin[n] / norm);
						gsl_matrix_set(negEigenvector[ii][jj], m, n, -readin[n] / norm);
					}
					gsl_matrix_set(eigenvector[ii][jj], m, n, readin[n] / norm);
					gsl_matrix_set(negEigenvector[ii][jj], m, n, readin[n] / norm);//暂不考虑z轴
				}
				cal(ii, jj);
			}
		}

		invertMatrix(Ainv, eigenvector[0][Y - 1]);
		invertMatrix(Cinv, eigenvector[0][0]);
		invertMatrix(Binv, eigenvector[X - 1][Y - 1]);
		invertMatrix(negAinv, negEigenvector[0][Y - 1]);
		invertMatrix(negCinv, negEigenvector[0][0]);
		invertMatrix(negBinv, negEigenvector[X - 1][Y - 1]);
		//=============================================different case=================
		for (int i = 0; i < 4; i++)
		{
			eigenvector_case[i][0][0] = eigenvector[0][0];
			eigenvector_case[i][0][1] = eigenvector[0][Y - 1];
			eigenvector_case[i][1][0] = eigenvector[X - 1][0];
			eigenvector_case[i][1][1] = eigenvector[X - 1][Y - 1];
			Ainv_case[i] = Ainv;
			Cinv_case[i] = Cinv;
			Binv_case[i] = Binv;
			//cout << gsl_matrix_get(eigenvector_case[i][1][1], 0, 0) << ' ' << gsl_matrix_get(eigenvector[X-1][Y-1], 0, 0) << endl;
		}

		//缓存四个角修改的情况--------------------------------------------left--------
		if (isBiggerThan90(eigenvector_case[0][0][0], eigenvector_case[0][1][0])) eigenvector_case[0][1][0] = negEigenvector[X - 1][0];
		if (isBiggerThan90(eigenvector_case[0][1][0], eigenvector_case[0][1][1]))
		{
			Binv_case[0] = negBinv;
			eigenvector_case[0][1][1] = negEigenvector[X - 1][Y - 1];
		}
		if (isBiggerThan90(eigenvector_case[0][1][1], eigenvector_case[0][0][1]))
		{
			eigenvector_case[0][0][1] = negEigenvector[0][Y - 1];
			Ainv_case[0] = negAinv;
		}
		//-----------------------------------------------------------down-----------
		if (isBiggerThan90(eigenvector_case[1][1][0], eigenvector_case[1][1][1]))
		{
			Binv_case[1] = negBinv;
			eigenvector_case[1][1][1] = negEigenvector[X - 1][Y - 1];
		}
		if (isBiggerThan90(eigenvector_case[1][1][1], eigenvector_case[1][0][1]))
		{
			eigenvector_case[1][0][1] = negEigenvector[0][Y - 1];
			Ainv_case[1] = negAinv;
		}
		if (isBiggerThan90(eigenvector_case[1][0][1], eigenvector_case[1][0][0]))
		{
			eigenvector_case[1][0][0] = negEigenvector[0][0];
			Cinv_case[1] = negCinv;
		}
		//---------------------------------------------------------right-------
		if (isBiggerThan90(eigenvector_case[2][1][1], eigenvector_case[2][0][1]))
		{
			eigenvector_case[2][0][1] = negEigenvector[0][Y - 1];
			Ainv_case[2] = negAinv;
		}
		if (isBiggerThan90(eigenvector_case[2][0][1], eigenvector_case[2][0][0]))
		{
			eigenvector_case[2][0][0] = negEigenvector[0][0];
			Cinv_case[2] = negCinv;
		}
		if (isBiggerThan90(eigenvector_case[2][0][0], eigenvector_case[2][1][0])) eigenvector_case[2][1][0] = negEigenvector[X - 1][0];
		//----------------------------------------------------------------up----
		if (isBiggerThan90(eigenvector_case[3][0][1], eigenvector_case[3][0][0]))
		{
			eigenvector_case[3][0][0] = negEigenvector[0][0];
			Cinv_case[3] = negCinv;
		}
		if (isBiggerThan90(eigenvector_case[3][0][0], eigenvector_case[3][1][0])) eigenvector_case[3][1][0] = negEigenvector[X - 1][0];
		if (isBiggerThan90(eigenvector_case[3][1][0], eigenvector_case[3][1][1]))
		{
			Binv_case[3] = negBinv;
			eigenvector_case[3][1][1] = negEigenvector[X - 1][Y - 1];
		}


		//==============================================glyph====================
		for (int i = 0; i < X; i++)
		{
			for (int j = 0; j < Y; j++)
			{
				if ((i == 0 && j == 0) || (i == X - 1 && j == Y - 1) || (i == X - 1 && j == 0) || (i == 0 && j == Y - 1)) continue;
				for (int k = 0; k < N; k++)
				{
					eigenvalue[i][j][k] = 0.0;
					eigenvalue[i][j][k] += (double)(X - i - 1) * (Y - j - 1) * eigenvalue[0][0][k] / (X - 1) / (Y - 1);
					eigenvalue[i][j][k] += (double)(X - i - 1) * (j)* eigenvalue[0][Y - 1][k] / (X - 1) / (Y - 1);
					eigenvalue[i][j][k] += (double)(i) * (Y - j - 1) * eigenvalue[X - 1][0][k] / (X - 1) / (Y - 1);
					eigenvalue[i][j][k] += (double)(i) * (j)* eigenvalue[X - 1][Y - 1][k] / (X - 1) / (Y - 1);
				}
				eigenvector[i][j] = gsl_matrix_calloc(N, N);
				fix(i+0.2, j, eigenvector[i][j]);
				cal(i, j);
			}
		}
		//开始追踪
		trace();

		///=========================================Eigen System===========
		//for (int i = 0; i < X; i++)
		//{
		//	for (int j = 0; j < Y; j++)
		//	{
		//		for (int m = 0; m < N; m++)
		//		{
		//			cout << eigenvalue[i][j][m] << ' ';
		//		}
		//		cout << "\n\n";
		//		for (int m = 0; m < N; m++)
		//		{
		//			for (int n = 0; n < N; n++)
		//			{
		//				cout << gsl_matrix_get(eigenvector[i][j], m, n) << ' ';
		//			}
		//			cout << endl;
		//		}
		//		cout << endl;
		//	}
		//}
		///====================================================

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
		glutInitWindowSize(1000, 1000);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("Interpolation");
		init();
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
		glutMouseFunc(mouse);
		glutIdleFunc(myIdle);
		glutMainLoop();
	}
	for (int i = 0; i < X; i++)
	{
		for (int j = 0; j < Y; j++)
		{
			gsl_matrix_free(eigenvector[i][j]);
		}
	}
	gsl_matrix_free(Ainv);
	gsl_matrix_free(Cinv);
	gsl_matrix_free(uptmp1);
	gsl_matrix_free(uptmp2);
	gsl_matrix_free(uptmp3);
	gsl_matrix_free(downtmp1);
	gsl_matrix_free(downtmp1);
	gsl_matrix_free(downtmp1);
	gsl_matrix_free(tmp4);
	gsl_matrix_free(tmp5);
	gsl_matrix_free(tmp6);
	return 0;
}

