//---------------------------------------------------------------------------

#pragma hdrstop

#include "MMMIDIOut.h"

#include <debugapi.h>
#include <windows.h>

#include <iomanip>
#include <sstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)

UINT MMMIDIOut::GetNumDevs() { return midiOutGetNumDevs(); }

MMMIDIOut::MMMIDIOut(UINT uDeviceID) { GetDeviceInfo(uDeviceID); }

MMMIDIOut::~MMMIDIOut() { Close(); }

/**
 * MIDI出力デバイス情報を取得する
 *
 * デバイス情報はメンバに格納
 */
void MMMIDIOut::GetDeviceInfo(UINT uDeviceID) {
  id = uDeviceID;
  r = midiOutGetDevCaps(id, &(moc), sizeof(MIDIOUTCAPS));
}

/**
 * 取得したデバイス情報に対応するデバイスを開く
 */
MMMIDIOut::RetCode MMMIDIOut::Open() {
  if (isOpened()) return RetCode::OK;

  r = midiOutOpen(&h, id, (DWORD_PTR)MMMIDIOut::callbackProc, (DWORD_PTR)this,
                  CALLBACK_FUNCTION);
  if (r != MMSYSERR_NOERROR) {
    return RetCode::Error;
  }
  opened = true;
  vHdr.push_back(MIDIHDR());
  MIDIHDR& hdr = vHdr.back();
  hdr.lpData = new char[65536];
  hdr.dwBufferLength = 65536;
  hdr.dwFlags = 0;
  r = midiOutPrepareHeader(h, &(vHdr.back()), sizeof(MIDIHDR));
  if (isError()) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * 開いたデバイスを解放する
 */
MMMIDIOut::RetCode MMMIDIOut::Close() {
  for (MIDIHDR hdr : vHdr) {
    r = midiOutUnprepareHeader(h, &hdr, sizeof(MIDIHDR));
    delete[] hdr.lpData;
  }
  r = midiOutClose(h);
  if (isError()) {
    return RetCode::Error;
  }
  opened = false;
  return RetCode::OK;
}

/**
 * 直前のAPI実行結果コードを返す
 */
MMRESULT MMMIDIOut::GetRecentResult() { return r; }

/**
 * 直前のAPI実行結果からエラー文字列を返す
 */
char* MMMIDIOut::GetRecentError() {
  midiOutGetErrorText(r, errorStr, sizeof errorStr);
  return errorStr;
}

WORD MMMIDIOut::GetManufacturerID() { return moc.wMid; }

char* MMMIDIOut::GetProductName() { return moc.szPname; }

/**
 * MIDIメッセージを送信する
 */
MMMIDIOut::RetCode MMMIDIOut::ShortMsg(DWORD dwMsg) {
  // TODO : ランニングステータス対応
  // ステータスバイト(0x80以上)がランニングなら除去する
  r = midiOutShortMsg(h, dwMsg);
  if (isError()) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * システムエクスクルーシブメッセージを送信する
 */
MMMIDIOut::RetCode MMMIDIOut::LongMsg(const char* longMsgData,
                                      unsigned int len) {
  if (!isOpened()) return RetCode::Error;

  MIDIHDR& hdr = vHdr.back();
  if (len > 65536) {
    // TODO : 出力バッファ分割送信対応
    return RetCode::Error;
  }
  hdr.dwBytesRecorded = len;
#ifdef _DEBUG
  std::ostringstream ss;
  ss << "MMMIDIOut::LongMsg ";
#endif
  for (unsigned int i = 0; i < len; i++) {
    hdr.lpData[i] = longMsgData[i];
#ifdef _DEBUG
    ss << std::setw(2) << std::setfill('0') << std::hex;
    ss << (int)(hdr.lpData[i] & 0xff);
#endif
  }
#ifdef _DEBUG
  OutputDebugStringA(ss.str().c_str());
#endif
  r = midiOutLongMsg(h, &hdr, sizeof(MIDIHDR));
  if (isError()) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

void MMMIDIOut::callback(HMIDIOUT hMidiOut, UINT wMsg, DWORD_PTR dwParam1,
                         DWORD_PTR dwParam2) {
  switch (wMsg) {
    case MOM_OPEN:
      OutputDebugStringA("MOM_OPEN.");
      break;
    case MOM_DONE:
      OutputDebugStringA("MOM_DONE.");
      break;
    case MOM_CLOSE:
      OutputDebugStringA("MOM_CLOSE.");
      break;
    default:
      OutputDebugStringA("MIDI Out Msg received.");
  }
}

/**
 * ピッチベンドを送信する
 */
MMMIDIOut::RetCode MMMIDIOut::PitchBend(unsigned char ch, unsigned int val) {
  DWORD msg = 0xE0 | ch;
  unsigned int lsb = ((unsigned int)val & 0x7f) << 8;
  unsigned int msb = ((unsigned int)val & 0b11111110000000) << 9;
  msg += msb;
  msg += lsb;
  return ShortMsg(msg);
}

/**
 * コントロールチェンジを送信する
 */
MMMIDIOut::RetCode MMMIDIOut::ControlChange(unsigned char ch,
                                            unsigned char ctrlNo,
                                            unsigned char val) {
  // TODO : 入力チェック
  DWORD msg = 0xB0 | ch;
  msg += (unsigned int)ctrlNo << 8;
  msg += (unsigned int)val << 16;
  return ShortMsg(msg);
}

/**
 * プログラムチェンジを送信する
 */
MMMIDIOut::RetCode MMMIDIOut::ProgramChange(unsigned char ch,
                                            unsigned char val) {
  // TODO : 入力チェック
  DWORD msg = 0xC0 | ch;
  msg += (unsigned int)val << 8;
  return ShortMsg(msg);
}

