#include "stdafx.h"
#include "mouseHook.h"
#include "kbHook.h"

MouseHook::MouseHook()
{
}

MouseHook::~MouseHook()
{
}
 
void MouseHook::mouseMove(int x, int y)//鼠标移动到指定位置  
{
	SetCursorPos(x, y);
}
void MouseHook::mouseLClick()
{
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void MouseHook::mouseRClick()
{ 
	mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}

 
void  MouseHook::mouseMoveClientToScreen(POINT *point, RECT *windowLocation)
{ 
	mouseMove((point->x + windowLocation->left), (point->y + windowLocation->top)); 

}
void  MouseHook::shiftAndMouseLClick()
{

	KBHook::pressKeyboardDown(VK_LSHIFT);
	mouseLClick();
	KBHook::keyboardUp(VK_LSHIFT);
}
void  MouseHook::shiftAndMouseRClick()
{

	KBHook::pressKeyboardDown(VK_LSHIFT);
	mouseRClick();
	KBHook::keyboardUp(VK_LSHIFT);
}