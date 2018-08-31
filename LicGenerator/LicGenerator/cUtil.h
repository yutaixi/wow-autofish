#ifndef __CUTIL__
#define __CUTIL__

typedef char                                        str_ansi;
typedef unsigned char                               str_utf8;
typedef wchar_t                                     str_utf16;

class CUtil
{
public:
	CUtil();
	~CUtil(); 
	static str_utf16 * GetUTF16FromANSI(const str_ansi * pANSI);
	static str_ansi * GetANSIFromUTF16(const str_utf16 * pUTF16);
private:

};



#endif // !__CUTIL__
