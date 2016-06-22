//graphics a3 by Mike Lang


/* Derived from scene.c in the The OpenGL Programming Guide */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
*/
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

	/* flags used to control the appearance of the image */
int lineDrawing = 1;	// draw polygons as solid or lines
int lighting = 0;	// use diffuse and specular lighting
int smoothShading = 0;  // smooth or flat shading
int textures = 0;

GLubyte  Image[7][200][320][4];
GLuint   textureID[7];

int curText = 0;

int floorsize = 6;
float cubesize = 1.5;

float theta = 0;//light angle
float light_distance = 6;

int px,py; //Mouse positions
int left_down,right_down; //Mouse button states
float cx,cy,cz; //Camera positions
float cx_angle = 20.0; //Camera angle on XZ PLANE
float cy_angle = 65*3.14/180; //Cameras viewing angle
float c_distance = 8; //Camera distance

typedef struct{
    float x;
    float y;
    float z;
} vec3f;

typedef struct{
    float x;
    float y;
    float z;
    float w;
} vec4f;

/*  Initialize material property and light source.
 */

//loads in a texture
void loadTexture(char * fName, int num) {
FILE *fp;
int  i, j;
int  red, green, blue;

   if ((fp = fopen(fName, "r")) == 0) {
      printf("Error, failed to find the file named %s.\n",fName);
      exit(0);
   } 
   int width, height, depth;
   char buff[100];
   fgets(buff,100,fp);
    fgets(buff,100,fp);
    fscanf(fp,"%d %d %d", &width, &height, &depth);
   for(i=0; i<height; i++) {
      for(j=0; j<width; j++) {
         fscanf(fp, "%d %d %d", &red, &green, &blue);
         Image[num][i][j][0] = red;
         Image[num][i][j][1] = green;
         Image[num][i][j][2] = blue;
         Image[num][i][j][3] = 255;
      }
   }

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glGenTextures(1,textureID);
   glBindTexture(GL_TEXTURE_2D, textureID[num]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, Image[num]);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);

   fclose(fp);
}

vec3f calcSurfaceNormal(vec3f a, vec3f b, vec3f c){
    vec3f ret;
    vec3f U;
    vec3f V;
    
    U.x = b.x - a.x;
    U.y = b.y - a.y;
    U.z = b.z - a.z;

    V.x = c.x - a.x;
    V.y = c.y - a.y;
    V.z = c.z - a.z;
    
    ret.x = U.y*V.z - (U.z*V.y);
    ret.y = U.z*V.x - (U.x*V.z);
    ret.z = U.x*V.y - (U.y*V.x);
    
    float length = sqrt((ret.x*ret.x) + (ret.y*ret.y) + (ret.z*ret.z));
    
    ret.x = ret.x / length;
    ret.y = ret.y / length;
    ret.z = ret.z / length;
    
    return ret;
}

//calculates a shadow matrix
void getShadowMatrix(GLfloat * shadowMat, vec4f floorNormal, vec4f lightpos){
    GLfloat dot =   floorNormal.x * lightpos.x +
                    floorNormal.y * lightpos.y +
                    floorNormal.z * lightpos.z +
                    floorNormal.w * lightpos.w;
                    
    shadowMat[0] = dot - lightpos.x * floorNormal.x; 
    shadowMat[4] = 0.f - lightpos.x * floorNormal.y; 
    shadowMat[8] = 0.f - lightpos.x * floorNormal.z; 
    shadowMat[12] = 0.f - lightpos.x * floorNormal.w; 
    
    shadowMat[1] = 0.f - lightpos.y * floorNormal.x; 
    shadowMat[5] = dot - lightpos.y * floorNormal.y; 
    shadowMat[9] = 0.f - lightpos.y * floorNormal.z; 
    shadowMat[13] = 0.f - lightpos.y * floorNormal.w; 
    
    shadowMat[2] = 0.f - lightpos.z * floorNormal.x; 
    shadowMat[6] = 0.f - lightpos.z * floorNormal.y; 
    shadowMat[10] = dot - lightpos.z * floorNormal.z; 
    shadowMat[14] = 0.f - lightpos.z * floorNormal.w; 
    
    shadowMat[3] = 0.f - lightpos.w * floorNormal.x; 
    shadowMat[7] = 0.f - lightpos.w * floorNormal.y; 
    shadowMat[11] = 0.f - lightpos.w * floorNormal.z; 
    shadowMat[15] = dot - lightpos.w * floorNormal.w; 
    
}

void moveLight(void){
    theta += 0.01;
    if (theta >= 2*3.14) theta = 0;
    GLfloat pos[] = {light_distance*cos(theta),2,light_distance*sin(theta),0.0};
    glLightfv (GL_LIGHT0, GL_POSITION, pos);
    glutPostRedisplay();
}

void drawCubeAndFloor(void){
    GLfloat black[] = {0,0,0,0};
    GLfloat grey[]  = {0.5, 0.5, 0.5, 1.0};
    GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
    float x_offset = floorsize/2 - cubesize/2;
    float z_offset = floorsize/2 - cubesize/2;
    
                                
                                
    GLfloat * shad = malloc(sizeof(GLfloat)*16);
    
    vec3f v1 = {0,-0.1,0};
    vec3f v2 = {floorsize,-0.1,0};
    vec3f v3 = {floorsize,-0.1,floorsize};
    vec3f v4 = {0,-0.1,floorsize};
    //side 1
    vec3f v5 = {x_offset+0,0,z_offset+0};
    vec3f v6 = {x_offset+0,0,z_offset+cubesize};
    vec3f v7 = {x_offset+0,cubesize,z_offset+cubesize};
    vec3f v8 = {x_offset+0,cubesize,z_offset+0};
    //side 2
    vec3f v9 = {x_offset+0,0,z_offset+0};
    vec3f v10 = {x_offset+cubesize,0,z_offset};
    vec3f v11 = {x_offset+cubesize,cubesize,z_offset};
    vec3f v12 = {x_offset+0,cubesize,z_offset+0};
    
    vec3f v13 = {x_offset+cubesize,0,z_offset+cubesize};
    vec3f v14 = {x_offset+0,0,z_offset+cubesize};
    vec3f v15 = {x_offset+0,cubesize,z_offset+cubesize};
    vec3f v16 = {x_offset+cubesize,cubesize,z_offset+cubesize};
    
    vec3f v17 = {x_offset+cubesize,0,z_offset+cubesize};
    vec3f v18 = {x_offset+cubesize,0,z_offset+0};
    vec3f v19 = {x_offset+cubesize,cubesize,z_offset+0};
    vec3f v20 = {x_offset+cubesize,cubesize,z_offset+cubesize};
    //top
    vec3f v21 = {x_offset+cubesize,cubesize,z_offset+cubesize};
    vec3f v22 = {x_offset+cubesize,cubesize,z_offset+0};
    vec3f v23 = {x_offset+0,cubesize,z_offset+0};
    vec3f v24 = {x_offset+0,cubesize,z_offset+cubesize};
    
    vec3f norm;
     glBegin(GL_QUADS);
//     
//     //draw floor
    norm = calcSurfaceNormal(v1,v4,v2);
    glNormal3f(norm.x,norm.y,norm.z);
//     printf("%f %f %f\n",norm.x,norm.y,norm.z);
    glVertex3f(v1.x,v1.y,v1.z);
    glVertex3f(v2.x,v2.y,v2.z);
    glVertex3f(v3.x,v3.y,v3.z);
    glVertex3f(v4.x,v4.y,v4.z);
    glEnd();
     
    vec4f normF = {0,1,0,0};
    vec4f light = {light_distance*cos(theta),2,light_distance*sin(theta),0};
    GLfloat shadowMatrix[] = {light.y,0,0,0,
                                -light.x,0,-light.z,-1,
                                0,0,light.y,0,
                                0,0,0,light.y};
                                
       if (textures == 1) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, textureID[0]);
	/* if textured, then use white as base colour */
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
   }


    
    glPushMatrix();
        glTranslatef(floorsize/2,0.5,floorsize/2);
        glutSolidCube(1);
        glDisable(GL_TEXTURE_2D);
        //glPushMatrix();
        glTranslatef(0,-0.499,0);
            //glRotatef(60,0,0,1);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
        glMultMatrixf(shadowMatrix);
        glutSolidCube(1);    
        //glPopMatrix();
        //glTranslatef(floorsize/2,0.5,floorsize/2);
    glPopMatrix();
    

    if (textures == 1) 
       glDisable(GL_TEXTURE_2D);
    
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, grey);
    glPushMatrix();
    glTranslatef(light.x,light.y,light.z);
    glTranslatef(floorsize/2,0,floorsize/2);
    glutSolidCube(0.5);
    glPopMatrix();
    
}

void init (void)
{
    loadTexture("brick.ppm",0);
    lineDrawing = 0;
    lighting = 1;
    smoothShading = 1;
    textures = 1;
   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_full_off[] = {0.0, 0.0, 0.0, 1.0};
   GLfloat light_full_on[] = {1.0, 1.0, 1.0, 1.0};

   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	/* if lighting is turned on then use ambient, diffuse and specular
	   lights, otherwise use ambient lighting only */
   if (lighting == 1) {
      glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
      glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
      glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
   } else {
      glLightfv (GL_LIGHT0, GL_AMBIENT, light_full_on);
      glLightfv (GL_LIGHT0, GL_DIFFUSE, light_full_off);
      glLightfv (GL_LIGHT0, GL_SPECULAR, light_full_off);
   }
   //glLightfv (GL_LIGHT0, GL_POSITION, light_position);
   
   glEnable (GL_LIGHTING);
   glEnable (GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

void display (void)
{
GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
GLfloat grey[]   = {0.5, 0.5, 0.5, 1.0};
GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw surfaces as either smooth or flat shaded */
   if (smoothShading == 1)
      glShadeModel(GL_SMOOTH);
   else
      glShadeModel(GL_FLAT);

	/* draw polygons as either solid or outlines */
   if (lineDrawing == 1)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   else 
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/* set starting location of objects */
   glPushMatrix ();
   //glTranslatef(0.0, 0.0, -14.0);
   //glRotatef (20.0, 1.0, 0.0, 0.0);

	/* give all objects the same shininess value */
   glMaterialf(GL_FRONT, GL_SHININESS, 30.0);

	/* set colour of cone */
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, grey);
   //glMaterialfv(GL_FRONT, GL_SPECULAR, white);
   
   
   float centre_height = floorsize/2;
   float centre_width = floorsize/2;
   float centre_depth = 0;
   //Calculating the camera position
   cx = centre_height + c_distance*cos(cx_angle)*sin(cy_angle);
   cz = centre_width + c_distance*sin(cx_angle)*sin(cy_angle);
   cy = centre_depth + c_distance*cos(cy_angle);
   //Aiming the camera
   gluLookAt(cx,cy,cz,centre_height,centre_depth,centre_width,0,1,0);
    drawCubeAndFloor();
    moveLight();
   glPopMatrix ();
   glFlush ();
}

void reshape(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 20.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();
}

//This function detects when a mouse button is pressed.
void mouse(int button, int state, int x, int y){
    switch (button){
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN){
                px = x;
                py = y;
                left_down = 1;

            }
            if (state == GLUT_UP){
                left_down = 0;
            }
            break;
    }
}
//This function controls camera movement while a button is held.
void up(int x, int y){
    if (left_down){
        float xdiff = (px - x)*0.05;
        float ydiff = (py - y)*0.1;
        cx_angle += xdiff;
        c_distance += ydiff;
        if (c_distance < 1.1) c_distance = 1.1;
        px = x;
        py = y;
        glutPostRedisplay();
    }
}



void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
      case 'q':
         exit(0);
         break;
      case '1':		// draw polygons as outlines
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 1;
         textures = 1;
         curText += 1;
         if (curText == 7) curText = 0;
         switch (curText){
             case 0:
                loadTexture("brick.ppm",0);
                break;
            case 1:
                loadTexture("horrible.ppm",0);
                break;
            case 2:
                loadTexture("psych.ppm",0);
                break;
            case 3:
                loadTexture("mud.ppm",0);
                break;
            case 4:
                loadTexture("spots.ppm",0);
                break;
            case 5:
                loadTexture("moon.ppm",0);
                break;
            case 6:
                loadTexture("wood.ppm",0);
                break;
         }
         display();
         break;
   }
}



/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize (1024, 768);
   glutCreateWindow (argv[0]);
   init();
   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
   //glutMouseFunc(mouse);
   //glutMotionFunc(up);
   glutMainLoop();
   return 0; 
}

