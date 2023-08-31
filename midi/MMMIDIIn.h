//---------------------------------------------------------------------------

#ifndef MMMIDIInH
#define MMMIDIInH

#include <windows.h>
#include <mmeapi.h>
#include <mmsystem.h>
//#include <winbase.h>

#include <functional>
#include <vector>

//---------------------------------------------------------------------------
class MMMIDIIn {
 public:
  enum class RetCode { OK, Error };

  MMMIDIIn() {}
  MMMIDIIn(UINT uDeviceID);
  static UINT GetNumDevs();

  void GetDeviceInfo(UINT uDeviceID);
  RetCode Open();
  RetCode Reset();
  RetCode Start();
  RetCode Stop();
  RetCode Close();
  MMRESULT GetRecentResult();
  char* GetRecentError();
  WORD GetManufacturerID();
  char* GetProductName();
  /**
   * 直前のAPI実行結果がエラーならtrueを返す
   */
  bool isError();
  bool isOpened() { return opened; }
  UINT GetID() { return id; }
  typedef std::function<void(void* instance, UINT wMsg, DWORD dwParam1,
                             DWORD dwParam2)>
      Callback;
  void SetMessageCallback(void* instance, Callback func);
  void UnsetMessageCallback();

 private:
  bool opened = false;
  bool reuseBuffer = false;
  UINT id;
  HMIDIIN h;
  MMRESULT r;  /// 直前のMMRESULT
  MIDIINCAPS mic;
  std::vector<LPMIDIHDR> vHdr;
  char errorStr[MAXERRORLENGTH];
  void* callbackInstance = nullptr;
  Callback callbackFunc = nullptr;
  static void CALLBACK callbackProc(HMIDIIN hMidiIn, UINT wMsg,
                                    DWORD_PTR dwInstance, DWORD_PTR dwParam1,
                                    DWORD_PTR dwParam2) {
    ((MMMIDIIn*)dwInstance)->callback(hMidiIn, wMsg, dwParam1, dwParam2);
  }
  void callback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwParam1,
                DWORD_PTR dwParam2);
};
#endif

