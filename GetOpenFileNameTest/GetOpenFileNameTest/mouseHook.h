#include "stdafx.h" 
class MouseHook
{
public:
	MouseHook();
	~MouseHook();
	static void mouseMove(int x, int y);
	static void mouseLClick();
	static void mouseRClick(); 
	static void shiftAndMouseLClick();
	static void shiftAndMouseRClick();
	static void mouseMoveClientToScreen(POINT *point, RECT *windowLocation);
private:

};
