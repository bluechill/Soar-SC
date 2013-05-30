/* This file is part of the Zenipex Library (zenilib).
 * Copyleft (C) 2011 Mitchell Keith Bloch (bazald).
 *
 * This source file is simply under the public domain.
 */

#include <zenilib.h>

#include "SVSViewerState.h"

#if defined(_DEBUG) && defined(_WINDOWS)
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

using namespace std;
using namespace Zeni;

class Bootstrap {
	class Gamestate_One_Initializer : public Gamestate_Zero_Initializer {
		virtual Gamestate_Base * operator()() {
			Window::set_title("Soar SVS Viewer - Windows");
				
			get_Joysticks();
			get_Video();
			get_Textures();
			get_Fonts();
			get_Sounds();
			get_Game().joy_mouse.enabled = true;
			
			return new SVSViewerState(Uint16(8888));
		}
	} m_goi;
	
public:
	Bootstrap() {
		g_gzi = &m_goi;
	}
} g_bootstrap;

int main(int argc, char **argv) {
	return zenilib_main(argc, argv);
}
