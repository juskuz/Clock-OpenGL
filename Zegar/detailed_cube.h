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

#ifndef DETAILED_CUBE_H
#define DETAILED_CUBE_H


#include "model.h"

namespace Models {
	class DetailedCube:public Model {
		public:
			DetailedCube();
			virtual ~DetailedCube();
			virtual void drawSolid();
		private:
			void initCube();
			void freeCube();
			void wall();
			void quad(int subdiv,int i1, int i2, float x, float y, float back, float nx, float ny, float s,float t,int pos);
	};
	
	extern DetailedCube detailedCube;
}




#endif 
