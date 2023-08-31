//---------------------------------------------------------------------------

#ifndef VT4H
#define VT4H

#include <windows.h>

#include <memory>
#include <mutex>

#include "midi/MMMIDIIn.h"
#include "midi/MMMIDIOut.h"
//---------------------------------------------------------------------------
class VT4 {
  friend class TTestVT4;

 public:
  static const unsigned int manID = 0x41;
  static const unsigned int devID = 0x10;
  enum class RetCode { OK, Error };
  // パッチのインデックス番号(SYSEXで使用)
  enum {
	PRM_PATCHNO_TEMPORARY = -1,  /// テンポラリパッチ
	PRM_PATCHNO_1 = 0,
	PRM_PATCHNO_2 = 1,
	PRM_PATCHNO_3 = 2,
	PRM_PATCHNO_4 = 3,
	PRM_PATCHNO_5 = 4,
	PRM_PATCHNO_6 = 5,
	PRM_PATCHNO_7 = 6,
	PRM_PATCHNO_8 = 7,
  };
  // シーンのMIDICH(プログラムチェンジで使用)
  enum {
    SCENE_MANUAL = 0,
    SCENE_1 = 1,
    SCENE_2 = 2,
    SCENE_3 = 3,
    SCENE_4 = 4,
    SCENE_5 = 5,
    SCENE_6 = 6,
    SCENE_7 = 7,
    SCENE_8 = 8,
  };
  enum {
	PRM_ROBOT_OFF      = 0,
	PRM_ROBOT_ON       = 1,
	PRM_ROBOT_MIDI_IN  = 2,
  };
  enum {
	PRM_HARMONY_OFF    = 0,
    PRM_HARMONY_ON     = 1
  };
  enum {
	PRM_VOCODER_OFF    = 0,
    PRM_VOCODER_ON     = 1
  };
  enum {
	PRM_MEGAPHONE_OFF    = 0,
	PRM_MEGAPHONE_ON     = 1
  };
  static const unsigned int PRM_MAX_ROBOT_VARIATION = 7;
  static const unsigned int PRM_MAX_HARMONY_VARIATION = 7;
  static const unsigned int PRM_MAX_VOCODER_VARIATION = 7;
  static const unsigned int PRM_MAX_MEGAPHONE_VARIATION = 7;
  static const unsigned int PRM_MAX_REVERB_VARIATION = 7;
  enum {
	PRM_KEY_C  = 0,
	PRM_KEY_CS = 1,
	PRM_KEY_D  = 2,
	PRM_KEY_DS = 3,
	PRM_KEY_E  = 4,
	PRM_KEY_F  = 5,
	PRM_KEY_FS = 6,
	PRM_KEY_G  = 7,
	PRM_KEY_GS = 8,
	PRM_KEY_A  = 9,
	PRM_KEY_AS = 10,
	PRM_KEY_B  = 11,
  };
   enum ControlChange {
	CTRLCHG_MODULATION          = 1,
	CTRLCHG_VOLUME              = 46,
	CTRLCHG_MIC_SENS            = 47,
	CTRLCHG_KEY                 = 48,
	CTRLCHG_ROBOT               = 49,
	CTRLCHG_MEGAPHONE           = 50,
	CTRLCHG_BYPASS              = 51,
	CTRLCHG_VOCODER             = 52,
	CTRLCHG_HARMONY             = 53,
	CTRLCHG_FORMANT             = 54,
	CTRLCHG_AUTO_PITCH          = 55,
	CTRLCHG_BALANCE             = 56,
	CTRLCHG_REVERB              = 57,
	CTRLCHG_LINE_OUT_SELECT     = 58,
	CTRLCHG_MODULATION_RATE     = 76,
	CTRLCHG_ROBOT_VARIATION     = 79,
	CTRLCHG_MEGAPHONE_VARIATION = 80,
	CTRLCHG_VOCODER_VARIATION   = 81,
	CTRLCHG_HARMONY_VARIATION   = 82,
	CTRLCHG_REVERB_VARIATION    = 83,
  };

  ~VT4();
  RetCode SearchVT4DevID();
  RetCode Open();
  RetCode Close();
  char* GetInDevName();
  char* GetOutDevName();
  static unsigned int GetInDevNums() { return MMMIDIIn::GetNumDevs(); }
  bool isInDevOpened();
  bool isOutDevOpened();
  RetCode RequestIdentity();
  typedef std::function<void(void* instancd, UINT wMsg, DWORD dwParam1,
                             DWORD dwParam2)>
      CallbackFunc;
  void SetInCallback(void* instance, CallbackFunc callback);
  /**
   * MIDI入力コールバック設定を解除する
   */
  void UnsetInCallback() {
    extInCallback = nullptr;
    extInCallbackInstance = nullptr;
  }
  RetCode SendSysEx(const char* longdata, size_t len);
  /**
   * アイデンティティーメッセージデータ
   */
  struct Identity {
    unsigned char idNum = 0;
    unsigned char deviceID;
    unsigned char subID1;
    unsigned char subID2;
    unsigned char manID;
  };
  /**
   * DT1データ
   */
  struct DT1 {
    unsigned char stat;
    unsigned char id;
    unsigned char devID;
    unsigned char modelID[4];
    unsigned char commandID;
    unsigned char addrs[4];
    unsigned char* data;
    unsigned char checkSum;
    unsigned char eox;
  };
  /**
   * システムパラメータデータ
   */
  struct SystemData {
    unsigned int midiCh;
    unsigned int gateLevel;
  };
  /**
   * パッチパラメータ
   */
  struct PatchData {
	int patchNo;  /// -1: Temporary, 0: User 1 - 7: User 8
    bool _updated = false;
    unsigned int robot;
    unsigned int harmony;
    unsigned int vocoder;
    unsigned int megaphone;
    unsigned int robotVariation;
    unsigned int harmonyVariation;
    unsigned int vocoderVariation;
    unsigned int megaphoneVariation;
    unsigned int reverbVariation;
    unsigned int pitch;
    unsigned int formant;
    unsigned int balance;
    unsigned int reverb;
    unsigned int autoPitch;
    unsigned int key;
    unsigned int globalLevel;
    char name[9];
  };
  /**
   * パラメータ一式
   */
  struct Parameters {
    SystemData system;
    PatchData temporaryPatch;
    PatchData userPatch[8];
  };
  Identity GetIdentity();
  int GetVolume();
  static unsigned char CheckSum(const unsigned char data[], size_t len);
  Parameters GetParameters();
  RetCode RequestSystemData();
  RetCode RequestTemporaryPatch();
  RetCode GetTemporaryPatch();
  RetCode RequestUserPatch(unsigned char patchNo);
  RetCode GetUserPatch(unsigned char patchNo);
  RetCode SendMIDICh(unsigned char midiCh);
  RetCode SendRobot(int patchNo, unsigned char value);
  RetCode SendPatchName0_3(int patchNo, char* name0_3);
  RetCode SendPatchName4_7(int patchNo, char* name4_7);
  RetCode SendTempPitch(unsigned char value);
  RetCode SendPitch(int patchNo, unsigned char value);
  RetCode SendPitchSysEx(int patchNo, unsigned char value);
  RetCode SendTempFormant(unsigned char value);
  RetCode SendTempBalance(unsigned char value);
  RetCode SendTempReverb(unsigned char value);
  RetCode SendTempRobot(unsigned char value);
  RetCode SendTempHarmony(unsigned char value);
  RetCode SendTempVocoder(unsigned char value);
  RetCode SendTempMegaphone(unsigned char value);
  RetCode SendTempRobotVariation(unsigned char value);
  RetCode SendTempMegaphoneVariation(unsigned char value);
  RetCode SendTempVocoderVariation(unsigned char value);
  RetCode SendTempHarmonyVariation(unsigned char value);
  RetCode SendTempReverbVariation(unsigned char value);
  RetCode LoadScene(unsigned char value);
  RetCode SavePatch(int patchNo, PatchData& patch);

 private:
  // clang-format off
  enum ParameterStartAddress : unsigned int {
	PRM_ADRS_SYSTEM				= 0x00000000,
	PRM_ADRS_TEMPORARY_PATCH 	= 0x10000000,
	PRM_ADRS_USER_PATCH_1 	    = 0x11000000,
	PRM_ADRS_USER_PATCH_2 	    = 0x11010000,
	PRM_ADRS_USER_PATCH_3 	    = 0x11020000,
	PRM_ADRS_USER_PATCH_4 	    = 0x11030000,
	PRM_ADRS_USER_PATCH_5 	    = 0x11040000,
	PRM_ADRS_USER_PATCH_6 	    = 0x11050000,
	PRM_ADRS_USER_PATCH_7 	    = 0x11060000,
	PRM_ADRS_USER_PATCH_8 	    = 0x11070000,
	PRM_ADRS_MAX
  };
  enum SystemParameterOffsetAddress : unsigned int {
	PRM_OFFSET_SYSTEM_MIDI_CH		= 0x0000,
	PRM_OFFSET_SYSTEM_GATE_LEVEL 	= 0x0001,
	PRM_OFFSET_SYSTEM_MAX	   		= 0x0010
  };
  enum PatchParameterOffsetAddress {
	PRM_OFFSET_PATCH_ROBOT          = 0x0000,
	PRM_OFFSET_PATCH_HARMONY        = 0x0001,
	PRM_OFFSET_PATCH_VOCODER        = 0x0002,
	PRM_OFFSET_PATCH_MEGAPHONE      = 0x0003,
	PRM_OFFSET_PATCH_ROBOT_VARIATION = 0x0004,
	PRM_OFFSET_PATCH_HARMONY_VARIATION = 0x0005,
	PRM_OFFSET_PATCH_VOCODER_VARIATION = 0x0006,
	PRM_OFFSET_PATCH_MEGAPHONE_VARIATION = 0x0007,
    PRM_OFFSET_PATCH_REVERB_VARIATION = 0x0008,
	PRM_OFFSET_PATCH_PITCH		    = 0x0009,
	PRM_OFFSET_PATCH_FORMANT		= 0x000B,
	PRM_OFFSET_PATCH_BALANCE		= 0x000D,
	PRM_OFFSET_PATCH_REVERB		    = 0x000F,
	PRM_OFFSET_PATCH_AUTO_PITCH		= 0x0011,
	PRM_OFFSET_PATCH_KEY		    = 0x0013,
	PRM_OFFSET_PATCH_GLOVAL_LEVEL	= 0x0014,
	PRM_OFFSET_PATCH_NAME_00_03	    = 0x0016,
	PRM_OFFSET_PATCH_NAME_04_07  	= 0x001E,
	PRM_OFFSET_PATCH_MAX		    = 0x0026
  };

  // clang-format : on
  std::mutex mtx;
  std::condition_variable cv;
  UINT inDevID = 0;
  UINT outDevID = 0;
  std::unique_ptr<MMMIDIIn> in;
  std::unique_ptr<MMMIDIOut> out;
  Parameters params;
  CallbackFunc extInCallback = nullptr;
  void* extInCallbackInstance = nullptr;
  static unsigned int bytesToUint(char* bytes, size_t len);
  static void midiInCallback(void* ptr, UINT wMsg, DWORD dwParam1,
							 DWORD dwParam2) {
	((VT4*)ptr)->inCallback(wMsg, dwParam1, dwParam2);
  }
  void inCallback(UINT wMsg, DWORD dwParam1, DWORD dwParam2);
  Identity extractIdentity(const LPMIDIHDR hdr);
  RetCode sendRQ1(char* address, char* dataSize);
  LPMIDIHDR getRQ1(char* address, char* dataSize);
  int extractDT1(LPMIDIHDR hdr);
  int parseDT1(unsigned int startParamAddrs, char* data, unsigned int offset);
  int parseDT1Patch(int patchNo, unsigned int startParamAddrs, char* data,
					   unsigned int offset);
  RetCode sendDT1(const char* address, const char* data, size_t dataLen);
  static void packDwordToBytes(uint32_t in, char *out);
  void packTwoBytesParam(unsigned char from, char* to);
};
#endif

