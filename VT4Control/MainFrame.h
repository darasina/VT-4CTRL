//---------------------------------------------------------------------------

#ifndef MainFrameH
#define MainFrameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.IniFiles.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.NumberBox.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.WinXCtrls.hpp>
#include <memory>

#include "vt-4/VT4.h"
//---------------------------------------------------------------------------
class TVT4ControlFrame : public TForm {
 __published:  // IDE で管理されるコンポーネント
  TTrackBar *PitchBar;
  TListBox *InMIDIList;
  TToggleSwitch *InMIDIOpenSwitch;
  TLabel *inMIDILabel;
  TLabel *OutMIDILabel;
  TListBox *OutMIDIList;
  TToggleSwitch *OutMIDIOpenSwitch;
  TTimer *Timer1;
  TButton *ReloadButton;
  TTrackBar *FormantBar;
  TButton *Patch1LoadButton;
  TButton *Patch1SaveButton;
  TToggleSwitch *SyncSwitch;
  TLabel *Label2;
  TToggleSwitch *SendSwitch;
  TLabel *Label3;
  TTrackBar *BalanceBar;
  TTrackBar *ReverbBar;
  TTrackBar *AutoPitchBar;
  TEdit *Patch1NameEdit;
  TGroupBox *TemporaryPatchGroup;
  TTrackBar *RobotBar;
  TGroupBox *PitchGroup;
  TGroupBox *RobotGroup;
  TGroupBox *Patch1Group;
  TNumberBox *PitchNumber;
  TGroupBox *VocoderGroup;
  TToggleSwitch *VocoderSwitch;
  TGroupBox *HarmonyGroup;
  TToggleSwitch *HarmonySwitch;
  TToggleSwitch *RobotSwitch;
  TGroupBox *MegaphoneGroup;
  TToggleSwitch *MegaphoneSwitch;
  TNumberBox *VocoderVarNumber;
  TNumberBox *RobotVarNumber;
  TButton *ManualButton;
  TGroupBox *Patch8Group;
  TButton *Patch8LoadButton;
  TEdit *Patch8NameEdit;
  TButton *Patch8SaveButton;
  TNumberBox *MegaphoneVarNumber;
  TNumberBox *HarmonyVarNumber;
  TGroupBox *ReverbVarGroup;
  TNumberBox *ReverbVarNumber;
  TGroupBox *FormantGroup;
  TNumberBox *FormantNumber;
  TGroupBox *AutoPitchGroup;
  TNumberBox *AutoPitchNumber;
  TGroupBox *BalanceGroup;
  TNumberBox *BalanceNumber;
  TGroupBox *ReverbGroup;
  TNumberBox *ReverbNumber;
  TPageControl *PatchPageControl;
  TTabSheet *Patch1_4TabSheet;
  TTabSheet *Patch5_8TabSheet;
  TGroupBox *Patch2Group;
  TButton *Patch2LoadButton;
  TEdit *Patch2NameEdit;
  TButton *Patch2SaveButton;
  TGroupBox *Patch3Group;
  TButton *Patch3LoadButton;
  TEdit *Patch3NameEdit;
  TButton *Patch3SaveButton;
  TGroupBox *Patch4Group;
  TButton *Patch4LoadButton;
  TEdit *Patch4NameEdit;
  TButton *Patch4SaveButton;
  TGroupBox *Patch5Group;
  TButton *Patch5LoadButton;
  TEdit *Patch5NameEdit;
  TButton *Patch5SaveButton;
  TGroupBox *GroupBox1;
  TButton *Patch6LoadButton;
  TEdit *Patch6NameEdit;
  TButton *Patch6SaveButton;
  TGroupBox *GroupBox2;
  TButton *Patch7LoadButton;
  TEdit *Patch7NameEdit;
  TButton *Patch7SaveButton;
  TToggleSwitch *SaveLockSwitch;
  TLabel *Label1;
  TTrackBar *VolumeBar;
  TTrackBar *MicSensBar;
  TNumberBox *VolumeNumber;
  TLabel *Label4;
  TLabel *Label5;
  TNumberBox *MicSensNumber;
  TToggleSwitch *ToggleSwitch1;
  TLabel *Label6;
  TEdit *TempPatchNameEdit;
  TLabel *Label7;
  TGroupBox *KeyGroup;
  TNumberBox *KeyNumberBox;
  TTrackBar *KeyBar;
  void __fastcall Timer1Timer(TObject *Sender);
  void __fastcall PitchBarChange(TObject *Sender);
  void __fastcall PitchNumberChangeValue(TObject *Sender);
  void __fastcall ReloadButtonClick(TObject *Sender);
  void __fastcall FormantBarChange(TObject *Sender);
  void __fastcall RobotBarChange(TObject *Sender);
  void __fastcall Patch1NameEditExit(TObject *Sender);
  void __fastcall BalanceBarChange(TObject *Sender);
  void __fastcall ReverbBarChange(TObject *Sender);
  void __fastcall RobotSwitchClick(TObject *Sender);
  void __fastcall ManualButtonClick(TObject *Sender);
  void __fastcall SaveLockSwitchClick(TObject *Sender);
  void __fastcall MegaphoneSwitchClick(TObject *Sender);
  void __fastcall VocoderSwitchClick(TObject *Sender);
  void __fastcall HarmonySwitchClick(TObject *Sender);
  void __fastcall VolumeBarChange(TObject *Sender);
  void __fastcall MicSensBarChange(TObject *Sender);
  void __fastcall Patch1LoadButtonClick(TObject *Sender);
  void __fastcall Patch2LoadButtonClick(TObject *Sender);
  void __fastcall Patch3LoadButtonClick(TObject *Sender);
  void __fastcall Patch4LoadButtonClick(TObject *Sender);
  void __fastcall Patch5LoadButtonClick(TObject *Sender);
  void __fastcall Patch6LoadButtonClick(TObject *Sender);
  void __fastcall Patch7LoadButtonClick(TObject *Sender);
  void __fastcall Patch8LoadButtonClick(TObject *Sender);
  void __fastcall FormantNumberChangeValue(TObject *Sender);
  void __fastcall BalanceNumberChangeValue(TObject *Sender);
  void __fastcall ReverbNumberChangeValue(TObject *Sender);
  void __fastcall AutoPitchBarChange(TObject *Sender);
  void __fastcall AutoPitchNumberChangeValue(TObject *Sender);
  void __fastcall Patch8SaveButtonClick(TObject *Sender);
  void __fastcall Patch1SaveButtonClick(TObject *Sender);
  void __fastcall KeyBarChange(TObject *Sender);
  void __fastcall RobotVarNumberChangeValue(TObject *Sender);
  void __fastcall MegaphoneVarNumberChangeValue(TObject *Sender);
  void __fastcall VocoderVarNumberChangeValue(TObject *Sender);
  void __fastcall HarmonyVarNumberChangeValue(TObject *Sender);
  void __fastcall ReverbVarNumberChangeValue(TObject *Sender);
  void __fastcall Patch6SaveButtonClick(TObject *Sender);
  void __fastcall Patch2SaveButtonClick(TObject *Sender);
  void __fastcall Patch3SaveButtonClick(TObject *Sender);
  void __fastcall Patch4SaveButtonClick(TObject *Sender);
  void __fastcall Patch5SaveButtonClick(TObject *Sender);
  void __fastcall Patch7SaveButtonClick(TObject *Sender);

 private:  // ユーザー宣言
  // clang-format off
  // SYSEX送信コード
  enum { REQ_TEMP_PATCH = 0,
		 REQ_SEND_PITCH,
		 REQ_USER_PATCH_1,
		 REQ_USER_PATCH_2,
		 REQ_USER_PATCH_3,
		 REQ_USER_PATCH_4,
		 REQ_USER_PATCH_5,
		 REQ_USER_PATCH_6,
		 REQ_USER_PATCH_7,
		 REQ_USER_PATCH_8,
		 REQ_SAVE_PATCH_8,
		 REQ_SEND_USER_PATCH_1_NAME,
		 REQ_MAX
	   };
  // clang-format on
  float midiPrevOutputMSec;
  float midiOutputInterval = 500;
  void requestSYSEX(int requestNo);
  bool sysexSendRequest[REQ_MAX];
  bool receiveNow = false;
  bool waitRQDT = false;
  VT4::Parameters vt4prms;
  static void midiInCallback(void *ptr, UINT wMsg, DWORD dwParam1,
                             DWORD dwParam2) {
    ((TVT4ControlFrame *)ptr)->callback(wMsg, dwParam1, dwParam2);
  }
  void callback(UINT wMsg, DWORD dwParam1, DWORD dwParam2);
  void getTemporaryPatchFromVT4();
  void midiShortMsg(DWORD dwMidiMessage, DWORD dwTimestamp);
  void packTemporaryPatch();

 public:  // ユーザー宣言
  std::unique_ptr<VT4> vt4;
  std::unique_ptr<TMemIniFile> ini;
  __fastcall TVT4ControlFrame(TComponent *Owner);
  __fastcall ~TVT4ControlFrame();
};
//---------------------------------------------------------------------------
extern PACKAGE TVT4ControlFrame *VT4ControlFrame;
//---------------------------------------------------------------------------
#endif

