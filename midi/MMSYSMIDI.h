//---------------------------------------------------------------------------

#ifndef MMSYSMIDIH
#define MMSYSMIDIH
#include <windows.h>
#include <mmeapi.h>
#include <mmsystem.h>
#include <System.Classes.hpp>

//---------------------------------------------------------------------------
class MMSYSMIDI {
public:
	static String GetInErrorText(MMRESULT ret);
};
#endif
