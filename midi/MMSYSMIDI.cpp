//---------------------------------------------------------------------------

#pragma hdrstop

#include "MMSYSMIDI.h"

#include <windows.h>
//---------------------------------------------------------------------------
/**
 * エラー文字列(midiInGetErrorText)を返す
 */
String MMSYSMIDI::GetInErrorText(MMRESULT ret) {
  char error[MAXERRORLENGTH];
  midiInGetErrorText(ret, error, sizeof error);
  return String(error);
}
#pragma package(smart_init)

