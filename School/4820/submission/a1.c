/* GAME PROGRAMMING A1 - 3D world with perlin noise, gravity and clouds
 * BY: Michael Lang, 0733368
 * mlang03@mail.uoguelph.ca
 * Jan 25 2015
 */

/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

/* Frames per second code taken from : */
/* http://www.lighthouse3d.com/opengl/glut/index.php?fps */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graphics.h"

	/* initialize graphics library */
extern void graphicsInit(int *, char **);

	/* lighting control */
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();

	/* viewpoint control */
extern void setViewPosition(float, float, float);
extern void getViewPosition(float *, float *, float *);
extern void getOldViewPosition(float *, float *, float *);
extern void getViewOrientation(float *, float *, float *);

	/* add cube to display list so it will be drawn */
extern int addDisplayList(int, int, int);

	/* mob controls */
extern void createMob(int, float, float, float, float);
extern void setMobPosition(int, float, float, float, float);
extern void hideMob(int);
extern void showMob(int);

	/* player controls */
extern void createPlayer(int, float, float, float, float);
extern void setPlayerPosition(int, float, float, float, float);
extern void hidePlayer(int);
extern void showPlayer(int);

	/* flag which is set to 1 when flying behaviour is desired */
extern int flycontrol;
	/* flag used to indicate that the test world should be used */
extern int testWorld;
	/* list and count of polygons to be displayed, set during culling */
extern int displayList[MAX_DISPLAY_LIST][3];
extern int displayCount;
	/* flag to print out frames per second */
extern int fps;
	/* flag to indicate removal of cube the viewer is facing */
extern int dig;
	/* flag indicates the program is a client when set = 1 */
extern int netClient;
	/* flag indicates the program is a server when set = 1 */
extern int netServer; 
extern int gravity;

	/* frustum corner coordinates, used for visibility determination  */
extern float corners[4][3];

	/* determine which cubes are visible e.g. in view frustum */
extern void ExtractFrustum();
extern void tree(float, float, float, float, float, float, int);




typedef struct {
    int xSize;
    int zSize;
    int xPos;
    int zPos;
} Cloud;

Cloud clouds[4];

void moveClouds(void);

float interpolate (float a, float b, float w);
void moveCloud(Cloud * cloud);
/********* end of extern variable declarations **************/

	/*** collisionResponse() ***/
	/* -performs collision detection and response */
	/*  sets new xyz  to position of the viewpoint after collision */
	/* -can also be used to implement gravity by updating y position of vp*/
	/* note that the world coordinates returned from getViewPosition()
	   will be the negative value of the array indices */
void collisionResponse() {
    static float oldpy, oldpx, oldpz;
    float px,py,pz;
    getViewPosition(&px,&py,&pz);
    
    if (world[(int)-px][(int)-py][(int)-pz] != 0){
        if (world[(int)-px][(int)-(py-1.0)][(int)-pz] == 0){
            setViewPosition(px,py-1.0,pz);
            oldpy = py-1.0;
            oldpx = px;
            oldpz = pz;
            return;
        }
        else{
            setViewPosition(oldpx,oldpy,oldpz);
            return;
        }
    }
    else{
        if (gravity == 1){
            if (world[(int)-px][(int)-(py+0.8)][(int)-pz] == 0){
                setViewPosition(px,py+0.05,pz);
                oldpy= py+0.05;
                oldpx = px;
                oldpz = pz;
                return;
            }
        }
        oldpy = py;
        oldpx = px;
        oldpz = pz;
    }
	/* your collision code goes here */
}

float interpolate (float a, float b, float w){
    return (1.0-w)*a + w*b;
}

void genRandomGradients(float **** gradient, int gridSize){
    *gradient = malloc(sizeof(float**)*gridSize);
    float *** grid = *gradient;
    int i,j;
    for (i = 0; i < gridSize; i++){
        grid[i] = malloc(sizeof(float*)*gridSize);
        for (j = 0; j < gridSize; j++){
            float vector = (float) rand() / (float) (RAND_MAX/6.2831);
            grid[i][j] = malloc(sizeof(float)*2);
            grid[i][j][0] = cos(vector);
            grid[i][j][1] = sin(vector);
        }
    }
    
}

float gridDotProduct(float *** grid, int gridX, int gridY, float x, float y){
    float distanceX = x - (double) gridX;
    float distanceY = y - (double) gridY;
    
    return (distanceX*grid[gridX][gridY][0] + distanceY*grid[gridX][gridY][1]);
}

float perlinNoise (float *** grid, float x, float z){
    
    //get coordinates of the current grid;
    int x0 = (int) x;
    int x1 = x0 + 1;
    int z0 = (int) z;
    int z1 = z0 + 1;
    
    float weightX = x - (double) x0;
    float weightZ = z - (double) z0;
    weightX *= weightX;
    weightZ *= weightZ;
    
    float dot1, dot2, intx1, intx2, result;
    dot1 = gridDotProduct(grid,x0,z0,x,z);
    dot2 = gridDotProduct(grid,x1,z0,x,z);
    intx1 = interpolate (dot1, dot2, weightX);
    
    dot1 = gridDotProduct(grid,x0,z1,x,z);
    dot2 = gridDotProduct(grid,x1,z1,x,z);
    intx2 = interpolate (dot1, dot2, weightX);
    
    result = interpolate(intx1,intx2,weightZ);
    return result;
}




	/*** update() ***/
	/* background process, it is called when there are no other events */
	/* -used to control animations and perform calculations while the  */
	/*  system is running */
	/* -gravity must also implemented here, duplicate collisionResponse */
void update() {
int i, j, k;
float *la;
float oldvpy, oldvpx, oldvpz;
	/* sample animation for the test world, don't remove this code */
	/* -demo of animating mobs */
   if (testWorld) {
	/* sample of rotation and positioning of mob */
	/* coordinates for mob 0 */
      static float mob0x = 50.0, mob0y = 25.0, mob0z = 52.0;
      static float mob0ry = 0.0;
      static int increasingmob0 = 1;
	/* coordinates for mob 1 */
      static float mob1x = 50.0, mob1y = 25.0, mob1z = 52.0;
      static float mob1ry = 0.0;
      static int increasingmob1 = 1;
      
      

	/* move mob 0 and rotate */
	/* set mob 0 position */
      setMobPosition(0, mob0x, mob0y, mob0z, mob0ry);

	/* move mob 0 in the x axis */
      if (increasingmob0 == 1)
         mob0x += 0.2;
      else 
         mob0x -= 0.2;
      if (mob0x > 50) increasingmob0 = 0;
      if (mob0x < 30) increasingmob0 = 1;

	/* rotate mob 0 around the y axis */
      mob0ry += 1.0;
      if (mob0ry > 360.0) mob0ry -= 360.0;

	/* move mob 1 and rotate */
      setMobPosition(1, mob1x, mob1y, mob1z, mob1ry);

	/* move mob 1 in the z axis */
	/* when mob is moving away it is visible, when moving back it */
	/* is hidden */
      if (increasingmob1 == 1) {
         mob1z += 0.2;
         showMob(1);
      } else {
         mob1z -= 0.2;
         hideMob(1);
      }
      if (mob1z > 72) increasingmob1 = 0;
      if (mob1z < 52) increasingmob1 = 1;

	/* rotate mob 1 around the y axis */
      mob1ry += 1.0;
      if (mob1ry > 360.0) mob1ry -= 360.0;
     
    /* end testworld animation */
   } else {
	/* your code goes here */
        collisionResponse();
        moveClouds();
   }
}

void moveCloud(Cloud * cloud){
    int i,j;
    for (i = 0; i < cloud->xSize; i++){
        for (j = 0; j < cloud->zSize; j++){
            if (world[cloud->xPos+i][40][cloud->zPos+j] == 5){
                world[cloud->xPos+i][40][cloud->zPos+j] = 0;
            }
        }
    }
    cloud->xPos++;
    if (cloud->xPos == 80)
        cloud->xPos = 0;
    for (i = 0; i < cloud->xSize; i++){
        for (j = 0; j < cloud->zSize; j++){
            if (world[cloud->xPos+i][40][cloud->zPos+j] == 0){
                world[cloud->xPos+i][40][cloud->zPos+j] = 5;
            }
        }
    }
}

void moveClouds(void){
    int i;
    for (i = 0; i < 4; i++)
        moveCloud(&(clouds[i]));
    return;
}
void randomCloud(Cloud * cloud){
    cloud->xSize = rand() % 8 + 2;
    cloud->zSize = rand() % cloud->xSize + 2;
    cloud->xPos = rand() % 70 + 10;
    cloud->zPos = rand() % 70 + 10;
}

void createClouds(void){
    int i;
    for (i = 0; i < 4; i++)
        randomCloud(&(clouds[i]));
    return;
}


int main(int argc, char** argv)
{
int i, j, k;
	/* initialize the graphics system */
   graphicsInit(&argc, argv);

	/* the first part of this if statement builds a sample */
	/* world which will be used for testing */
	/* DO NOT remove this code. */
	/* Put your code in the else statment below */
	/* The testworld is only guaranteed to work with a world of
		with dimensions of 100,50,100. */
   if (testWorld == 1) {

	/* initialize world to empty */
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][j][k] = 0;
            
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][24][k] = 3;
	
	/* some sample objects */
	/* build a red platform */
	 //create some green and blue cubes 
      world[50][25][50] = 1;
      world[49][25][50] = 1;
      world[49][26][50] = 1;
      world[52][25][52] = 2;
      world[52][26][52] = 2;
      
      

	/* blue box shows xy bounds of the world */
      for(i=0; i<WORLDX-1; i++) {
         world[i][25][0] = 2;
         world[i][25][WORLDZ-1] = 2;
      }
      for(i=0; i<WORLDZ-1; i++) {
         world[0][25][i] = 2;
         world[WORLDX-1][25][i] = 2;
      }

	/* create two sample mobs */
	/* these are animated in the update() function */
      createMob(0, 50.0, 25.0, 52.0, 0.0);
      createMob(1, 50.0, 25.0, 52.0, 0.0);

	/* create sample player */
      createPlayer(0, 52.0, 27.0, 52.0, 0.0);
      

   } else {
        srand(time(NULL));
        float *** grads;
        int gridSize = 6;
        genRandomGradients(&grads,gridSize);
        createClouds();
        for(i=0; i<WORLDX; i++) {
            for(j=0; j<WORLDZ; j++) {
                world[i][25][j] = 8;
                float height = perlinNoise(grads,((float)i)/20.0,((float)j)/20.0)*25;
                int h = height > 0.0 ? (int) height: (int) -height;
                for (k = 20; k < 25 + h; k++){
                    world[i][k][j] = 1;
                }
            }
        }
      
      /*
      for (i = 0; i < gridSize; i++){
            for (j = 0; j < gridSize; j++){
                printf("Grid %d,%d: [%.2f, %.2f]\n", i,j,grads[i][j][0],grads[i][j][1]);
            }
      }
       for (i = 0; i < 100; i++){
            for (j = 0; j < 100; j++){
                printf("PNV = %.2f\n",perlinNoise(grads,((float)i)/10,((float)j/10)));
            }
      }*/

	/* your code to build the world goes here */

   }


	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}

