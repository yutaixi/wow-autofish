#include <Windows.h>
#include <opencv\cxcore.h>

HBITMAP drawCursor(HWND hwnd);
int  SaveBitmapToFile(HBITMAP hBitmap, string lpFileName);
IplImage * hBitmapToIpl(HBITMAP hBmp);
HBITMAP  drawFishHdc();
bool  drawFishingFloatField(bool saveToFile, HBITMAP *bitmap);
bool  drawFishStatusHdc(bool saveToFile, HBITMAP *bitmap);
bool getCurrentCursorBitmap(CURSORINFO *pci, ICONINFO *iconinfo, string fileName);