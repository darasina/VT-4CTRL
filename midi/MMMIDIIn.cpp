//---------------------------------------------------------------------------

#pragma hdrstop

#include "MMMIDIIn.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)

/**
 * MIDI入力デバイス数を取得する
 *
 * THRU接続などしてたらでてこないと思われる
 */
unsigned int MMMIDIIn::GetNumDevs() { return midiInGetNumDevs(); }

/**
 * デバイスIDを指定してオブジェクト初期化
 */
MMMIDIIn::MMMIDIIn(UINT uDeviceID) { GetDeviceInfo(uDeviceID); }

/**
 * デバイス情報を取得してオブジェクトに格納する
 */
void MMMIDIIn::GetDeviceInfo(UINT uDeviceID) {
  id = uDeviceID;
  r = midiInGetDevCaps(id, &(mic), sizeof(MIDIINCAPS));
}

/**
 * デバイスを握って、バッファ設定、入力開始する
 */
MMMIDIIn::RetCode MMMIDIIn::Open() {
  r = midiInOpen(&h, id, (DWORD_PTR)MMMIDIIn::callbackProc, (DWORD_PTR)this,
                 CALLBACK_FUNCTION);
  if (r != MMSYSERR_NOERROR) {
    return RetCode::Error;
  }
  opened = true;
  vHdr.push_back(new MIDIHDR());
  MIDIHDR& hdr = *(vHdr.back());
  hdr.lpData = new char[65536];
  hdr.dwBufferLength = 65536;
  hdr.dwFlags = 0;
  r = midiInPrepareHeader(h, vHdr.back(), sizeof(MIDIHDR));
  if (isError()) {
    return RetCode::Error;
  }
  r = midiInAddBuffer(h, vHdr.back(), sizeof(MIDIHDR));
  if (isError()) {
    return RetCode::Error;
  }
  reuseBuffer = true;
  return RetCode::OK;
}

/**
 * デバイスを閉じる、バッファやらの始末もしてから。
 */
MMMIDIIn::RetCode MMMIDIIn::Close() {
  reuseBuffer = false;
  r = midiInReset(h);
  if (isError()) {
    return RetCode::Error;
  }
  for (LPMIDIHDR hdr : vHdr) {
    r = midiInUnprepareHeader(h, hdr, sizeof(MIDIHDR));
    if (hdr->lpData != nullptr) {
      delete[] hdr->lpData;
    }
    delete hdr;
  }
  r = midiInClose(h);
  if (isError()) {
    return RetCode::Error;
  }
  { opened = false; }
  return RetCode::OK;
}

/**
 * 直前の実行結果コードを取得する
 */
MMRESULT MMMIDIIn::GetRecentResult() { return r; }

/**
 * 直前の実行結果がエラーならtrue
 */
bool MMMIDIIn::isError() { return r != MMSYSERR_NOERROR; }

/**
 * 直前の実行結果のエラー文字列を返す
 */
char* MMMIDIIn::GetRecentError() {
  midiInGetErrorText(r, errorStr, sizeof errorStr);
  return errorStr;
}

WORD MMMIDIIn::GetManufacturerID() { return mic.wMid; }

char* MMMIDIIn::GetProductName() { return mic.szPname; }

MMMIDIIn::RetCode MMMIDIIn::Reset() {
  reuseBuffer = false;
  r = midiInReset(h);
  if (isError()) return RetCode::Error;
  return RetCode::OK;
}

MMMIDIIn::RetCode MMMIDIIn::Start() {
  r = midiInStart(h);
  if (isError()) return RetCode::Error;
  return RetCode::OK;
}

MMMIDIIn::RetCode MMMIDIIn::Stop() {
  r = midiInStop(h);
  if (isError()) return RetCode::Error;
  return RetCode::OK;
}

void MMMIDIIn::callback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwParam1,
                        DWORD_PTR dwParam2) {
  switch (wMsg) {
    case MIM_OPEN:
      OutputDebugStringA("MIM_OPEN.");
      break;
    case MIM_ERROR:
      OutputDebugStringA("MIM_ERROR.");
      break;
    case MIM_LONGERROR:
      OutputDebugStringA("MIM_LONGERROR.");
      break;
    case MIM_DATA:
      OutputDebugStringA("MIM_DATA.");
      break;
    case MIM_LONGDATA: {
      OutputDebugStringA("MIM_LONGDATA.");
      MIDIHDR& hdr = *(LPMIDIHDR)dwParam1;
      //#ifdef _DEBUG
      //      std::ostringstream ss;
      //      ss << "MMMIDIIn::LongMsg ";
      //      for (unsigned int i = 0; i < hdr.dwBytesRecorded; i++) {
      //        ss << std::setw(2) << std::setfill('0') << std::hex;
      //        ss << (int)(hdr.lpData[i] & 0xff);
      //      }
      //      OutputDebugString(ss.str().c_str());
      //#endif
    } break;
    case MIM_MOREDATA:
      OutputDebugStringA("MIM_MOREDATA.");
      break;
    case MIM_CLOSE:
      OutputDebugStringA("MIM_CLOSE.");
      break;
    default:
      OutputDebugStringA("MIDI In Msg received.");
  }
  if (callbackFunc) {
    callbackFunc(callbackInstance, wMsg, dwParam1, dwParam2);
  }
  // バッファ再利用
  if (reuseBuffer) {
    switch (wMsg) {
      case MIM_LONGDATA:
        auto i = std::find(vHdr.begin(), vHdr.end(), (LPMIDIHDR)dwParam1);
        if (i != vHdr.end()) {
          MMRESULT ret = midiInAddBuffer(h, *i, sizeof(MIDIHDR));
          if (ret != MMSYSERR_NOERROR) {
            OutputDebugStringA("MIDIIN add buffer failed.");
          }
        }
        break;
    }
  }
}

void MMMIDIIn::SetMessageCallback(void* instance, Callback func) {
  callbackInstance = instance;
  callbackFunc = func;
}

void MMMIDIIn::UnsetMessageCallback() { SetMessageCallback(nullptr, nullptr); }

