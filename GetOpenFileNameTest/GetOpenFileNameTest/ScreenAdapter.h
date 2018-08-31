#ifndef __SCREEN_ADAPTER__
#define __SCREEN_ADAPTER__

class ScreenAdapter
{
public:
	ScreenAdapter();
	~ScreenAdapter();

private:
	int screenSize[13][2] = { //ÆÁÄ»³ß´ç
		{ 1980,1020 },
		{ 1024,768 },
		{ 1280,600 },
		{ 1280,720 },
		{ 1280,768 },
		{ 1280,800 },
		{ 1280,960 },
		{ 1280,1024 },
		{ 1366,768 },
		{ 1400,900 },
		{ 1400,1050 },
		{ 1600,900 },
		{ 1680,1050 }
	};
	int imgIdPath[13] = { 0 };
};

 

#endif // !__SCREEN_ADAPTER__

