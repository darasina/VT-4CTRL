object VT4ControlFrame: TVT4ControlFrame
  Left = 0
  Top = 0
  Hint = 'NAME'
  Caption = 'VT-4 Control'
  ClientHeight = 597
  ClientWidth = 496
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  TextHeight = 15
  object inMIDILabel: TLabel
    Left = 8
    Top = 77
    Width = 12
    Height = 15
    Caption = 'IN'
  end
  object OutMIDILabel: TLabel
    Left = 8
    Top = 103
    Width = 23
    Height = 15
    Caption = 'OUT'
  end
  object Label2: TLabel
    Left = 347
    Top = 8
    Width = 30
    Height = 15
    Caption = 'SYNC'
  end
  object Label3: TLabel
    Left = 347
    Top = 39
    Width = 29
    Height = 15
    Caption = 'SEND'
  end
  object Label1: TLabel
    Left = 347
    Top = 178
    Width = 59
    Height = 15
    Caption = 'SAVE LOCK'
  end
  object Label4: TLabel
    Left = 8
    Top = 11
    Width = 47
    Height = 15
    Caption = 'VOLUME'
  end
  object Label5: TLabel
    Left = 170
    Top = 12
    Width = 52
    Height = 15
    Caption = 'MIC SENS'
  end
  object Label6: TLabel
    Left = 347
    Top = 129
    Width = 40
    Height = 15
    Caption = 'BYPASS'
  end
  object InMIDIList: TListBox
    Left = 40
    Top = 72
    Width = 177
    Height = 20
    ItemHeight = 15
    TabOrder = 0
  end
  object InMIDIOpenSwitch: TToggleSwitch
    Left = 239
    Top = 72
    Width = 98
    Height = 20
    Enabled = False
    StateCaptions.CaptionOn = 'Opened'
    StateCaptions.CaptionOff = 'Closed'
    TabOrder = 1
  end
  object OutMIDIList: TListBox
    Left = 40
    Top = 98
    Width = 177
    Height = 20
    ItemHeight = 15
    TabOrder = 2
  end
  object OutMIDIOpenSwitch: TToggleSwitch
    Left = 239
    Top = 98
    Width = 98
    Height = 20
    Enabled = False
    StateCaptions.CaptionOn = 'Opened'
    StateCaptions.CaptionOff = 'Closed'
    TabOrder = 3
  end
  object ReloadButton: TButton
    Left = 391
    Top = 60
    Width = 89
    Height = 22
    Caption = 'Reload Params'
    TabOrder = 4
    OnClick = ReloadButtonClick
  end
  object SyncSwitch: TToggleSwitch
    Left = 391
    Top = 8
    Width = 77
    Height = 20
    StateCaptions.CaptionOn = 'ON'
    StateCaptions.CaptionOff = 'OFF'
    TabOrder = 5
  end
  object SendSwitch: TToggleSwitch
    Left = 391
    Top = 34
    Width = 77
    Height = 20
    StateCaptions.CaptionOn = 'ON'
    StateCaptions.CaptionOff = 'OFF'
    TabOrder = 6
  end
  object TemporaryPatchGroup: TGroupBox
    Left = -1
    Top = 271
    Width = 489
    Height = 322
    TabOrder = 7
    object Label7: TLabel
      Left = 171
      Top = 282
      Width = 34
      Height = 15
      Caption = 'NAME'
    end
    object PitchGroup: TGroupBox
      Left = 3
      Top = 124
      Width = 69
      Height = 195
      Caption = 'PITCH'
      TabOrder = 0
      object PitchBar: TTrackBar
        Left = 3
        Top = 11
        Width = 28
        Height = 150
        Max = 255
        Orientation = trVertical
        Frequency = 25
        Position = 128
        TabOrder = 0
        OnChange = PitchBarChange
      end
      object PitchNumber: TNumberBox
        Left = 2
        Top = 167
        Width = 55
        Height = 23
        Alignment = taCenter
        MaxValue = 255.000000000000000000
        TabOrder = 1
        Value = 127.000000000000000000
        SpinButtonOptions.Placement = nbspCompact
        OnChangeValue = PitchNumberChangeValue
      end
    end
    object RobotGroup: TGroupBox
      Left = 3
      Top = 13
      Width = 90
      Height = 105
      Caption = 'ROBOT'
      TabOrder = 1
      object RobotBar: TTrackBar
        Left = 3
        Top = 47
        Width = 81
        Height = 25
        Enabled = False
        Max = 2
        TabOrder = 0
        OnChange = RobotBarChange
      end
      object RobotSwitch: TToggleSwitch
        Left = 3
        Top = 21
        Width = 77
        Height = 20
        StateCaptions.CaptionOn = 'ON'
        StateCaptions.CaptionOff = 'OFF'
        TabOrder = 1
        OnClick = RobotSwitchClick
      end
      object RobotVarNumber: TNumberBox
        Left = 3
        Top = 78
        Width = 76
        Height = 23
        Hint = 'VARIATION'
        MinValue = 1.000000000000000000
        MaxValue = 8.000000000000000000
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        TextHint = 'VARIATION'
        Value = 1.000000000000000000
        SpinButtonOptions.Placement = nbspInline
        OnChangeValue = RobotVarNumberChangeValue
      end
    end
    object VocoderGroup: TGroupBox
      Left = 195
      Top = 13
      Width = 90
      Height = 105
      Caption = 'VOCODER'
      TabOrder = 2
      object VocoderSwitch: TToggleSwitch
        Left = 3
        Top = 21
        Width = 77
        Height = 20
        StateCaptions.CaptionOn = 'ON'
        StateCaptions.CaptionOff = 'OFF'
        TabOrder = 0
        OnClick = VocoderSwitchClick
      end
      object VocoderVarNumber: TNumberBox
        Left = 3
        Top = 47
        Width = 76
        Height = 23
        Hint = 'VARIATION'
        MinValue = 1.000000000000000000
        MaxValue = 8.000000000000000000
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        Value = 1.000000000000000000
        SpinButtonOptions.Placement = nbspInline
        OnChangeValue = VocoderVarNumberChangeValue
      end
    end
    object HarmonyGroup: TGroupBox
      Left = 291
      Top = 13
      Width = 90
      Height = 105
      Caption = 'HARMONY'
      TabOrder = 3
      object HarmonySwitch: TToggleSwitch
        Left = 3
        Top = 21
        Width = 77
        Height = 20
        StateCaptions.CaptionOn = 'ON'
        StateCaptions.CaptionOff = 'OFF'
        TabOrder = 0
        OnClick = HarmonySwitchClick
      end
      object HarmonyVarNumber: TNumberBox
        Left = 3
        Top = 47
        Width = 76
        Height = 23
        Hint = 'VARIATION'
        MinValue = 1.000000000000000000
        MaxValue = 8.000000000000000000
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        Value = 1.000000000000000000
        SpinButtonOptions.Placement = nbspInline
        OnChangeValue = HarmonyVarNumberChangeValue
      end
    end
    object MegaphoneGroup: TGroupBox
      Left = 99
      Top = 13
      Width = 90
      Height = 105
      Caption = 'MEGAPHONE'
      TabOrder = 4
      object MegaphoneSwitch: TToggleSwitch
        Left = 3
        Top = 21
        Width = 77
        Height = 20
        StateCaptions.CaptionOn = 'ON'
        StateCaptions.CaptionOff = 'OFF'
        TabOrder = 0
        OnClick = MegaphoneSwitchClick
      end
      object MegaphoneVarNumber: TNumberBox
        Left = 3
        Top = 47
        Width = 76
        Height = 23
        Hint = 'VARIATION'
        MinValue = 1.000000000000000000
        MaxValue = 8.000000000000000000
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        Value = 1.000000000000000000
        SpinButtonOptions.Placement = nbspInline
        OnChangeValue = MegaphoneVarNumberChangeValue
      end
    end
    object ReverbVarGroup: TGroupBox
      Left = 387
      Top = 13
      Width = 90
      Height = 105
      Caption = 'REVERB'
      TabOrder = 5
      object ReverbVarNumber: TNumberBox
        Left = 3
        Top = 47
        Width = 76
        Height = 23
        Hint = 'VARIATION'
        MinValue = 1.000000000000000000
        MaxValue = 8.000000000000000000
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        Value = 1.000000000000000000
        SpinButtonOptions.Placement = nbspInline
        OnChangeValue = ReverbVarNumberChangeValue
      end
    end
    object FormantGroup: TGroupBox
      Left = 78
      Top = 124
      Width = 70
      Height = 195
      Caption = 'FORMANT'
      TabOrder = 6
      object FormantNumber: TNumberBox
        Left = 3
        Top = 167
        Width = 55
        Height = 23
        Alignment = taCenter
        MaxValue = 255.000000000000000000
        TabOrder = 0
        Value = 127.000000000000000000
        SpinButtonOptions.Placement = nbspCompact
        OnChangeValue = FormantNumberChangeValue
      end
      object FormantBar: TTrackBar
        Left = 0
        Top = 11
        Width = 45
        Height = 150
        Max = 255
        Orientation = trVertical
        Frequency = 25
        Position = 128
        TabOrder = 1
        OnChange = FormantBarChange
      end
    end
    object AutoPitchGroup: TGroupBox
      Left = 163
      Top = 124
      Width = 158
      Height = 71
      Caption = 'AUTO PITCH'
      TabOrder = 7
      object AutoPitchNumber: TNumberBox
        Left = 100
        Top = 45
        Width = 55
        Height = 23
        Alignment = taCenter
        MaxValue = 255.000000000000000000
        TabOrder = 0
        SpinButtonOptions.Placement = nbspCompact
        OnChangeValue = AutoPitchNumberChangeValue
      end
      object AutoPitchBar: TTrackBar
        Left = 5
        Top = 13
        Width = 150
        Height = 29
        Max = 255
        Frequency = 25
        TabOrder = 1
        OnChange = AutoPitchBarChange
      end
    end
    object BalanceGroup: TGroupBox
      Left = 327
      Top = 124
      Width = 70
      Height = 195
      Caption = 'BALANCE'
      TabOrder = 8
      object BalanceNumber: TNumberBox
        Left = 3
        Top = 167
        Width = 55
        Height = 23
        Alignment = taCenter
        MaxValue = 255.000000000000000000
        TabOrder = 0
        Value = 255.000000000000000000
        SpinButtonOptions.Placement = nbspCompact
        OnChangeValue = BalanceNumberChangeValue
      end
      object BalanceBar: TTrackBar
        Left = 3
        Top = 11
        Width = 45
        Height = 150
        Hint = '255: 100% WET'
        Max = 255
        Orientation = trVertical
        ParentShowHint = False
        Frequency = 25
        ShowHint = True
        TabOrder = 1
        OnChange = BalanceBarChange
      end
    end
    object ReverbGroup: TGroupBox
      Left = 403
      Top = 124
      Width = 70
      Height = 195
      Caption = 'REVERB'
      TabOrder = 9
      object ReverbNumber: TNumberBox
        Left = 3
        Top = 167
        Width = 55
        Height = 23
        Alignment = taCenter
        MaxValue = 255.000000000000000000
        TabOrder = 0
        SpinButtonOptions.Placement = nbspCompact
        OnChangeValue = ReverbNumberChangeValue
      end
      object ReverbBar: TTrackBar
        Left = 3
        Top = 11
        Width = 45
        Height = 150
        Max = 255
        Orientation = trVertical
        Frequency = 25
        Position = 255
        TabOrder = 1
        OnChange = ReverbBarChange
      end
    end
    object TempPatchNameEdit: TEdit
      Left = 209
      Top = 279
      Width = 76
      Height = 26
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'FixedSys'
      Font.Pitch = fpFixed
      Font.Style = []
      Font.Quality = fqAntialiased
      ParentFont = False
      TabOrder = 10
      Text = 'ABCD5678'
    end
    object KeyGroup: TGroupBox
      Left = 163
      Top = 201
      Width = 158
      Height = 75
      Caption = 'KEY'
      TabOrder = 11
      object KeyNumberBox: TNumberBox
        Left = 100
        Top = 49
        Width = 55
        Height = 23
        Alignment = taCenter
        MaxValue = 11.000000000000000000
        TabOrder = 0
        SpinButtonOptions.Placement = nbspCompact
        OnChangeValue = AutoPitchNumberChangeValue
      end
      object KeyBar: TTrackBar
        Left = 5
        Top = 15
        Width = 150
        Height = 28
        Max = 11
        TabOrder = 1
        OnChange = KeyBarChange
      end
    end
  end
  object ManualButton: TButton
    Left = 391
    Top = 225
    Width = 75
    Height = 25
    Hint = 'VT-4'#23455#27231#12467#12531#12497#12493#12398#35373#23450#20516#12434#35501#12415#36796#12416#12508#12479#12531
    Caption = 'MANUAL'
    TabOrder = 8
    OnClick = ManualButtonClick
  end
  object PatchPageControl: TPageControl
    Left = 0
    Top = 124
    Width = 345
    Height = 141
    ActivePage = Patch1_4TabSheet
    TabOrder = 9
    object Patch1_4TabSheet: TTabSheet
      Caption = 'MEMORY #1 - 4'
      object Patch1Group: TGroupBox
        Left = 2
        Top = -1
        Width = 81
        Height = 109
        Caption = '#1'
        TabOrder = 0
        object Patch1LoadButton: TButton
          Left = 3
          Top = 80
          Width = 75
          Height = 25
          Caption = 'LOAD'
          TabOrder = 0
          OnClick = Patch1LoadButtonClick
        end
        object Patch1NameEdit: TEdit
          Left = 3
          Top = 20
          Width = 75
          Height = 26
          Hint = 'NAME'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'FixedSys'
          Font.Pitch = fpFixed
          Font.Style = []
          ImeMode = imDisable
          MaxLength = 8
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '12345678'
          OnExit = Patch1NameEditExit
        end
        object Patch1SaveButton: TButton
          Left = 3
          Top = 49
          Width = 75
          Height = 25
          Caption = 'SAVE'
          TabOrder = 2
          OnClick = Patch1SaveButtonClick
        end
      end
      object Patch2Group: TGroupBox
        Left = 86
        Top = 0
        Width = 81
        Height = 109
        Caption = '#2'
        TabOrder = 1
        object Patch2LoadButton: TButton
          Left = 3
          Top = 80
          Width = 75
          Height = 25
          Caption = 'LOAD'
          TabOrder = 0
          OnClick = Patch2LoadButtonClick
        end
        object Patch2NameEdit: TEdit
          Left = 3
          Top = 20
          Width = 75
          Height = 26
          Hint = 'NAME'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'FixedSys'
          Font.Pitch = fpFixed
          Font.Style = []
          ImeMode = imDisable
          MaxLength = 8
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '12345678'
          OnExit = Patch1NameEditExit
        end
        object Patch2SaveButton: TButton
          Left = 3
          Top = 49
          Width = 75
          Height = 25
          Caption = 'SAVE'
          TabOrder = 2
          OnClick = Patch2SaveButtonClick
        end
      end
      object Patch3Group: TGroupBox
        Left = 170
        Top = 0
        Width = 81
        Height = 109
        Caption = '#3'
        TabOrder = 2
        object Patch3LoadButton: TButton
          Left = 3
          Top = 80
          Width = 75
          Height = 25
          Caption = 'LOAD'
          TabOrder = 0
          OnClick = Patch3LoadButtonClick
        end
        object Patch3NameEdit: TEdit
          Left = 3
          Top = 20
          Width = 75
          Height = 26
          Hint = 'NAME'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'FixedSys'
          Font.Pitch = fpFixed
          Font.Style = []
          ImeMode = imDisable
          MaxLength = 8
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '12345678'
          OnExit = Patch1NameEditExit
        end
        object Patch3SaveButton: TButton
          Left = 3
          Top = 49
          Width = 75
          Height = 25
          Caption = 'SAVE'
          TabOrder = 2
          OnClick = Patch3SaveButtonClick
        end
      end
      object Patch4Group: TGroupBox
        Left = 254
        Top = -1
        Width = 81
        Height = 109
        Caption = '#4'
        TabOrder = 3
        object Patch4LoadButton: TButton
          Left = 3
          Top = 80
          Width = 75
          Height = 25
          Caption = 'LOAD'
          TabOrder = 0
          OnClick = Patch4LoadButtonClick
        end
        object Patch4NameEdit: TEdit
          Left = 3
          Top = 20
          Width = 75
          Height = 26
          Hint = 'NAME'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'FixedSys'
          Font.Pitch = fpFixed
          Font.Style = []
          ImeMode = imDisable
          MaxLength = 8
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '12345678'
          OnExit = Patch1NameEditExit
        end
        object Patch4SaveButton: TButton
          Left = 3
          Top = 49
          Width = 75
          Height = 25
          Caption = 'SAVE'
          TabOrder = 2
          OnClick = Patch4SaveButtonClick
        end
      end
    end
    object Patch5_8TabSheet: TTabSheet
      Caption = 'MEMORY #5 - 8'
      ImageIndex = 1
      object Patch8Group: TGroupBox
        Left = 254
        Top = -1
        Width = 81
        Height = 109
        Caption = '#8'
        TabOrder = 0
        object Patch8LoadButton: TButton
          Left = 3
          Top = 80
          Width = 75
          Height = 25
          Caption = 'LOAD'
          TabOrder = 0
          OnClick = Patch8LoadButtonClick
        end
        object Patch8NameEdit: TEdit
          Left = 3
          Top = 20
          Width = 75
          Height = 26
          Hint = 'NAME'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'FixedSys'
          Font.Pitch = fpFixed
          Font.Style = []
          ImeMode = imDisable
          MaxLength = 8
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '12345678'
          OnExit = Patch1NameEditExit
        end
        object Patch8SaveButton: TButton
          Left = 3
          Top = 49
          Width = 75
          Height = 25
          Caption = 'SAVE'
          TabOrder = 2
          OnClick = Patch8SaveButtonClick
        end
      end
      object Patch5Group: TGroupBox
        Left = 2
        Top = -1
        Width = 81
        Height = 109
        Caption = '#5'
        TabOrder = 1
        object Patch5LoadButton: TButton
          Left = 3
          Top = 80
          Width = 75
          Height = 25
          Caption = 'LOAD'
          TabOrder = 0
          OnClick = Patch5LoadButtonClick
        end
        object Patch5NameEdit: TEdit
          Left = 3
          Top = 20
          Width = 75
          Height = 26
          Hint = 'NAME'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'FixedSys'
          Font.Pitch = fpFixed
          Font.Style = []
          ImeMode = imDisable
          MaxLength = 8
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '12345678'
          OnExit = Patch1NameEditExit
        end
        object Patch5SaveButton: TButton
          Left = 3
          Top = 49
          Width = 75
          Height = 25
          Caption = 'SAVE'
          TabOrder = 2
          OnClick = Patch5SaveButtonClick
        end
      end
      object GroupBox1: TGroupBox
        Left = 86
        Top = 0
        Width = 81
        Height = 109
        Caption = '#6'
        TabOrder = 2
        object Patch6LoadButton: TButton
          Left = 3
          Top = 80
          Width = 75
          Height = 25
          Caption = 'LOAD'
          TabOrder = 0
          OnClick = Patch6LoadButtonClick
        end
        object Patch6NameEdit: TEdit
          Left = 3
          Top = 20
          Width = 75
          Height = 26
          Hint = 'NAME'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'FixedSys'
          Font.Pitch = fpFixed
          Font.Style = []
          ImeMode = imDisable
          MaxLength = 8
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '12345678'
          OnExit = Patch1NameEditExit
        end
        object Patch6SaveButton: TButton
          Left = 3
          Top = 49
          Width = 75
          Height = 25
          Caption = 'SAVE'
          TabOrder = 2
          OnClick = Patch6SaveButtonClick
        end
      end
      object GroupBox2: TGroupBox
        Left = 170
        Top = 0
        Width = 81
        Height = 109
        Caption = '#7'
        TabOrder = 3
        object Patch7LoadButton: TButton
          Left = 3
          Top = 80
          Width = 75
          Height = 25
          Caption = 'LOAD'
          TabOrder = 0
          OnClick = Patch7LoadButtonClick
        end
        object Patch7NameEdit: TEdit
          Left = 3
          Top = 20
          Width = 75
          Height = 26
          Hint = 'NAME'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'FixedSys'
          Font.Pitch = fpFixed
          Font.Style = []
          ImeMode = imDisable
          MaxLength = 8
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '12345678'
          OnExit = Patch1NameEditExit
        end
        object Patch7SaveButton: TButton
          Left = 3
          Top = 49
          Width = 75
          Height = 25
          Caption = 'SAVE'
          TabOrder = 2
          OnClick = Patch7SaveButtonClick
        end
      end
    end
  end
  object SaveLockSwitch: TToggleSwitch
    Left = 391
    Top = 199
    Width = 103
    Height = 20
    State = tssOn
    StateCaptions.CaptionOn = 'LOCK'
    StateCaptions.CaptionOff = 'UNLOCK'
    TabOrder = 10
    OnClick = SaveLockSwitchClick
  end
  object VolumeBar: TTrackBar
    Left = 2
    Top = 33
    Width = 150
    Height = 33
    Max = 127
    Frequency = 32
    TabOrder = 11
    OnChange = VolumeBarChange
  end
  object MicSensBar: TTrackBar
    Left = 158
    Top = 33
    Width = 150
    Height = 33
    Max = 127
    Frequency = 32
    TabOrder = 12
    OnChange = MicSensBarChange
  end
  object VolumeNumber: TNumberBox
    Left = 87
    Top = 8
    Width = 55
    Height = 23
    Alignment = taCenter
    MaxValue = 255.000000000000000000
    TabOrder = 13
    SpinButtonOptions.Placement = nbspCompact
    OnChangeValue = PitchNumberChangeValue
  end
  object MicSensNumber: TNumberBox
    Left = 239
    Top = 8
    Width = 55
    Height = 23
    Alignment = taCenter
    MaxValue = 255.000000000000000000
    TabOrder = 14
    SpinButtonOptions.Placement = nbspCompact
    OnChangeValue = PitchNumberChangeValue
  end
  object ToggleSwitch1: TToggleSwitch
    Left = 391
    Top = 152
    Width = 77
    Height = 20
    StateCaptions.CaptionOn = 'ON'
    StateCaptions.CaptionOff = 'OFF'
    TabOrder = 15
  end
  object Timer1: TTimer
    Interval = 300
    OnTimer = Timer1Timer
    Left = 416
    Top = 95
  end
end
