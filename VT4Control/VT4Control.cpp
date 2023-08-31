// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
// ---------------------------------------------------------------------------
USEFORM("MainFrame.cpp", VT4ControlFrame);

// ---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	try {
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->Title = "VT-4 Control";
		Application->CreateForm(__classid(TVT4ControlFrame), &VT4ControlFrame);
		Application->Run();
	}
	catch (Exception &exception) {
		Application->ShowException(&exception);
	}
	catch (...) {
		try {
			throw Exception("");
		}
		catch (Exception &exception) {
			Application->ShowException(&exception);
		}
	}
	return 0;
}
// ---------------------------------------------------------------------------
