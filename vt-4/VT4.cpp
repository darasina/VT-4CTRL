//---------------------------------------------------------------------------

#pragma hdrstop

#include "VT4.h"

#include <debugapi.h>
#include <mmreg.h>
#include <windows.h>

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "HexStr2Bytes.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

VT4::~VT4() { Close(); }

/**
 * MIDI入力デバイスの中からVT-4を探す
 *
 * USB接続でない場合はWindowsの入力デバイスとしては認識されないと思われる。
 * 発見したデバイスIDを保管する。(Openで使用される)
 */
VT4::RetCode VT4::SearchVT4DevID() {
  UINT devNum = 0;
  bool inVT4Found = false;
  bool outVT4Found = false;
  // MIDI入力デバイスを走査してVT4があったらIDを格納
  devNum = MMMIDIIn::GetNumDevs();
  for (UINT devID = 0; devID < devNum; devID++) {
    in = std::make_unique<MMMIDIIn>(devID);
    char* pn = in->GetProductName();
    std::string pname = pn;
    OutputDebugStringA(pname.c_str());
    // プリフィックスがついたりすることがあるので部分一致で検索
    if (pname.find("VT-4") != std::string::npos) {
      inDevID = devID;
      inVT4Found = true;
      break;
    } else {
      continue;
    }
  }
  // MIDI出力デバイスを走査してVT4があったらIDを格納
  devNum = MMMIDIOut::GetNumDevs();
  for (UINT devID = 0; devID < devNum; devID++) {
    out = std::make_unique<MMMIDIOut>(devID);
    std::string pname = out->GetProductName();
    OutputDebugStringA(pname.c_str());
    if (pname.find("VT-4") != std::string::npos) {
      outDevID = devID;
      outVT4Found = true;
      break;
    } else {
      continue;
    }
  }
  if (inVT4Found && outVT4Found) {
    return RetCode::OK;
  }

  return RetCode::Error;
}

/**
 * デバイスをつかむ
 */
VT4::RetCode VT4::Open() {
  if (!in) {
    in = std::make_unique<MMMIDIIn>();
    in->GetDeviceInfo(inDevID);
    OutputDebugStringA(
        (std::string("set in dev: ") + in->GetProductName()).c_str());
  }
  if (in->Open() != MMMIDIIn::RetCode::OK) {
    OutputDebugStringA("open failed midiin device.");
    return RetCode::Error;
  }

  // メッセージコールバック設定
  in->SetMessageCallback(this, midiInCallback);

  // 入力開始
  if (in->Start() != MMMIDIIn::RetCode::OK) {
    OutputDebugStringA("start failed midiin device.");
    return RetCode::Error;
  }
  OutputDebugStringA(
      (std::string("open midiin device: ") + in->GetProductName()).c_str());
  if (!out) {
    out = std::make_unique<MMMIDIOut>();
    out->GetDeviceInfo(outDevID);
    OutputDebugStringA(
        (std::string("set out dev: ") + out->GetProductName()).c_str());
  }
  if (out->Open() != MMMIDIOut::RetCode::OK) {
    OutputDebugStringA("open failed midiout device.");
    return RetCode::Error;
  }
  OutputDebugStringA(
      (std::string("open midiout device: ") + out->GetProductName()).c_str());
  return RetCode::OK;
}

/**
 * VT-4を解放する
 */
VT4::RetCode VT4::Close() {
  MMMIDIIn::RetCode inRet;
  if (in != nullptr) {
    // MMMIDIInでResetするので不要
    // inRet = in->Reset();
    in->UnsetMessageCallback();  // MIM_CLOSEが受け取れないので。
    inRet = in->Close();
    in = nullptr;
  }
  MMMIDIOut::RetCode outRet;
  if (out != nullptr) {
    outRet = out->Close();
    out = nullptr;
  }
  if (inRet != MMMIDIIn::RetCode::OK) {
    return RetCode::Error;
  }
  if (outRet != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

char* VT4::GetInDevName() {
  if (!in) {
    return nullptr;
  }
  return in->GetProductName();
}

/**
 * MIDI出力デバイス名を返す
 */
char* VT4::GetOutDevName() {
  if (!out) {
    return nullptr;
  }
  return out->GetProductName();
}

/**
 * MIDI入力デバイスが開かれているか
 */
bool VT4::isInDevOpened() {
  if (!in) return false;
  return in->isOpened();
}

bool VT4::isOutDevOpened() {
  if (!out) return false;
  return out->isOpened();
}

/**
 * リクエストアイデンティティーメッセージを送信する
 */
VT4::RetCode VT4::RequestIdentity() {
  RetCode outRet;
  char longmsgRequestIdentity[] = {0xf0, 0x7e, 0x10, 0x06, 0x01, 0xf7};
  return SendSysEx((char*)longmsgRequestIdentity,
                   sizeof longmsgRequestIdentity);
}

/**
 * MIDI入力メッセージコールバック
 */
void VT4::inCallback(UINT wMsg, DWORD dwParam1, DWORD dwParam2) {
  int dataSize = 0;
  LPMIDIHDR hdr = nullptr;
  switch (wMsg) {
    case MIM_LONGDATA:
      OutputDebugStringA("VT4::MIM_LONGDATA.");
      hdr = (LPMIDIHDR)dwParam1;
      dataSize = extractDT1(hdr);
      if (dataSize > 0) {
      }
      break;
  }
  if (extInCallback) {
    extInCallback(extInCallbackInstance, wMsg, dwParam1, dwParam2);
  }
}

/**
 * MIDI入力コールバックを設定する
 */
void VT4::SetInCallback(void* instance, CallbackFunc callback) {
  extInCallbackInstance = instance;
  extInCallback = callback;
}

/**
 * システムエクスクルーシブメッセージを送信する
 */
VT4::RetCode VT4::SendSysEx(const char* longdata, size_t len) {
#ifdef _DEBUG
  std::ostringstream __oss;
  __oss << std::hex << std::setfill('0');
  for (int i = 0; i < len; i++) {
    __oss << std::setw(2);
    int ___v = longdata[i] & 0x00ff;
    __oss << ___v;
  }
  OutputDebugStringA(__oss.str().c_str());
#endif
  MMMIDIOut::RetCode outRet;
  outRet = out->LongMsg(longdata, len);
  if (outRet != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * アイデンティティー・リプライメッセージを取得する。
 *
 * 返答まで一寸待つ
 */
VT4::Identity VT4::GetIdentity() {
  Identity id;

  UINT msg = 0;
  CallbackFunc f = [&](void* instance, UINT wMsg, DWORD dwParam1,
                       DWORD dwParam2) {
    {
      if (wMsg != MIM_LONGDATA) {
        return;
      }
      std::lock_guard<std::mutex> lg(mtx);
      LPMIDIHDR hdr = (LPMIDIHDR)dwParam1;
      id = extractIdentity(hdr);
    }
    cv.notify_all();
    return;
  };
  std::thread thr([this] {
    std::unique_lock<std::mutex> ul(mtx);
    if (cv.wait_for(ul, std::chrono::seconds(3)) == std::cv_status::timeout) {
      OutputDebugStringA("timeout.");
      return;
    }
    return;
  });

  // コールバックを一時横取り
  void* backupInstance = extInCallbackInstance;
  CallbackFunc backupF = extInCallback;
  SetInCallback(this, f);
  RetCode ret = RequestIdentity();

  thr.join();
  SetInCallback(backupInstance, backupF);
  return id;
}

/**
 * アイデンティティー・リプライをヘッダーから構造体へ展開して返す
 */
VT4::Identity VT4::extractIdentity(const LPMIDIHDR hdr) {
  Identity id;
  if (hdr == nullptr) {
    return id;
  }
  id.idNum = hdr->lpData[1];
  id.deviceID = hdr->lpData[2];
  id.subID1 = hdr->lpData[3];
  id.subID2 = hdr->lpData[4];
  id.manID = hdr->lpData[5];
  return id;
}

/**
 * RQ1メッセージを送信する
 */
VT4::RetCode VT4::sendRQ1(char* address, char* dataSize) {
  char modelID[] = {0x00, 0x00, 0x00, 0x51};
  std::vector<char> vAddress;
  vAddress.reserve(4);
  for (int i = 0; i < 4; i++) {
    vAddress.push_back(address[i]);
  }
  std::vector<char> vDataSize;
  vDataSize.reserve(4);
  for (int i = 0; i < 4; i++) {
    vDataSize.push_back(dataSize[i]);
  }

  std::vector<char> vData;
  vData.insert(vData.end(), vAddress.begin(), vAddress.end());
  vData.insert(vData.end(), vDataSize.begin(), vDataSize.end());
  vData.push_back(CheckSum((unsigned char*)vData.data(), vData.size()));

  std::vector<char> longmsg;
  longmsg.push_back(0xf0);
  longmsg.push_back(0x41);
  longmsg.push_back(0x10);
  for (int i = 0; i < sizeof modelID; i++) {
    longmsg.push_back(modelID[i]);
  }
  longmsg.push_back(0x11);
  longmsg.insert(longmsg.end(), vData.begin(), vData.end());
  longmsg.push_back(0xf7);
  size_t data_len = longmsg.size();
  int data[data_len];
  for (int i = 0; i < data_len; i++) {
    data[i] = longmsg[i];
  }

  RetCode ret = SendSysEx(longmsg.data(), longmsg.size());
  if (ret != RetCode::OK) {
    return RetCode::Error;
  }

  return RetCode::OK;
}

/**
 * RQ1メッセージを送信して返信を一回受信する
 */
LPMIDIHDR VT4::getRQ1(char* address, char* dataSize) {
  LPMIDIHDR header = nullptr;
  CallbackFunc f = [&header, this](void* instance, UINT wMsg, DWORD dwParam1,
                                   DWORD dwParam2) {
    {
      if (wMsg != MIM_LONGDATA) {
        return;
      }
      // TODO : DT1かチェックする
      std::lock_guard<std::mutex> lg(mtx);
      header = (LPMIDIHDR)dwParam1;
    }
    cv.notify_all();
    return;
  };
  std::thread thr([this] {
    std::unique_lock<std::mutex> ul(mtx);
    if (cv.wait_for(ul, std::chrono::seconds(3)) == std::cv_status::timeout) {
      OutputDebugStringA("timeout.");
      return;
    }
    return;
  });

  void* backupInstance = extInCallbackInstance;
  CallbackFunc backupF = extInCallback;
  SetInCallback(this, f);
  RetCode ret = sendRQ1(address, dataSize);
  thr.join();
  SetInCallback(backupInstance, backupF);
  return header;
}
/**
 * Rolandシステムエクスクルーシブのチェックサムを返す
 */
unsigned char VT4::CheckSum(const unsigned char data[], size_t len) {
  unsigned int sum = 0;
  for (int i = 0; i < len; i++) {
    sum += data[i];
  }
  return 128 - (sum % 128);
}

/**
 * テンポラリパッチのグローバルレベルを返す。
 *
 * @return 取れなかったら-1
 */
int VT4::GetVolume() {
  RetCode ret;
  char address[] = {0x10, 0x00, 0x00, 0x14};
  char dataSize[] = {0x00, 0x00, 0x00, 0x02};

  LPMIDIHDR header = getRQ1(address, dataSize);
  if (header == nullptr) {
    return -1;
  }
  int volume = 0;
  volume += (int)header->lpData[12] << 4;
  volume += (int)header->lpData[13];

  return volume;
}

/**
 * DT1データをメンバ変数に展開する
 *
 * @return 結果コードを返す
 * @retval >=0 成功
 */
int VT4::extractDT1(LPMIDIHDR hdr) {
  assert(hdr != nullptr);

  char* data = hdr->lpData;
  size_t size = hdr->dwBytesRecorded;
#ifdef _DEBUG
  std::ostringstream oss;
  oss << "recoreded: " << size;
  OutputDebugStringA(oss.str().c_str());
#endif
  const unsigned int idx_command_id = 7;
  const unsigned int IDX_ADDR_1 = 8;
  const unsigned int IDX_ADDR_2 = 9;
  const unsigned int IDX_ADDR_3 = 10;
  const unsigned int IDX_ADDR_4 = 11;
  const unsigned int OFFSET_BYTES_DATA_START = 12;
  unsigned int addrs;
  addrs = ((unsigned int)data[IDX_ADDR_1]) << 24;
  addrs += data[IDX_ADDR_2] << 16;
  addrs += data[IDX_ADDR_3] << 8;
  addrs += data[IDX_ADDR_4];
  unsigned int offset = 0;
  int ret = 0;

  if (size < 16) {
    // データ長が不足
    return -2;
  }
  if (data[idx_command_id] != 0x12) {
    // コマンドIDがDT1ではない
    return -3;
  }
  // パラメータをメンバに展開
  char* dataStart = data + OFFSET_BYTES_DATA_START;
  ret = parseDT1(addrs, dataStart, offset);
  while (ret > 0) {
    offset += ret;
    if (hdr->dwBytesRecorded < (offset + OFFSET_BYTES_DATA_START)) {
      return 0;
    }
    ret = parseDT1(addrs, dataStart, offset);
  }
  if (ret == 0) {
    return 0;
  } else if (ret < 0) {
    return -4;
  }
  return -1;
}

/**
 * DT1データを解析・展開して、次のパラメータまでのアドレス長を返す
 *
 * @return 次のパラメータまでのアドレス長.
 * 入力したアドレスに加えることで次の入力アドレスに進めることができる.
 */
int VT4::parseDT1(unsigned int startParamAddrs, char* data,
                  unsigned int offset) {
  unsigned int adrs = startParamAddrs + offset;
  switch ((adrs & 0xFFFF0000)) {
    case PRM_ADRS_SYSTEM:
      // SYSTEM
      switch (adrs - PRM_ADRS_SYSTEM) {
        case PRM_OFFSET_SYSTEM_MIDI_CH:
          params.system.midiCh = data[offset];
          return PRM_OFFSET_SYSTEM_GATE_LEVEL - PRM_OFFSET_SYSTEM_MIDI_CH;
        case PRM_OFFSET_SYSTEM_GATE_LEVEL:
          params.system.gateLevel = data[offset];
          return PRM_OFFSET_SYSTEM_MAX - PRM_OFFSET_SYSTEM_GATE_LEVEL;
        default:
          // TODO : 未対応システムパラメータ
          // LOW CUT
          // ENHANCER
          // FORMANT DEPTH
          // MONITOR MODE
          // EXTERNAL CARRIER
          // USB MIXING
          // MIDI IN MODE
          // PITCH AND FORMANT ROUTING
          // MUTE MODE
          return -1;
      }
      break;
    case PRM_ADRS_TEMPORARY_PATCH:
      // Temporary Patch
      return parseDT1Patch(PRM_PATCHNO_TEMPORARY, startParamAddrs, data,
                           offset);
    case PRM_ADRS_USER_PATCH_1:
      return parseDT1Patch(PRM_PATCHNO_1, startParamAddrs, data, offset);
    case PRM_ADRS_USER_PATCH_2:
      return parseDT1Patch(PRM_PATCHNO_2, startParamAddrs, data, offset);
    case PRM_ADRS_USER_PATCH_3:
      return parseDT1Patch(PRM_PATCHNO_3, startParamAddrs, data, offset);
    case PRM_ADRS_USER_PATCH_4:
      return parseDT1Patch(PRM_PATCHNO_4, startParamAddrs, data, offset);
    case PRM_ADRS_USER_PATCH_5:
      return parseDT1Patch(PRM_PATCHNO_5, startParamAddrs, data, offset);
    case PRM_ADRS_USER_PATCH_6:
      return parseDT1Patch(PRM_PATCHNO_6, startParamAddrs, data, offset);
    case PRM_ADRS_USER_PATCH_7:
      return parseDT1Patch(PRM_PATCHNO_7, startParamAddrs, data, offset);
    case PRM_ADRS_USER_PATCH_8:
      return parseDT1Patch(PRM_PATCHNO_8, startParamAddrs, data, offset);
    default:
      // TODO : 未対応スタートアドレス
      // Temporary Robot
      // User Robot N
      // Temporary Harmony
      // User Harmony N
      // Temporary Megaphone
      // User Megaphone N
      // Temporary Reverb
      // User Reverb N
      // Temporary Vocoder
      // User Vocoder N
      // Temporary Equalizer
      // User Equalizer 1
      return -1;
  }
}

/**
 * パッチパラメータデータを解析してメンバに展開する
 */
int VT4::parseDT1Patch(int patchNo, unsigned int startParamAddrs, char* data,
                       unsigned int offset) {
  PatchData* patch = nullptr;
  if (patchNo == -1) {
    patch = &(params.temporaryPatch);
    patch->_updated = true;
  } else {
    if (patchNo > 7) {
      return -1;
    }
    patch = &(params.userPatch[patchNo]);
    patch->_updated = true;
  }
  assert(patch);
  patch->patchNo = patchNo;
  unsigned int lower_adrs = (startParamAddrs + offset) & 0x0000FFFF;
  // 下位アドレスでパラメータを特定
  switch (lower_adrs) {
    case PRM_OFFSET_PATCH_ROBOT:
      patch->robot = data[offset];
      return PRM_OFFSET_PATCH_HARMONY - PRM_OFFSET_PATCH_ROBOT;
    case PRM_OFFSET_PATCH_HARMONY:
      patch->harmony = data[offset];
      return PRM_OFFSET_PATCH_VOCODER - PRM_OFFSET_PATCH_HARMONY;
    case PRM_OFFSET_PATCH_VOCODER:
      patch->vocoder = data[offset];
      return PRM_OFFSET_PATCH_MEGAPHONE - PRM_OFFSET_PATCH_VOCODER;
    case PRM_OFFSET_PATCH_MEGAPHONE:
      patch->megaphone = data[offset];
      return PRM_OFFSET_PATCH_ROBOT_VARIATION - PRM_OFFSET_PATCH_MEGAPHONE;
    case PRM_OFFSET_PATCH_ROBOT_VARIATION:
      patch->robotVariation = data[offset];
      return PRM_OFFSET_PATCH_HARMONY_VARIATION -
             PRM_OFFSET_PATCH_ROBOT_VARIATION;
    case PRM_OFFSET_PATCH_HARMONY_VARIATION:
      patch->harmonyVariation = data[offset];
      return PRM_OFFSET_PATCH_VOCODER_VARIATION -
             PRM_OFFSET_PATCH_HARMONY_VARIATION;
    case PRM_OFFSET_PATCH_VOCODER_VARIATION:
      patch->vocoderVariation = data[offset];
      return PRM_OFFSET_PATCH_MEGAPHONE_VARIATION -
             PRM_OFFSET_PATCH_VOCODER_VARIATION;
    case PRM_OFFSET_PATCH_MEGAPHONE_VARIATION:
      patch->megaphoneVariation = data[offset];
      return PRM_OFFSET_PATCH_REVERB_VARIATION -
             PRM_OFFSET_PATCH_MEGAPHONE_VARIATION;
    case PRM_OFFSET_PATCH_REVERB_VARIATION:
      patch->reverbVariation = data[offset];
      return PRM_OFFSET_PATCH_PITCH - PRM_OFFSET_PATCH_REVERB_VARIATION;
    case PRM_OFFSET_PATCH_PITCH:
      patch->pitch = (unsigned int)data[offset] << 4;
      patch->pitch += (unsigned int)data[offset + 1];
      return PRM_OFFSET_PATCH_FORMANT - PRM_OFFSET_PATCH_PITCH;
    case PRM_OFFSET_PATCH_FORMANT:
      patch->formant = (unsigned int)data[offset] << 4;
      patch->formant += (unsigned int)data[offset + 1];
      return PRM_OFFSET_PATCH_BALANCE - PRM_OFFSET_PATCH_FORMANT;
    case PRM_OFFSET_PATCH_BALANCE:
      patch->balance = (unsigned int)data[offset] << 4;
      patch->balance += (unsigned int)data[offset + 1];
      return PRM_OFFSET_PATCH_REVERB - PRM_OFFSET_PATCH_BALANCE;
    case PRM_OFFSET_PATCH_REVERB:
      patch->reverb = (unsigned int)data[offset] << 4;
      patch->reverb += (unsigned int)data[offset + 1];
      return PRM_OFFSET_PATCH_AUTO_PITCH - PRM_OFFSET_PATCH_REVERB;
    case PRM_OFFSET_PATCH_AUTO_PITCH:
      patch->autoPitch = (unsigned int)data[offset] << 4;
      patch->autoPitch += (unsigned int)data[offset + 1];
      return PRM_OFFSET_PATCH_KEY - PRM_OFFSET_PATCH_AUTO_PITCH;
    case PRM_OFFSET_PATCH_KEY:
      patch->key = data[offset];
      return PRM_OFFSET_PATCH_GLOVAL_LEVEL - PRM_OFFSET_PATCH_KEY;
    case PRM_OFFSET_PATCH_GLOVAL_LEVEL:
      patch->globalLevel = (unsigned int)data[offset] << 4;
      patch->globalLevel += (unsigned int)data[offset + 1];
      return PRM_OFFSET_PATCH_NAME_00_03 - PRM_OFFSET_PATCH_GLOVAL_LEVEL;
    case PRM_OFFSET_PATCH_NAME_00_03:
      patch->name[0] = (unsigned int)data[offset] << 4;
      patch->name[0] += (unsigned int)data[offset + 1];
      patch->name[1] = (unsigned int)data[offset + 2] << 4;
      patch->name[1] += (unsigned int)data[offset + 3];
      patch->name[2] = (unsigned int)data[offset + 4] << 4;
      patch->name[2] += (unsigned int)data[offset + 5];
      patch->name[3] = (unsigned int)data[offset + 6] << 4;
      patch->name[3] += (unsigned int)data[offset + 7];
      return PRM_OFFSET_PATCH_NAME_04_07 - PRM_OFFSET_PATCH_NAME_00_03;
    case PRM_OFFSET_PATCH_NAME_04_07:
      patch->name[4] = (unsigned int)data[offset] << 4;
      patch->name[4] += (unsigned int)data[offset + 1];
      patch->name[5] = (unsigned int)data[offset + 2] << 4;
      patch->name[5] += (unsigned int)data[offset + 3];
      patch->name[6] = (unsigned int)data[offset + 4] << 4;
      patch->name[6] += (unsigned int)data[offset + 5];
      patch->name[7] = (unsigned int)data[offset + 6] << 4;
      patch->name[7] += (unsigned int)data[offset + 7];
      patch->name[8] = '\0';
      return PRM_OFFSET_PATCH_MAX - PRM_OFFSET_PATCH_NAME_04_07;
    default:
      return -1;
  }
}

/**
 * Systemパラメータのデータを要求する
 *
 * 返答がきたらコールバックでsystemメンバが更新される。
 */
VT4::RetCode VT4::RequestSystemData() {
  char addr[] = {0x00, 0x00, 0x00, 0x00};
  char size[] = {0, 0, 0, 0x10};
  return sendRQ1(addr, size);
}

/**
 * テンポラリパッチパラメータのデータを要求する
 */
VT4::RetCode VT4::RequestTemporaryPatch() {
  // Temporary Patch
  char addr[] = {0x10, 0x00, 0x00, 0x00};
  char size[] = {0, 0, 0, 0x26};
  return sendRQ1(addr, size);
}

VT4::RetCode VT4::GetTemporaryPatch() {
  // Temporary Patch
  char addr[] = {0x10, 0x00, 0x00, 0x00};
  char size[] = {0, 0, 0, 0x26};
  LPMIDIHDR hdr = getRQ1(addr, size);
  if (hdr == nullptr) {
    return RetCode::Error;
  }

  if (hdr->dwBytesRecorded > 0) {
  }
  return RetCode::OK;
}

/**
 * VT4のパラメータを構造体で返す
 */
VT4::Parameters VT4::GetParameters() { return params; }

/**
 * ビッグエンディアンのバイト列をunsigned intにパックする
 */
unsigned int VT4::bytesToUint(char* bytes, size_t len) {
  unsigned int ret = 0;
  int shift_count = 0;
  for (size_t i = 0; i < len; i++) {
    shift_count = len - 1 - i;
    if (shift_count > 0) {
      ret += (unsigned int)bytes[i] << (8 * shift_count);
    } else
      ret += bytes[i];
  }
  return ret;
}

/**
 * 32ビットデータをビッグエンディアンで格納する
 */
void VT4::packDwordToBytes(uint32_t in, char* out) {
  out[0] = ((in & 0xFF000000) >> 24);
  out[1] = (in & 0x00FF0000 >> 16);
  out[2] = (in & 0x0000FF00 >> 8);
  out[3] = (in & 0x000000FF);
}

/**
 * 先頭4ビットを1バイト目の下位4ビットに、残りを2バイト目の下位4ビットに格納する
 */
void VT4::packTwoBytesParam(unsigned char from, char* to) {
  to[0] = (from & 0b11110000) >> 4;
  to[1] = (from & 0b00001111);
}

/**
 * DT1メッセージを送信する
 */
VT4::RetCode VT4::sendDT1(const char* address, const char* data,
                          size_t dataLen) {
  std::byte status{0xf0};
  char modelID[] = {0x00, 0x00, 0x00, 0x51};
  std::ostringstream oss;
  std::ostringstream ossData;
  oss << (char)status << (char)0x41 << (char)0x10;
  oss.write(modelID, sizeof modelID);
  oss << (char)0x12;  // DT1
  ossData.write(address, 4);
  ossData.write(data, dataLen);
  oss.write(ossData.str().c_str(), 4 + dataLen);
  oss << CheckSum((const unsigned char*)ossData.str().c_str(),
                  ossData.str().length());
  oss << (char)0xf7;
  return SendSysEx(oss.str().c_str(), oss.str().length());
}

/**
 * MIDICHパラメータを送信する
 */
VT4::RetCode VT4::SendMIDICh(unsigned char midiCh) {
  char adrs[] = {0, 0, 0, 0};
  return sendDT1(adrs, (const char*)&midiCh, 1);
}

/**
 * パッチのROBOTパラメータをSysEx送信する
 */
VT4::RetCode VT4::SendRobot(int patchNo, unsigned char value) {
  //パラメータチェック
  if (patchNo < -1) {
    return RetCode::Error;
  }
  if (patchNo > 7) {
    return RetCode::Error;
  }
  if (value > 2) {
    return RetCode::Error;
  }
  // パラメータアドレス
  char adrs[] = {0, 0, 0, 0};
  if (patchNo == -1) {
    // テンポラリパッチ
    adrs[0] = 0x10;
  } else {
    // ユーザパッチ
    adrs[0] = 0x11;
    adrs[1] = patchNo;
  }
  return sendDT1(adrs, (const char*)&value, 1);
}

/**
 * ユーザパッチパラメータを要求する
 *
 * @param patchNo パッチ番号. 0: ユーザーパッチ1
 */
VT4::RetCode VT4::RequestUserPatch(unsigned char patchNo) {
  char addr[] = {0x11, 0x00, 0x00, 0x00};
  char size[] = {0, 0, 0, 0x26};
  if (patchNo > 7) {
    return RetCode::Error;
  }
  addr[1] = patchNo;
  return sendRQ1(addr, size);
}

/**
 * ユーザパッチパラメータを要求して返信を待つ
 */
VT4::RetCode VT4::GetUserPatch(unsigned char patchNo) {
  char addr[] = {0x11, 0x00, 0x00, 0x00};
  char size[] = {0, 0, 0, 0x26};
  if (patchNo > 7) {
    return RetCode::Error;
  }
  addr[1] = patchNo;
  LPMIDIHDR hdr = getRQ1(addr, size);
  if (hdr == nullptr) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * PITCHパラメータをSysEx送信する
 */
VT4::RetCode VT4::SendPitchSysEx(int patchNo, unsigned char value) {
  // パラメータアドレス
  char adrs[] = {0, 0, 0, 0x09};
  char data[] = {0, 0};
  // パラメータチェック
  if (patchNo < -1) {
    return RetCode::Error;
  }
  if (patchNo > 7) {
    return RetCode::Error;
  }
  if (patchNo == -1) {
    // テンポラリパッチ
    adrs[0] = 0x10;
  } else {
    // ユーザパッチ
    adrs[0] = 0x11;
    adrs[1] = patchNo;
  }
  // 2バイトの下位4ビットに分ける
  data[0] = value >> 4;
  data[1] = value & 0x00ff;
  return sendDT1(adrs, (const char*)data, sizeof data);
}

/**
 * パッチ名の前半分を送信する
 */
VT4::RetCode VT4::SendPatchName0_3(int patchNo, char* name0_3) {
  // パラメータアドレス
  char adrs[] = {0, 0, 0, 0x16};
  char data[8];
  // パラメータチェック
  if (patchNo < -1) {
    return RetCode::Error;
  }
  if (patchNo > 7) {
    return RetCode::Error;
  }
  if (patchNo == -1) {
    // テンポラリパッチ
    adrs[0] = 0x10;
  } else {
    // ユーザパッチ
    adrs[0] = 0x11;
    adrs[1] = patchNo;
  }
  // 2バイトの下位4ビットに分ける
  data[0] = (name0_3[0] & 0b11110000) >> 4;
  data[1] = name0_3[0] & 0b00001111;
  data[2] = (name0_3[1] & 0b11110000) >> 4;
  data[3] = name0_3[1] & 0b00001111;
  data[4] = (name0_3[2] & 0b11110000) >> 4;
  data[5] = name0_3[2] & 0b00001111;
  data[6] = (name0_3[3] & 0b11110000) >> 4;
  data[7] = name0_3[3] & 0b00001111;
  return sendDT1(adrs, (const char*)data, sizeof data);
}

/**
 * PITCHパラメータを送信する
 */
VT4::RetCode VT4::SendPitch(int patchNo, unsigned char value) {
  if (patchNo != PRM_PATCHNO_TEMPORARY) return SendPitchSysEx(patchNo, value);

  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  MMMIDIOut::RetCode r = out->PitchBend(ch, value);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return SendTempPitch(value);
}

/**
 * PITCHパラメータを送信する
 */
VT4::RetCode VT4::SendTempPitch(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  // VT-4PITCH(255)からMIDI Pitch Bend(7Fx7F)へ変換
  unsigned int pb = std::ceil((double)value / 255.0 * 16129.0);
  MMMIDIOut::RetCode r = out->PitchBend(ch, pb);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * コントロールチェンジでFORMANTを送信する
 */
VT4::RetCode VT4::SendTempFormant(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  // VT-4 FORMANT(255)からMIDI CONTROL CHANGE(7F)へ変換
  unsigned char ctrlVal = value >> 1;
  MMMIDIOut::RetCode r = out->ControlChange(ch, 54, ctrlVal);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * コントロールチェンジでBALANCEを送信する
 */
VT4::RetCode VT4::SendTempBalance(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  // VT-4 BALANCE(255)からMIDI CONTROL CHANGE(7F)へ変換
  unsigned char ctrlVal = value >> 1;
  MMMIDIOut::RetCode r = out->ControlChange(ch, 56, ctrlVal);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * コントロールチェンジでREVERBを送信する
 */
VT4::RetCode VT4::SendTempReverb(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  // VT-4 REVERB(255)からMIDI CONTROL CHANGE(7F)へ変換
  unsigned char ctrlVal = value >> 1;
  MMMIDIOut::RetCode r = out->ControlChange(ch, 57, ctrlVal);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * コントロールチェンジでROBOTを送信する
 *
 * 7Fが1、それ以外は0. 2: MIDI INは設定できないっぽい
 */
VT4::RetCode VT4::SendTempRobot(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  // VT-4 ROBOT(0-2)からMIDI CONTROL CHANGE(7F)へ変換
  unsigned char ctrlVal = value != 0 ? 0x7F : 0;
  MMMIDIOut::RetCode r = out->ControlChange(ch, 49, ctrlVal);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * コントロールチェンジでHARMONYを送信する
 */
VT4::RetCode VT4::SendTempHarmony(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  // VT-4 HARMONY(0-1)からMIDI CONTROL CHANGE(7F)へ変換
  unsigned char ctrlVal = value == 0 ? 0x00 : 0x7F;
  MMMIDIOut::RetCode r = out->ControlChange(ch, CTRLCHG_HARMONY, ctrlVal);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * コントロールチェンジでVOCODERを送信する
 */
VT4::RetCode VT4::SendTempVocoder(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  // VT-4 VOCODER(0-1)からMIDI CONTROL CHANGE(7F)へ変換
  unsigned char ctrlVal = value == 0 ? 0x00 : 0x7F;
  MMMIDIOut::RetCode r = out->ControlChange(ch, CTRLCHG_VOCODER, ctrlVal);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * コントロールチェンジでMEGAPHONEを送信する
 */
VT4::RetCode VT4::SendTempMegaphone(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  // VT-4 MEGAPHONE(0-1)からMIDI CONTROL CHANGE(7F)へ変換
  unsigned char ctrlVal = value == 0 ? 0x00 : 0x7F;
  MMMIDIOut::RetCode r = out->ControlChange(ch, CTRLCHG_MEGAPHONE, ctrlVal);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

VT4::RetCode VT4::SendTempRobotVariation(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0x7f) {
    ch = 0x7f;
  }
  MMMIDIOut::RetCode r = out->ControlChange(ch, CTRLCHG_ROBOT_VARIATION, value);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

VT4::RetCode VT4::SendTempMegaphoneVariation(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  MMMIDIOut::RetCode r =
      out->ControlChange(ch, CTRLCHG_MEGAPHONE_VARIATION, value);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

VT4::RetCode VT4::SendTempVocoderVariation(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  MMMIDIOut::RetCode r =
      out->ControlChange(ch, CTRLCHG_VOCODER_VARIATION, value);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

VT4::RetCode VT4::SendTempHarmonyVariation(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  MMMIDIOut::RetCode r =
      out->ControlChange(ch, CTRLCHG_HARMONY_VARIATION, value);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

VT4::RetCode VT4::SendTempReverbVariation(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  MMMIDIOut::RetCode r =
      out->ControlChange(ch, CTRLCHG_REVERB_VARIATION, value);
  if (r != MMMIDIOut::RetCode::OK) {
    return RetCode::Error;
  }
  return RetCode::OK;
}

/**
 * シーンを呼び出す
 *
 * 実機上での呼び出し。メンバ変数には読み込まない
 */
VT4::RetCode VT4::LoadScene(unsigned char value) {
  unsigned char ch = params.system.midiCh;
  if (ch == 17) {
    ch = 0;
  }
  if (ch > 0) {
    ch--;
  }
  MMMIDIOut::RetCode r = out->ProgramChange(ch, value);
  return RetCode::OK;
}

/**
 * パッチデータを書き込む（シーンを保存する）
 */
VT4::RetCode VT4::SavePatch(int patchNo, VT4::PatchData& patch) {
  assert(patchNo <= PRM_PATCHNO_8);
  char adrs1[] = {0x10, 0, 0, 0};
  char data[PRM_OFFSET_PATCH_MAX] = {};

  if (patchNo != PRM_PATCHNO_TEMPORARY) {
    adrs1[0] = 0x11;
    unsigned char pn = patchNo;
    adrs1[1] = pn;
  }

  // pack
  data[PRM_OFFSET_PATCH_ROBOT] = patch.robot;
  data[PRM_OFFSET_PATCH_HARMONY] = patch.harmony;
  data[PRM_OFFSET_PATCH_VOCODER] = patch.vocoder;
  data[PRM_OFFSET_PATCH_MEGAPHONE] = patch.megaphone;
  data[PRM_OFFSET_PATCH_ROBOT_VARIATION] = patch.robotVariation;
  data[PRM_OFFSET_PATCH_HARMONY_VARIATION] = patch.harmonyVariation;
  data[PRM_OFFSET_PATCH_VOCODER_VARIATION] = patch.vocoderVariation;
  data[PRM_OFFSET_PATCH_MEGAPHONE_VARIATION] = patch.megaphoneVariation;
  data[PRM_OFFSET_PATCH_REVERB_VARIATION] = patch.reverbVariation;
  packTwoBytesParam(patch.pitch, (char*)data + PRM_OFFSET_PATCH_PITCH);
  packTwoBytesParam(patch.formant, (char*)data + PRM_OFFSET_PATCH_FORMANT);
  packTwoBytesParam(patch.balance, (char*)data + PRM_OFFSET_PATCH_BALANCE);
  packTwoBytesParam(patch.reverb, (char*)data + PRM_OFFSET_PATCH_REVERB);
  packTwoBytesParam(patch.autoPitch, (char*)data + PRM_OFFSET_PATCH_AUTO_PITCH);
  data[PRM_OFFSET_PATCH_KEY] = patch.key;
  packTwoBytesParam(patch.globalLevel,
                    (char*)data + PRM_OFFSET_PATCH_GLOVAL_LEVEL);
  for (int i = 0; i < 4; i++) {
    packTwoBytesParam(patch.name[i],
                      (char*)data + PRM_OFFSET_PATCH_NAME_00_03 + i * 2);
  }
  for (int i = 0; i < 4; i++) {
    packTwoBytesParam(patch.name[4 + i],
                      (char*)data + PRM_OFFSET_PATCH_NAME_04_07 + i * 2);
  }
  // 送信
  RetCode r;
  r = sendDT1(adrs1, (const char*)data, sizeof data);
  if (r != RetCode::OK) {
    return RetCode::Error;
  }
  // 内部データを送ったデータで更新
  if (patchNo == PRM_PATCHNO_TEMPORARY) {
    params.temporaryPatch = patch;
  } else
    params.userPatch[patchNo] = patch;
  return RetCode::OK;
}

