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

GLuint tex; //Uchwyt – deklaracja globalna
std::vector<unsigned char> image1; //Alokuj wektor do wczytania obrazka 1
std::vector<unsigned char> image2; //Alokuj wektor do wczytania obrazka 2
std::vector<unsigned char> image3; //Alokuj wektor do wczytania obrazka 3
std::vector<unsigned char> image4; //Alokuj wektor do wczytania obrazka 4 (tarcza zegara)
std::vector<unsigned char> image5; //Alokuj wektor do wczytania obrazka 5 (tło)
unsigned width1, height1, width2, height2, width3, height3, width4, height4, width5, height5; //Zmienne do których wczytamy wymiary obrazków

using namespace glm;

float aspect=1.0f; //Aktualny stosunek szerokości do wysokości okna
float speed_x=0; //Szybkość kątowa obrotu obiektu w radianach na sekundę wokół osi x
float speed_y=0; //Szybkość kątowa obrotu obiektu w radianach na sekundę wokół osi y
float speed_pendulum=1;

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
        if (key == GLFW_KEY_UP) speed_x=PI/2;
        if (key == GLFW_KEY_DOWN) speed_x=-PI/2;
    }

    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT) speed_y=0;
        if (key == GLFW_KEY_RIGHT) speed_y=0;
        if (key == GLFW_KEY_UP) speed_x=0;
        if (key == GLFW_KEY_DOWN) speed_x=0;
    }
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
    glfwSetFramebufferSizeCallback(window, windowResize); //Zarejestruj procedurę obsługi zmiany rozdzielczości bufora ramki
    glfwSetKeyCallback(window, key_callback); //Zarejestruj procedurę obsługi klawiatury

	glClearColor(0,0,0,1); //Ustaw kolor czyszczenia ekranu

	//glEnable(GL_LIGHTING); //Włącz tryb cieniowania
	glEnable(GL_LIGHT0); //Włącz zerowe źródło światła
	glEnable(GL_DEPTH_TEST); //Włącz używanie budora głębokości
	glEnable(GL_COLOR_MATERIAL); //Włącz śledzenie kolorów przez materiał

    //Wczytaj obrazki
    unsigned error = lodepng::decode(image1, width1, height1, "img/wood1.png");
    unsigned error2 = lodepng::decode(image2, width2, height2, "img/wood2.png");
    unsigned error3 = lodepng::decode(image3, width3, height3, "img/metal1.png");
    unsigned error4 = lodepng::decode(image4, width4, height4, "img/clock_222.png");
    unsigned error5 = lodepng::decode(image5, width5, height5, "img/gray_bricks.png"); // blue-background.png

    //Import do pamięci karty graficznej
    glGenTextures(1,&tex); //Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt

    //Bilinear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float angle_x,float angle_y, float angle_pendulum) {
	//************Tutaj umieszczaj kod rysujący obraz******************l

    //Statyczna deklaracja modeli - żeby nie były tworzone co klatkę
    static Models::Sphere p1(0.8,36,36); //kula wahadła
    //static Models::Sphere p2(0.8,400,400); //kula wahadła


    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Wyczyść bufor kolorów (czyli przygotuj "płótno" do rysowania)

    //***Przygotowanie do rysowania****
    mat4 P=perspective(50.0f*PI/180.0f,aspect,1.0f,50.0f); //Wylicz macierz rzutowania P
    mat4 V=lookAt( //Wylicz macierz widoku
                  vec3(0.0f,0.0f,-5.0f),
                  vec3(0.0f,0.0f,0.0f),
                  vec3(0.0f,1.0f,0.0f));
    glMatrixMode(GL_PROJECTION); //Włącz tryb modyfikacji macierzy rzutowania
    glLoadMatrixf(value_ptr(P)); //Załaduj macierz rzutowania
    glMatrixMode(GL_MODELVIEW);  //Włącz tryb modyfikacji macierzy model-widok

    //Macierz wyjściowa
    mat4 M=mat4(1.0f);
    M=rotate(M,angle_x,vec3(1.0f,0.0f,0.0f));
    M=rotate(M,angle_y,vec3(0.0f,1.0f,0.0f));

    // TŁO
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width5, height5, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image5.data());
    glEnable(GL_TEXTURE_2D); //włączenie tekstury

    mat4 M_t=M; // macierz tła skopiuj z głównej macierzy
    M_t=scale(M_t, vec3 (20, 20, 20)); // powiększa odpowiednio
    glLoadMatrixf(value_ptr(V*M_t)); //Załaduj macierz model-widok
    //p2.drawSolid(); // narysuj kulę
    Models::cube.drawSolid();
    glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury

    //Macierz ścian
    mat4 Ms;

    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width1, height1, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image1.data());
    glEnable(GL_TEXTURE_2D); //włączenie tekstury

    //Ściana tylna
    Ms=M;
    Ms=scale(Ms, vec3(0.75,1.7,0.1));
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

    glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury

    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width2, height2, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image2.data());
    glEnable(GL_TEXTURE_2D); //włączenie tekstury

    //Ściana przednia
    Ms=M;
    Ms=translate(Ms, vec3 (0.0f,1.0f,-0.82f));
    Ms=scale(Ms, vec3(0.75,0.7,0.1));
    glLoadMatrixf(value_ptr(V*Ms));
    Models::cube.drawSolid();

    glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury

/*
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width3, height3, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image3.data());
    glEnable(GL_TEXTURE_2D); //włączenie tekstury
*/
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width4, height4, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image4.data());
    glEnable(GL_TEXTURE_2D); //włączenie tekstury

    //tarcza_zegara
    //Models::Sphere tarcza_zegara(0.6,36,36);
    mat4 Mt=M;
    Mt=translate(Mt, vec3 (0.0f,1.0f,-0.915f));
    Mt=scale(Mt, vec3 (0.5, 0.5, 0.01));
    glLoadMatrixf(value_ptr(V*Mt));
    //glColor3d(0.95,0.95,0.95); //Kolor biały
    Models::cube.drawSolid();
    //tarcza_zegara.drawSolid();

    glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width3, height3, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image3.data());
    glEnable(GL_TEXTURE_2D); //włączenie tekstury

    //UKŁAD "WAHADŁO-LINKA"
    mat4 Ms2=M; //Macierz wahadła z linką -> układ "WAHADŁO -LINKA"
    Ms2=translate(Ms2, vec3 (0.0f,0.3f,0.0f));
    Ms2=rotate(Ms2,angle_pendulum,vec3(0,0,1)); // tutaj zmieniamy kąt wychylenia wahadła

    // LINKA
    mat4 Mw=Ms2; // pobierz dane z układu "WAHADŁO-LINKA"
    Mw=translate(Mw, vec3 (0.0f,-0.35f,-0.6f)); //umieść linkę odpowiednio w układzie
    Mw=scale(Mw, vec3(0.04,0.7,0.02));
    glLoadMatrixf(value_ptr(V*Mw));
    Models::cube.drawSolid();

    // WAHADŁO
    Mw=Ms2; // macierz kuli wahadła pobierz z układu "WAHADŁO-LINKA"
    Mw=translate(Mw, vec3 (0.0f,-1.25f,-0.6f)); // umieszczamy kulę odpowiednio w układzie "WAHADŁO-LINKA"
    Mw=scale(Mw, vec3 (0.3, 0.3, 0.05));
    glLoadMatrixf(value_ptr(V*Mw)); //Załaduj macierz model-widok
    //glColor3d(1.0,1.0,0.0); //Kolor żółty powoduje masakrę
    p1.drawSolid(); // narysuj kulę

    glDisable(GL_TEXTURE_2D); //Wyłączenie tekstury

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


	float angle_x=0.0f; //Aktualny kąt obrotu obiektu wokół osi x
	float angle_y=0.0f; //Aktualny kąt obrotu obiektu wokół osi y
	float angle_pendulum=0;
	int left=1;
	glfwSetTime(0); //Wyzeruj timer

	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
	    angle_x+=speed_x*glfwGetTime(); //Oblicz przyrost kąta obrotu i zwiększ aktualny kąt
        angle_y+=speed_y*glfwGetTime(); //Oblicz przyrost kąta obrotu i zwiększ aktualny kąt
        if(angle_pendulum>=0.3) {left=-1;}
        else if (angle_pendulum<=-0.3) {left=1;}
        angle_pendulum=angle_pendulum+left*speed_pendulum*0.8*glfwGetTime(); //left decyduje czy prawo czy lewo
	    glfwSetTime(0); //Wyzeruj timer
		drawScene(window,angle_x,angle_y, angle_pendulum); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	//Usunięcie tekstury z pamięci karty graficznej – po głownej pętli
    glDeleteTextures(1,&tex);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
