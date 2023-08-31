// ---------------------------------------------------------------------------
// DUnit Project File.
// Entry point of C++ project using DUnit framework.
// ---------------------------------------------------------------------------
#include <System.hpp>
#pragma hdrstop

#include <tchar.h>
#include <vcl.h>
#include <GUITestRunner.hpp>

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	try {
		Application->Initialize();
		Guitestrunner::RunRegisteredTests();
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
