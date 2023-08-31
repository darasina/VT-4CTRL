//---------------------------------------------------------------------------

#ifndef MMMIDIOutH
#define MMMIDIOutH

#include <windows.h>
#include <mmeapi.h>
#include <mmsystem.h>
#include <winbase.h>

#include <vector>
//---------------------------------------------------------------------------
/**
 * MIDI出力クラス
 */
class MMMIDIOut {
 public:
  enum class RetCode { OK, Error };

  MMMIDIOut() {}
  MMMIDIOut(UINT uDeviceID);
  ~MMMIDIOut();
  static UINT GetNumDevs();
  void GetDeviceInfo(UINT uDeviceID);
  RetCode Open();
  RetCode Close();
  MMRESULT GetRecentResult();
  char* GetRecentError();
  WORD GetManufacturerID();
  char* GetProductName();
  RetCode ShortMsg(DWORD dwMsg);
  RetCode LongMsg(const char* longMsgData, unsigned int len);
  bool isError() { return r != MMSYSERR_NOERROR; };
  bool isOpened() { return opened; }
  UINT GetID() { return id; }
  RetCode PitchBend(unsigned char ch, unsigned int val);
  RetCode ControlChange(unsigned char ch, unsigned char ctrlNo, unsigned char val);
  RetCode ProgramChange(unsigned char ch, unsigned char val);

 private:
  bool opened = false;
  UINT id;     /// MMSystemでのdevice id
  HMIDIOUT h;  /// MMSystemでのハンドル
  MMRESULT r;  /// 直前のMMRESULT
  MIDIOUTCAPS moc;
  char errorStr[MAXERRORLENGTH];
  static void CALLBACK callbackProc(HMIDIOUT hMidiOut, UINT wMsg,
                                    DWORD_PTR dwInstance, DWORD_PTR dwParam1,
                                    DWORD_PTR dwParam2) {
    ((MMMIDIOut*)dwInstance)->callback(hMidiOut, wMsg, dwParam1, dwParam2);
  }
  std::vector<MIDIHDR> vHdr;
  void CALLBACK callback(HMIDIOUT hMidiOut, UINT wMsg, DWORD_PTR dwParam1,
                         DWORD_PTR dwParam2);
};

#endif

