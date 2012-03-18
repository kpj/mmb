/* 
 * 
 * 
 * gcc -Wall -g -c render.c -o render
 * 
 * */

#include "render.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "GL/freeglut.h"
#if !defined(GLUT_WHEEL_UP)
#  define GLUT_WHEEL_UP   3
#  define GLUT_WHEEL_DOWN 4
#endif

static Render render;

// =====================================================================

void moveCamera(double dx, double dy, double dz) {
	render.camera.x += dx;
	render.camera.y += dy;
	render.camera.z += dz;
	render.camera.pos.x = (int)floor(render.camera.x);
	render.camera.pos.y = (int)floor(render.camera.y);
	render.camera.pos.z = (int)floor(render.camera.z);
	render.camera.low.x = render.camera.pos.x - render.camera.range;
	render.camera.low.y = render.camera.pos.y - render.camera.range;
	render.camera.low.z = render.camera.pos.z - render.camera.range;
	render.camera.high.x = render.camera.pos.x + render.camera.range;
	render.camera.high.y = render.camera.pos.y + render.camera.range;
	render.camera.high.z = render.camera.pos.z + render.camera.range;
}

void rotateCamera(float dax, float day) {
	/* 
	 * Adds dax and day to the current camera rotation.
	 * 
	 * Attention: The rotation relative to the X-axis cannot be raised over
	 * PI and not be lowered below zero.
	 * 
	 * */

	render.camera.ax += dax;
	if (render.camera.ax < 0.1) render.camera.ax = 0.1;
	if (render.camera.ax > 3.1) render.camera.ax = 3.1;
	render.camera.ay = fmodf(render.camera.ay + day, M_PI * 2);

	render.camera.dx = sinf(render.camera.ax) * sinf(render.camera.ay);
	render.camera.dy = cosf(render.camera.ax);
	render.camera.dz = sinf(render.camera.ax) * cosf(render.camera.ay);
}

void renderDebug() {
	printf("===== RENDER DEBUG =====\n");
	printf("Camera: %1.1f/%1.1f/%1.1f %1.1f/%1.1f/%1.1f %i/%i/%i\n",
			render.camera.x,
			render.camera.y,
			render.camera.z,
			render.camera.dx, render.camera.dy, render.camera.dz,
			render.camera.pos.x,
			render.camera.pos.y,
			render.camera.pos.z);
	printf("Vertices drawn: %i\n", render.vertices);
	printf("Camera range: %i\n", render.camera.range);
	printf("===== END =====\n");
}

void onPassiveMotion(int x, int y) {
	int cx = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int cy = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	int dx = x - cx;
	int dy = y - cy;
	if (dx == 0 && dy == 0)
		return;

	float ax = dy * render.mouseSens * M_PI;
	float ay = dx * render.mouseSens * M_PI * 2;
	rotateCamera(ax, -ay);

	glutWarpPointer(cx, cy);
	glutPostRedisplay();
}

void renderOnMouse(int button, int state, int x, int y);
void renderOnMouseHook(int button, int state, int x, int y);

void catchPointer() {
	glutSetCursor(GLUT_CURSOR_NONE);
	int cx = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int cy = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	glutWarpPointer(cx, cy);
	glutPassiveMotionFunc(&onPassiveMotion);
	glutMouseFunc(&renderOnMouseHook);
}

void freePointer() {
	glutSetCursor(GLUT_CURSOR_INHERIT);
	glutPassiveMotionFunc(NULL);
	glutMouseFunc(&renderOnMouse);
}

void onKeyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'w':
			moveCamera(render.camera.dx, 0, render.camera.dz);
			break;
		case 's':
			moveCamera(-render.camera.dx, 0, -render.camera.dz);
			break;
		case 'a':
			moveCamera(render.camera.dz, 0, -render.camera.dx);
			break;
		case 'd':
			moveCamera(-render.camera.dz, 0, render.camera.dx);
			break;
		case ' ':
			moveCamera(0, 0.5, 0);
			break;
		case '<':
			moveCamera(0, -0.5, 0);
			break;

		case 27: // ESC
			freePointer();
			break;

		case 'q':
			glutLeaveMainLoop();
			break;
		case 'i':
			renderDebug();
			break;
		case '+':
			render.camera.range += 1;
			moveCamera(0,0,0);
			break;
		case '-':
			render.camera.range -= 1;
			moveCamera(0,0,0);
			break;
		default:
			printf("No mapping for key %i\n", key);
			break;
	}
	glutPostRedisplay();
}

void renderOnMouse(int button, int state, int x, int y) {
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		catchPointer();
	}
}

void renderOnMouseHook(int button, int state, int x, int y) {
	if (render.onMouse)
		render.onMouse(button, state, render.onMouseData);
}

void onIdle() {
	glutPostRedisplay();
}

Render *renderInit(int argc, char *argv[]) {
	render.onMouse = NULL;
	render.onMouseData = NULL;

	render.mouseSens = 1.0/5000;

	render.camera.range = 60;
	moveCamera(0.5, 2.5, -0.5);
	rotateCamera(1.8, -0.7);


	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
			GLUT_ACTION_CONTINUE_EXECUTION);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	render.window = glutCreateWindow("Mmb " VERSION);

	glutKeyboardFunc(&onKeyboard);
	glutMouseFunc(&renderOnMouse);
	glutIdleFunc(&onIdle);

	return &render;
}

void renderRun() {
	glutMainLoop();
}

void renderHookMouse(void (*func)(int button, int state, void *data),
		void *data)
{
	render.onMouse = func;
	render.onMouseData = data;
}

