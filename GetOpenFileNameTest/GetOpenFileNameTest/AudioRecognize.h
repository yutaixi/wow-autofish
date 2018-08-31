#ifndef __AUDIO_RECOGNIZE__
#define __AUDIO_RECOGNIZE__
#include <Windows.h> 

class AudioRecognize
{
public:
	AudioRecognize();
	~AudioRecognize();
	static void voicePrintIdentify(HWND hwnd);
	static BOOL buildGMM(HWND hwnd);
	static void stopVoice(HWND hwnd);
	static void startBuild(HWND hwnd);
	static void identify(HWND hwnd);
	static void exportGMM(HWND hwnd, string exportFilePath);
	static void importGMM(HWND hwnd, DWORD sampleFrequency);
	static void deleteVoiceFile();
	static void reset(HWND hwnd);
	static void freeVoicePrint(); 
	static 	void initVoicePrint(); 
	static 	void updateVoicePrint(int percent, int m, int frame_num, bool identify);
	static 	BOOL hasVoiceId();
	static 	bool beginRecord(HWND hwnd);
	static 	bool endRecord(int msg);
	static 	VOID startRecord();
	static 	VOID stopRecord();
	static 	VOID startPlay();
	static 	VOID stopPlay();
	static bool  refreshSoundDeviceFrequency();
private:

};




 

#endif // !__AUDIO_RECOGNIZE__
