#include <System.hpp>
#pragma hdrstop

#include <TestFramework.hpp>
#include <vector>

#include "midi/MMMIDIIn.h"

class TTestMMMIDIIn : public TTestCase {
 public:
  __fastcall virtual TTestMMMIDIIn(System::String name) : TTestCase(name) {}
  virtual void __fastcall SetUp();
  virtual void __fastcall TearDown();

 __published:
  void __fastcall TestOpen();
  void __fastcall TestGetNumDevs();
  void __fastcall TestClose();
  void __fastcall TestReset();
  void __fastcall TestStart();
  void __fastcall TestStop();

 private:
  std::vector<MMMIDIIn*> devs;
};

void __fastcall TTestMMMIDIIn::SetUp() {
  UINT devNum = MMMIDIIn::GetNumDevs();
  devs.reserve(devNum);
  for (UINT i = 0; i < devNum; i++) {
    devs.push_back(new MMMIDIIn(i));
  }
}

void __fastcall TTestMMMIDIIn::TearDown() {
  for (auto e : devs) {
    e->Reset();
    e->Close();
  }
}

void __fastcall TTestMMMIDIIn::TestOpen() {
  if (devs.empty()) return;
  for (auto e : devs) {
    e->Open();
    CheckFalse(e->isError(), e->GetRecentError());
    e->Close();
    CheckFalse(e->isError(), e->GetRecentError());
  }
}

void __fastcall TTestMMMIDIIn::TestGetNumDevs() { MMMIDIIn::GetNumDevs(); }

void __fastcall TTestMMMIDIIn::TestClose() {
  devs.clear();
  UINT devNum = MMMIDIIn::GetNumDevs();
  devs.reserve(devNum);
  for (UINT i = 0; i < devNum; i++) {
    devs.push_back(new MMMIDIIn(i));
  }
  if (devs.empty()) return;
  for (auto e : devs) {
    e->Open();
    if (e->isError()) {
      continue;
    }
    e->Close();
    CheckFalse(e->isError(), e->GetRecentError());
  }
}

void __fastcall TTestMMMIDIIn::TestReset() {
  if (devs.empty()) return;
  for (auto e : devs) {
    e->Open();
    Check(e->Reset() == MMMIDIIn::RetCode::OK, e->GetRecentError());
  }
}

void __fastcall TTestMMMIDIIn::TestStart() {
  if (devs.empty()) return;
  for (auto e : devs) {
    e->Open();
    Check(e->Start() == MMMIDIIn::RetCode::OK, e->GetRecentError());
  }
}
void __fastcall TTestMMMIDIIn::TestStop() {
  if (devs.empty()) return;
  for (auto e : devs) {
    e->Open();
    Check(e->Stop() == MMMIDIIn::RetCode::OK, e->GetRecentError());
  }
}

static void registerTests() {
  Testframework::RegisterTest(TTestMMMIDIIn::Suite());
}
#pragma startup registerTests 33

