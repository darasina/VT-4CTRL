#include <System.hpp>
#pragma hdrstop

#include <debugapi.h>
#include <windows.h>

#include <TestFramework.hpp>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

//#include "HexStr2Bytes.h"
#include "vt-4/vt4.h"

class TTestVT4 : public TTestCase {
 public:
  __fastcall virtual TTestVT4(System::String name) : TTestCase(name) {}
  virtual void __fastcall SetUp();
  virtual void __fastcall TearDown();

 __published:
  void __fastcall TestSearchVT4DevID();
  void __fastcall TestOpen();
  void __fastcall TestRequestIdentity();
  void __fastcall TestGetIdentity();
  void __fastcall TestGetVolume();
  void __fastcall TestClose();
  void __fastcall TestCheckSum();
  void __fastcall TestGetTemporaryPatch();
  void __fastcall TestparseDT1();
  void __fastcall TestRequestSystemData();
  void __fastcall TestRequestTemporaryPatch();
  void __fastcall TestbytesToUint();
  void __fastcall TestpackDwordToBytes();
  void __fastcall TestSendMIDICh();
  void __fastcall TestSendRobot();
  void __fastcall TestSendPatchName();
  void __fastcall TestSavePatch();

 private:
  unsigned int inDevNum;
  std::unique_ptr<VT4> vt4;
};

void __fastcall TTestVT4::SetUp() {
  inDevNum = VT4::GetInDevNums();
  vt4 = std::make_unique<VT4>();
}

void __fastcall TTestVT4::TearDown() {}

void __fastcall TTestVT4::TestSearchVT4DevID() {
  if (inDevNum == 0) {
    Check(false, "no in midi device.");
  }
  VT4::RetCode vt4found = vt4->SearchVT4DevID();
  std::string pname = vt4->GetInDevName();
  std::string outpname = vt4->GetOutDevName();
  OutputDebugStringA(pname.c_str());
  if (vt4found == VT4::RetCode::OK) {
    Check(pname == "VT-4" && outpname == "VT-4");
  } else {
    Check(pname != "VT-4" || outpname != "VT-4");
  }
}

void __fastcall TTestVT4::TestOpen() {
  vt4->SearchVT4DevID();
  Check(vt4->Open() == VT4::RetCode::OK, "open failed.");
  // Check(vt4.Close() == VT4::RetCode::OK, "close failed.");
  vt4->Close();
}

void __fastcall TTestVT4::TestRequestIdentity() {
  std::mutex mtx;
  std::condition_variable cv;
  UINT msg = 0;
  VT4::CallbackFunc f = [&](void* instance, UINT wMsg, DWORD dwParam1,
                            DWORD dwParam2) {
    {
      std::lock_guard<std::mutex> lg(mtx);
      msg = wMsg;
    }
    cv.notify_all();
    return;
  };
  vt4->SearchVT4DevID();
  vt4->SetInCallback(this, f);
  vt4->Open();
  std::thread thr([&] {
    std::unique_lock<std::mutex> ul(mtx);
    if (cv.wait_for(ul, std::chrono::seconds(3)) == std::cv_status::timeout) {
      OutputDebugStringA("timeout.");
      return;
    }
    Check(msg == MIM_LONGDATA);
  });

  Check(vt4->RequestIdentity() == VT4::RetCode::OK, "request identity failed.");

  thr.join();
  vt4->Close();
}

void __fastcall TTestVT4::TestGetIdentity() {
  vt4->SearchVT4DevID();
  Check(vt4->Open() == VT4::RetCode::OK, "open failed.");
  VT4::Identity id = vt4->GetIdentity();
  vt4->Close();
  Check(id.idNum == 0x7e, "not match: ID Number");
  Check(id.deviceID == 0x10, "not match: Device ID");
}

void __fastcall TTestVT4::TestGetVolume() {
  vt4->SearchVT4DevID();
  Check(vt4->Open() == VT4::RetCode::OK, "open failed.");
  int val = vt4->GetVolume();
  vt4->Close();
  CheckFalse(val < 0, "error value.");
}

void __fastcall TTestVT4::TestClose() {
  vt4->SearchVT4DevID();
  Check(vt4->Open() == VT4::RetCode::OK, "open failed.");
  Check(vt4->Close() == VT4::RetCode::OK, "close failed.");
}

void __fastcall TTestVT4::TestCheckSum() {
  unsigned char bytes[] = {0x10, 0x00, 0x00, 0x09, 0x0f, 0x0f};
  Check(VT4::CheckSum(bytes, sizeof bytes) == 73);
  Check(VT4::CheckSum(bytes, sizeof bytes) == 0x49);
}

void __fastcall TTestVT4::TestGetTemporaryPatch() {
  vt4->SearchVT4DevID();
  Check(vt4->Open() == VT4::RetCode::OK, "open failed.");
  VT4::RetCode ret = vt4->GetTemporaryPatch();
  Check(ret == VT4::RetCode::OK, "Get Temporary Patch failed.");
  Check(vt4->Close() == VT4::RetCode::OK, "close failed.");
}

void __fastcall TTestVT4::TestparseDT1() {
  unsigned int startPrmAdrs1 = 0x00000000;
  // clang-format off
  char data1[] = { 0xf0, 0x41, 0x10, 0, 0, 0, 0x51, 0x12,
	0, 0, 0, 0,
	1, 3
  };
  // clang-format on
  unsigned int offset1 = 0;
  int ret = 0;
  ret = vt4->parseDT1(startPrmAdrs1, ((char*)data1) + 12, offset1);
  Check(ret > 0, "DT1 parse error.");
  VT4::Parameters prm = vt4->GetParameters();
  Check(prm.system.midiCh == 0x0001, "MIDI CH parse error.");
  offset1 += ret;
  ret = vt4->parseDT1(startPrmAdrs1, ((char*)data1) + 12, offset1);
  Check(ret > 0, "DT1 parse error.");
  prm = vt4->GetParameters();
  Check(prm.system.gateLevel == 0x03, "GATE LEVEL parse error.");
}

void __fastcall TTestVT4::TestbytesToUint() {
  char bytes1[] = {0x12, 0x34};
  unsigned int ret;
  ret = VT4::bytesToUint(bytes1, 2);
  Check(ret == 0x1234, "%");
  char bytes2[] = {0x12, 0x34, 0x56, 0x78};
  ret = VT4::bytesToUint(bytes2, 4);
  Check(ret == 0x12345678, "12345678");
}

void __fastcall TTestVT4::TestpackDwordToBytes() {
  char bytes[4];
  uint32_t in1 = 0x12345678;
  VT4::packDwordToBytes(in1, bytes);
  Check(bytes[0] == 0x12);
  Check(bytes[3] == 0x78);
}

void __fastcall TTestVT4::TestRequestSystemData() {
  std::mutex mtx;
  std::condition_variable cv;
  UINT msg = 0;
  VT4::CallbackFunc f = [&](void* instance, UINT wMsg, DWORD dwParam1,
                            DWORD dwParam2) {
    {
      std::lock_guard<std::mutex> lg(mtx);
      msg = wMsg;
    }
    cv.notify_all();
    return;
  };
  vt4->SearchVT4DevID();
  vt4->SetInCallback(this, f);
  vt4->Open();
  std::thread thr([&] {
    std::unique_lock<std::mutex> ul(mtx);
    if (cv.wait_for(ul, std::chrono::seconds(3),
                    [this, msg] { return msg != 0; })) {
      OutputDebugStringA("timeout.");
      return;
    }
    Check(msg == MIM_LONGDATA);
  });

  Check(vt4->RequestSystemData() == VT4::RetCode::OK, "REQ System failed.");

  thr.join();
  VT4::Parameters prms = vt4->GetParameters();
  vt4->Close();
  Check(prms.system.midiCh <= 17);
}

void __fastcall TTestVT4::TestRequestTemporaryPatch() {
  std::mutex mtx;
  std::condition_variable cv;
  UINT msg = 0;
  VT4::CallbackFunc f = [&](void* instance, UINT wMsg, DWORD dwParam1,
                            DWORD dwParam2) {
    {
      std::lock_guard<std::mutex> lg(mtx);
      msg = wMsg;
    }
    cv.notify_all();
    return;
  };
  vt4->SearchVT4DevID();
  vt4->SetInCallback(this, f);
  vt4->Open();
  std::thread thr([&] {
    std::unique_lock<std::mutex> ul(mtx);
    if (cv.wait_for(ul, std::chrono::seconds(3),
                    [this, msg] { return msg != 0; })) {
      OutputDebugStringA("timeout.");
      return;
    }
    Check(msg == MIM_LONGDATA);
  });

  Check(vt4->RequestTemporaryPatch() == VT4::RetCode::OK,
        "REQ Temp Patch failed.");

  thr.join();
  VT4::Parameters prms = vt4->GetParameters();
  vt4->Close();
  Check(prms.temporaryPatch.robot <= 2);
  Check(prms.temporaryPatch.pitch <= 255);
}

void __fastcall TTestVT4::TestSendMIDICh() {
  VT4::RetCode ret;
  std::mutex mtx;
  std::condition_variable cv;
  UINT msg = 0;
  VT4::CallbackFunc f = [&](void* instance, UINT wMsg, DWORD dwParam1,
                            DWORD dwParam2) {
    if (wMsg == MIM_LONGDATA) {
      {
        std::lock_guard<std::mutex> lg(mtx);
        msg = wMsg;
      }
      cv.notify_all();
    }
    return;
  };
  // 初期処理
  vt4->SearchVT4DevID();
  vt4->SetInCallback(this, f);
  vt4->Open();
  // 開始時の設定値を取得（後で戻すため）
  std::thread thr([&] {
    std::unique_lock<std::mutex> ul(mtx);
    if (cv.wait_for(ul, std::chrono::seconds(3),
                    [this, msg] { return msg != 0; })) {
      OutputDebugStringA("timeout thr1.");
      return;
    }
    Check(msg == MIM_LONGDATA);
  });
  Check(vt4->RequestSystemData() == VT4::RetCode::OK, "REQ System failed.");
  thr.join();
  unsigned char backupMIDICh = 0;
  VT4::Parameters prms = vt4->GetParameters();
  backupMIDICh = prms.system.midiCh;

  // 設定値変更のコマンド送信
  unsigned char midiCh1 = backupMIDICh + 1;
  if (midiCh1 > 17) {
    midiCh1 = 0;
  }
  Sleep(20);
  ret = vt4->SendMIDICh(midiCh1);
  Check(ret == VT4::RetCode::OK, "SEND failed.");

  msg = 0;
  Sleep(1000);
  std::thread thr2([&] {
    OutputDebugStringA("start thr2.");
    std::unique_lock<std::mutex> ul(mtx);
    if (!cv.wait_for(ul, std::chrono::seconds(3),
                     [this, msg] { return msg != 0; })) {
      OutputDebugStringA("timeout thr2.");
      return;
    }
    Check(msg == MIM_LONGDATA);
  });
  ret = vt4->RequestSystemData();
  Check(ret == VT4::RetCode::OK, "REQ System failed.");
  thr2.join();
  VT4::Parameters prms2 = vt4->GetParameters();
  Check(prms2.system.midiCh == midiCh1, "送信した設定値が反映されてないっぽい");

  // 設定値を戻すコマンド送信
  Sleep(20);
  Check(vt4->SendMIDICh(backupMIDICh) == VT4::RetCode::OK, "SEND failed.");

  // 後処理
  vt4->UnsetInCallback();
  vt4->Close();
}

void __fastcall TTestVT4::TestSendRobot() {
  // 初期処理
  vt4->SearchVT4DevID();
  vt4->Open();
  // 初期値を取得
  for (unsigned int i = 0; i < 8; i++) {
    Check(vt4->RequestUserPatch(i) == VT4::RetCode::OK);
  }
  vt4->GetTemporaryPatch();
  unsigned char backupRobot;
  VT4::Parameters prms;
  prms = vt4->GetParameters();
  backupRobot = prms.temporaryPatch.robot;
  // 設定値を更新
  unsigned char newRobot = backupRobot + 1;
  if (newRobot > 2) {
    newRobot = 0;
  }
  vt4->SendRobot(-1, newRobot);
  // 更新後の設定値を取得して確認
  vt4->GetTemporaryPatch();
  prms = vt4->GetParameters();
  Check(newRobot == prms.temporaryPatch.robot);

  // 初期値に戻す
  Check(vt4->SendRobot(-1, backupRobot) == VT4::RetCode::OK);
  // 後処理
  vt4->Close();
}

void __fastcall TTestVT4::TestSendPatchName() {
  // 初期処理
  vt4->SearchVT4DevID();
  vt4->Open();
  // 初期値を取得
  for (unsigned int i = 0; i < 8; i++) {
    vt4->RequestUserPatch(i);
  }
  Sleep(40);
  vt4->GetTemporaryPatch();
  VT4::Parameters backprms;
  backprms = vt4->GetParameters();
  char pn[] = {'1', '2', '3', '4'};
  char pn2[] = {'5', '6', '7', '8'};
  // 設定値を更新
  Sleep(40);
  vt4->SendPatchName0_3(VT4::PRM_PATCHNO_1, pn);
  Sleep(40);
  vt4->SendPatchName4_7(VT4::PRM_PATCHNO_1, pn2);
  Sleep(40);
  // 更新後の設定値を取得して確認
  for (unsigned int i = 0; i < 8; i++) {
    vt4->RequestUserPatch(i);
  }
  Sleep(40);
  vt4->GetTemporaryPatch();
  VT4::Parameters prms = vt4->GetParameters();
  std::string newpn = prms.userPatch[VT4::PRM_PATCHNO_1].name;
  Check(newpn == "12345678");

  char pn3[] = {'a','b','c','d','e','f','g','h'};
  // 設定値を更新
  Sleep(40);
  vt4->SendPatchName(VT4::PRM_PATCHNO_1, pn3);
  // 更新後の設定値を取得して確認
  for (unsigned int i = 0; i < 8; i++) {
    vt4->RequestUserPatch(i);
  }
  Sleep(40);
  vt4->GetTemporaryPatch();
  prms = vt4->GetParameters();
  std::string newpn2 = prms.userPatch[VT4::PRM_PATCHNO_1].name;
  Check(newpn2 == "abcdefgh", "newpn2");

  // 初期値に戻す
  Check(vt4->SendPatchName4_7(VT4::PRM_PATCHNO_1, backprms.userPatch[VT4::PRM_PATCHNO_1].name) ==
        VT4::RetCode::OK);
  Check(vt4->SendPatchName4_7(VT4::PRM_PATCHNO_1, (backprms.userPatch[VT4::PRM_PATCHNO_1].name)+4) ==
        VT4::RetCode::OK);
  // 後処理
  vt4->Close();
}

void __fastcall TTestVT4::TestSavePatch() {
  char newName[] = {'t', 'e', 's', 't', 0, 0, 0, 0};
  char newName2[] = {'T', 'E', 's', 't', 'a', 'b', '1', '2'};
  //  char bakName[9];
  VT4::PatchData newPatch;   // 1回目更新用
  VT4::PatchData new2Patch;  // 2回目更新用
  VT4::PatchData bakPatch;   // 設定復帰用
  // 初期処理
  vt4->SearchVT4DevID();
  Check(vt4->Open() == VT4::RetCode::OK, "open failed.");
  VT4::RetCode ret = vt4->RequestUserPatch(VT4::PRM_PATCHNO_8);
  ret = vt4->GetTemporaryPatch();
  Check(ret == VT4::RetCode::OK, "Get Temporary Patch failed.");
  VT4::Parameters prms = vt4->GetParameters();
  newPatch = prms.userPatch[VT4::PRM_PATCHNO_8];
  bakPatch = newPatch;  // 設定バックアップ
  // データ更新
  newPatch.robot = VT4::PRM_ROBOT_ON;
  newPatch.harmony = VT4::PRM_HARMONY_OFF;
  newPatch.vocoder = VT4::PRM_VOCODER_ON;
  newPatch.megaphone = VT4::PRM_MEGAPHONE_ON;
  newPatch.robotVariation = 2;
  newPatch.harmonyVariation = 3;
  newPatch.vocoderVariation = 4;
  newPatch.megaphoneVariation = 5;
  newPatch.reverbVariation = 6;
  newPatch.pitch = 127;
  newPatch.formant = 128;
  newPatch.balance = 129;
  newPatch.reverb = 130;
  newPatch.autoPitch = 131;
  newPatch.key = VT4::PRM_KEY_A;
  newPatch.globalLevel = 0;
  std::strncpy(newPatch.name, newName, 8);
  // 書き込み
  ret = vt4->SavePatch(VT4::PRM_PATCHNO_8, newPatch);
  Check(ret == VT4::RetCode::OK, "Save Patch failed.");
  ret = vt4->RequestUserPatch(VT4::PRM_PATCHNO_8);
  ret = vt4->GetTemporaryPatch();
  prms = vt4->GetParameters();
  VT4::PatchData& savedPatch = (prms.userPatch[VT4::PRM_PATCHNO_8]);
  Check(ret == VT4::RetCode::OK, "Get Temporary Patch failed.");
  Check(newPatch.robot == savedPatch.robot, "ROBOT");
  Check(newPatch.harmony == savedPatch.harmony, "harmony");
  Check(newPatch.vocoder == savedPatch.vocoder, "vocoder");
  Check(newPatch.megaphone == savedPatch.megaphone, "megaphone");
  Check(newPatch.robotVariation == savedPatch.robotVariation, "robotVariation");
  Check(newPatch.harmonyVariation == savedPatch.harmonyVariation,
        "harmonyVariation");
  Check(newPatch.vocoderVariation == savedPatch.vocoderVariation,
        "vocoderVariation");
  Check(newPatch.megaphoneVariation == savedPatch.megaphoneVariation,
        "megaphoneVariation");
  Check(newPatch.reverbVariation == savedPatch.reverbVariation,
        "reverbVariation");
  Check(newPatch.pitch == savedPatch.pitch, "pitch");
  Check(newPatch.formant == savedPatch.formant, "formant");
  Check(newPatch.balance == savedPatch.balance, "balance");
  Check(newPatch.reverb == savedPatch.reverb, "reverb");
  Check(newPatch.autoPitch == savedPatch.autoPitch, "autoPitch");
  Check(newPatch.key == savedPatch.key, "key");
  Check(newPatch.globalLevel == savedPatch.globalLevel, "globalLevel");
  Check(prms.userPatch[VT4::PRM_PATCHNO_8].name[0] == newName[0],
        "save failed: patch name");
  // データ更新2
  new2Patch = newPatch;
  new2Patch.robot = VT4::PRM_ROBOT_OFF;
  new2Patch.harmony = VT4::PRM_HARMONY_ON;
  new2Patch.vocoder = VT4::PRM_VOCODER_OFF;
  new2Patch.megaphone = VT4::PRM_MEGAPHONE_OFF;
  new2Patch.robotVariation = 7;
  new2Patch.harmonyVariation = 7;
  new2Patch.vocoderVariation = 7;
  new2Patch.megaphoneVariation = 7;
  new2Patch.reverbVariation = 7;
  new2Patch.pitch = 255;
  new2Patch.formant = 255;
  new2Patch.balance = 255;
  new2Patch.reverb = 255;
  new2Patch.autoPitch = 255;
  new2Patch.key = VT4::PRM_KEY_B;
  new2Patch.globalLevel = 255;
  std::strncpy(new2Patch.name, newName2, 8);
  // 書き込み2
  ret = vt4->SavePatch(VT4::PRM_PATCHNO_8, new2Patch);
  Check(ret == VT4::RetCode::OK, "Save Patch failed.");
  Sleep(400);
  // パッチ読み込み
  ret = vt4->RequestUserPatch(VT4::PRM_PATCHNO_8);
  Sleep(400);
  ret = vt4->GetTemporaryPatch();
  prms = vt4->GetParameters();
  VT4::PatchData& saved2Patch = (prms.userPatch[VT4::PRM_PATCHNO_8]);
  Check(ret == VT4::RetCode::OK, "Get Temporary Patch failed.");
  Check(new2Patch.robot == saved2Patch.robot, "2: ROBOT");
  Check(new2Patch.harmony == saved2Patch.harmony, "2: harmony");
  Check(new2Patch.vocoder == saved2Patch.vocoder, "2: vocoder");
  Check(new2Patch.megaphone == saved2Patch.megaphone, "2: megaphone");
  Check(new2Patch.robotVariation == saved2Patch.robotVariation,
        "2: robotVariation");
  Check(new2Patch.harmonyVariation == saved2Patch.harmonyVariation,
        "2: harmonyVariation");
  Check(new2Patch.vocoderVariation == saved2Patch.vocoderVariation,
        "2: vocoderVariation");
  Check(new2Patch.megaphoneVariation == saved2Patch.megaphoneVariation,
        "2: megaphoneVariation");
  Check(new2Patch.reverbVariation == saved2Patch.reverbVariation,
        "2: reverbVariation");
  Check(new2Patch.pitch == saved2Patch.pitch, "2: pitch");
  Check(new2Patch.formant == savedPatch.formant, "2: formant");
  Check(new2Patch.balance == saved2Patch.balance, "2: balance");
  Check(new2Patch.reverb == saved2Patch.reverb, "2: reverb");
  Check(new2Patch.autoPitch == saved2Patch.autoPitch, "2: autoPitch");
  Check(new2Patch.key == saved2Patch.key, "2: key");
  Check(new2Patch.globalLevel == saved2Patch.globalLevel, "2: globalLevel");
  Check(saved2Patch.name[0] == newName2[0], "save failed: patch name");
  Check(saved2Patch.name[7] == newName2[7], "save failed: patch name");
  // 元に戻す
  vt4->SavePatch(VT4::PRM_PATCHNO_8, bakPatch);
  Check(vt4->Close() == VT4::RetCode::OK, "close failed.");
}

static void registerTests() { Testframework::RegisterTest(TTestVT4::Suite()); }
#pragma startup registerTests 33

