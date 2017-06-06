/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <lodepng.h>
#include "constants.h"
#include "allmodels.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace glm;

GLuint tex0; //Uchwyt – tekstura 0
GLuint tex1; //Uchwyt – tekstura 1
GLuint tex2; //Uchwyt – tekstura 2
GLuint tex3; //Uchwyt – tekstura 3
GLuint tex4; //Uchwyt – tekstura 4
GLuint model3D; //Uchwyt - model 3D

vector<float> objV;
vector<float> objN;
vector<float> objT;

vec3 pos=vec3(0,0,-5);
float obr=PI/2;

float aspect=1.0f; //Aktualny stosunek szerokości do wysokości okna
float speed_x=0; //Szybkość kątowa obrotu zegara w radianach na sekundę wokół osi x
float speed_y=0; //Szybkość kątowa obrotu zegara w radianach na sekundę wokół osi y
float speed_pendulum=0.5; //Szybkość kątowa obrotu wahadła w radianach na sekundę wokół osi x
float speed_sprocket=1; //Szybkość kątowa obrotu koła zębatego w radianach na sekundę wokół osi z
float speed_door=0; //Szybkość kątowa obrotu koła zębatego w radianach na sekundę

/*
vector<unsigned char> image1; //Alokuj wektor do wczytania obrazka 1 (tło)
vector<unsigned char> image2; //Alokuj wektor do wczytania obrazka 2 (drewniane elementy 1)
vector<unsigned char> image3; //Alokuj wektor do wczytania obrazka 3 (drewniane elementy 2)
vector<unsigned char> image4; //Alokuj wektor do wczytania obrazka 4 (tarcza zegara)
vector<unsigned char> image5; //Alokuj wektor do wczytania obrazka 5 (metalowe elementy)

unsigned width1, height1, width2, height2, width3, height3, width4, height4, width5, height5; //Zmienne do których wczytamy wymiary obrazków
*/

vec3 dir(float obr,float speed) {
    return speed*vec3(cos(obr),0,sin(obr));
}

void split(string &in, vector<string> &out,char sep=' ') {
    out.clear();
    std::istringstream iss(in);
    std::string sub;

    while(std::getline(iss, sub, sep)) {
        out.push_back(sub);
    }
}

void processVertex(vector<string> &in,vector<vec3> &out,vec3 &total) {

    out.push_back(vec3(atof(in[1].c_str()),
                       atof(in[2].c_str()),
                       atof(in[3].c_str())
                       ));
    total=total+out.back();
}

void processNormal(vector<string> &in,vector<vec3> &out) {
    vec3 tmp;
    processVertex(in,out,tmp);
}

void processTexCoord(vector<string> &in,vector<vec2> &out) {
    out.push_back(vec2(atof(in[1].c_str()),
                       atof(in[2].c_str())
                       ));
}

void decompressVertex(string vertexStr,
                      vector<vec3> &vertices,vector<vec3> &normals, vector<vec2> &texCoords,
                      vector<float> &outV, vector<float> &outN, vector<float> &outT) {

    vector<string> tmp;
    split(vertexStr,tmp,'/');

    outV.push_back(vertices[atoi(tmp[0].c_str())-1].x);
    outV.push_back(vertices[atoi(tmp[0].c_str())-1].y);
    outV.push_back(vertices[atoi(tmp[0].c_str())-1].z);

    outT.push_back(texCoords[atoi(tmp[1].c_str())-1].x);
    outT.push_back(texCoords[atoi(tmp[1].c_str())-1].y);

    outN.push_back(normals[atoi(tmp[2].c_str())-1].x);
    outN.push_back(normals[atoi(tmp[2].c_str())-1].y);
    outN.push_back(normals[atoi(tmp[2].c_str())-1].z);

}

vec3 getPos(string vertexStr,vector<vec3> &vertices) {

    vector<string> tmp;
    split(vertexStr,tmp,'/');
    return vertices[atoi(tmp[0].c_str())-1];

}

void processFace(vector<string> &in,
                 vector<vec3> &vertices,vector<vec3> &normals, vector<vec2> &texCoords,
                 vector<float> &outV, vector<float> &outN, vector<float> &outT) {

    decompressVertex(in[1],vertices,normals,texCoords,outV,outN,outT);
    decompressVertex(in[2],vertices,normals,texCoords,outV,outN,outT);
    decompressVertex(in[3],vertices,normals,texCoords,outV,outN,outT);

    decompressVertex(in[1],vertices,normals,texCoords,outV,outN,outT);
    decompressVertex(in[3],vertices,normals,texCoords,outV,outN,outT);
    decompressVertex(in[4],vertices,normals,texCoords,outV,outN,outT);
}

void processObj(string filename,vector<float> &outV, vector<float> &outN, vector<float> &outT) {
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<vec2> texCoords;



    ifstream inFile;
    inFile.open(filename.c_str());

    string line;
    vector<string> tmp;
    vec3 total=vec3(0,0,0);

    while (getline(inFile,line)) {
            split(line,tmp);
            if (tmp.size()>0) {
                if (tmp[0]=="v") {
                    processVertex(tmp,vertices,total);
                    //printf("wierzcholek %f,%f,%f\n",vertices.back().x,vertices.back().y,vertices.back().z);
                } else if (tmp[0]=="vn") {
                    processNormal(tmp,normals);
                    //printf("normalna %f,%f,%f\n",normals.back().x,normals.back().y,normals.back().z);
                } else if (tmp[0]=="vt") {
                    processTexCoord(tmp,texCoords);
                    //printf("wsp. tex. %f,%f\n",texCoords.back().x,texCoords.back().y);
                }
            }
    }

    inFile.close();

    total=total/(float)vertices.size();

    for (int i=0;i<vertices.size();i++) {
        vertices[i]=vertices[i]-total;
    }

    inFile.open(filename.c_str());

    while (getline(inFile,line)) {
            split(line,tmp);
            if (tmp.size()>0) {
                if (tmp[0]=="f") {
                    processFace(tmp,vertices,normals,texCoords,outV,outN,outT);
                }
            }
    }

    inFile.close();

    //printf("%d %d %d\n",vertices.size(),normals.size(),texCoords.size());

}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Procedura obługi zmiany rozmiaru bufora ramki
void windowResize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); //Obraz ma być generowany w oknie o tej rozdzielczości
    aspect=(float)width/(float)height; //Stosunek szerokości do wysokości okna
}

//Procedura obsługi klawiatury
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed_y=PI/2;
        if (key == GLFW_KEY_RIGHT) speed_y=-PI/2;
        if (key == GLFW_KEY_UP) speed_x=-PI/2;
        if (key == GLFW_KEY_DOWN) speed_x=PI/2;
        if (key==GLFW_KEY_W) pos=pos+dir(obr,0.2);
        if (key==GLFW_KEY_S) pos=pos-dir(obr,0.2);
        if (key==GLFW_KEY_A) obr=obr-PI/36;
        if (key==GLFW_KEY_D) obr=obr+PI/36;
        if (key==GLFW_KEY_Z&&speed_door<=PI/2) speed_door=speed_door+PI/36;
        if (key==GLFW_KEY_X&&speed_door>0) speed_door=speed_door-PI/36;
    }

    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT) speed_y=0;
        if (key == GLFW_KEY_RIGHT) speed_y=0;
        if (key == GLFW_KEY_UP) speed_x=0;
        if (key == GLFW_KEY_DOWN) speed_x=0;
    }
}

//procedura ładowania modelu w formacie OBJ // do usunięcia
void loadObj(char *fname) {
    FILE *fp;
    int read;
    GLfloat x, y, z;
    char ch;
    model3D=glGenLists(1);
    fp=fopen(fname,"r");
    if (!fp) {
        printf("can't open file %s\n", fname);
        exit(1);GLuint tex0; //Uchwyt – tekstura
    }
    glPointSize(2.0);
    glNewList(model3D, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_POINTS);
        while(!(feof(fp))) {
            read=fscanf(fp,"%c %f %f %f",&ch,&x,&y,&z);
            if(read==4&&ch=='v') {
                glVertex3f(x,y,z);
            }
        }
        glEnd();
    }
    glPopMatrix();
    glEndList();
    fclose(fp);
}

GLuint readTexture(char* nazwa) {
    vector<unsigned char> image;
    unsigned error;
    unsigned int width; // szerokość tekstury
    unsigned int height; // wysokość tekstury
    error = lodepng::decode(image, width, height, nazwa);

    GLuint tex; // uchwyt

    //Import do pamięci karty graficznej
    glGenTextures(1,&tex); //Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt

    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

    //Bilinear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
    glfwSetFramebufferSizeCallback(window, windowResize); //Zarejestruj procedurę obsługi zmiany rozdzielczości bufora ramki
    glfwSetKeyCallback(window, key_callback); //Zarejestruj procedurę obsługi klawiatury

	glClearColor(0,0,0,1); //Ustaw kolor czyszczenia ekranu

	glEnable(GL_LIGHTING); //Włącz tryb cieniowania
	glEnable(GL_LIGHT0); //Włącz zerowe źródło światła
	//glEnable(GL_LIGHT1); // Włącz pierwsze źródło światła

	glEnable(GL_DEPTH_TEST); //Włącz używanie budora głębokości
	glEnable(GL_COLOR_MATERIAL); //Włącz śledzenie kolorów przez materiał // do czego to jest?
	// poniżej przeklejone z innego projektu (włączenie/wyłączenie nic nie zmienia)
	glEnable(GL_TEXTURE_2D); //Włącz teksturowanie
    glEnable(GL_NORMALIZE); //Włącz automatyczną normalizację wektorów normalnych


    //wczytywanie modelu 3D
    //loadObj("obj/sprocket3.obj");

    //wczytywanie tekstur
    tex0=readTexture( "img/gray_bricks.png");
    tex1=readTexture( "img/wood_1.png");
    tex2=readTexture( "img/wood_2.png");
    tex3=readTexture( "img/clock_face.png");
    tex4=readTexture( "img/metal.png");

}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float angle_x,float angle_y, float angle_pendulum, float angle_clock, float angle_sprocket, float angle_door) {
	//************Tutaj umieszczaj kod rysujący obraz******************l

    //Statyczna deklaracja modeli - żeby nie były tworzone co klatkę
    static Models::Sphere p1(0.8,36,36); //kula wahadła

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Wyczyść bufor kolorów (czyli przygotuj "płótno" do rysowania)

    //***Przygotowanie do rysowania****
    mat4 P=perspective(50.0f*PI/180.0f,aspect,1.0f,50.0f); //Wylicz macierz rzutowania P
    mat4 V=lookAt( //Wylicz macierz widoku
                  pos,
                  pos+dir(obr,1),
                  vec3(0.0f,1.0f,0.0f));
    glMatrixMode(GL_PROJECTION); //Włącz tryb modyfikacji macierzy rzutowania
    glLoadMatrixf(value_ptr(P)); //Załaduj macierz rzutowania
    glMatrixMode(GL_MODELVIEW);  //Włącz tryb modyfikacji macierzy model-widok

    //Macierz wyjściowa
    mat4 M=mat4(1.0f);
    M=rotate(M,angle_x,vec3(1.0f,0.0f,0.0f));
    M=rotate(M,angle_y,vec3(0.0f,1.0f,0.0f));

    //TŁO
    glBindTexture(GL_TEXTURE_2D,tex0);
    glEnable(GL_TEXTURE_2D); //włączenie tekstury
        mat4 M_t=M; // macierz tła skopiuj z głównej macierzy
        M_t=scale(M_t, vec3 (20, 20, 20)); // powiększa odpowiednio
        glLoadMatrixf(value_ptr(V*M_t)); //Załaduj macierz model-widok
        Models::cube.drawSolid();
    //glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury

    //ŚCIANY
    mat4 Ms; //Macierz ścian
    //glEnable(GL_TEXTURE_2D); //włączenie tekstury
    glBindTexture(GL_TEXTURE_2D,tex1); // połącz odpowiednią teksturę
        //Ściana tylna
        Ms=M;
        Ms=scale(Ms, vec3(0.7,1.6,0.1));
        glLoadMatrixf(value_ptr(V*Ms));
        Models::cube.drawSolid();

        //Ściana prawa
        Ms=M;
        Ms=translate(Ms, vec3 (0.75f,0.0f,-0.4f));
        Ms=scale(Ms, vec3(0.05,1.7,0.5));
        glLoadMatrixf(value_ptr(V*Ms));
        Models::cube.drawSolid();

        //Ściana lewa
        Ms=M;
        Ms=translate(Ms, vec3 (-0.75f,0.0f,-0.4f));
        Ms=scale(Ms, vec3(0.05,1.7,0.5));
        glLoadMatrixf(value_ptr(V*Ms));
        Models::cube.drawSolid();

        //Ściana górna
        Ms=M;
        Ms=translate(Ms, vec3 (0.0f,1.65f,-0.4f));
        Ms=scale(Ms, vec3(0.7,0.05,0.5));
        glLoadMatrixf(value_ptr(V*Ms));
        Models::cube.drawSolid();

        //Ściana dolna
        Ms=M;
        Ms=translate(Ms, vec3 (0.0f,-1.65f,-0.4f));
        Ms=scale(Ms, vec3(0.7,0.05,0.5));
        glLoadMatrixf(value_ptr(V*Ms));
        Models::cube.drawSolid();
    //glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury  // zbędne

    //FRONT ZEGARA
    glBindTexture(GL_TEXTURE_2D,tex2); // przełącz na odpowiednią teksturę
    //glEnable(GL_TEXTURE_2D); //włączenie tekstury  // zbędne
        //Ściana przednia górna
        Ms=M;
        Ms=translate(Ms, vec3 (0.0f,0.95f,-0.82f));
        Ms=scale(Ms, vec3(0.7,0.65,0.1));
        glLoadMatrixf(value_ptr(V*Ms));
        Models::cube.drawSolid();

glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury  // tu jest konieczne dla szyby!

//Włączenie łączenia kolorów:
glEnable(GL_BLEND);
//Ustawienie bufora głębokości w tryb odczytu:
glDepthMask(GL_FALSE);
//Skonfigurowanie sposobu łączenia kolorów:
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//Określenie koloru obiektu przezroczystego:
glColor4d(0.5,0.5,0.75,0.15);

        //Ściana przednia dolna (otwierana)
        Ms=M;
        //Ms=translate(Ms, vec3 (0.0f,-0.65f,-0.90f));
        Ms=translate(Ms, vec3 (0.7f,-0.65f,-0.90f));
        Ms=rotate(Ms, speed_door, vec3(0.0f, -1.0f, 0.0f));
        mat4 Ms4=Ms;
        Ms4=translate(Ms4, vec3 (-0.7f,-0.0f,0.0f));
        Ms4=scale(Ms4, vec3(0.7,0.95,0.01));
        glLoadMatrixf(value_ptr(V*Ms4));
        //glColor4d(1.0f,1.0f,0.0f, 0.01f); //Ustaw kolor rysowania
        Models::cube.drawSolid();
        glColor4d(1.0f,1.0f,1.0f, 1.0f); //Ustaw kolor rysowania

//Przywrócenie bufora głębokości w pełny tryb zapisu/odczytu:
glDepthMask(GL_TRUE);
//Wyłączenie łączenia kolorów:
glDisable(GL_BLEND);


    //TARCZA ZEGARA
    glBindTexture(GL_TEXTURE_2D,tex3);
    glEnable(GL_TEXTURE_2D); //włączenie tekstury // musi zostać
        //tarcza_zegara
        mat4 Mt=M;
        Mt=translate(Mt, vec3 (0.0f,1.0f,-0.912f));
        Mt=scale(Mt, vec3 (0.5, 0.5, 0.01));
        glLoadMatrixf(value_ptr(V*Mt));
        Models::cube.drawSolid();
    //glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury  // zbędne

    //WSKAZÓWKI I WAHADŁO
    glBindTexture(GL_TEXTURE_2D,tex4); // przełącz teksturę
    //glEnable(GL_TEXTURE_2D); //włączenie tekstury  // zbędne
        //wskazówka minutowa
        Mt=M;
        Mt=translate(Mt, vec3 (-0.0f,1.0f,-0.918f));
        Mt=rotate(Mt,angle_clock,vec3(0,0,1)); // tutaj zmieniamy kąt wychylenia wahadła

        mat4 Mt2=Mt;
        Mt2=translate(Mt2, vec3 (0.0f,0.2f,0.0f));
        Mt2=scale(Mt2, vec3(0.01,0.18,0.01));
        glLoadMatrixf(value_ptr(V*Mt2));
        Models::cube.drawSolid();

        //wskazówka godzinowa
        Mt=M;
        Mt=translate(Mt, vec3 (-0.0f,1.0f,-0.918f));
        Mt=rotate(Mt,angle_clock/12,vec3(0,0,1)); // tutaj zmieniamy kąt wychylenia wahadła

        Mt2=Mt;
        Mt2=translate(Mt2, vec3 (-0.1f,0.0f,0.0f));
        Mt2=scale(Mt2, vec3(0.10,0.01,0.01));
        glLoadMatrixf(value_ptr(V*Mt2));
        Models::cube.drawSolid();

        //układ "wahadło-linka"
        mat4 Ms2=M; //Macierz wahadła z linką -> układ "wahadło-linka"
        Ms2=translate(Ms2, vec3 (0.0f,0.3f,0.0f));
        Ms2=rotate(Ms2,angle_pendulum,vec3(0,0,1)); // tutaj zmieniamy kąt wychylenia wahadła

        //linka
        mat4 Mw=Ms2; // pobierz dane z układu "wahadło-linka"
        Mw=translate(Mw, vec3 (0.0f,-0.35f,-0.6f)); //umieść linkę odpowiednio w układzie
        Mw=scale(Mw, vec3(0.04,0.7,0.02));
        glLoadMatrixf(value_ptr(V*Mw));
        Models::cube.drawSolid();

        //wahadło
        Mw=Ms2; // macierz kuli wahadła pobierz z układu "wahadło-linka"
        Mw=translate(Mw, vec3 (0.0f,-1.25f,-0.6f)); // umieszczamy kulę odpowiednio w układzie "WAHADŁO-LINKA"
        Mw=scale(Mw, vec3 (0.3, 0.3, 0.05));
        glLoadMatrixf(value_ptr(V*Mw)); //Załaduj macierz model-widok
        p1.drawSolid(); // narysuj kulę

    // NOWY MODEL KÓŁ ZĘBATYCH
    //glBindTexture(GL_TEXTURE_2D,tex4);

    //1. Wyliczenie i załadowanie macierzy modelu
    mat4 Mz=M; // załadowanie macierzy głównej
    Mz=translate(Mz, vec3(0.0,-0.7,-0.3)); // umieszczamy zębatki w odpowiednim miejscu
    Mz=rotate(Mz, PI/2, vec3(1.0f, 0.0f, 0.0f)); // ustawiamy obroty zębatki
    Mz=rotate(Mz, angle_sprocket, vec3(0.0f, 1.0f, 0.0f)); // ustawiamy obroty zębatki
    Mz=scale(Mz, vec3(0.025f,0.05f,0.025f));
    glLoadMatrixf(value_ptr(V*Mz));

    //2. Rysowanie modelu
    glEnableClientState(GL_VERTEX_ARRAY); //Podczas rysowania używaj tablicy wierzchołków
    glEnableClientState(GL_TEXTURE_COORD_ARRAY); //Podczas rysowania używaj tablicy wsp. teksturowania
    glEnableClientState(GL_NORMAL_ARRAY); //Podczas rysowania używaj tablicy wektorów normalnych

    glVertexPointer(3,GL_FLOAT,0,objV.data()); //Ustaw tablicę myCubeVertices jako tablicę wierzchołków
    glTexCoordPointer(2,GL_FLOAT,0,objT.data()); //Ustaw tablicę myCubeTexCoords jako tablicę wsp. teksturowania
    //glNormalPointer(GL_FLOAT,0,myCubeNormals); //Ustaw tablicę myCubeNormals jako tablicę wektorów normalnych
    glNormalPointer(GL_FLOAT,0,objN.data()); //Ustaw tablicę myCubeVertices jako tablicę wektorów normalnych - tutaj akurat wsp. wierzchołka=suma wektorów normalnych ścian sąsiadujących

    glDrawArrays(GL_TRIANGLES,0,objV.size()/3); //Rysuj model

    // DRUGA ZĘBATKA
    Mz=M; // powróć do macierzy głównej
    Mz=translate(Mz, vec3(-0.35,-0.23,-0.3)); // umieszczamy zębatkę 2 w odpowiednim miejscu
    Mz=rotate(Mz, PI/2, vec3(1.0f, 0.003f, 0.0f)); // ustawiamy obroty zębatki
    Mz=rotate(Mz, -angle_sprocket, vec3(0.0f, 1.0f, 0.0f)); // ustawiamy obroty zębatki
    Mz=scale(Mz, vec3(0.025f,0.05f,0.025f));
    glLoadMatrixf(value_ptr(V*Mz));

    glDrawArrays(GL_TRIANGLES,0,objV.size()/3); //Rysuj model

    //Posprzątaj po sobie
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury


    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void)
{
    processObj("Gear4.obj",objV,objN,objT);

	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "Clock", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) { //Jeżeli okna nie udało się utworzyć, to zamknij program
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	float angle_x=0.0f; //Aktualny kąt obrotu zegara wokół osi x
	float angle_y=0.0f; //Aktualny kąt obrotu zegara wokół osi y
	float angle_pendulum=0.0f; //Aktualny kąt obrotu wahadła wokół osi x
	float angle_clock=0.0f; //Aktualny kąt obrotu wskazówek zegara wokół osi z
	float angle_sprocket=0.0f; //Aktualny kąt kół zębatych wokół osi z
	float angle_door=0.0f; //Aktualny kąt obrotu drzwi
	int left=1;
	glfwSetTime(0); //Wyzeruj timer

	//Główna pętla
	while (!glfwWindowShouldClose(window)) { //Tak długo jak okno nie powinno zostać zamknięte
	    angle_x+=speed_x*glfwGetTime(); //Oblicz przyrost kąta obrotu i zwiększ aktualny kąt
        angle_y+=speed_y*glfwGetTime(); //Oblicz przyrost kąta obrotu i zwiększ aktualny kąt

        angle_clock+=speed_pendulum*0.1707*glfwGetTime();
        if (angle_pendulum>=0.3) left=-1;
        else if (angle_pendulum<=-0.3) left=1;
        angle_pendulum=(angle_pendulum)+left*speed_pendulum*glfwGetTime(); //left decyduje czy prawo czy lewo

        angle_sprocket+=speed_sprocket*glfwGetTime();

	    glfwSetTime(0); //Wyzeruj timer

		drawScene(window,angle_x,angle_y, angle_pendulum, angle_clock, angle_sprocket, angle_door); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	//Usunięcie tekstury z pamięci karty graficznej – po głownej pętli
    glDeleteTextures(1,&tex0);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
