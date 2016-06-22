/* Computer Graphics Assignment #2 
 * By: Michael Lang, 0733368
 * Feb 29th 2016
 * mlang03@mail.uoguelph.ca
 */
/* Derived from scene.c in the The OpenGL Programming Guide */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
*/

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

	/* flags used to control the appearance of the image */
int lineDrawing = 0;	// draw polygons as solid or lines
int lighting = 1;	// use diffuse and specular lighting
int smoothShading = 0;  // smooth or flat shading
int textures = 0;

GLubyte  Image[64][64][4];
GLuint   textureID[1];

//Vector data type
typedef struct{
    float x;
    float y;
    float z;
} vec3f;

int ** heightmap;
float width = 0; //Width of the heightmap
float height = 0; //height of the heightmap
float depth = 0; //maximum value of the heightmap
float scale = 0.005; //X/Z scale for the object
float hscale = 0.05; // Y Scale for the object
int px,py; //Mouse positions
int left_down,right_down; //Mouse button states
float cx,cy,cz; //Camera positions
float cx_angle = 0.0; //Camera angle on XZ PLANE
float cy_angle = 45*3.14/180; //Cameras viewing angle
float c_distance = 7; //Camera distance

vec3f **upperN; //Array of surface normals for top triangle of each square
vec3f **lowerN; //Array of surface normals for bottom triangle of each square
vec3f **vertN; //Array of vertex normals
vec3f **vertC; // Array of vertex colours

//Required to fix the map being drawn backwards.
void flipHeightMap(){
    int i,j;
    int tempMap[(int)height][(int)width];
    for (i = 0; i < height; i++){
        for (j = 0; j < width; j++){
            tempMap[i][j] = heightmap[i][j];
        }
    }
    int k = 0;
    for (j = width - 1; j >= 0; j--){
        for (i = 0; i < height; i++){
            heightmap[i][k] = tempMap[i][j];
        }
        k++;
    }
}

//Reads a .pgm file into memory.
void loadHeightMap(int argc, char ** argv){
    if (argc == 1){
        printf("No input file specified. Exiting.\n");
        exit(1);
    }
    int i,j;
    int k;
    i = 0; j = 0;
    char * mem = calloc(200,sizeof(char));
    char *buffer = mem;
    FILE* fp = fopen(argv[1],"r");
    while (fgets(buffer,199,fp) != NULL){
        if (buffer[0] == 'P' && buffer[1] == '2') continue;
        if (buffer[0] == '#') continue;
        int offset = 0;
        int read;
        while (sscanf(buffer, "%d%n", &read, &offset) != EOF){
            if (width == 0){
                width = read;
            }
            else if (height == 0){
                height = read;
                heightmap = malloc(sizeof(int*)*height);
                for (k = 0; k < height; k++){
                    heightmap[k] = malloc(sizeof(int)*width);
                }
            }
            else if (depth == 0){
                depth = read;
            }
            else {
                heightmap[i][j] = read;
                j++;
                if (j == width){
                    i++;
                    j = 0;
                }
            }
            buffer += offset;
        }
        buffer = mem;
    }
    free(mem);
     upperN = malloc(sizeof(vec3f*)*height);
     lowerN = malloc(sizeof(vec3f*)*height);
     vertN = malloc(sizeof(vec3f)*height);
     vertC = malloc(sizeof(vec3f)*height);
     for (i = 0; i < height; i++){
         upperN[i] = malloc(sizeof(vec3f)*width);
         lowerN[i] = malloc(sizeof(vec3f)*width);
         vertN[i] = malloc(sizeof(vec3f)*width);
         vertC[i] = malloc(sizeof(vec3f)*width);
    }
    flipHeightMap();
}
//Calculates a particular triangles' surface normal.
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
//Calculates surface normals for each triangle.
void calcSurfaceNormals(){
    int i,j;
    for (i = 0; i < height - 1; i++){
        for (j = 0; j < width - 1; j++){
            vec3f v1 = {i*hscale, heightmap[i][j]*scale, j*hscale};
            vec3f v2 = {i*hscale,heightmap[i][j+1]*scale,(j+1)*hscale};
            vec3f v3 = {(i+1)*hscale,heightmap[i+1][j]*scale,j*hscale};
            vec3f v4 = {(i+1)*hscale,heightmap[i+1][j+1]*scale,(j+1)*hscale};
            
            
            vec3f norm = calcSurfaceNormal(v1,v2,v3);
            upperN[i][j].x = norm.x;
            upperN[i][j].y = norm.y;
            upperN[i][j].z = norm.z;
            
            vec3f norm2 = calcSurfaceNormal(v2,v4,v3);
            lowerN[i][j].x = norm2.x;
            lowerN[i][j].y = norm2.y;
            lowerN[i][j].z = norm2.z;
        }
    }
}

//Calculates the appropriate height based colour for each vertex.
//mode == 0 -> Grey only
//otherwise uses height based colour
void calcVertexColour(int mode){
    int i,j;
    for (i = 0; i < height; i++){
        for (j = 0; j < width; j++){
            if (mode == 0){
                vertC[i][j].x = 0.5;
                vertC[i][j].y = 0.5;
                vertC[i][j].z = 0.5;
            }
            else{
                if (heightmap[i][j] < depth/3){
                    vertC[i][j].x = 0.54;//0.64;
                    vertC[i][j].y = 0.27;//0.165;
                    vertC[i][j].z = 0.07;//0.165;
                }
                else if (heightmap[i][j] < depth*2/3){
                    vertC[i][j].x = 0.0;
                    vertC[i][j].y = 1.0;
                    vertC[i][j].z = 0.0;
                }
                else{
                    vertC[i][j].x = 1.0;
                    vertC[i][j].y = 1.0;
                    vertC[i][j].z = 1.0;
                }
            }
        }
    }
}

//Adds +/-10% random colour variation to each vertex.
void randomizeColour(){
    int i,j;
    for (i = 0; i < height; i++){
        for (j = 0; j < width; j++){
            float valx = (float)((rand() % 20) -10)/100;
            float valy = (float)((rand() % 20) -10)/100;
            float valz = (float)((rand() % 20) -10)/100;
            //printf("%f %f %f\n", valx,valy,valz);
            vertC[i][j].x += valx;
            if (vertC[i][j].x > 1.0) vertC[i][j].x = 1.0;
            if (vertC[i][j].x < 0.0) vertC[i][j].x = 0.0;
            vertC[i][j].y += valy;
            if (vertC[i][j].y > 1.0) vertC[i][j].y = 1.0;
            if (vertC[i][j].y < 0.0) vertC[i][j].y = 0.0;
            vertC[i][j].z += valz;
            if (vertC[i][j].z > 1.0) vertC[i][j].z = 1.0;
            if (vertC[i][j].z < 0.0) vertC[i][j].z = 0.0;
        }
    }
}

void calcVertexNormals(){
    int i,j;
    //left side
    for (i = 0; i < height; i++){
        //top left corner
        if (i == 0){
            vertN[0][0].x = upperN[0][0].x;
            vertN[0][0].y = upperN[0][0].y;
            vertN[0][0].z = upperN[0][0].z;
        }
        //bottom left corner
        else if (i == height-1){
            float sumx = lowerN[(int)height-2][0].x + upperN[(int)height-2][0].x;
            float sumy = lowerN[(int)height-2][0].y + upperN[(int)height-2][0].y;
            float sumz = lowerN[(int)height-2][0].z + upperN[(int)height-2][0].z;
            float length = sqrt((sumx*sumx) + (sumy*sumy) + (sumz*sumz));
            vertN[(int)height-1][0].x = sumx/length;
            vertN[(int)height-1][0].y = sumy/length;
            vertN[(int)height-1][0].z = sumz/length;
        }
        //interior left side
        else{
            float sumx = lowerN[i-1][0].x + upperN[i-1][0].x + upperN[i][0].x;
            float sumy = lowerN[i-1][0].y + upperN[i-1][0].y + upperN[i][0].y;
            float sumz = lowerN[i-1][0].z + upperN[i-1][0].z + upperN[i][0].z;
            float length = sqrt((sumx*sumx) + (sumy*sumy) + (sumz*sumz));
            vertN[i][0].x = sumx/length;
            vertN[i][0].y = sumy/length;
            vertN[i][0].z = sumz/length;
        }
    }
    //right side
    for (i = 0; i < height; i++){
        //top right corner
        if (i == 0){
            float sumx = lowerN[0][(int)width-2].x + upperN[0][(int)width-2].x;
            float sumy = lowerN[0][(int)width-2].y + upperN[0][(int)width-2].y;
            float sumz = lowerN[0][(int)width-2].z + upperN[0][(int)width-2].z;
            float length = sqrt((sumx*sumx) + (sumy*sumy) + (sumz*sumz));
            vertN[0][(int)width-1].x = sumx/length;
            vertN[0][(int)width-1].y = sumy/length;
            vertN[0][(int)width-1].z = sumz/length;
        }
        //bottom right corner
        else if (i == height-1){
            vertN[(int)height-1][(int)width-1].x = lowerN[(int)height-2][(int)width-2].x;
            vertN[(int)height-1][(int)width-1].y = lowerN[(int)height-2][(int)width-2].y;
            vertN[(int)height-1][(int)width-1].z = lowerN[(int)height-2][(int)width-2].z;
        }
        //interior right side
        else{
            float sumx = lowerN[i-1][(int)width-2].x + lowerN[i][(int)width-2].x + upperN[i][(int)width-2].x;
            float sumy = lowerN[i-1][(int)width-2].y + lowerN[i][(int)width-2].y + upperN[i][(int)width-2].y;
            float sumz = lowerN[i-1][(int)width-2].z + lowerN[i][(int)width-2].z + upperN[i][(int)width-2].z;
            float length = sqrt((sumx*sumx) + (sumy*sumy) + (sumz*sumz));
            vertN[i][(int)width-1].x = sumx/length;
            vertN[i][(int)width-1].y = sumy/length;
            vertN[i][(int)width-1].z = sumz/length;
        }
    }
    //interior top row
    for (j = 1; j < width-1; j++){
        float sumx = lowerN[0][j-1].x + upperN[0][j-1].x + upperN[0][j].x;
        float sumy = lowerN[0][j-1].y + upperN[0][j-1].y + upperN[0][j].y;
        float sumz = lowerN[0][j-1].z + upperN[0][j-1].z + upperN[0][j].z;
        float length = sqrt((sumx*sumx) + (sumy*sumy) + (sumz*sumz));
        vertN[0][j].x = sumx/length;
        vertN[0][j].y = sumy/length;
        vertN[0][j].z = sumz/length;
    }
    
    //interior bottom row
    for (j = 1; j < width-1; j++){
        float sumx = lowerN[(int)height-2][j-1].x + upperN[(int)height-2][j].x + lowerN[(int)height-2][j].x;
        float sumy = lowerN[(int)height-2][j-1].y + upperN[(int)height-2][j].y + lowerN[(int)height-2][j].y;
        float sumz = lowerN[(int)height-2][j-1].z + upperN[(int)height-2][j].z + lowerN[(int)height-2][j].z;
        float length = sqrt((sumx*sumx) + (sumy*sumy) + (sumz*sumz));
        vertN[(int)height-1][j].x = sumx/length;
        vertN[(int)height-1][j].y = sumy/length;
        vertN[(int)height-1][j].z = sumz/length;
    }
    
    //interior points
    for (i = 1; i < height - 1; i++){
        for (j = 1; j < width - 1; j++){
            float sumx = lowerN[i-1][j-1].x + upperN[i-1][j].x + lowerN[i-1][j].x + upperN[i][j].x + upperN[i][j-1].x + lowerN[i][j-1].x;
            float sumy = lowerN[i-1][j-1].y + upperN[i-1][j].y + lowerN[i-1][j].y + upperN[i][j].y + upperN[i][j-1].y + lowerN[i][j-1].y;
            float sumz = lowerN[i-1][j-1].z + upperN[i-1][j].z + lowerN[i-1][j].z + upperN[i][j].z + upperN[i][j-1].z + lowerN[i][j-1].z;
            float length = sqrt((sumx*sumx) + (sumy*sumy) + (sumz*sumz));
            vertN[i][j].x = sumx/length;
            vertN[i][j].y = sumy/length;
            vertN[i][j].z = sumz/length;
        }
    }
    
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
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN){
                py = y;
                right_down = 1;
            }
            if (state == GLUT_UP){
                right_down = 0;
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
        px = x;
        py = y;
        glutPostRedisplay();
    }
    if (right_down){
        float ydiff = (py - y)*0.00025;
        scale += ydiff;
        py = y;
        calcSurfaceNormals();
        calcVertexNormals();
        glutPostRedisplay();
    }
}

//This function draws the heightmap using raw triangles, calculates their normals for lighting, and applies their colour.
void draw(){
    GLfloat brown[]  = {0.64, 0.165, 0.165, 1.0};
    GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
    int i,j;
    for (i = 0; i < height-1; i++){
        for (j = 0; j < width-1; j++){
            glBegin(GL_TRIANGLES);
            glMaterialf(GL_FRONT, GL_SHININESS, 30.0);
            glMaterialfv(GL_FRONT, GL_SPECULAR, white);
            
            vec3f v1 = {i*hscale, heightmap[i][j]*scale, j*hscale};
            vec3f v2 = {i*hscale,heightmap[i][j+1]*scale,(j+1)*hscale};
            vec3f v3 = {(i+1)*hscale,heightmap[i+1][j]*scale,j*hscale};
            vec3f v4 = {(i+1)*hscale,heightmap[i+1][j+1]*scale,(j+1)*hscale};
            
            float vColour1[] = {vertC[i][j].x,vertC[i][j].y,vertC[i][j].z,1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, vColour1);
            glNormal3f(vertN[i][j].x,vertN[i][j].y,vertN[i][j].z);
            glVertex3f(v1.x,v1.y,v1.z);
            
            float vColour2[] = {vertC[i][j+1].x,vertC[i][j+1].y,vertC[i][j+1].z,1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, vColour2);
            glNormal3f(vertN[i][j+1].x,vertN[i][j+1].y,vertN[i][j+1].z);
            glVertex3f(v2.x,v2.y,v2.z);
            
            float vColour3[] = {vertC[i+1][j].x,vertC[i+1][j].y,vertC[i+1][j].z,1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, vColour3);
            glNormal3f(vertN[i+1][j].x,vertN[i+1][j].y,vertN[i+1][j].z);
            glVertex3f(v3.x,v3.y,v3.z);
            
            float vColour4[] = {vertC[i+1][j].x,vertC[i+1][j].y,vertC[i+1][j].z,1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, vColour4);
            glNormal3f(vertN[i+1][j].x,vertN[i+1][j].y,vertN[i+1][j].z);
            glVertex3f(v3.x,v3.y,v3.z);
            
            float vColour6[] = {vertC[i][j+1].x,vertC[i][j+1].y,vertC[i][j+1].z,1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, vColour6);
            glNormal3f(vertN[i][j+1].x,vertN[i][j+1].y,vertN[i][j+1].z);
            glVertex3f(v2.x,v2.y,v2.z);
            
            float vColour5[] = {vertC[i+1][j+1].x,vertC[i+1][j+1].y,vertC[i+1][j+1].z,1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, vColour5);
            glNormal3f(vertN[i+1][j+1].x,vertN[i+1][j+1].y,vertN[i+1][j+1].z);
            glVertex3f(v4.x,v4.y,v4.z);
            
            glEnd();
        }
    }
}



/*  Initialize material property and light source.
 */
void init (void)
{
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
   glLightfv (GL_LIGHT0, GL_POSITION, light_position);
   
   glEnable (GL_LIGHTING);
   glEnable (GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

void display (void)
{
GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
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
   
   glPushMatrix ();
   
   float centre_height = height*hscale/2.0;
   float centre_width = width*hscale/2.0;
   float centre_depth = width*hscale/2.0;

   //Calculating the camera position
   cx = centre_height + c_distance*cos(cx_angle)*sin(cy_angle);
   cz = centre_width + c_distance*sin(cx_angle)*sin(cy_angle);
   cy = centre_depth + c_distance*cos(cy_angle);
   //Aiming the camera
   gluLookAt(cx,cy,cz,centre_height,width*scale/2.0,centre_depth,0,1,0);
   //Drawing the object
   draw();
   
   glPopMatrix();
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

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
      case 'q':
         exit(0);
         break;
      case '1':		// Flat shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 0;
         textures = 0;
         init();
         display();
         break;
      case '2':		// Smooth shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 1;
         textures = 0;
         init();
         display();
         break;
      case '3':		// plain grey colour
//          lineDrawing = 0;
//          lighting = 1;
//          smoothShading = 1;
         calcVertexColour(0);
         init();
         display();
         break;
      case '4':		// Uses height based colour
//          lineDrawing = 0;
//          lighting = 1;
//          smoothShading = 1;
         calcVertexColour(1);  
         init();
         display();
         break;
       case '5':		//adds random colours
         randomizeColour();   
         init();
         display();
         break;
   }
}

void loadTexture() {
FILE *fp;
int  i, j;
int  red, green, blue;

   if ((fp = fopen("image.txt", "r")) == 0) {
      printf("Error, failed to find the file named image.txt.\n");
      exit(0);
   } 

   for(i=0; i<64; i++) {
      for(j=0; j<64; j++) {
         fscanf(fp, "%d %d %d", &red, &green, &blue);
         Image[i][j][0] = red;
         Image[i][j][1] = green;
         Image[i][j][2] = blue;
         Image[i][j][3] = 255;
      }
   }

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glGenTextures(1,textureID);
   glBindTexture(GL_TEXTURE_2D, textureID[0]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, Image);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);

   fclose(fp);
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   srand(time(NULL));
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize (1024, 768);
   glutCreateWindow (argv[0]);
   init();
   loadHeightMap(argc,argv);
   calcSurfaceNormals();
   calcVertexNormals();
   calcVertexColour(0);
   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
   glutMouseFunc (mouse);
   glutMotionFunc(up);
   glutMainLoop();
   return 0; 
}

