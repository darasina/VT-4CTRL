//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVT4ControlFrame* VT4ControlFrame;
//---------------------------------------------------------------------------
__fastcall TVT4ControlFrame::TVT4ControlFrame(TComponent* Owner)
    : TForm(Owner) {
  VT4::RetCode ret;
  // UI初期化
  InMIDIOpenSwitch->State = tssOff;
  OutMIDIOpenSwitch->State = tssOff;
  SyncSwitch->State = tssOff;
  SendSwitch->State = tssOff;
  TemporaryPatchGroup->Enabled = false;

  // 設定ファイル
  ini = std::make_unique<TMemIniFile>("settings.ini");
  ini->AutoSave = true;
  VolumeBar->Position = ini->ReadInteger("VT4", "VOLUME", 127);
  MicSensBar->Position = ini->ReadInteger("VT4", "MIC_SENS", 127);

  vt4 = std::make_unique<VT4>();
  // USB接続のVT4を探す
  ret = vt4->SearchVT4DevID();
  if (ret != VT4::RetCode::OK) {
    // みつからなかった
    return;
  }
  InMIDIList->AddItem(vt4->GetInDevName(), nullptr);
  InMIDIList->SelectAll();
  OutMIDIList->AddItem(vt4->GetOutDevName(), nullptr);
  vt4->SetInCallback(this, midiInCallback);
  ret = vt4->Open();
  if (ret != VT4::RetCode::OK) {
    return;
  }
  if (vt4->isInDevOpened()) {
    InMIDIList->Enabled = false;
    InMIDIOpenSwitch->State = tssOn;
    SyncSwitch->State = tssOn;
    TemporaryPatchGroup->Enabled = true;
    requestSYSEX(REQ_TEMP_PATCH);
    requestSYSEX(REQ_USER_PATCH_1);
    requestSYSEX(REQ_USER_PATCH_2);
    requestSYSEX(REQ_USER_PATCH_3);
    requestSYSEX(REQ_USER_PATCH_4);
    requestSYSEX(REQ_USER_PATCH_5);
    requestSYSEX(REQ_USER_PATCH_6);
    requestSYSEX(REQ_USER_PATCH_7);
    requestSYSEX(REQ_USER_PATCH_8);
  } else {
    InMIDIOpenSwitch->State = tssOff;
  }
  if (vt4->isOutDevOpened()) {
    OutMIDIList->Enabled = false;
    OutMIDIOpenSwitch->State = tssOn;
  } else {
    OutMIDIOpenSwitch->State = tssOff;
  }
  if (vt4->isOutDevOpened() && vt4->isInDevOpened()) {
    SendSwitch->State = tssOn;
  }

  // UI連動処理
  SaveLockSwitchClick(this);
}

__fastcall TVT4ControlFrame::~TVT4ControlFrame() {
  // MIDI入力コールバック解除
  vt4->UnsetInCallback();
  // 設定ファイル書き込み
  ini->WriteInteger("VT4", "VOLUME", VolumeBar->Position);
  ini->WriteInteger("VT4", "MIC_SENS", MicSensBar->Position);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::PitchBarChange(TObject* Sender) {
  PitchNumber->Value = PitchBar->Max - PitchBar->Position;
  if (SendSwitch->State == tssOn && !receiveNow) {
    // 軸が上下逆なので変換
    vt4->SendTempPitch(PitchBar->Max - PitchBar->Position);
  }
}
//---------------------------------------------------------------------------

/**
 * SYSEXリクエストを設定する
 */
void TVT4ControlFrame::requestSYSEX(int requestNo) {
  if (!sysexSendRequest[requestNo]) {
    sysexSendRequest[requestNo] = true;
  }
  if (!Timer1->Enabled) {
    Timer1->Enabled = true;
  }
}

void __fastcall TVT4ControlFrame::Timer1Timer(TObject* Sender) {
  // 同期OFFならなにもしない
  if (SyncSwitch->State == tssOff) {
    return;
  }
  // 前回のリクエストの応答待ちならなにもしない
  if (waitRQDT) {
    return;
  }

  // リクエストを優先度順にみて1つ実行する
  for (int i = 0; i < REQ_MAX; i++) {
    if (sysexSendRequest[i]) {
      // リクエストあり
      sysexSendRequest[i] = false;  // リクエスト処理済み
      switch (i) {
        case REQ_TEMP_PATCH:
          waitRQDT = true;
          vt4->RequestTemporaryPatch();
          break;
        case REQ_USER_PATCH_1:
          waitRQDT = true;
          vt4->RequestUserPatch(VT4::PRM_PATCHNO_1);
          break;
        case REQ_USER_PATCH_2:
          waitRQDT = true;
          vt4->RequestUserPatch(VT4::PRM_PATCHNO_2);
          break;
        case REQ_USER_PATCH_3:
          waitRQDT = true;
          vt4->RequestUserPatch(VT4::PRM_PATCHNO_3);
          break;
        case REQ_USER_PATCH_4:
          waitRQDT = true;
          vt4->RequestUserPatch(VT4::PRM_PATCHNO_4);
          break;
        case REQ_USER_PATCH_5:
          waitRQDT = true;
          vt4->RequestUserPatch(VT4::PRM_PATCHNO_5);
          break;
        case REQ_USER_PATCH_6:
          waitRQDT = true;
          vt4->RequestUserPatch(VT4::PRM_PATCHNO_6);
          break;
        case REQ_USER_PATCH_7:
          waitRQDT = true;
          vt4->RequestUserPatch(VT4::PRM_PATCHNO_7);
          break;
        case REQ_USER_PATCH_8:
          waitRQDT = true;
          vt4->RequestUserPatch(VT4::PRM_PATCHNO_8);
          break;
        case REQ_SAVE_PATCH_8:
          vt4->SavePatch(VT4::PRM_PATCHNO_8,
                         vt4prms.userPatch[VT4::PRM_PATCHNO_8]);
          break;
        case REQ_SEND_PITCH:
          if (SendSwitch->State == tssOn) {
            // 送信ONなら
            waitRQDT = true;
            vt4->SendPitch(VT4::PRM_PATCHNO_TEMPORARY, PitchBar->Position);
          }
          break;
        case REQ_SEND_USER_PATCH_1_NAME:
          if (SendSwitch->State == tssOn) {
            char pn[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            size_t len = 8;
            if (Patch1NameEdit->Text.Length() < 8) {
              len = Patch1NameEdit->Text.Length();
            }
            for (int i = 0; i < len; i++) {
              pn[i] = Patch1NameEdit->Text.c_str()[i];
            }

            waitRQDT = true;
            vt4->SendPatchName0_3(VT4::PRM_PATCHNO_1, pn);
          }
          break;
        default:
          assert(false);
          break;
      }
      // 1つ実行したら処理返す(タイマーウェイトする: 複数SYSEXを連続送信しない)
      return;
    }
  }
  // 一つもリクエストがなかったらタイマーを停止してもいい気がする
}

void TVT4ControlFrame::callback(UINT wMsg, DWORD dwParam1, DWORD dwParam2) {
  // MIDI入力処理[開始]
  receiveNow = true;
  switch (wMsg) {
    case MIM_DATA:
      midiShortMsg(dwParam1, dwParam2);
      break;
    case MIM_LONGDATA:
      waitRQDT = false;  // 応答待ちを解除
      getTemporaryPatchFromVT4();
      break;
  }
  // MIDI入力処理[完了]
  receiveNow = false;
}

/**
 * テンポラリパッチ変数のデータを画面に反映する
 */
void TVT4ControlFrame::getTemporaryPatchFromVT4() {
  vt4prms = vt4->GetParameters();
  VT4::Parameters& p = vt4prms;
  // パッチ名
  Patch1NameEdit->Text = p.userPatch[VT4::PRM_PATCHNO_1].name;
  Patch2NameEdit->Text = p.userPatch[VT4::PRM_PATCHNO_2].name;
  Patch3NameEdit->Text = p.userPatch[VT4::PRM_PATCHNO_3].name;
  Patch4NameEdit->Text = p.userPatch[VT4::PRM_PATCHNO_4].name;
  Patch5NameEdit->Text = p.userPatch[VT4::PRM_PATCHNO_5].name;
  Patch6NameEdit->Text = p.userPatch[VT4::PRM_PATCHNO_6].name;
  Patch7NameEdit->Text = p.userPatch[VT4::PRM_PATCHNO_7].name;
  Patch8NameEdit->Text = p.userPatch[VT4::PRM_PATCHNO_8].name;

  if (!p.temporaryPatch._updated) {
    // テンポラリパッチが未取得ならテンポラリパッチの画面反映しない
    // 未初期化の値がVT4に送信されないように
    return;
  }

  TempPatchNameEdit->Text = p.temporaryPatch.name;

  // エフェクトスイッチ
  // ROBOTだけMIDI IN=2がある
  RobotBar->Position = p.temporaryPatch.robot;
  if (p.temporaryPatch.robot == 0) {
    RobotSwitch->State = tssOff;
  } else {
    RobotSwitch->State = tssOn;
  }
  MegaphoneSwitch->State =
      p.temporaryPatch.megaphone == VT4::PRM_MEGAPHONE_OFF ? tssOff : tssOn;
  VocoderSwitch->State =
      p.temporaryPatch.vocoder == VT4::PRM_VOCODER_OFF ? tssOff : tssOn;
  HarmonySwitch->State =
      p.temporaryPatch.harmony == VT4::PRM_HARMONY_OFF ? tssOff : tssOn;

  // 軸が上下逆なので変換
  PitchBar->Position = PitchBar->Max - p.temporaryPatch.pitch;
  FormantBar->Position = FormantBar->Max - p.temporaryPatch.formant;
  BalanceBar->Position = BalanceBar->Max - p.temporaryPatch.balance;
  ReverbBar->Position = ReverbBar->Max - p.temporaryPatch.reverb;
  // そのまま
  AutoPitchBar->Position = p.temporaryPatch.autoPitch;
  KeyBar->Position = p.temporaryPatch.key;

  // エフェクトバリエーション
  // 内部データはzero originなのでUI表示値は+1
  RobotVarNumber->Value = p.temporaryPatch.robotVariation + 1;
  MegaphoneVarNumber->Value = p.temporaryPatch.megaphoneVariation + 1;
  VocoderVarNumber->Value = p.temporaryPatch.vocoderVariation + 1;
  HarmonyVarNumber->Value = p.temporaryPatch.harmonyVariation + 1;
  ReverbVarNumber->Value = p.temporaryPatch.reverbVariation + 1;
}

/**
 * 画面上の設定値をテンポラリパッチ変数に反映する
 */
void TVT4ControlFrame::packTemporaryPatch() {
  // エフェクトボリューム
  VT4::PatchData& temp = vt4prms.temporaryPatch;
  temp.pitch = PitchBar->Max - PitchBar->Position;
  temp.formant = FormantBar->Max - FormantBar->Position;
  temp.autoPitch = AutoPitchBar->Position;
  temp.balance = BalanceBar->Max - BalanceBar->Position;
  temp.reverb = ReverbBar->Max - ReverbBar->Position;

  // エフェクトスイッチ
  temp.robot =
      RobotSwitch->State == tssOn ? VT4::PRM_ROBOT_ON : VT4::PRM_ROBOT_OFF;
  temp.megaphone = MegaphoneSwitch->State == tssOn ? VT4::PRM_MEGAPHONE_ON
                                                   : VT4::PRM_MEGAPHONE_OFF;
  temp.vocoder = VocoderSwitch->State == tssOn ? VT4::PRM_VOCODER_ON
                                               : VT4::PRM_VOCODER_OFF;
  temp.harmony = HarmonySwitch->State == tssOn ? VT4::PRM_HARMONY_ON
                                               : VT4::PRM_HARMONY_OFF;

  // エフェクトヴァリエーション
  // UI表示値は+1しているので-1して内部データに書き込む
  temp.robotVariation = RobotVarNumber->Value - 1;
  temp.megaphoneVariation = MegaphoneVarNumber->Value - 1;
  temp.vocoderVariation = VocoderVarNumber->Value - 1;
  temp.harmonyVariation = HarmonyVarNumber->Value - 1;
  temp.reverbVariation = ReverbVarNumber->Value - 1;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::ReloadButtonClick(TObject* Sender) {
  requestSYSEX(REQ_TEMP_PATCH);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::FormantBarChange(TObject* Sender) {
  FormantNumber->Value = FormantBar->Max - FormantBar->Position;
  if (SendSwitch->State == tssOn && !receiveNow) {
    // 軸が上下逆なので変換
    vt4->SendTempFormant(FormantBar->Max - FormantBar->Position);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::RobotBarChange(TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    // vt4->SendTempRobot(RobotBar->Position);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::RobotSwitchClick(TObject* Sender) {
  int pos = 0;
  if (RobotSwitch->State == tssOff) {
    pos = 0;
  } else
    pos = 1;

  if (SendSwitch->State == tssOn && !receiveNow) {
    RobotBar->Position = pos;
    vt4->SendTempRobot(pos);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch1NameEditExit(TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    requestSYSEX(REQ_SEND_USER_PATCH_1_NAME);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::BalanceBarChange(TObject* Sender) {
  BalanceNumber->Value = BalanceBar->Max - BalanceBar->Position;
  if (SendSwitch->State == tssOn && !receiveNow) {
    // 軸が上下逆なので変換
    vt4->SendTempBalance(BalanceBar->Max - BalanceBar->Position);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::ReverbBarChange(TObject* Sender) {
  ReverbNumber->Value = ReverbBar->Max - ReverbBar->Position;
  if (SendSwitch->State == tssOn && !receiveNow) {
    // 軸が上下逆なので変換
    vt4->SendTempReverb(ReverbBar->Max - ReverbBar->Position);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::PitchNumberChangeValue(TObject* Sender) {
  PitchBar->Position = PitchBar->Max - PitchNumber->Value;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::ManualButtonClick(TObject* Sender) {
  if (!receiveNow) {
    vt4->LoadScene(VT4::SCENE_MANUAL);
    requestSYSEX(REQ_TEMP_PATCH);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::SaveLockSwitchClick(TObject* Sender) {
  bool lockEnable = false;
  if (SaveLockSwitch->State == tssOn) {
    lockEnable = false;
  } else {
    lockEnable = true;
  }
  Patch1SaveButton->Enabled = lockEnable;
  Patch2SaveButton->Enabled = lockEnable;
  Patch3SaveButton->Enabled = lockEnable;
  Patch4SaveButton->Enabled = lockEnable;
  Patch5SaveButton->Enabled = lockEnable;
  Patch6SaveButton->Enabled = lockEnable;
  Patch7SaveButton->Enabled = lockEnable;
  Patch8SaveButton->Enabled = lockEnable;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::MegaphoneSwitchClick(TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    if (MegaphoneSwitch->State == tssOn) {
      vt4->SendTempMegaphone(VT4::PRM_MEGAPHONE_ON);
    } else
      vt4->SendTempMegaphone(VT4::PRM_MEGAPHONE_OFF);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::VocoderSwitchClick(TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    if (VocoderSwitch->State == tssOn) {
      vt4->SendTempVocoder(VT4::PRM_VOCODER_ON);
    } else
      vt4->SendTempVocoder(VT4::PRM_VOCODER_OFF);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::HarmonySwitchClick(TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    if (HarmonySwitch->State == tssOn) {
      vt4->SendTempHarmony(VT4::PRM_HARMONY_ON);
    } else
      vt4->SendTempHarmony(VT4::PRM_HARMONY_OFF);
  }
}
//---------------------------------------------------------------------------

/**
 * 受信したMIM_DATA対応処理
 */
void TVT4ControlFrame::midiShortMsg(DWORD dwMidiMessage, DWORD dwTimestamp) {
  unsigned char stat = dwMidiMessage & 0x000000F0;
  unsigned char ch = dwMidiMessage & 0x0000000F;
  unsigned char data1 = (dwMidiMessage & 0x0000FF00) >> 8;
  unsigned char data2 = (dwMidiMessage & 0x00FF0000) >> 16;
  // ステータス
  switch (stat) {
    case 0xB0:
      // コントロール番号
      switch (data1) {
        case VT4::CTRLCHG_VOLUME:
          VolumeBar->Position = data2;
          break;
        case VT4::CTRLCHG_MIC_SENS:
          MicSensBar->Position = data2;
          break;
      }
      break;
  }
}

void __fastcall TVT4ControlFrame::VolumeBarChange(TObject* Sender) {
  VolumeNumber->Value = VolumeBar->Position;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::MicSensBarChange(TObject* Sender) {
  MicSensNumber->Value = MicSensBar->Position;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch1LoadButtonClick(TObject* Sender) {
  if (!receiveNow) {
    vt4->LoadScene(VT4::SCENE_1);
    requestSYSEX(REQ_TEMP_PATCH);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch2LoadButtonClick(TObject* Sender) {
  if (!receiveNow) {
    vt4->LoadScene(VT4::SCENE_2);
    requestSYSEX(REQ_TEMP_PATCH);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch3LoadButtonClick(TObject* Sender) {
  if (!receiveNow) {
    vt4->LoadScene(VT4::SCENE_3);
    requestSYSEX(REQ_TEMP_PATCH);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch4LoadButtonClick(TObject* Sender) {
  if (!receiveNow) {
    vt4->LoadScene(VT4::SCENE_4);
    requestSYSEX(REQ_TEMP_PATCH);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch5LoadButtonClick(TObject* Sender) {
  if (!receiveNow) {
    vt4->LoadScene(VT4::SCENE_5);
    requestSYSEX(REQ_TEMP_PATCH);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch6LoadButtonClick(TObject* Sender) {
  if (!receiveNow) {
    vt4->LoadScene(VT4::SCENE_6);
    requestSYSEX(REQ_TEMP_PATCH);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch7LoadButtonClick(TObject* Sender) {
  if (!receiveNow) {
    vt4->LoadScene(VT4::SCENE_7);
    requestSYSEX(REQ_TEMP_PATCH);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch8LoadButtonClick(TObject* Sender) {
  if (!receiveNow) {
    vt4->LoadScene(VT4::SCENE_8);
    requestSYSEX(REQ_TEMP_PATCH);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch1SaveButtonClick(TObject* Sender) {
  packTemporaryPatch();
  vt4->SavePatch(VT4::PRM_PATCHNO_1, vt4prms.temporaryPatch);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch2SaveButtonClick(TObject* Sender) {
  packTemporaryPatch();
  vt4->SavePatch(VT4::PRM_PATCHNO_2, vt4prms.temporaryPatch);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch3SaveButtonClick(TObject* Sender) {
  packTemporaryPatch();
  vt4->SavePatch(VT4::PRM_PATCHNO_3, vt4prms.temporaryPatch);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch4SaveButtonClick(TObject* Sender) {
  packTemporaryPatch();
  vt4->SavePatch(VT4::PRM_PATCHNO_4, vt4prms.temporaryPatch);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch5SaveButtonClick(TObject* Sender) {
  packTemporaryPatch();
  vt4->SavePatch(VT4::PRM_PATCHNO_5, vt4prms.temporaryPatch);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch6SaveButtonClick(TObject* Sender) {
  packTemporaryPatch();
  vt4->SavePatch(VT4::PRM_PATCHNO_6, vt4prms.temporaryPatch);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch7SaveButtonClick(TObject* Sender) {
  packTemporaryPatch();
  vt4->SavePatch(VT4::PRM_PATCHNO_7, vt4prms.temporaryPatch);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::Patch8SaveButtonClick(TObject* Sender) {
  packTemporaryPatch();
  vt4->SavePatch(VT4::PRM_PATCHNO_8, vt4prms.temporaryPatch);
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::FormantNumberChangeValue(TObject* Sender) {
  FormantBar->Position = FormantBar->Max - FormantNumber->Value;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::BalanceNumberChangeValue(TObject* Sender) {
  BalanceBar->Position = BalanceBar->Max - BalanceNumber->Value;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::ReverbNumberChangeValue(TObject* Sender) {
  ReverbBar->Position = ReverbBar->Max - ReverbNumber->Value;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::AutoPitchBarChange(TObject* Sender) {
  AutoPitchNumber->Value = AutoPitchBar->Position;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::AutoPitchNumberChangeValue(TObject* Sender) {
  AutoPitchBar->Position = AutoPitchNumber->Value;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::KeyBarChange(TObject* Sender) {
  KeyNumberBox->Value = KeyBar->Position;
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::RobotVarNumberChangeValue(TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    // 1-8 -> 0-7
    vt4->SendTempRobotVariation(RobotVarNumber->Value - 1);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::MegaphoneVarNumberChangeValue(
    TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    // 1-8 -> 0-7
    vt4->SendTempMegaphoneVariation(MegaphoneVarNumber->Value - 1);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::VocoderVarNumberChangeValue(TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    // 1-8 -> 0-7
    vt4->SendTempVocoderVariation(VocoderVarNumber->Value - 1);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::HarmonyVarNumberChangeValue(TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    // 1-8 -> 0-7
    vt4->SendTempHarmonyVariation(HarmonyVarNumber->Value - 1);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVT4ControlFrame::ReverbVarNumberChangeValue(TObject* Sender) {
  if (SendSwitch->State == tssOn && !receiveNow) {
    // 1-8 -> 0-7
    vt4->SendTempReverbVariation(ReverbVarNumber->Value - 1);
  }
}
//---------------------------------------------------------------------------

