/* Ray tracing program by: Michael Lang
 * for cis4800
 * April 3rd 2016
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#define MAX_SIZE 100

int width = 1024;
int height = 768;
GLfloat scalex = 0.013;//0.0097;
GLfloat scaley = 0.013;

typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} vec3f;

typedef struct {
    vec3f pos;
    vec3f colour;
    GLfloat radius;
} sphere;

sphere spList[MAX_SIZE];
int numSpheres;
int snooker = 0;

vec3f l1 = {0,2,-2};
vec3f lc = {1,1,1};

//clamps a float between two values
GLfloat clamp (GLfloat val, GLfloat min, GLfloat max){
    val = val >= max ? max : val;
    val = val <= min ? min : val;
    return val;
}

//returns the dot product of two vectors
GLfloat dotProduct (vec3f a, vec3f b){
    return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z));
}

//normalizes a vector
vec3f normalize(vec3f v){
    vec3f ret;
    GLfloat length = sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
    ret.x = v.x / length;
    ret.y = v.y / length;
    ret.z = v.z / length;
    return ret;
}

//check if a point is in shadow
int checkIfInShadow(vec3f rayOrigin, vec3f rayDir, int k){
    //printf("After: %f %f %f\n",rayDir.x,rayDir.y,rayDir.z);
    int i;
    for (i = 0; i < numSpheres; i++){
        if (i == k){
            continue;
        }
        sphere s = spList[i];
        vec3f sp = s.pos;
        GLfloat sr = s.radius;
        //printf("%f %f\n",sp.x,s.pos.x);
        double A = rayDir.x*rayDir.x + rayDir.y*rayDir.y + rayDir.z*rayDir.z;
        
        double B = 2 * (rayDir.x*(rayOrigin.x - sp.x) +
                        rayDir.y*(rayOrigin.y - sp.y) +
                        rayDir.z*(rayOrigin.z - sp.z));
        
        double C = pow((rayOrigin.x - sp.x),2) +
                    pow((rayOrigin.y - sp.y),2) +
                    pow((rayOrigin.z - sp.z),2) - pow(sr,2);
                    
        double disc = pow(B,2) + (-4)*A*C;
        
        
        if (disc > 0){
            GLfloat t0 = (-B - sqrt(disc))/2.0f;
            GLfloat t1 = (-B + sqrt(disc))/2.0f;
            
            if (t0 < 0){
                return 0;
            }
            //t0 = -t0;
            
            GLfloat t = t0 > 0 ? t0 : t1;
            
            vec3f intersect;
            intersect.x = rayOrigin.x + rayDir.x*t;
            intersect.y = rayOrigin.y + rayDir.y*t;
            intersect.z = rayOrigin.z + rayDir.z*t;
            
            return 1;
        }
    }
    return 0;
}

//calculates a pixels colour, pixel is return in pixel variable
GLfloat calcPixelColour(vec3f * pixel,GLfloat pz, GLfloat py){
    vec3f rayOrigin = {-10,0,0};
    vec3f plane = {2,py*scaley-(height*scaley/2),pz*scalex-(width*scalex/2)};
    vec3f rayDir;
    rayDir.x = plane.x - rayOrigin.x;
    rayDir.y = plane.y - rayOrigin.y;
    rayDir.z = plane.z - rayOrigin.z;
    
    rayDir = normalize(rayDir);
    
    int i;
    GLfloat p = 100000;
    int spN = -1;
    //loop through each sphere to find the closest collision
    for (i = 0; i < numSpheres; i++){
        sphere s = spList[i];
        vec3f sp = s.pos;
        GLfloat sr = s.radius;
        GLfloat A = rayDir.x*rayDir.x + rayDir.y*rayDir.y + rayDir.z*rayDir.z;
        
        GLfloat B = 2 * (rayDir.x*(rayOrigin.x - sp.x) +
                        rayDir.y*(rayOrigin.y - sp.y) +
                        rayDir.z*(rayOrigin.z - sp.z));
        
        GLfloat C = (rayOrigin.x - sp.x)*(rayOrigin.x - sp.x) +
                    (rayOrigin.y - sp.y)*(rayOrigin.y - sp.y) +
                    (rayOrigin.z - sp.z)*(rayOrigin.z - sp.z) - sr*sr;
                    
        //printf("%f %f\n", B,C);
                    
        GLfloat disc = (B*B) - (4*C);
        
        
        
        if (disc >= 0){
            GLfloat t0 = (-B - sqrt(disc))/2.0f;
            GLfloat t1 = (-B + sqrt(disc))/2.0f;
            
            GLfloat t = t0 > 0 ? t0 : t1;
            
            if (t < p){
                p = t;
                spN = i;
            }
        }
    }
    
    if (spN == -1){
        return 0;
    }
    
    sphere s = spList[spN];
    vec3f sp = s.pos;
    GLfloat sr = s.radius;
            
    vec3f intersect;
    intersect.x = rayOrigin.x + rayDir.x*p;
    intersect.y = rayOrigin.y + rayDir.y*p;
    intersect.z = rayOrigin.z + rayDir.z*p;
    
    vec3f intNorm;
    intNorm.x = (intersect.x - sp.x) / sr;
    intNorm.y = (intersect.y - sp.y) / sr;
    intNorm.z = (intersect.z - sp.z) / sr;
    
    vec3f lightdir;
    lightdir.x = l1.x - intersect.x;
    lightdir.y = l1.y - intersect.y;
    lightdir.z = l1.z - intersect.z;
    
    lightdir = normalize(lightdir);
    
    int shad = checkIfInShadow(intersect,lightdir,spN);
    
    GLfloat a = dotProduct(intNorm,lightdir);
    
    vec3f reflect;
    reflect.x = 2*a*intNorm.x - lightdir.x;
    reflect.y = 2*a*intNorm.y - lightdir.y;
    reflect.z = 2*a*intNorm.z - lightdir.z;
    
    reflect = normalize(reflect);
    
    vec3f VP;
    VP.x = rayOrigin.x - intersect.x;
    VP.y = rayOrigin.y - intersect.y;
    VP.z = rayOrigin.z - intersect.z;
    
    VP = normalize(VP);
    
    
    int n = 64;
    GLfloat cosN = pow(clamp(dotProduct(reflect,VP),0,3.14),n);
    cosN = clamp(cosN,0,3.14);
    
    vec3f diffColour;
    vec3f ambColour;
    vec3f specColour;
    GLfloat Id = 0.9;
    GLfloat Ia = 0.1;
    GLfloat Is = 1;
    GLfloat NdotV = dotProduct(intNorm, lightdir);
    
    diffColour.x = clamp(Id*s.colour.x*NdotV,0,1);
    diffColour.y = clamp(Id*s.colour.y*NdotV,0,1);
    diffColour.z = clamp(Id*s.colour.z*NdotV,0,1);
    
    ambColour.x = clamp(Ia*s.colour.x,0,1);
    ambColour.y = clamp(Ia*s.colour.y,0,1);
    ambColour.z = clamp(Ia*s.colour.z,0,1);
    
    specColour.x = clamp(Is*lc.x*cosN,0,1);
    specColour.y = clamp(Is*lc.y*cosN,0,1);
    specColour.z = clamp(Is*lc.z*cosN,0,1);
    
    //if not in shadow use ambient + diffuse + specular
    if (shad == 0){
        (*pixel).x = clamp(ambColour.x + diffColour.x + specColour.x,0,1);
        (*pixel).y = clamp(ambColour.y + diffColour.y + specColour.y,0,1);
        (*pixel).z = clamp(ambColour.z + diffColour.z + specColour.z,0,1);
    }
    //else use only ambient
    else if (shad == 1){
        (*pixel).x = clamp(ambColour.x,0,1);
        (*pixel).y = clamp(ambColour.y,0,1);
        (*pixel).z = clamp(ambColour.z,0,1);
    }
    return 1;
}

void display(void){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    int size = width*height*3;
    GLfloat * b = malloc(sizeof(GLfloat)*size);
    int i,j;
    for (i = 0; i < size; i = i+3){
        b[i] = 0;
        b[i+1] = snooker == 1 ? 0.6 : 0;
        b[i+2] = 0;
    }
    int p = 0;
    for (i = 0; i < height; i++){
        for (j = 0; j < width; j++){
            vec3f pixel;
            if (calcPixelColour(&pixel,j,i) == 1){;
                b[p] = pixel.x;
                b[p+1] = pixel.y;
                b[p+2] = pixel.z;
            }
            p = p + 3;
        }
    }
    glDrawPixels(width,height,GL_RGB,GL_FLOAT,b);
    free(b);
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 'q':
            exit(0);
            break;
    }
}

void reshape(int w, int h){
    height = h;
    width = w;
    float ratio = (float)w/(float)h;
    scaley = 10/(float) height;
    scalex = 10*ratio/(float) width;
}


void readFile(char * fName){
    FILE * inFile = fopen(fName,"r");
    char buffer[1000] = {0};
    numSpheres = 0;
    while (fgets(buffer,1000, inFile) != NULL){
        char type[100] = {0};
        sscanf(buffer,"%s",type);

        GLfloat posx,posy,posz,colx,coly,colz,r;
        if (strcmp(type,"sphere") == 0){
            sscanf(buffer,"%*s %f %f %f %f %f %f %f",&posx,&posy,&posz,&r,&colx,&coly,&colz);
            spList[numSpheres].pos.x = posx;
            spList[numSpheres].pos.y = posy;
            spList[numSpheres].pos.z = posz;
            spList[numSpheres].colour.x = colx;
            spList[numSpheres].colour.y = coly;
            spList[numSpheres].colour.z = colz;
            spList[numSpheres].radius = r;
            numSpheres++;
        }
        else if (strcmp(type,"light") == 0){
            sscanf(buffer,"%*s %f %f %f %f %f %f",&posx,&posy,&posz,&colx,&coly,&colz);
            l1.x = posx;
            l1.y = posy;
            l1.z = posz;
            lc.x = colx;
            lc.y = coly;
            lc.z = colz;
        }
    }
    
}


int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (width, height);
    glutCreateWindow (argv[0]);
    glutDisplayFunc(display);
    glutKeyboardFunc (keyboard);
    glutReshapeFunc(reshape);
    glEnable(GL_DEPTH_TEST);
    if (strcmp(argv[1],"snooker.txt") == 0)
        snooker = 1;
    readFile(argv[1]);
    
    glutMainLoop();
}