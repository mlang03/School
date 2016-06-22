
all: a4

TARGET = -mmacosx-version-min=10.8
LIBS = -F/System/Library/Frameworks -framework OpenGL -framework GLUT

a4: a4.c
	clang a4.c -o a4 $(LIBS) $(TARGET)

scene: scene.c
	clang scene.c -o scene $(LIBS) $(TARGET)

