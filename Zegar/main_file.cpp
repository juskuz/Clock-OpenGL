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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"

using namespace glm;

float speed=0; //prędkość kątowa obrotu w radianach na sekundę

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed=-PI;
        if (key == GLFW_KEY_RIGHT) speed=PI;
    }

    if (action == GLFW_RELEASE) {
        speed=0;
    }
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************

    glfwSetKeyCallback(window, key_callback); //Zarejestruj procedurę obsługi zdarzeń klawiatury

    glClearColor(0,0,0,1); //Ustaw kolor czyszczenia bufora kolorów na czarno
    glEnable(GL_LIGHTING); //Włącz tryb cieniowania
    glEnable(GL_LIGHT0); //Włącz domyślne zerowe źródło światła
    glEnable(GL_DEPTH_TEST); //Włącz wykorzystanie bufora głębokości do usuwania niewidocznych powierzchni
    glEnable(GL_COLOR_MATERIAL); //Włącz śledzenie kolorów przez materiał
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float angle) {
    //Statyczna deklaracja modeli - żeby nie były tworzone co klatkę

    //static Models::Cube wardrobe();
    static Models::Sphere planet1(0.8,36,36);
    static Models::Sphere moon1(0.1,36,36);
    static Models::Sphere planet2(0.25,36,36);
    static Models::Sphere moon2(0.07,36,36);


	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor kolorów i bufor głębokości

	//Przygotuj macierze rzutowania i widoku dla renderowanego obrazu
	mat4 P=perspective(50.0f*PI/180.0f, 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

	mat4 V=lookAt( //Wylicz macierz widoku
     vec3(0.0f,0.0f,-5.0f),
     vec3(0.0f,0.0f,0.0f),
     vec3(0.0f,1.0f,0.0f));

    glMatrixMode(GL_PROJECTION); //Włącz tryb modyfikacji macierzy rzutowania
    glLoadMatrixf(value_ptr(P)); //Skopiuj macierz rzutowania
    glMatrixMode(GL_MODELVIEW); //Włącz tryb modyfikacji macierzy model-widok. UWAGA! Macierz ta będzie ładowana przed narysowaniem każdego modelu

    //szafa
    mat4 I=mat4(1.0f); //Macierz jednostkowa
    mat4 Ms=I;



    Ms=rotate(Ms,angle,vec3(0,1,0)); // Ms to macierz wyjsciowa, jej nie zmieniamy
    mat4 Ms1=Ms;

    //ŚCIANA TYLNA
    //Mp1=rotate(Mp1,angle,vec3(0,1,0))
    //Ms=translate(Ms,vec3(-0.75,0,0));
    Ms1=scale(Ms1, vec3(0.75,1.7,0.1));
    glLoadMatrixf(value_ptr(V*Ms1));
    //glColor3d(0.2,0.2,0.2); //Kolor szary
    glColor3d(0.2,0.12,0.12); //Kolor szary
    Models::cube.drawSolid();

    //PRAWA ŚCIANA
    Ms1=Ms;
    Ms1=translate(Ms1, vec3 (0.75f,0.0f,-0.4f));
    Ms1=scale(Ms1, vec3(0.05,1.7,0.5));
    glLoadMatrixf(value_ptr(V*Ms1));
    glColor3d(0.2,0.1,0.1); //Kolor szary
    Models::cube.drawSolid();

    //LEWA ŚCIANA
    Ms1=Ms;
    Ms1=translate(Ms1, vec3 (-0.75f,0.0f,-0.4f));
    Ms1=scale(Ms1, vec3(0.05,1.7,0.5));
    glLoadMatrixf(value_ptr(V*Ms1));
    //glColor3d(0.5,0.5,0.5); //Kolor szary
    Models::cube.drawSolid();

    //GÓRNA ŚCIANA
    Ms1=Ms;
    Ms1=translate(Ms1, vec3 (0.0f,1.65f,-0.4f));
    Ms1=scale(Ms1, vec3(0.7,0.05,0.5));
    glLoadMatrixf(value_ptr(V*Ms1));
    //glColor3d(0.5,0.5,0.5); //Kolor szary
    Models::cube.drawSolid();

    //DOLNA ŚCIANA
    Ms1=Ms;
    Ms1=translate(Ms1, vec3 (0.0f,-1.65f,-0.4f));
    Ms1=scale(Ms1, vec3(0.7,0.05,0.5));
    glLoadMatrixf(value_ptr(V*Ms1));
    //glColor3d(0.5,0.5,0.5); //Kolor szary
    Models::cube.drawSolid();

    //PRZEDNIA ŚCIANA
    Ms1=Ms;
    Ms1=translate(Ms1, vec3 (0.0f,1.0f,-0.82f));
    Ms1=scale(Ms1, vec3(0.75,0.7,0.1));
    glLoadMatrixf(value_ptr(V*Ms1));
    //glColor3d(0.95,0.95,0.95); //Kolor szary
    Models::cube.drawSolid();

    //TARCZA
    Ms1=Ms;
    Ms1=translate(Ms1, vec3 (0.0f,1.0f,-0.95f));
    Ms1=scale(Ms1, vec3 (0.75, 0.75, 0.01));
    glLoadMatrixf(value_ptr(V*Ms1)); //Załaduj macierz model-widok
    glColor3d(0.95,0.95,0.95); //Kolor
    planet1.drawSolid();

    //WAHADŁO
    Ms1=Ms;
    Ms1=translate(Ms1, vec3 (0.0f,-1.0f,-0.6f));
    Ms1=scale(Ms1, vec3 (0.3, 0.3, 0.01));
    glLoadMatrixf(value_ptr(V*Ms1)); //Załaduj macierz model-widok
    glColor3d(1.0,1.0,0.0); //Kolor
    planet1.drawSolid();




    /*
    //Słońce
    mat4 Ms=I;    //Słońce nie podlega transformacjom
    glLoadMatrixf(value_ptr(V*Ms)); //Załaduj macierz model-widok
    glColor3d(1,1,0); //Kolor żółty
    sun.drawSolid();

    //Planeta 1
    mat4 Mp1=Ms; //Planeta 1 orbituje dookoła słońca
    Mp1=rotate(Mp1,angle,vec3(0,1,0));//Oś orbity planety 1 (0,1,0)
    Mp1=translate(Mp1,vec3(1.5,0,0));//Promień orbity planety 1 = 1.5
    glLoadMatrixf(value_ptr(V*Mp1)); //Załaduj macierz model-widok
    glColor3d(0,1,0); //Kolor zielony
    planet1.drawSolid();

    //Księżyc 1
    mat4 Mk1=Mp1; //Księżyc 1 orbituje dookoła planety 1
    Mk1=rotate(Mk1,angle,vec3(0,1,0));//Oś orbity księżyca 1 (0,1,0)
    Mk1=translate(Mk1,vec3(0.5,0,0));//Promień orbity księżyca 1 = 0.5
    glLoadMatrixf(value_ptr(V*Mk1)); //Załaduj macierz model-widok
    glColor3d(0.5,0.5,0.5); //Kolor szary
    moon1.drawSolid();

    //Planeta 2
    mat4 Mp2=Ms; //Planeta 2 orbituje dookoła słońca
    Mp2=rotate(Mp2,angle,vec3(0,0,1));//Oś orbity planety 2 (0,0,1)
    Mp2=translate(Mp2,vec3(2,0,0));//Promień orbity planety 2 = 2
    glLoadMatrixf(value_ptr(V*Mp2)); //Załaduj macierz model-widok
    glColor3d(0,0,1); //Kolor niebieski
    planet2.drawSolid();

    //Księżyc 2
    mat4 Mk2=Mp2; //Księżyc 2 orbituje dookoła planety 2
    Mk2=rotate(Mk2,angle,vec3(1,0,0));//Oś orbity księżyca 2 (1,0,0)
    Mk2=translate(Mk2,vec3(0,0.3,0));//Promień orbity księżyca 2 = 0.3,przesunięcie nie według osi X tylko Y bo oś X jest osią obrotu
    glLoadMatrixf(value_ptr(V*Mk2)); //Załaduj macierz model-widok
    glColor3d(0.2,0.2,0.2); //Kolor ciemnoszary
    moon2.drawSolid();
    */

    /*
    Models::Torus t1(0.5,0.2,36,36);
    mat4 Mt1=I;    //Słońce nie podlega transformacjom
    Mt1=rotate(Mt1,angle,vec3(0,0,1));
    glLoadMatrixf(value_ptr(V*Mt1)); //Załaduj macierz model-widok
    glColor3d(0.5,0.5,0.5); //Kolor szary
    t1.drawSolid();

    mat4 Mt3 =Mt1;
    for (int x=0; x<20; x++)
    {
    Mt3=rotate(Mt1,(18*x*PI)/180,vec3(0,0,1));
    Mt3=translate(Mt3,vec3(-0.75,0,0));
    Mt3=scale(Mt3, vec3(0.05,0.05,0.05));
    glLoadMatrixf(value_ptr(V*Mt3));
    glColor3d(0.5,0.5,0.5); //Kolor szary
    Models::cube.drawSolid();
    }


    Models::Torus t2(0.5,0.2,36,36);
    mat4 Mt2=I;    //Słońce nie podlega transformacjom
    Mt2=translate(Mt2,vec3(1.5,0,0));
    Mt2=rotate(Mt2,-(angle+(18*1*PI)/360),vec3(0,0,1));
    glLoadMatrixf(value_ptr(V*Mt2)); //Załaduj macierz model-widok

    glColor3d(0.3,0.3,0.3); //Kolor ciemnoszary
    t2.drawSolid();


    mat4 Mt4 =Mt2;
    for (int x=20; x>0; x--)
    {
    Mt4=rotate(Mt2,(18*x*PI)/180,vec3(0,0,1));
    Mt4=translate(Mt4,vec3(-0.75,0,0));
    Mt4=scale(Mt4, vec3(0.05,0.05,0.05));
    glLoadMatrixf(value_ptr(V*Mt4));
    glColor3d(0.5,0.5,0.5); //Kolor szary
    Models::cube.drawSolid();
    }

    */




	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
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

	float angle=0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Wyzeruj licznik czasu
	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
	    angle+=speed*glfwGetTime(); //Powiększ kąt obrotu o szybkość obrotu razy czas, który minął od poprzedniej klatki
	    glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window,angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
