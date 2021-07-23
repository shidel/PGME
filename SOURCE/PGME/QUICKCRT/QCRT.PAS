{ Copyright 2015 Jerome Shidel }


(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QCrt; { QuickCRT, version 8.40 }
(*
	QCrt is the text mode CRT video unit of QuickCrt. 
	When version 9+ of QCrt is begun, it will be removed from the QuickCrt project
	permanently. It will become an assembly language project with Pascal wrappers and
	will be hosted at https://github.com/loopz. The intent will be to make QCrt support
	for ASM and Pascal development with support for Dos, Windows, Mac, Linux and bare
	metal OS Development.
*)
{$I QCrt.def}
{$F+,O-}

{ 
NOTE:  
	
	This text mode video interface requires an EGA/VGA compatable
    system.  Although many of the routines in this unit have the same
    names as those in Borland's CRT unit, and preform similar functions in
    a similar way to their counterparts, they are 278% INcompatable with
    that unit, and CANNOT be used together in the same application.  Also,
    you can use system's Write/WriteLn procedures, but the FWriteChar/
    FWrite/FWriteLn are many, many, many times faster.

    This unit also contains quite a bit more code then Borland's CRT unit,
    and when used with very small programs, the compiled size is noticably
    larger.  However, it is usually many times faster, and contains many
    functions overlooked by the CRT unit.  So, if you would like a more
    flexible line editor with mouse support or just a faster screen
    interface give QCrt a try.


    Some functions that are available here, but not CRT unit:

      SubWindow           Sets a new window, within the current window.
      GetMaxX             Returns the width in characters of the window.
      GetMaxY             Returns the height in characters of the window.
      InsChar             Inserts a space on the current line.
      DelChar             Deletes a character on the current line.
      InsColumn           Inserts a column.
      DelColumn           Deletes a column.
      PrintScreen         Prints the screen to the printer.
      FWriteChar          High speed character write.
      FWrite              High speed version of the Write Procedure.
      FWriteLn            High Speed version of the WriteLn Procedure.
      FReadChar           Reads a character directly from the screen.
      FReadAttr           Reads the Textattr directly from the screen.
      FReadCharAttr       Reads a character and attribute from the screen.
      WindowSize          Figures out the memory requirements to store a
                          window.
      GetWindow           Saves an entire window into memory.
      PutWindow           Puts an entire window on the screen.
      MoveCursor          Refreshes the cursor.
      GetCursor           Returns the cursor size and shape.
      SetCursor           Sets the cursors size and shape.
      HideCursor          Make the cursor disappear.
      NormalCursor        Resets the text cursor to its original size/shape.
      SmallCursor         Makes the cursor small.
      HalfCursor          Makes the cursor half as high as a character.
      FullCursor          Makes the cursor the same height as a character.
      GetBlink            Returns the current Blink/Intensity state.
      SetBlink            Toggles the Blink/Intensity state.
      GetMousePos         Returns the mouses position and buttons status.
      SetMousePos         Sets the mouses position.
      TurnMouseOff        Disables the mouse.
      TurnMouseOn         Enables the mouse, if it's found.
      DrawBox             Draws a box.
      DrawShadow          Draws a box shadow.
      DrawLine            Draws a line.
      PurgeEvents         Clears all event buffers.
      ClearEvent          Nulls an event.
      GetEvent            Retrieves a event if it exists.
      PutEvent            Puts an event in a buffer.
      EditLn              High power line editor.
      GetVideoSize        Size of the current video page.
      GetVideoPtr         Returns pointer where screen reads/writes go.
      SetVideoPtr         Sets pointer where screen reads/writes go.
      CopyToVideo         Copies memory from videoptr to the screen.
      CopyFromVideo       Copies screen memory to VideoPtr.
      InitQCrt            Reinitializes the QCrt unit for mode changes.
}

{$DEFINE RollOver} { when defined, if x or y are out of the current window
                     the cursor wraps from one end of the screen to the
                     other.  For example:
                       Window( 1,1,80,25 );
                       GotoXY( 81,25);
                     would be the same as:
                       Window( 1,1,80,25 );
                       GotoXY( 1, 25 ).

                     if not defined, the cursor stops at the end of the
                     window. }

interface

  const
  { CRT modes }
    BW40          = 0;            { 40x25 B/W on Color Adapter }
    CO40          = 1;            { 40x25 Color on Color Adapter }
    BW80          = 2;            { 80x25 B/W on Color Adapter }
    CO80          = 3;            { 80x25 Color on Color Adapter }
    Mono          = 7;            { 80x25 on Monochrome Adapter }
    Font8x8       = $0800;        { Add-in for 8x8 ROM font }
    Font8x14      = $0E00;        { Add-in for 8x14 ROM font, EGA and VGA }
    Font8x16      = $1000;        { Add-in for 8x16 ROM font, VGA only }
    FontUser      = $FF00;        { Add-in for User defined font, VGA only }

  { Mode constants for CRT 3.0 compatibility }
    C40           = CO40;
    C80           = CO80;

  { Foreground and background color/attribute constants }
    Black         = 0;
    Blue          = 1;
    Green         = 2;
    Cyan          = 3;
    Red           = 4;
    Magenta       = 5;
    Brown         = 6;
    LightGray     = 7;

  { Foreground color/attribute constants }
    DarkGray      = 8;
    LightBlue     = 9;
    LightGreen    = 10;
    LightCyan     = 11;
    LightRed      = 12;
    LightMagenta  = 13;
    Yellow        = 14;
    White         = 15;

  { Add-in for blinking and intense background constants }
    Blink         = 128;
    Intensity     = Blink;

  { TTL Monochrome attributes constants }
    Underline               = $01;
    Normal                  = $07;
    BrightUnderline         = $09;
    Bold                    = $0F;
    Reverse                 = $70;
    BlinkingUnderline       = $81;
    BlinkingNormal          = $87;
    BlinkingBrightUnderline = $89;
    BlinkingBold            = $8F;

  { One-Color composite attributes constants }
  { Normal                  = $07; }
    GrayOnBlack             = $08;
  { Bold                    = $0F; }
  { Reverse                 = $70; }
    GrayOnWhite             = $78;
    WhiteOnWhite            = $7F;
  { BlinkingNormal          = $87; }
  { BlinkingBold            = $8F; }

  { Box Constants }
    bxSingle     = $00; { Single Side/Single Top }
    bxDouble     = $03; { Double Side/Double Top }
    bxDoubleSide = $01; { Double Side/Single Top }
    bxDoubleTop  = $02; { Double Top/Single Side }

  { Box Shadow Styles }
    bsSingleWide = $00; { one character wide }
    bsDoubleWide = $01; { two characters wide }

  { Line Constants }
    lnSingle     = $00;
    lnDouble     = $01;
    lnVertical   = $02;
    lnHorizontal = $00;
    lnNoEnds     = $04;
    lnNoCenter   = $08;

  { Event Constants }
    evNothing   = $0000; { Event already handled }
    evNone		= evNothing;
    evMouseDown = $0001; { Mouse button depressed }
    evMouseUp   = $0002; { Mouse button released }
    evMouseMove = $0004; { Mouse changed location }
    evMouseAuto = $0008; { Periodic event while mouse button held down }
    evMouse     = $000F; { Mouse event }
    evKeyDown   = $0010; { Key pressed }
    evKeyboard  = $0010; { Keyboard event }
    evCommand   = $0100; { Command event }
    evBroadcast = $0200; { Broadcast event }
    evSystem    = $0400; { System Event }
    evMessage   = $FF00; { Message (command, broadcast, or user-defined) event }

  { Predefined System Event Commands }
    cmNone          = $0000;
    cmClearedEvent  = $0001;
    cmMakeSysReq    = $0002;
    cmBreakSysReq   = $0003;
    cmPrintScreen   = $0004;
    cmBreak         = $0005;
    cmQuit          = $0006;
    cmHelp          = $0007;
    cmCloseWindow	= $0008; cmClose = cmCloseWindow;
    cmTimer			= $0009;
    cmClock			= $000A;
    cmSpeaker		= $000B;
    cmSearch		= $000C;
  { Predefined Keyboard Editing Commands }
	cmTab			= $0100;
	cmUntab			= $0101;
	cmBackspace		= $0102;
	cmDelete		= $0103;
	cmStop			= $0104;
	cmEnter			= $0105;
	cmReturn		= $0106;
	cmEscape		= $0107;
	cmInsert		= $0108;
	cmClear			= $0109;
	cmAdd			= $010A;
	cmModify		= $010B;
	cmRemove		= $010C;
	
  { Predefined Keyboard Navigation Commands }
	cmLeft			= $0110;
	cmRight			= $0111;
	cmUp			= $0112;
	cmDown			= $0113;
	cmPrevious		= $0114;
	cmNext			= $0115;
	cmLineStart		= $0116;
	cmLineEnd		= $0117;
	cmPageUp		= $0118;
	cmPageDown		= $0119;
	cmGroupPrevious = $011A;
	cmGroupNext		= $011B;
	cmTextStart		= $011C;
	cmTextEnd		= $011D;
	cmBefore		= $011E;
	cmAfter			= $011F;	
	
	cmCut			= $0200;
	cmCopy			= $0201;
	cmPaste			= $0202;
	cmFind			= $0203;
	cmFindAgain		= $0204;
	cmReplace		= $0205;
	cmReplaceAll	= $0206;
	
	{ Special Commands }
	cmCharMap		= $0300; 
	cmMakeDir 		= $0301;
	cmNewFile		= $0302;
	cmOpenFile		= $0303;
	cmSaveFile		= $0304;
	cmSaveFileAs	= $0305;
	cmCloseFile		= $0306;
	cmFileInfo		= $0307;
	cmExecuteFile	= $0308;
	cmEditFile		= $0309;
	
	cmAbout			= $0310;
		
	{ Modal Result Commands }
	cmOK			= $0400;
	cmCancel		= $0401;
	cmAbort			= $0402;
	cmRetry			= $0403;
	
	{ TUI Commands }
	cmScrollbar		= $0500;
	cmToggle		= $0501;
	cmCheckbox		= $0502;
	cmRadioButton	= $0503;
	cmListView		= $0504;
	cmListViewDouble= $0505;
	cmListViewChange = $0506;
	cmDoDropList	= $0507;
	cmDropList		= $0508;
	cmEdit			= $0509;
		
  { User Commands }
    cmUser			= $1000;

  { Keyboard Shift Key Status Flags }
    kbRightShift  = $0001;
    kbLeftShift   = $0002;
    kbEitherShift = kbRightShift or kbLeftShift;
    kbEitherCtrl  = $0004;
    kbEitherAlt   = $0008;
    kbScrollLock  = $0010;
    kbNumsLock    = $0020;
    kbCapsLock    = $0040;
    kbInsertLock  = $0080;
    kbLeftCtrl    = $0100;
    kbLeftAlt     = $0200;
    kbSysDown     = $0400;
    kbPauseFlag   = $0800;
    kbScrollDown  = $1000;
    kbNumsDown    = $2000;
    kbCapsDown    = $4000;
    kbInsertDown  = $8000;
    
    { Key Mapping Flags }
    kfAny		  = $0000;
    kfContains	  = $0001;
    kfMatches     = $0002;
    
    { standard keyboard key constants }
    kbNone   = $0000;
    kbEscape = $001B;
    kbF1     = $3B00; kbShiftF1  = $5400; kbCtrlF1  = $5E00; kbAltF1  = $6800;
    kbF2     = $3C00; kbShiftF2  = $5500; kbCtrlF2  = $5F00; kbAltF2  = $6900;
    kbF3     = $3D00; kbShiftF3  = $5600; kbCtrlF3  = $6000; kbAltF3  = $6A00;
    kbF4     = $3E00; kbShiftF4  = $5700; kbCtrlF4  = $6100; kbAltF4  = $6B00;
    kbF5     = $3F00; kbShiftF5  = $5800; kbCtrlF5  = $6200; kbAltF5  = $6C00;
    kbF6     = $4000; kbShiftF6  = $5900; kbCtrlF6  = $6300; kbAltF6  = $6D00;
    kbF7     = $4100; kbShiftF7  = $5A00; kbCtrlF7  = $6400; kbAltF7  = $6E00;
    kbF8     = $4200; kbShiftF8  = $5B00; kbCtrlF8  = $6500; kbAltF8  = $6F00;
    kbF9     = $4300; kbShiftF9  = $5C00; kbCtrlF9  = $6600; kbAltF9  = $7000;
    kbF10    = $4400; kbShiftF10 = $5D00; kbCtrlF10 = $6700; kbAltF10 = $7100;
    kbHome   = $4700; kbCtrlHome = $7700;
    kbUp     = $4800; 
    kbPgUp   = $4900; kbCtrlPgUp = $8400;
    kbLeft   = $4B00;
    kbRight  = $4D00;
    kbEnd    = $4F00; kbCtrlEnd  = $7500;
    kbDown   = $5000; 
    kbPgDown = $5100; kbCtrlPgDown = $7600;
    kbInsert = $5200;
    kbDelete = $5300;
    kbEnter  = $000D; kbCtrlEnter = $000A;
    kbTab    = $0009; kbShiftTab  = $0F00;
    kbSpace  = $0020;
    kbBackSpace = $0008; kbCtrlBackSpace = $007F;
    kb0      = $0030; kbShift0 = $0029; kbAlt0 = $8100;
    kb1      = $0031; kbShift1 = $0021; kbAlt1 = $7800;
    kb2      = $0032; kbShift2 = $0040; kbAlt2 = $7900; kbCtrl2 = $0300;
    kb3      = $0033; kbShift3 = $0023; kbAlt3 = $7A00;
    kb4      = $0034; kbShift4 = $0024; kbAlt4 = $7B00;
    kb5      = $0035; kbShift5 = $0025; kbAlt5 = $7C00;
    kb6      = $0036; kbShift6 = $005E; kbAlt6 = $7D00; kbCtrl6 = $001E;
    kb7      = $0037; kbShift7 = $0026; kbAlt7 = $7E00;
    kb8      = $0038; kbShift8 = $002A; kbAlt8 = $7F00;
    kb9      = $0039; kbShift9 = $0028; kbAlt9 = $8000;
    kbMinus  = $002D; kbShiftMinus = $005F; kbAltMinus = $001F;
    kbCtrlMinus = $8200;
    kbEqual  = $003D; kbShiftEqual = $002B; kbAltEqual = $8300;
    kbA      = $0061; kbShiftA = $0041; kbCtrlA = $0001; kbAltA = $1E00;
    kbB      = $0062; kbShiftB = $0042; kbCtrlB = $0002; kbAltB = $3000;
    kbC      = $0063; kbShiftC = $0043; kbCtrlC = $0003; kbAltC = $2E00;
    kbD      = $0064; kbShiftD = $0044; kbCtrlD = $0004; kbAltD = $2000;
    kbE      = $0065; kbShiftE = $0045; kbCtrlE = $0005; kbAltE = $1200;
    kbF      = $0066; kbShiftF = $0046; kbCtrlF = $0006; kbAltF = $2100;
    kbG      = $0067; kbShiftG = $0047; kbCtrlG = $0007; kbAltG = $2200;
    kbH      = $0068; kbShiftH = $0048; kbCtrlH = $0008; kbAltH = $2300;
    kbI      = $0069; kbShiftI = $0049; kbCtrlI = $0009; kbAltI = $1700;
    kbJ      = $006A; kbShiftJ = $004A; kbCtrlJ = $000A; kbAltJ = $2400;
    kbK      = $006B; kbShiftK = $004B; kbCtrlK = $000B; kbAltK = $2500;
    kbL      = $006C; kbShiftL = $004C; kbCtrlL = $000C; kbAltL = $2600;
    kbM      = $006D; kbShiftM = $004D; kbCtrlM = $000D; kbAltM = $3200;
    kbN      = $006E; kbShiftN = $004E; kbCtrlN = $000E; kbAltN = $3100;
    kbO      = $006F; kbShiftO = $004F; kbCtrlO = $000F; kbAltO = $1800;
    kbP      = $0070; kbShiftP = $0050; kbCtrlP = $0010; kbAltP = $1900;
    kbQ      = $0071; kbShiftQ = $0051; kbCtrlQ = $0011; kbAltQ = $1000;
    kbR      = $0072; kbShiftR = $0052; kbCtrlR = $0012; kbAltR = $1300;
    kbS      = $0073; kbShiftS = $0053; kbCtrlS = $0013; kbAltS = $1F00;
    kbT      = $0074; kbShiftT = $0054; kbCtrlT = $0014; kbAltT = $1400;
    kbU      = $0075; kbShiftU = $0055; kbCtrlU = $0015; kbAltU = $1600;
    kbV      = $0076; kbShiftV = $0056; kbCtrlV = $0016; kbAltV = $2F00;
    kbW      = $0077; kbShiftW = $0057; kbCtrlW = $0017; kbAltW = $1100;
    kbX      = $0078; kbShiftX = $0058; kbCtrlX = $0018; kbAltX = $2D00;
    kbY      = $0079; kbShiftY = $0059; kbCtrlY = $0019; kbAltY = $1500;
    kbZ      = $007A; kbShiftZ = $005A; kbCtrlZ = $001A; kbAltZ = $2C00;

    { enhanced 101-key keyboard constants }
    kbAltEsc  = $0100;
    kbF11     = $8500; kbShiftF11 = $8700; kbCtrlF11 = $8900; kbAltF11 = $8B00;
    kbF12     = $8600; kbShiftF12 = $8800; kbCtrlF12 = $8A00; kbAltF12 = $8C00;
    kbCtrlLeft   = $7300; kbAltLeft   = $9B00;
    kbCtrlRight  = $7400; kbAltRight  = $9D00;
    kbCtrlUp     = $8D00; kbAltUp     = $9800;
    kbCtrlDown   = $9100; kbAltDown   = $A000;
    kbCtrlInsert = $9200; kbAltInsert = $A200;
    kbCtrlDelete = $9300; kbAltDelete = $A300;
    kbCtrlTab    = $9400; kbAltTab    = $A500;
    kbAltHome    = $9700;
    kbAltPgUp    = $9900;
    kbAltEnd     = $9F00;
    kbAltPgDown  = $A100;
    kbAltEnter   = $1C00;
    kbAltBackSpace = $0E00;

  type
    TPoint = record
      X : byte;
      Y : byte;
    end;

    TEvent = record
      What: Word;
      case Word of
        evNothing: ();
        evMouse: (
          Buttons: Byte;
          Double: Boolean;
          Where: TPoint);
        evKeyDown: (
          ShiftCode : word;
          Original : word; { if not remapped 0 else is original value, only for reference }
          case Integer of
            0: (KeyCode: Word);
            1: (CharCode: Char;
                ScanCode: Byte));
        evMessage: (
          Command: Word;
          case Word of
            0: (InfoPtr: Pointer);
            1: (InfoLong: Longint);
            2: (InfoWord: Word);
            3: (InfoInt: Integer);
            4: (InfoByte: Byte);
            5: (InfoChar: Char));
    end;
      
  DWORD = record 
  	case Word of
  		0 : ( Bytes : array[0..3] of byte );
  		1 : ( H, L : word );
  		2 : ( Long : LongInt );
  		3 : ( Ptr : Pointer );
  end;
  QWORD = record
  	case Word of
  		0 : ( Bytes : array[0..7] of byte );
  		1 : ( H, L : DWord );
  end;  		

  BYTES = array[0..$FFFE] of byte;
  WORDS = array[0..$7FFE] of word;
  INTEGERS = array[0..$7FFE] of integer;
  LONGINTS = array[0..$3FFE] of longint;
  DWORDS = array[0..$3FFE] of DWORD;
  POINTERS = array[0..$3FFE] of Pointer;
  QWORDS = array[0..$1FFE] of QWORD;
  STRINGS = array[0..$00FE] of String;
    
  TCodeMapEntry = record
    FromCode, ToCode, ShiftCode, Flags : Word;
    Reserved : QWord;
  end;

  PCodeMap = ^TCodeMap;
  TCodeMap = array[0..$0FFE] of TCodeMapEntry;

  TQCrtSettings = record
    CheckBreak: Boolean;
    CheckPrint: Boolean;
    CheckSysReq: Boolean;
    DirectVideo: Boolean;
    CheckCursor: Boolean;
    CheckScroll: Boolean;
    CheckChar: Boolean;
    CheckMouse : Boolean;
    Check101: Boolean;
    CheckTab: Boolean;
    LastMode: Word;
    TextAttr: Byte;
    TextChar: Char;
    InsertMode: boolean;
    WindMin: Word;
    WindMax: Word;
    ScreenMax: Word;
    FontHeight: word;
    UserFontSize: byte;
    UserFontPtr: Pointer;
    MouseAvail: Boolean;
    MouseShift: word;
    MouseDouble: LongInt;
    MouseHomeX : integer;
    MouseHomeY : integer;
    Cursor : word;
    Blink : boolean;
    X, Y : integer;
  end;

  var
  { Interface variables }
    CheckBreak: Boolean;    { Enable Ctrl-Break checking }
    CheckPrint: Boolean;    { Enable Print Screen checking }
    CheckSysReq: Boolean;   { Enable System Request checking }
    CheckEOF: Boolean;      { Does not do anything }
    DirectVideo: Boolean;   { Enable direct video addressing }
    CheckSnow: Boolean;     { Does not do anything }
    CheckCursor: Boolean;   { Enables automatic cursor movement }
    CheckScroll: Boolean;   { Enables screen scrolling }
    CheckChar: Boolean;     { Enables some ascii control characters }
    CheckMouse : Boolean;   { Enables mouse detection on mode changes }
    Check101: Boolean;      { Enables use of Enhanced 101-key keyboard }
    CheckTab:Boolean;		{ Enables Tabbing out of EditLn }
    LastMode: Word;         { Current text mode }
    TextAttr: Byte;         { Current text attribute }
    TextChar: Char;         { Current background fill character }
    InsertMode: boolean;    { Insert/Overtype mode switch }
    WindMin: Word;          { Window upper left coordinates }
    WindMax: Word;          { Window lower right coordinates }
    ScreenMax: Word;        { Screen lower right coordinates }
    BreakCount: Word;       { Number of times CTRL-BREAK has been pressed }
    PrintCount: word;       { Number of times PRINT-SCREEN has been pressed }
    SysReqCount: Word;      { Number of times SYSREQ has been pressed }
    EventFlag : boolean;	{ True when a event has been received, 
    	good for screen savers. You must set to false after processing. Also,
    	cleared by PurgeEvents. }
    FontHeight: word;       { Height of current defined font }
    UserFontSize: byte;     { Height of user defined font }
    UserFontPtr: Pointer;   { Pointer to user defined font }
    MouseAvail: Boolean;    { Enables mouse functions and cursor }
    MouseButtons: integer;  { Number of mouse buttons }
    MouseShift: word;       { Value shift mouse for Screen Cooridinates }
    MouseDouble: LongInt;   { Mouse Double Click Speed }
    MouseHomeX : integer;   { Default Mouse X Coordinate }
    MouseHomeY : integer;   { Default Mouse Y Coordinate }
    EditSender : pointer;	{ Events reposted by EditLn with a replace nil InfoPtr }

    IdleProc : procedure;      { Called while processing a delay }
    PreWriteProc : procedure;  { Called before a screen write }
    PostWriteProc : procedure; { Called after a screen write }
    ReInitProc : procedure;    { Called after Textmode, and InitQCrt }
    
	FirstMemAvail, FirstMaxAvail : longInt; { Initial Memory Available }
	MinMemAvail, MinMaxAvail : LongInt;    { Lowest amount of memory available during execution }
	KeyboardIgnore : word;     { Keyboard status flags to ignore }

{ Interface procedures }
  procedure AssignCrt(var F : Text);

{ 
  Currently broken due to recent additions of automatic Keystroke to Command translations.
  I'll Probably fix it later, by adding a switch from legacy to event driven when the
  first call to GetEvent is made. If no call is made, then will just stay in legacy 
  mode and not translate any keys into commands.
    
  function KeyPressed : Boolean;
  function ReadKey : Char;

}
  procedure TextMode( Mode : word );
  procedure Window( X1, Y1, X2, Y2 : Byte );
  procedure SubWindow ( X1, Y1, X2, Y2 : byte );
  function GetMaxX : byte;
  function GetMaxY : byte;
  procedure GotoXY( X, Y : Byte );
  function WhereX : Byte;
  function WhereY : Byte;
  procedure ClrScr;
  procedure ClrEol;
  procedure InsLine;
  procedure DelLine;
  procedure InsChar;
  procedure DelChar;
  procedure InsColumn;
  procedure DelColumn;
  procedure TextColor( Color : Byte );
  procedure TextBackground( Color : Byte );
  procedure LowVideo;
  procedure HighVideo;
  procedure NormVideo;
  procedure Delay( MS : Word );
  procedure Sound( Hz : Word );
  procedure NoSound;

{ Keyboard Stuff }
	function GetShiftCode : word;
	procedure SetShiftCode(AValue : word);

{ Color Function to set Foreground and Background Colors at the same time. }
  procedure Color(Fore, Back : byte);

{ Print screen dump routine }
  procedure PrintScreen;

{ High speed video write procedures }
  procedure FWriteChar ( Letter : Char );
  procedure FWrite ( Str : String );
  procedure FWriteLn ( Str : String );
  procedure FWriteAttr;

{ High speed video read functions }
  function FReadChar : Char;
  function FReadAttr : byte;
  function FReadCharAttr : word;

{ Window functions }
  function WindowSize ( X1, Y1, X2, Y2 : byte ) : word;
  procedure GetWindow ( X1, Y1, X2, Y2 : byte; var Window );
  procedure PutWindow ( X1, Y1 : byte; var Window );

{ Cursor move procedure; Moves cursor to Current XY position when CheckCursor
  is false. }
  procedure MoveCursor;

{ Cursor Size/Shape functions }
  function  GetCursor : word;
  procedure SetCursor ( Cursor : word );
  procedure HideCursor;
  procedure NormalCursor;
  procedure SmallCursor;
  procedure HalfCursor;
  procedure FullCursor;

{ Blink/intensity control }
  function GetBlink : boolean;
  procedure SetBlink ( Blink : boolean );

{ Mouse procedures }
  procedure GetMousePos(var X, Y, Buttons : integer);
  procedure SetMousePos(X, Y : integer);
  procedure MouseHome;

  procedure TurnMouseOff;
  procedure TurnMouseOn;
  
  procedure ReleaseButtons;

{ Box and Line Functions }
  procedure DrawBox ( X1, Y1, X2, Y2, Style : byte );
  { Draws a box.  Does not move Cursor. }
  procedure DrawShadow ( X1, Y1, X2, Y2, Style : byte );
  { Draws a shadow for a box.  Does not move Cursor. }
  procedure DrawLine ( X1, Y1, Len, Style : byte );
  { Draws a line.  Does not move the Cursor. }

{ Event Functions }
  procedure PurgeEvents;
  procedure ClearEvent(var Event : TEvent);
  procedure GetEvent(var Event : TEvent);
  function  PutEvent(var Event : TEvent) : boolean;	
  function  PutCommand(AValue : word; ASender : Pointer) : boolean;	
  function  PutSystem(AValue : word; ASender : Pointer) : boolean;	

{ Text input functions }
  function EditLn ( var Dest : String; MaxLen, MaxWide : Byte ) : boolean;
  { High power line editor. And is used internally by System's Read and
    ReadLn procedures. }    
  function Pause : string;
  { Pause until event is triggered. }

  procedure SetKeyMap(CodeMap : PCodeMap; Count : word);
  { When set all keystrokes are translated through the key map. Map is copied. }
  procedure SetCommandMap(CodeMap : PCodeMap; Count : word);
  { When set all keystrokes are translated through the key map. Map is copied. }

  procedure GetKeyMap(var CodeMap : PCodeMap; var Count : word);
  { Just returns pointer and count to current map }
  procedure GetCommandMap(var CodeMap : PCodeMap; var Count : word);
  
  procedure InitQCrtMaps; 

{ Direct Video Memory Procedures }
  { Returns total bytes of memory required to save current video page }
  function GetVideoSize : word;
  { Returns current video read/write pointer }
  function  GetVideoPtr : Pointer;
  { Sets current video read/write pointer }
  procedure SetVideoPtr ( P : Pointer );
  { Copies video from video read/write ptr to video memory pointer }
  procedure CopyToVideo;
  { Copies video from video memory to the video read/write pointer.  Note that
    it copies GetVideoSize bytes to memory, and when changing text modes the
    number of bytes may change also, the video read/write pointer is reset to
    video memory. }
  procedure CopyFromVideo;
  
  function MemCheck(AValue : LongInt) : boolean;
  { Check Availible Memory, and update MinMaxAvail/MinMemAvail to new values. }

{ Procedures to preserve and restore settings in QCrt }
  procedure GetQCrtState(var ASettings : TQCrtSettings);
  procedure SetQCrtState(var ASettings : TQCrtSettings);

{ Unit variable reset procedure; Must call if an external procedure changes
  video modes (InitQCrt is called internally by TextMode, and at application
  startup). }
  procedure InitQCrt;  
  
implementation
{$F+}

(* Internal Data Types and variables *)

  const
    DefaultDoubleClick = 4;
    KeyBufSize         = 16;
    MouseBufSize       = 16;
    CommandBufSize     = 128;
    fmClosed           = $D7B0;
    fmInput            = $D7B1;
    fmOutput           = $D7B2;
    fmInOut            = $D7B3;

  type
    BIOSDataType = record
      Mode            : byte;
      Columns         : word;
      RegenSize       : word;
      VisualOfs       : word;
      Location        : array[0..7] of word;
      CursorSize      : word;
      VisualPage      : byte;
      Port            : word;
      CRTMode         : byte;
      CRTPalette      : byte;
      PostData        : array[0..4] of byte;
      TimerTickCount  : longint;
      TimerOverflow   : byte;
      BreakFlag       : byte;
      RebootData      : word;
      ATHardDiskData  : longint;
      PrinterTimeOut  : longint;
      SerialTimeOut   : longint;
      ATKeyboardStart : word;
      ATKeyboardEnd   : word;
      Rows            : byte;
      CharSize        : word;
    end;
    WindowType = record
      Width  : byte;
      Height : byte;
      Data   : Array[0..$7FFD] of word;
    end;

    PTextBuf = ^TTextBuf;
    TTextBuf = array[0..127] of Char;
    TTextName = array[0..79] of char;
    TTextRec = record
      Handle: Word;
      Mode: Word;
      BufSize: Word;
      Private: Word;
      BufPos: Word;
      BufEnd: Word;
      BufPtr: PTextBuf;
      OpenFunc: function  ( var F ) : integer;
      InOutFunc: function ( var F ) : integer;
      FlushFunc: function ( var F ) : integer;
      CloseFunc: function ( var F ) : integer;
      UserData: array[1..16] of Byte;
      Name: TTextName;
      Buffer: TTextBuf;
    end;

    BoxStr = Array[0..6] of Char;
    LineStr = String[41];

  const
    BoxData : array[bxSingle..bxDouble] of BoxStr = (
       #$DA#$BF#$C0#$D9#$C4#$B3#$00, {bxSingle}
       #$D6#$B7#$D3#$BD#$C4#$BA#$00, {bxDoubleSide}
       #$D5#$B8#$D4#$BE#$CD#$B3#$00, {bxDoubleTop}
       #$C9#$BB#$C8#$BC#$CD#$BA#$00  {bxDouble}
    );
    LineData : record
      Current : LineStr;
      Style: array[lnSingle..lnDouble,boolean,(Top, Middle, Bottom)] of LineStr;
    end = (
      Current:#$CD#$C4#$BA#$B3#$C9#$CB#$BB#$CC#$CE#$B9#$C8#$CA#$BC#$D6#$D2#$B7#$C7#$D7 +
      		  #$B6#$D3#$D0#$BD#$D5#$D1#$B8#$C6#$D8#$B5#$D4#$CF#$BE#$DA;
      Style:(

      ( { Single Line }
        ( { Horizontal }
          { Left }		#$C4#$C4#$C7#$C3#$D6#$D2#$D2#$C7#$D7#$D7#$D3#$D0#$D0#$D6#$D2#$D2 +
          				#$C7#$D7#$D7#$D3#$D0#$D0#$DA#$C2#$C2#$C3#$C5#$C5#$C0#$C1#$C1#$DA +
          				#$C2#$C2#$C3#$C5#$C5#$C0#$C1#$C1#$C4,
          { Middle }	#$C4#$C4#$D7#$C5#$D2#$D2#$D2#$D7#$D7#$D7#$D0#$D0#$D0#$D2#$D2#$D2 +
          				#$D7#$D7#$D7#$D0#$D0#$D0#$C2#$C2#$C2#$C5#$C5#$C5#$C1#$C1#$C1#$C2 +
          				#$C2#$C2#$C5#$C5#$C5#$C1#$C1#$C1#$C4,
          { Right }		#$C4#$C4#$B6#$B4#$D2#$D2#$B7#$D7#$D7#$B6#$D0#$D0#$BD#$D2#$D2#$B7 +
          				#$D7#$D7#$B6#$D0#$D0#$BD#$C2#$C2#$BF#$C5#$C5#$B4#$C1#$C1#$D9#$C2 +
          				#$C2#$BF#$C5#$C5#$B4#$C1#$C1#$D9#$C4
        ),
        ( { Vertical }
          { Top }		#$D1#$C2#$B3#$B3#$D5#$D1#$B8#$C6#$D8#$B5#$C6#$D8#$B5#$DA#$C2#$BF +
          				#$C3#$C5#$B4#$C3#$C5#$B4#$D5#$D1#$B8#$C6#$D8#$B5#$C6#$D8#$B5#$DA +
          				#$C2#$BF#$C3#$C5#$B4#$C3#$C5#$B4#$B3,	
          { Middle }	#$D8#$C5#$B3#$B3#$C6#$D8#$B5#$C6#$D8#$B5#$C6#$D8#$B5#$C3#$C5#$B4 +
          				#$C3#$C5#$B4#$C3#$C5#$B4#$C6#$D8#$B5#$C6#$D8#$B5#$C6#$D8#$B5#$C3 +
          				#$C5#$B4#$C3#$C5#$B4#$C3#$C5#$B4#$B3,
          { Bottom }	#$CF#$C1#$B3#$B3#$C6#$D8#$B5#$C6#$D8#$B5#$D4#$CF#$BE#$C3#$C5#$B4 +
          				#$C3#$C5#$B4#$C0#$C1#$D9#$C6#$D8#$B5#$C6#$D8#$B5#$D4#$CF#$BE#$C3 +
          				#$C5#$B4#$C3#$C5#$B4#$C0#$C1#$D9#$B3
        )
	  ),
      ( { Double Line }
        ( { Horizontal }
          { Left }		#$CD#$CD#$CC#$C6#$C9#$CB#$CB#$CC#$CE#$CE#$C8#$CA#$CA#$C9#$CB#$CB +
          				#$CC#$CE#$CE#$C8#$CA#$CA#$D5#$D1#$D1#$C6#$D8#$D8#$D4#$CF#$CF#$D5 +
          				#$D1#$D1#$C6#$D8#$D8#$D4#$CF#$CF#$CD,
          { Middle }	#$CD#$CD#$CE#$D8#$CB#$CB#$CB#$CE#$CE#$CE#$CA#$CA#$CA#$CB#$CB#$CB +
          				#$CE#$CE#$CE#$CA#$CA#$CA#$D1#$D1#$D1#$D8#$D8#$D8#$CF#$CF#$CF#$D1 +
          				#$D1#$D1#$D8#$D8#$D8#$CF#$CF#$CF#$CD,
          { Right }		#$CD#$CD#$B9#$B5#$CB#$CB#$BB#$CE#$CE#$B9#$CA#$CA#$BC#$CB#$CB#$BB +
          				#$CE#$CE#$B9#$CA#$CA#$BC#$D1#$D1#$B8#$D8#$D8#$B5#$CF#$CF#$BE#$D1 +
          				#$D1#$B8#$D8#$D8#$B5#$CF#$CF#$BE#$CD
        ),
        ( { Vertical }
          { Top }		#$CB#$D2#$BA#$BA#$C9#$CB#$BB#$CC#$CE#$B9#$CC#$CE#$B9#$D6#$D2#$B7 +
          				#$C7#$D7#$B6#$C7#$D7#$B6#$C9#$CB#$BB#$CC#$CE#$B9#$CC#$CE#$B9#$D6 +
          				#$D2#$B7#$C7#$D7#$B6#$C7#$D7#$B6#$BA,
          { Middle }	#$CE#$D7#$BA#$BA#$CC#$CE#$B9#$CC#$CE#$B9#$CC#$CE#$B9#$C7#$D7#$B6 +
          				#$C7#$D7#$B6#$C7#$D7#$B6#$CC#$CE#$B9#$CC#$CE#$B9#$CC#$CE#$B9#$C7 +
          				#$D7#$B6#$C7#$D7#$B6#$C7#$D7#$B6#$BA,
          { Bottom }	#$CA#$D0#$BA#$BA#$CC#$CE#$B9#$CC#$CE#$B9#$C8#$CA#$BC#$C7#$D7#$B6 +
          				#$C7#$D7#$B6#$D3#$D0#$BD#$CC#$CE#$B9#$CC#$CE#$B9#$C8#$CA#$BC#$C7 +
          				#$D7#$B6#$C7#$D7#$B6#$D3#$D0#$BD#$BA
        )
      )
      )
    );
    
  const	
  	DefaultCommandsCount = 41;
  	DefaultCommands : array [0..DefaultCommandsCount - 1] of TCodeMapEntry = (
	{  0}( FromCode:$0008;  ToCode:$0102;  ShiftCode:$0000;  Flags:$0002 ), 
	{  1}( FromCode:$0063;  ToCode:$0005;  ShiftCode:$0104;  Flags:$0002 ), 
	{  2}( FromCode:$0077;  ToCode:$0008;  ShiftCode:$0208;  Flags:$0002 ), 
	{  3}( FromCode:$0063;  ToCode:$0201;  ShiftCode:$0008;  Flags:$0002 ), 
	{  4}( FromCode:$0063;  ToCode:$0201;  ShiftCode:$0208;  Flags:$0002 ), 
	{  5}( FromCode:$0078;  ToCode:$0200;  ShiftCode:$0008;  Flags:$0002 ), 
	{  6}( FromCode:$0078;  ToCode:$0200;  ShiftCode:$0208;  Flags:$0002 ), 
	{  7}( FromCode:$0008;  ToCode:$0103;  ShiftCode:$0104;  Flags:$0001 ), 
	{  8}( FromCode:$0053;  ToCode:$0103;  ShiftCode:$0000;  Flags:$0002 ), 
	{  9}( FromCode:$5000;  ToCode:$0113;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 10}( FromCode:$000D;  ToCode:$0105;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 11}( FromCode:$001B;  ToCode:$0107;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 12}( FromCode:$5100;  ToCode:$011B;  ShiftCode:$0001;  Flags:$0002 ), 
	{ 13}( FromCode:$5100;  ToCode:$011B;  ShiftCode:$0002;  Flags:$0002 ), 
	{ 14}( FromCode:$4900;  ToCode:$011A;  ShiftCode:$0001;  Flags:$0002 ), 
	{ 15}( FromCode:$4900;  ToCode:$011A;  ShiftCode:$0002;  Flags:$0002 ), 
	{ 16}( FromCode:$3B00;  ToCode:$0007;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 17}( FromCode:$5200;  ToCode:$0108;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 18}( FromCode:$4B00;  ToCode:$0110;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 19}( FromCode:$4D00;  ToCode:$0117;  ShiftCode:$0008;  Flags:$0002 ), 
	{ 20}( FromCode:$4B00;  ToCode:$0117;  ShiftCode:$0208;  Flags:$0002 ), 
	{ 21}( FromCode:$4B00;  ToCode:$0116;  ShiftCode:$0008;  Flags:$0002 ), 
	{ 22}( FromCode:$4D00;  ToCode:$0116;  ShiftCode:$0208;  Flags:$0002 ), 
	{ 23}( FromCode:$4D00;  ToCode:$0115;  ShiftCode:$0001;  Flags:$0002 ), 
	{ 24}( FromCode:$4D00;  ToCode:$0115;  ShiftCode:$0002;  Flags:$0002 ), 
	{ 25}( FromCode:$0000;  ToCode:$0000;  ShiftCode:$0000;  Flags:$0000 ), 
	{ 26}( FromCode:$5100;  ToCode:$0119;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 27}( FromCode:$4900;  ToCode:$0118;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 28}( FromCode:$0076;  ToCode:$0202;  ShiftCode:$0008;  Flags:$0002 ), 
	{ 29}( FromCode:$0076;  ToCode:$0202;  ShiftCode:$0208;  Flags:$0002 ), 
	{ 30}( FromCode:$4B00;  ToCode:$0114;  ShiftCode:$0001;  Flags:$0002 ), 
	{ 31}( FromCode:$4B00;  ToCode:$0114;  ShiftCode:$0002;  Flags:$0002 ), 
	{ 32}( FromCode:$0071;  ToCode:$0006;  ShiftCode:$0208;  Flags:$0001 ), 
	{ 33}( FromCode:$000D;  ToCode:$0106;  ShiftCode:$0004;  Flags:$0001 ), 
	{ 34}( FromCode:$4D00;  ToCode:$0111;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 35}( FromCode:$007A;  ToCode:$0104;  ShiftCode:$0104;  Flags:$0001 ), 
	{ 36}( FromCode:$0009;  ToCode:$0100;  ShiftCode:$0000;  Flags:$0002 ), 
	{ 37}( FromCode:$5100;  ToCode:$011D;  ShiftCode:$0208;  Flags:$0001 ), 
	{ 38}( FromCode:$4900;  ToCode:$011C;  ShiftCode:$0208;  Flags:$0001 ), 
	{ 39}( FromCode:$0009;  ToCode:$0101;  ShiftCode:$0003;  Flags:$0001 ), 
	{ 40}( FromCode:$4800;  ToCode:$0112;  ShiftCode:$0000;  Flags:$0002 )
	);

  type
    PEvents = ^TEvents;
    TEvents = array[1..$FFFF div Sizeof(TEvent)] of TEvent;
    EventBuf = object
         Buf   : PEvents;
         Head,
         Tail,
         Max,
         Count : word;
      procedure Init( ABuf : PEvents; AMax : word );
      procedure Done;
      procedure Purge;
      function  UsedSpace : word;
      function  FreeSpace : word;
      function  GetEvent(var Event : TEvent) : boolean;
      function  PutEvent(var Event : TEvent) : boolean;
    end;

  var
    TimerTick    : LongInt absolute $0040:$006c;  { Timer tick counter }
    OldExitProc  : pointer;        { Address of old exit procedure }
    OldInt05     : procedure;      { Address of old interrupt vector 05h }
    OldInt09     : procedure;      { Address of old interrupt vector 09h }
    OldInt15     : procedure;      { Address of old interrupt vector 15h }
    OldInt1B     : procedure;      { Address of old interrupt vector 1Bh }
    OldInt23     : procedure;      { Address of old interrupt vector 23h }
    BIOSPtr      : ^BIOSDataType;  { Pointer to main Video BIOS Data }
    OrigVideoPtr : pointer;		   { Pointer to video memory at start up }
    VideoPtr     : pointer;        { Pointer to video memory }
    VideoSeg     : word;           { Video segment for direct video writes }
    ActiveOfs    : word;           { Offset for direct video writes }
    ActivePage   : byte;           { Page number of active video page }
    RegenSize    : word;           { Size of video regen buffer }
    CursorXY     : word;           { Absolute screen location of cursor }
    BytesPerLine : word;

    FirstAttr    : byte;           { StartUp TextAttr }
    FirstCursor  : word;           { StartUp Cursor Size/Shape }
    FirstBlink   : boolean;        { Startup Blink/Intensity setting }

    SysReqMake   : word;
    SysReqBreak  : word;

    KeyBuffer     : array[1..KeyBufSize] of TEvent;
    MouseBuffer   : array[1..MouseBufSize] of TEvent;
    CommandBuffer : array[1..CommandBufSize] of TEvent;
    KeyBuf        : EventBuf;
    MouseBuf      : EventBuf;
    CommandBuf    : EventBuf;

    HandlerMask   : word;     { Mouse Handler Event Mask }
    HandlerProc   : pointer;  { Mouse Handler Procedure }
    
    KeyMap,   CommandMap  : PCodeMap;
    KeyCount, CommandCount : word; 
 
function MemCheck(AValue : LongInt) : boolean;
begin
	if MemAvail < MinMemAvail then MinMemAvail := MemAvail;
	if MaxAvail < MinMaxAvail then MinMaxAvail := MaxAvail;
	MemCheck := MaxAvail > AValue;
end;
   
(* Key mapper for events and Keypresses *)
function MapKeyCode(Map : pointer; Count, ShiftCode, KeyCode : word) : word; assembler;
asm
	PUSH  DS
	PUSH  DI
	MOV   AX, KeyCode
	MOV   BX, Count
	CMP	  BX, 0
	JE	  @@NotFound
	MOV   CX, ShiftCode
	LDS   DI, DWORD PTR Map
@@Search:
	CMP   AX, DS:[DI]
	JE    @@Maybe
@@ContinueSearch:
	ADD   DI, 16
	DEC   BX
	CMP   BX, 0
	JE    @@NotFound
	JMP   @@Search
@@Maybe:
	MOV   DX, DS:[DI + 6]		{ Get flags word }
	AND   DX, 0003h				{ first 3 bits are for shiftcode requirements }
	CMP   DX, 0000h				{ Ignore shift code }
	JE	  @@Found
	CMP   DX, 0002h				{ Only Exact ShiftCode }
	JE    @@ExactOnly
@@ContainsCode:
	MOV   DX, DS:[DI + 4]		{ Get ShiftCode }
	AND   CX, DX
	CMP   CX, DX	
	JE	  @@Found
	JMP   @@ContinueSearch
@@ExactOnly:
	MOV   DX, DS:[DI + 4]		{ Get ShiftCode }
	CMP   CX, DX	
	JE	  @@Found
	JMP   @@ContinueSearch
@@Found:
	MOV   AX, DS:[DI + 2]
@@NotFound:
	POP   DI
	POP   DS
end;

{ Basically MapCommand is identical to MapKeyCode, except returns 0, if not found }
function MapCommand(Map : pointer; Count, ShiftCode, KeyCode : word) : word; assembler;
asm
	PUSH  DS
	PUSH  DI
	MOV   AX, KeyCode
	MOV   BX, Count
	CMP	  BX, 0
	JE	  @@NotFound
	MOV   CX, ShiftCode
	LDS   DI, DWORD PTR Map
@@Search:
	CMP   AX, DS:[DI]
	JE    @@Maybe
@@ContinueSearch:
	ADD   DI, 16
	DEC   BX
	CMP   BX, 0
	JE    @@NotFound
	JMP   @@Search
@@Maybe:
	MOV   DX, DS:[DI + 6]		{ Get flags word }
	AND   DX, 0003h				{ first 3 bits are for shiftcode requirements }
	CMP   DX, 0000h				{ Ignore shift code }
	JE	  @@Found
	CMP   DX, 0002h				{ Only Exact ShiftCode }
	JE    @@ExactOnly
@@ContainsCode:
	MOV   DX, DS:[DI + 4]		{ Get ShiftCode }
	AND   CX, DX
	CMP   CX, DX	
	JE	  @@Found
	JMP   @@ContinueSearch
@@ExactOnly:
	MOV   DX, DS:[DI + 4]		{ Get ShiftCode }
	CMP   CX, DX	
	JE	  @@Found
	JMP   @@ContinueSearch
@@Found:
	MOV   AX, DS:[DI + 2]
	JMP   @@Done
@@NotFound:
	XOR   AX, AX
@@Done:
	POP   DI
	POP   DS
end;

procedure SetKeyMap(CodeMap : PCodeMap; Count : word);
begin
	if Assigned(Keymap) then begin
		FreeMem(Keymap, KeyCount * Sizeof(TCodeMapEntry));
		Keymap := nil;
		KeyCount := 0;
	end;
	if Not Assigned(CodeMap) then exit;
	if not MemCheck(Count * Sizeof(TCodeMapEntry)) then halt(8);
	KeyCount := Count;
	GetMem(Keymap, KeyCount * Sizeof(TCodeMapEntry));
	Move(CodeMap^, KeyMap^, KeyCount * Sizeof(TCodeMapEntry)); 
end;

procedure SetCommandMap(CodeMap : PCodeMap; Count : word);
begin
	if Assigned(CommandMap) then begin
		FreeMem(CommandMap, CommandCount * Sizeof(TCodeMapEntry));
		CommandMap := nil;
		CommandCount := 0;
	end;
	if Not Assigned(CodeMap) then exit;
	if not MemCheck(Count * Sizeof(TCodeMapEntry)) then halt(8);
	CommandCount := Count;
	GetMem(CommandMap, CommandCount * Sizeof(TCodeMapEntry));
	Move(CodeMap^, CommandMap^, CommandCount * Sizeof(TCodeMapEntry)); 
end;

procedure GetKeyMap(var CodeMap : PCodeMap; var Count : word);
begin
	CodeMap := KeyMap;
	Count := KeyCount;
end;

procedure GetCommandMap(var CodeMap : PCodeMap; var Count : word);
begin
	CodeMap := CommandMap;
	Count := CommandCount;
end;

procedure InitQCrtMaps;
begin
	SetKeyMap(nil,0);
	SetCommandMap(nil,0);
	SetCommandMap(@DefaultCommands, DefaultCommandsCount);
end;

(* Internal Event Buffer Handler *)

  procedure EventBuf.Init( ABuf : PEvents; AMax : word );
    begin
      Buf := ABuf;
      Max := AMax;
      Purge;
    end;

  procedure EventBuf.Done;
    begin
      Purge;
    end;

  procedure EventBuf.Purge;
    begin
      Head  := 1;
      Tail  := 1;
      Count := 0;
    end;

  function  EventBuf.UsedSpace : word;
    begin
      UsedSpace := Count;
    end;

  function  EventBuf.FreeSpace : word;
    begin
      FreeSPace := Max - Count;
    end;

  function  EventBuf.GetEvent(var Event : TEvent) : boolean;
    begin
      if UsedSpace > 0 then
        begin
          Event := Buf^[Head];
          Inc(Head);
          Inc(Count);
          if Head > Max then Head := 1;
          GetEvent := True;
        end
      else
        GetEvent := False;
    end;

  function  EventBuf.PutEvent(var Event : TEvent) : boolean;
  	var
  		Temp : word;
  		{KS : DWord;}
    begin
      if FreeSpace > 0 then
        begin
          if (Event.What = evKeyDown) and Assigned(KeyMap) then begin
          	{ Translate/remap any keys in the KeyMap }
          	Temp := MapKeyCode(KeyMap, KeyCount, Event.ShiftCode, Event.KeyCode);
          	if Temp <> Event.KeyCode then begin
          		Event.Original := Event.KeyCode;
          		Event.KeyCode := Temp;
          	end else
          		Event.Original := 0;
          end;
          if (Event.What = evKeyDown) and Assigned(CommandMap) then begin
          	{ Translate/remap any keys into commands in the CommandMap }
          	Temp := MapCommand(CommandMap, CommandCount, Event.ShiftCode, Event.KeyCode);
          	if Temp <> $0000 then begin
          		{KS.L := Event.KeyCode;
          		KS.H := Event.ScanCode;}
          		ClearEvent(Event);
          		Event.InfoPtr := nil;
          		Event.What := evCommand;
          		Event.Command := Temp;
          	end;          	
          end;
          Buf^[Tail] := Event;
          Inc(Tail);
          Dec(Count);
          if Tail > Max then Tail := 1;
          PutEvent := True;
        end
      else
        PutEvent := False;
    end;
    
  function PutCommand(AValue : word; ASender : pointer) : boolean;
  var
  	Temp : TEvent;
  begin
  	ClearEvent(Temp);
  	Temp.InfoPtr := ASender;
  	Temp.What := evCommand;
  	Temp.Command := AValue;
  	PutCommand := PutEvent(Temp);
  end;

  function PutSystem(AValue : word; ASender : pointer) : boolean;
  var
  	Temp : TEvent;
  begin
  	ClearEvent(Temp);
  	Temp.InfoPtr := ASender;
  	Temp.What := evSystem;
  	Temp.Command := AValue;
  	PutSystem := PutEvent(Temp);
  end;
  
(* Internal Keyboard Routines *)
  function ReadKeyboardStd : word; assembler;
    asm
      MOV  AH, 00h
      INT  16h
      CMP  AL, 00h
      JE   @@Done
      MOV  AH, 00h
    @@Done:
    end;

  function KeypressedStd : boolean; assembler;
    asm
      MOV  AH, 01h
      INT  16h
      MOV  AL, False
      JZ   @@Done
      MOV  AL, True
    @@Done:
    end;

  function ReadKeyboardEnh : word; assembler;
    asm
      MOV  AH, 10h
      INT  16h
      CMP  AL, 00h
      JE   @@Done2
      CMP  AL, 0E0h
      JE   @@Done1
      MOV  AH, 00h
      JMP  @@Done2
    @@Done1:
      MOV  AL, 00h
    @@Done2:
    end;

  function KeypressedEnh : boolean; assembler;
    asm
      MOV  AH, 11h
      INT  16h
      MOV  AL, False
      JZ   @@Done
      MOV  AL, True
    @@Done:
    end;
    
(* Internal Mouse Routines *)
  function ResetMouse(var Found : boolean; var Buttons : integer) : Boolean; assembler;
    asm
      MOV   AX, 0
      INT   33h
      CMP   AX, 0
      JE    @NoMouse
      MOV   AL, True
      JMP   @Done
    @NoMouse:
      MOV  BX, 0
      MOV  AL, False
    @Done:
      LES   DI, Buttons
      MOV   ES:[DI], BX
      LES   DI, Found
      MOV   ES:[DI], AL
    end;

  procedure ShowMouse; assembler;
    asm
      MOV  AX, 1
      INT  33h
    end;

  procedure HideMouse; assembler;
    asm
      MOV  AX, 2
      INT  33h
    end;

  procedure GetMousePos(var X, Y, Buttons : integer); assembler;
    asm
      MOV  AX, 3
      MOV  BX, 0
      INT  33h
      LES  DI, Buttons
      MOV  ES:[DI], BX

      MOV  BX, CX
      MOV  CX, MouseShift
      SHR  BX, CL
      INC  BX
      XCHG CL, CH
      SHR  DX, CL
      INC  DX

      LES  DI, X
      MOV  ES:[DI], BX
      LES  DI, Y
      MOV  ES:[DI], DX
    end;

  procedure SetMousePos(X, Y : integer); assembler;
    asm
      MOV  AX, 4
      MOV  BX, X
      MOV  DX, Y
      DEC  BX
      DEC  DX
      MOV  CX, MouseShift
      SHL  BX, CL
      XCHG CL, CH
      SHL  DX, CL
      MOV  CX, BX
      INT  33h
    end;

  procedure SetMouseTextPtr(Style, AMask, XMask : word); assembler;
    asm
      MOV  AX, 0Ah
      MOV  BX, Style
      MOV  CX, AMask
      MOV  DX, XMask
      INT  33h
    end;

  procedure SwapMouseHandler(var EventMask : word; var Handler : Pointer); assembler;
    asm
      MOV  AX, 14h
      LES  DI, EventMask
      MOV  CX, ES:[DI]
      LES  DI, Handler
      MOV  DX, ES:[DI]
      MOV  ES, ES:[DI + 2]
      INT  33h
      MOV  BX, ES
      LES  DI, Handler
      MOV  ES:[DI], DX
      MOV  ES:[DI + 2], BX
      LES  DI, EventMask
      MOV  ES:[DI], CX
    end;

  procedure MouseHandler ( Flags, CS, IP, AX, BX, CX, DX, SI,
    DI, DS, ES, BP : word); interrupt;
    const
      LastX : word = 0;
      LastY : word = 0;
      LastB : word = 0;
      LastC : array[0..15] of LongInt = (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    var
      Event : TEvent;
      I     : byte;
      PutIt : boolean;
    begin
      {
        AX = Event Flag
        BX = Button State
        CX = X coor
        DX = Y Coor
        SI = X Movement
        DI = Y Movement
        DS = Mouse Driver Data Segment
      }
      PutIt             := False;
      Event.What        := evNothing;
      Event.Double      := False;
      Event.Buttons     := BX;
      Event.Where.X     := (CX shr Lo(MouseShift)) + 1;
      Event.Where.Y     := (DX shr Hi(MouseShift)) + 1;
      if (Event.Where.X <> LastX) or (Event.Where.Y <> LastY) then
        begin
          PutIt      := True;
          Event.What := Event.What or evMouseMove;
        end;
      if (Event.Buttons <> LastB) then
        begin
          PutIt := True;
          for I := 0 to 15 do
            if LastB and (1 shl I) <> Event.Buttons and (1 shl I) then
              case Event.Buttons and (1 shl I) = (1 shl I) of
                False : Event.What := Event.What or evMouseUp;
                True  : begin
                  Event.What := Event.What or evMouseDown;
                  if TimerTick - LastC[I] <= MouseDouble then
                    Event.Double := True;
                  LastC[I] := TimerTick;
                end;
              end;
        end;
      LastX := Event.Where.X;
      LastY := Event.Where.Y;
      LastB := Event.Buttons;
      if PutIt then PutEvent(Event);
      asm
        MOV  AX, 0Bh
        INT  33h
        MOV  SP,BP
        pop  bp
        pop  es
        pop  ds
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        retf
      end;
    end;

(* Internal service routines *)
  procedure GetIntVec ( IntNo : byte; var Vec : pointer ); assembler;
    asm
      MOV  AH, $35
      MOV  AL, IntNo
      INT  $21
      MOV  DX, ES
      LES  DI, Vec
      MOV  AX, BX
      STOSW
      MOV  AX, DX
      STOSW
    end;

  procedure SetIntVec ( IntNo : byte; Vec : pointer ); assembler;
    asm
      PUSH DS
      MOV  AH, $25
      MOV  AL, IntNo
      LDS  DX, Vec
      INT  $21
      POP  DS
    end;

  procedure SetCursorLoc; assembler;
    asm
      MOV  AH, 02h
      MOV  BH, ActivePage
      MOV  DX, CursorXY
      INT  10h
    end;

  procedure GetCursorLoc; assembler;
    asm
      MOV  AH, 03h
      MOV  BH, ActivePage
      INT  10h
      MOV  CursorXY, DX
    end;

  procedure UpdateCursor; assembler;
    asm
      CMP  CheckCursor, True
      JNE  @@Done
      CALL  SetCursorLoc
    @@Done:
    end;

  function GetVideoOfs( XY : word ) : word; assembler;
    asm
      PUSH BX
      PUSH CX
      PUSH DX
      MOV  BX, ActiveOfs
      MOV  AX, BytesPerLine
      MOV  CX, XY
      MOV  DL, CH
      XOR  DH, DH
      MUL  DX
      ADD  BX, AX
      MOV  AX, 0002h
      XOR  CH, CH
      MUL  CX
      ADD  AX, BX
      POP  DX
      POP  CX
      POP  BX
    end;

  function Return : word; assembler;
    asm
      MOV  DX, CursorXY
      MOV  AX, WindMin
      MOV  CX, WindMax
      MOV  DL, AL
      INC  DH
      CMP  DH, CH
      JNG  @@Return2
      CMP  CheckScroll, False
      JE   @@Return1
      PUSH DX
      MOV  CursorXY, AX
      CALL DelLine
      POP  DX
      DEC  DH
      JMP  @@Return3
    @@Return1:
      MOV  DX, AX
    @@Return2:
    @@Return3:
      MOV  CursorXY, DX
      CALL UpdateCursor
      MOV  AX, CursorXY
    end;

  function LineFeed : word; assembler;
    asm
      MOV  DX, CursorXY
      MOV  AX, WindMin
      MOV  CX, WindMax
      MOV  DL, AL
      INC  DH
      CMP  DH, CH
      JNG  @@Return2
      CMP  CheckScroll, False
      JE   @@Return1
      PUSH DX
      MOV  CursorXY, AX
      CALL DelLine
      POP  DX
      DEC  DH
      JMP  @@Return3
    @@Return1:
      MOV  DX, AX
    @@Return2:
    @@Return3:
      MOV  CX, CursorXY
      MOV  DL, CL
      MOV  CursorXY, DX
      CALL UpdateCursor
      MOV  AX, CursorXY
    end;

  procedure NullProc; far; assembler;
    asm
    end;

  procedure IdleMouseProc; far;
    var
      X,
      Y,
      Attr   : byte;
      WMin,
      WMax,
      Cursor : word;
    begin
      if @IdleProc <> @NullProc then 
      	begin
		  Cursor := GetCursor;
		  X := WhereX;
		  Y := WhereY;
		  WMin := WindMin;
		  WMax := WindMax;
		  Attr := TextAttr;
		  Window(1, 1, Lo(ScreenMax) + 1, Hi(ScreenMax) + 1);
		  IdleProc;
		  Window(Lo(WMin) + 1, Hi(WMin) + 1, Lo(WMax) + 1, Hi(WMax) + 1);
		  GotoXY ( X, Y );
		  TextAttr := Attr;
		  MoveCursor;
		  SetCursor(Cursor);
		end;
    end;

  const
  	PreMouse : boolean = false;
  	
  procedure PreWriteMouseProc; far;
    begin
      PreMouse := MouseAvail and (VideoPtr = OrigVideoPtr);
      if PreMouse then HideMouse;
      PreWriteProc;
    end;

  procedure PostWriteMouseProc; far;
    begin
      PostWriteProc;
      If MouseAvail and PreMouse then ShowMouse;
    end;

  procedure ReInitMouseProc; far;
    begin
      if CheckMouse then
        begin
          ResetMouse (MouseAvail, MouseButtons);
          if MouseAvail then
            begin
              HandlerMask := $FFFF;
              HandlerProc := @MouseHandler;
              SwapMouseHandler(HandlerMask, HandlerProc);
            end;
        end;
      if MouseAvail then ShowMouse;
      case LastMode and $FF of
      { TextModes }
        0..1    : MouseShift := $0304;
        2..3, 7 : MouseShift := $0303;
      { Graphics Modes }
        $0D, $13 : MouseShift := $0304;
        $0E      : MouseShift := $0303;
        $0F, $10 : MouseShift := $0403; { Not Correct }
        $11, $12 : MouseShift := $0403;
      end;
      Case LastMode and $FF of
        0..3 : SetMouseTextPtr(0, $7FFF, $7F00);
        7    : SetMouseTextPtr(0, $FFFF, $1000);
      end;
    end;

  procedure PreWrite; assembler;
    const
      Level : word = 0;
    asm
      CMP  Level, 0
      JNE  @@Done
      INC  Level
      PUSH AX
      PUSH BX
      PUSH CX
      PUSH DX
      PUSH SI
      PUSH DI
      PUSH ES
      CALL PreWriteMouseProc
      POP  ES
      POP  DI
      POP  SI
      POP  DX
      POP  CX
      POP  BX
      POP  AX
      DEC  Level
    @@Done:
    end;

  procedure PostWrite; assembler;
    const
      Level : word = 0;
    asm
      CMP  Level, 0
      JNE  @@Done
      INC  Level
      PUSH AX
      PUSH BX
      PUSH CX
      PUSH DX
      PUSH SI
      PUSH DI
      PUSH ES
      CALL PostWriteMouseProc
      POP  ES
      POP  DI
      POP  SI
      POP  DX
      POP  CX
      POP  BX
      POP  AX
      DEC  Level
    @@Done:
    end;

{ Print-Screen Handler }
  procedure VideoInt05; Interrupt; assembler; { Print-Screen }
    asm
      CMP  CheckPrint, True
      JE   @PrintIt
      INC  PrintCount
      JMP  @Done
    @PrintIt:
      CALL PrintScreen
    @Done:
    end;

{ Keyboard Interrupt }
  procedure VideoInt09; interrupt;
    var
      Event : TEvent;
    begin
      asm
        PUSHF
        CALL OldInt09
      end;
      Event.What := evKeyDown;
      Event.InfoPtr := nil;
      Event.ShiftCode := MemW[Seg0040:$0017] and (not KeyboardIgnore);
      case Check101 of
        False : if KeypressedStd then begin
          Event.KeyCode := ReadKeyboardStd;
          KeyBuf.PutEvent( Event );
        end;
        True : if KeypressedEnh then begin
          Event.KeyCode := ReadKeyboardEnh;
          KeyBuf.PutEvent( Event );
        end;
      end;
  end;

{ SysReq Handler }
  procedure SysReqPtr; assembler;
    asm
      DW 0,0
    end;

  procedure VideoInt15; assembler; { SysReq }
  { This CANNOT be compiled as interrupt or it won't work properly. The
    SysReq key is only on of the services provided by this interrupt.
    And Turbo Pascal PUSH'S AND POP'S everthing in an interrupt procedure,
    so the return data would get lost. }
    asm
      PUSHF
      PUSH DS
      PUSH AX
      MOV  AX, SEG @DATA
      MOV  DS, AX
      POP  AX

      CMP  AH, 85h
      JNE  @Done

      CMP  CheckSysReq, True
      JE   @Done

      CMP  AL, 0
      JNE  @KeyBreak
      INC  SysReqCount
      INC  SysReqMake
      JMP  @Bye
    @KeyBreak:
      INC  SysReqBreak
    @Bye:
      POP  DS
      POPF
      IRET
    @Done:
      POP  DS
      CALL DWORD PTR CS:[OFFSET SysReqPtr]
      IRET
    end;

(* New Interrupt 1B *)
  procedure VideoINT1B; interrupt; assembler; { CTRL-BREAK }
    asm
      CMP CheckBreak, True
      JE  @@DoBreak
      PUSHF
      INC BreakCount
      POPF
      JMP @@BreakDone
    @@DoBreak:
      PUSHF
      CALL OldInt1B
    @@BreakDone:
    end;

(* New Interrupt 23 *)
  procedure VideoInt23; interrupt; assembler; { CTRL-BREAK }
    asm
      CMP CheckBreak, True
      JE  @@DoBreak
      PUSHF
      INC BreakCount
      POPF
      JMP @@BreakDone
    @@DoBreak:
      PUSHF
      CALL OldInt23
    @@BreakDone:
    end;

(* Unit shutdown procedure *)
  procedure DoneVideoUnit; far;
    begin
      ExitProc      := OldExitProc;
      if MouseAvail then HideMouse;
      ResetMouse(MouseAvail, MouseButtons);
      IdleProc      := NullProc;
      PreWriteProc  := NullProc;
      PostWriteProc := NullProc;
      ReInitProc    := NullProc;
      SetIntVec($05, @OldInt05 );
      SetIntVec($09, @OldInt09 );
      SetIntVec($15, @OldInt15 );
      SetIntVec($1B, @OldInt1B );
      SetIntVec($23, @OldInt23 );
      Assign(Output, '');
      Assign(Input, '');
      ReWrite(Output);
      Reset(Input);
      CommandBuf.Done;
      MouseBuf.Done;
      KeyBuf.Done;
      if Assigned(KeyMap) then begin
		FreeMem(KeyMap, KeyCount * Sizeof(TCodeMapEntry));
		Keymap := nil;
	  end;
      if Assigned(CommandMap) then begin
		FreeMem(CommandMap, CommandCount * Sizeof(TCodeMapEntry));
		CommandMap := nil;
	  end;
    end;

(* Unit initialization procedure *)
  procedure InitQCrtUnit;
    var
      P : Pointer;
    begin
      FirstMaxAvail := MaxAvail;
      FirstMemAvail := MemAvail;
      MinMaxAvail := MaxAvail;
      MinMemAvail := MemAvail;

      KeyBuf.Init    (@KeyBuffer, KeyBufSize);
      MouseBuf.Init  (@KeyBuffer, KeyBufSize);
      CommandBuf.Init(@KeyBuffer, KeyBufSize);
      EventFlag     := False;
      EditSender    := nil;
      KeyMap 		:= nil;
      KeyCount   	:= 0;
      CommandMap	:= nil;
      CommandCount 	:= 0;
      KeyboardIgnore := kbScrollLock or kbInsertLock or kbPauseFlag or kbNumsLock;
      IdleProc      := NullProc;
      PreWriteProc  := NullProc;
      PostWriteProc := NullProc;
      ReInitProc    := NullProc;
      BIOSptr := Ptr(Seg0040, $0049);
      CheckBreak    := True;
      CheckPrint    := True;
      CheckSysReq   := True;
      CheckMouse    := True;
      InsertMode    := True;
      Check101      := (Mem[Seg0040:$0096]) and $10 = $10;
      CheckTab 		:= False;
      MouseAvail    := False;
      MouseButtons  := 0;
      MouseShift    := $0303;
      MouseDouble   := DefaultDoubleClick;
      CheckEOF      := False;
      CheckSnow     := False;
      CheckCursor   := True;
      CheckScroll   := True;
      CheckChar     := True;
      BreakCount    := 0;
      PrintCount    := 0;
      SysReqCount   := 0;
      SysReqMake    := 0;
      SysReqBreak   := 0;
      UserFontSize  := 8;
      UserFontPtr   := Pointer(MemL[0:$0043 * 4]);
      FirstCursor   := GetCursor;
      FirstBlink    := GetBlink;
      FirstAttr     := TextAttr;
      GetIntVec($05, @OldInt05 );
      GetIntVec($09, @OldInt09 );
      GetIntVec($15, @OldInt15 );
      GetIntVec($1B, @OldInt1B );
      GetIntVec($23, @OldInt23 );
      P := @SysReqPtr;
      P := Ptr(Seg(P^), Ofs(P^));
      Move (OldInt15, P^, Sizeof(Pointer));
      OldExitProc   := ExitProc;
      ExitProc      := @DoneVideoUnit;
      SetIntVec($05, @VideoInt05 );
      SetIntVec($09, @VideoInt09 );
      SetIntVec($15, @VideoInt15 );
      SetIntVec($1B, @VideoInt1B );
      SetIntVec($23, @VideoInt23 );
 	  { ResetMaps; }
      AssignCrt(Output);
      Rewrite(Output);
      AssignCRT(Input);
      Reset(Input);
      InitQCrt;
    end;

(* variable reset procedure *)
  procedure InitQCrt;
    begin
    { Default DirectVideo to True if in any text mode. }
      DirectVideo := (BIOSPtr^.Mode = BW40) or (BIOSPtr^.Mode = BW80) or
                     (BIOSPtr^.Mode = CO40) or (BIOSPtr^.Mode = CO80) or
                     (BIOSPtr^.Mode = MONO);
    { Initialize LastMode to Current Mode and Font size }
      LastMode    := BIOSPtr^.Mode + (BIOSPtr^.CharSize Shl 8);
    { Initialize VideoSeg, and Offsets, and pages. }
      if LastMode and $00FF = Mono then
        VideoSeg := SegB000
      else
        VideoSeg := SegB800;
      ActiveOfs  := BIOSPtr^.VisualOfs;
      VideoPtr   := Ptr(VideoSeg, ActiveOfs);
      OrigVideoPtr := VideoPtr;
      ActivePage := BIOSPtr^.VisualPage;
      RegenSize  := BIOSPtr^.RegenSize;
    { Initialize Current Text Attribute }
      Case DirectVideo of
        True  : TextAttr := Mem[VideoSeg:ActiveOfs + (BIOSPtr^.Columns shl 1) * (BIOSPtr^.Rows + 1) - 1];
        False : TextAttr := LightGray;
      end;
      TextChar := #32;
    { Initialize Window Limits }
      ScreenMax  := (Word(BIOSPtr^.Rows) Shl 8) + (BIOSPtr^.Columns - 1);
      WindMin    := $0000;
      WindMax    := ScreenMax;
      BytesPerLine := BIOSPtr^.Columns shl 1;
      FontHeight := Word(Mem[Seg0040:$0085]);
      MouseHomeX := BIOSPtr^.Columns shr 1;
      MouseHomeY := BIOSPtr^.Rows shr 1;
      GetCursorLoc;
      ReInitMouseProc;
    end;

(* Standard CRT like procedures *)
  function KeyPressed: Boolean;
    begin
      Keypressed := KeyBuf.UsedSpace > 0;
    end;

  function ReadKey: Char;
    const
      Next : String[1] = '';
    var
      Event : TEvent;
    begin
      if Next <> '' then
        begin
          ReadKey := Next[1];
          Next := '';
        end
      else
        begin
          While Not KeyBuf.GetEvent(Event) do;
          if Event.CharCode = #0 then
            Next := Char(Event.ScanCode);
          ReadKey := Event.CharCode;
        end;
    end;

  procedure TextMode(Mode: word); assembler;
    asm
      MOV AX, Mode
      XOR AH, AH
      INT $10
      MOV CX, Mode
      XOR CL, CL
      CMP CX, 0
      JE  @@Done
      MOV AL, $12
      CMP CX, Font8x8
      JE  @@LoadFont
      MOV AL, $11
      CMP CX, Font8x14
      JE  @@LoadFont
      MOV AL, $14
      CMP CX, Font8x16
      JE  @@LoadFont
      CMP CX, FontUser
      JE  @@LoadUserFont
      JMP @@Done
    @@LoadUserFont:
      PUSH BP
      MOV AX, $1110
      MOV BL, $00
      MOV CX, $00FF
      MOV DX, $0000
      MOV BH, UserFontSize
      LES BP, UserFontPtr
      INT $10
      POP BP
      JMP @@Done
    @@LoadFont:
      MOV AH, $11
      MOV BL, 0 { Font block to Load }
      INT $10
    @@Done:
      CALL InitQCrt
    end;
    
  procedure Window(X1,Y1,X2,Y2: Byte); assembler;
    asm
      MOV  AL, X1
      MOV  AH, Y1
      MOV  CL, X2
      MOV  CH, Y2
      SUB  AX, 0101h
      SUB  CX, 0101h
      CMP  AL, CL
      JLE  @@NoSwap1
      XCHG AL, CL
    @@NoSwap1:
      CMP  AH, CH
      JLE  @@NoSwap2
      XCHG AH, CH
    @@NoSwap2:
      CMP  AL, 0
      JL   @@BadWindow
      CMP  AH, 0
      JL   @@BadWindow
      MOV  DX, ScreenMax
      CMP  CL, DL
      JG   @@BadWindow
      CMP  CH, DH
      JG   @@BadWindow
      MOV  WindMin, AX
      MOV  WIndMax, CX
      MOV  CursorXY, AX
      CALL UpdateCursor
    @@BadWindow:
    end;

  procedure SubWindow ( X1, Y1, X2, Y2 : byte ); assembler;
    asm
      MOV BX, WindMin
      MOV CX, WindMax
      ADD CX, 0101h
      MOV AL, X1
      MOV AH, Y1
      MOV DL, X2
      MOV DH, Y2
      ADD AX, BX
      ADD DX, BX
      CMP AL, CL
      JG  @@Done
      CMP DL, CL
      JG  @@Done
      CMP AH, CH
      JG  @@Done
      CMP DH, CH
      JG  @@Done
      XOR  CX, CX
      MOV  CL, AL
      PUSH CX
      MOV  CL, AH
      PUSH CX
      MOV  CL, DL
      PUSH CX
      MOV  CL, DH
      PUSH CX
      CALL Window
    @@Done:
    end;

  function GetMaxX : byte; assembler;
    asm
      MOV  AX, WindMax
      SUB  AX, WindMin
      INC  AL
    end;

  function GetMaxY : byte; assembler;
    asm
      MOV  AX, WindMax
      SUB  AX, WindMin
      MOV  AL, AH
      INC  AL
    end;

  procedure GotoXY(X,Y: Byte); assembler;
    asm
      MOV  AL, X
      MOV  AH, Y
      ADD  AX, WindMin
      SUB  AX, 0101h
      MOV  DX, WindMax
      MOV  CX, WindMin
    {$IFDEF RollOver }
      PUSH BX
      MOV  BX, DX
      SUB  BX, CX
      ADD  BX, 0101H
    {$ENDIF}
    @@Step1:
      CMP  AL, DL
      JLE  @@Step2
    {$IFDEF RollOver }
      SUB  AL, BL
      JMP  @@Step1
    {$ELSE}
      MOV  AL, DL
    {$ENDIF}
    @@Step2:
      CMP  AL, CL
      JGE  @@Step3
    {$IFDEF RollOver}
      ADD  AL, BL
      JMP  @@Step2
    {$ELSE}
      MOV  AL, CL
    {$ENDIF}
    @@Step3:
      CMP  AH, DH
      JLE  @@Step4
    {$IFDEF RollOver}
      SUB  AH, BH
      JMP  @@Step3
    {$ELSE}
      MOV  AH, DH
    {$ENDIF}
    @@Step4:
      CMP  AH, CH
      JGE  @@Step5
    {$IFDEF RollOver}
      ADD  AH, BH
      JMP  @@Step4
    {$ELSE}
      MOV  AH, CH
    {$ENDIF}
    @@Step5:
    {$IFDEF RollOver}
      POP  BX
    {$ENDIF}
      MOV  CursorXY, AX
      CALL UpdateCursor
    end;

  function WhereX: Byte; assembler;
    asm
      MOV  AX, CursorXY
      SUB  AX, WindMin
      INC  AL
    end;

  function WhereY: Byte; assembler;
    asm
      MOV  AX, CursorXY
      SUB  AX, WindMin
      INC  AH
      MOV  AL, AH
    end;

  procedure ClrArea ( XY1, XY2 : word ); assembler;
    asm
      CALL PreWrite
      CMP  DirectVideo, True
      JE   @@Direct
      MOV  CX, XY1
      MOV  DX, XY2
      CMP  TextChar, 32
      JNE  @@Write1
      MOV  AX, 0600h
      MOV  BH, TextAttr
      INT  10h
      JMP  @@Done
    @@Write1:
      ADD  DX, $0101
      PUSH CX
      PUSH DX
      MOV  AH, 03h
      MOV  BH, ActivePage
      INT  10h
      MOV  AX, DX
      POP  DX
      POP  CX
      PUSH AX
      MOV  AL, TextChar
      MOV  BL, TextAttr
    @@Write2:
      PUSH CX
      PUSH DX
      MOV  AH, 02h
      XCHG CX, DX
      INT  10h
      MOV  AH, 09h
      SUB  CX, DX
      XOR  CH, CH
      INT  10h
      POP  DX
      POP  CX
      INC  CH
      CMP  CH, DH
      JNE  @@Write2
      POP  DX
      MOV  AH, 02h
      INT  10h
      JMP  @@Done
    @@Direct:
      MOV  DX, XY1
      MOV  CX, XY2
      SUB  CX, DX
      ADD  CX, $0101
      PUSH DX; CALL GetVideoOfs
      MOV  DI, AX
      MOV  DX, BytesPerLine
      MOV  AL, TextChar
      MOV  AH, TextAttr
      MOV  ES, VideoSeg
      CLD
    @@WriteLoop1:
      PUSH CX
      PUSH DI
      XOR  CH, CH
      REP  STOSW
      POP  DI
      ADD  DI, DX
      POP  CX
      DEC  CH
      CMP  CH, 0
      JNZ  @@WriteLoop1
    @@Done:
      CALL PostWrite
    end;

  procedure ClrScr; assembler;
    asm
      MOV  CX, WindMin
      MOV  DX, WindMax
      MOV  CursorXY, CX
      PUSH CX
      PUSH DX
      CALL ClrArea
      CALL UpdateCursor
    end;

  procedure ClrEol; assembler;
    asm
      MOV  CX, CursorXY
      MOV  DX, WindMax
      MOV  DH, CH
      PUSH CX
      PUSH DX
      CALL ClrArea
      CALL UpdateCursor
    end;

  procedure InsLine; assembler;
    asm
      CALL PreWrite
      MOV  DX, WindMin
      MOV  CX, WindMax
      MOV  AX, CursorXY
      MOV  DH, AH
      CMP  DirectVideo, True
      JE   @@Direct
      XCHG CX, DX
      MOV  AX, 0701h
      MOV  BH, TextAttr
      INT  10h
      CMP  TextChar, 32
      JNE  @@Write1
      CALL UpdateCursor
      JMP  @@Done
    @@Write1:
      CALL ClrEOL
      JMP  @@Done
    @@Direct:
      { Compute Range, Bytes Per Line & Video Offset }
      SUB  CX, DX
      ADD  CX, $0101
      PUSH CX
      MOV  CX, DX
      MOV  DI, ActiveOfs
      MOV  AX, ScreenMax
      XOR  AH, AH
      INC  AL
      PUSH AX
      MOV  DX, CX
      MOV  DL, DH
      XOR  DH, DH
      MUL  DX
      MOV  DX, CX
      XOR  DH, DH
      ADD  AX, DX
      MOV  DL, 02h
      MUL  DX
      ADD  DI, AX
      POP  AX
      MOV  DL, 02h
      MUL  DX
      MOV  DX, AX
      POP  CX
      PUSH DX
      MOV  DX, CX
      MOV  DL, DH
      XOR  DH, DH
      MUL  DX
      MOV  SI, AX
      ADD  SI, DI
      POP  DX
      { DI = Video Offset; CX = WindRange; DX = Bytes per Line }
      MOV  AL, TextChar
      MOV  AH, TextAttr
      MOV  ES, VideoSeg
      PUSH DS
      PUSH ES; POP DS { MOV DS, ES }
      CMP  CH, 1
      JE   @@Direct3
      PUSH AX
      PUSH DI
      SUB  SI, DX
      MOV  DI, SI
      SUB  SI, DX
      DEC  CH
    @@Direct2:
      PUSH CX
      PUSH SI
      PUSH DI
      XOR  CH, CH
      CLD
      REP  MOVSW
      POP  DI
      POP  SI
      POP  CX
      SUB  DI, DX
      SUB  SI, DX
      DEC  CH
      CMP  CH, 0
      JNE  @@Direct2
      POP  DI
      POP  AX
    @@Direct3:
      XOR  CH, CH
      REP  STOSW
      POP  DS
      CALL UpdateCursor
    @@Done:
      CALL PostWrite
    end;

  procedure DelLine; assembler;
    asm
      CALL PreWrite
      MOV  DX, WindMin
      MOV  CX, WindMax
      MOV  AX, CursorXY
      MOV  DH, AH
      CMP  DirectVideo, True
      JE   @@Direct
      XCHG CX, DX
      MOV  AX, 0601h
      MOV  BH, TextAttr
      INT  10h
      CMP  TextChar, 32
      JE  @@Write1
      MOV  CH, DH
      PUSH CX
      PUSH DX
      CALL ClrArea
    @@Write1:
      CALL UpdateCursor
      JMP  @@Done
    @@Direct:
      { Compute Range, Bytes Per Line & Video Offset }
      SUB  CX, DX
      ADD  CX, $0101
      PUSH CX
      MOV  CX, DX
      MOV  DI, ActiveOfs
      MOV  AX, ScreenMax
      XOR  AH, AH
      INC  AL
      PUSH AX
      MOV  DX, CX
      MOV  DL, DH
      XOR  DH, DH
      MUL  DX
      MOV  DX, CX
      XOR  DH, DH
      ADD  AX, DX
      MOV  DL, 02h
      MUL  DX
      ADD  DI, AX
      POP  AX
      MOV  DL, 02h
      MUL  DX
      MOV  DX, AX
      POP  CX
      { DI = Video Offset; CX = WindRange; DX = Bytes per Line }
      MOV  AL, TextChar
      MOV  AH, TextAttr
      MOV  ES, VideoSeg
      PUSH DS
      PUSH ES; POP DS { MOV DS, ES }
      CMP  CH, 1
      JE   @@Direct3
      PUSH AX
      MOV  SI, DI
      ADD  SI, DX
      DEC  CH
    @@Direct2:
      PUSH CX
      PUSH SI
      PUSH DI
      XOR  CH, CH
      CLD
      REP  MOVSW
      POP  DI
      POP  SI
      POP  CX
      ADD  DI, DX
      ADD  SI, DX
      DEC  CH
      CMP  CH, 0
      JNE  @@Direct2
      POP  AX
    @@Direct3:
      XOR  CH, CH
      REP  STOSW
      POP  DS
      CALL UpdateCursor
    @@Done:
      CALL PostWrite
    end;

  procedure InsChar; assembler;
    asm
      CALL PreWrite
      MOV  DX, WindMax
      MOV  CX, CursorXY
      CMP  DirectVideo, True
      JE   @@Direct
      MOV  DH, CH
      MOV  BH, ActivePage
    @@Bios1:
      CMP  CL, DL
      JE   @@Bios2
      PUSH CX
      MOV  AH, 02h
      DEC  DX
      INT  10h
      MOV  AH, 08h
      INT  10h
      MOV  BL, AH
      MOV  CX, 01h
      MOV  AH, 02h
      INC  DX
      INT  10h
      MOV  AH, 09h
      INT  10h
      POP  CX
      DEC  DX
      JMP  @@Bios1
    @@Bios2:
      MOV  AH, 02h
      INT  10h
      MOV  AH, 09h
      MOV  CX, 01h
      MOV  BL, TextAttr
      MOV  AL, TextChar
      INT  10h
      JMP  @@Done
    @@Direct:
      PUSH DS
      PUSH DX
      MOV  AX, ScreenMax
      XOR  AH, AH
      INC  AL
      MUL  CH
      POP  DX
      XOR  DH, DH
      ADD  AX, DX
      SHL  AX, 1
      SUB  DL, CL
      MOV  CX, DX
      MOV  DI, AX
      MOV  SI, AX
      SUB  SI, 2
      MOV  ES, VideoSeg
      MOV  AL, TextChar
      MOV  AH, TextAttr
      MOV  DS, VideoSeg
      STD
      PUSH AX
      REP  MOVSW
      POP  AX
      STOSW
      POP  DS
      CALL UpdateCursor
    @@Done:
      CALL PostWrite
    end;

  procedure DelChar; assembler;
    asm
      CALL PreWrite
      MOV  CX, WindMax
      MOV  DX, CursorXY
      CMP  DirectVideo, True
      JE   @@Direct
      MOV  BH, ActivePage
    @@Bios1:
      CMP  CL, DL
      JE   @@Bios2
      PUSH CX
      MOV  AH, 02h
      INC  DX
      INT  10h
      MOV  AH, 08h
      INT  10h
      MOV  BL, AH
      MOV  CX, 01h
      MOV  AH, 02h
      DEC  DX
      INT  10h
      MOV  AH, 09h
      INT  10h
      POP  CX
      INC  DX
      JMP  @@Bios1
    @@Bios2:
      MOV  AH, 02h
      INT  10h
      MOV  AH, 09h
      MOV  CX, 01h
      MOV  BL, TextAttr
      MOV  AL, TextChar
      INT  10h
      CALL UpdateCursor
      JMP  @@Done
    @@Direct:
      PUSH DS
      PUSH DX
      MOV  AX, ScreenMax
      XOR  AH, AH
      INC  AL
      MUL  DH
      POP  DX
      XOR  DH, DH
      ADD  AX, DX
      SHL  AX, 1
      SUB  CL, DL
      XOR  CH, CH
      MOV  DI, AX
      MOV  SI, AX
      ADD  SI, 2
      MOV  ES, VideoSeg
      MOV  AL, TextChar
      MOV  AH, TextAttr
      MOV  DS, VideoSeg
      CLD
      CMP  CX, 0
      JE   @@Direct2
      PUSH AX
      REP  MOVSW
      POP  AX
    @@Direct2:
      STOSW
      POP  DS
      CALL UpdateCursor
    @@Done:
      CALL PostWrite
    end;

  procedure InsColumn; assembler;
    asm
      CALL PreWrite
      MOV  BX, WindMin
      MOV  DX, WindMax
      MOV  CX, CursorXY
      CMP  DirectVideo, True
      JE   @@Direct
      MOV  AX, BX
      PUSH CX
      MOV  CH, DH
      MOV  DH, AH
      MOV  BH, ActivePage
    @@Bios0:
      CMP  CH, DH
      JL   @@Bios3
      PUSH  DX
      PUSH  CX
    @@Bios1:
      CMP  CL, DL
      JE   @@Bios2
      PUSH CX
      MOV  AH, 02h
      DEC  DX
      INT  10h
      MOV  AH, 08h
      INT  10h
      MOV  BL, AH
      MOV  CX, 01h
      MOV  AH, 02h
      INC  DX
      INT  10h
      MOV  AH, 09h
      INT  10h
      POP  CX
      DEC  DX
      JMP  @@Bios1
    @@Bios2:
      MOV  AH, 02h
      INT  10h
      MOV  AH, 09h
      MOV  CX, 01h
      MOV  BL, TextAttr
      MOV  AL, TextChar
      INT  10h
      POP  CX
      POP  DX
      INC  DH
      JMP  @@Bios0
    @@Bios3:
      POP  DX
      MOV  AH, 02h
      INT  10h
      JMP  @@Done
    @@Direct:
      MOV  CH, BH
      XOR  BX, BX
      MOV  BL, DH
      INC  BL
      SUB  BL, CH
      PUSH DS
      PUSH BX
      PUSH DX
      MOV  AX, ScreenMax
      XOR  AH, AH
      INC  AL
      MOV  BX, AX
      SHL  BX, 1
      MUL  CH
      POP  DX
      XOR  DH, DH
      ADD  AX, DX
      SHL  AX, 1
      SUB  DL, CL
      MOV  CX, DX
      MOV  DI, AX
      MOV  SI, AX
      SUB  SI, 2
      MOV  ES, VideoSeg
      MOV  AL, TextChar
      MOV  AH, TextAttr
      MOV  DS, VideoSeg
      POP  DX
    @@Direct2:
      STD
      PUSH SI
      PUSH DI
      PUSH CX
      PUSH AX
      REP  MOVSW
      POP  AX
      STOSW
      POP  CX
      POP  DI
      POP  SI
      ADD  SI, BX
      ADD  DI, BX
      DEC  DX
      JNZ  @@Direct2
      POP  DS
      CALL UpdateCursor
    @@Done:
      CALL PostWrite
    end;

  procedure DelColumn; assembler;
    asm
      CALL PreWrite
      MOV  AX, WindMin
      MOV  CX, WindMax
      MOV  DX, CursorXY
      CMP  DirectVideo, True
      JE   @@Direct
      PUSH CX
      MOV  DH, AH
      MOV  BH, ActivePage
    @@Bios0:
      CMP  CH, DH
      JL   @@Bios3
      PUSH  DX
      PUSH  CX
      MOV  BH, ActivePage
    @@Bios1:
      CMP  CL, DL
      JE   @@Bios2
      PUSH CX
      MOV  AH, 02h
      INC  DX
      INT  10h
      MOV  AH, 08h
      INT  10h
      MOV  BL, AH
      MOV  CX, 01h
      MOV  AH, 02h
      DEC  DX
      INT  10h
      MOV  AH, 09h
      INT  10h
      POP  CX
      INC  DX
      JMP  @@Bios1
    @@Bios2:
      MOV  AH, 02h
      INT  10h
      MOV  AH, 09h
      MOV  CX, 01h
      MOV  BL, TextAttr
      MOV  AL, TextChar
      INT  10h
      POP  CX
      POP  DX
      INC  DH
      JMP  @@Bios0
    @@Bios3:
      POP  DX
      MOV  AH, 02h
      INT  10h
      CALL UpdateCursor
      JMP  @@Done
    @@Direct:
      MOV  DH, AH
      XOR  BX, BX
      MOV  BL, CH
      INC  BL
      SUB  BL, DH
      PUSH DS
      PUSH BX
      PUSH DX
      MOV  AX, ScreenMax
      XOR  AH, AH
      INC  AL
      MOV  BX, AX
      SHL  BX, 1
      MUL  DH
      POP  DX
      XOR  DH, DH
      ADD  AX, DX
      SHL  AX, 1
      SUB  CL, DL
      XOR  CH, CH
      MOV  DI, AX
      MOV  SI, AX
      ADD  SI, 2
      MOV  ES, VideoSeg
      MOV  AL, TextChar
      MOV  AH, TextAttr
      MOV  DS, VideoSeg
      POP  DX
      CLD
    @@Direct2:
      PUSH SI
      PUSH DI
      PUSH CX
      CMP  CX, 0
      JE   @@Direct3
      PUSH AX
      REP  MOVSW
      POP  AX
    @@Direct3:
      STOSW
      POP  CX
      POP  DI
      POP  SI
      ADD  SI, BX
      ADD  DI, BX
      DEC  DX
      JNZ  @@Direct2
      POP  DS
      CALL UpdateCursor
    @@Done:
      CALL PostWrite
    end;

  procedure TextColor(Color: Byte); assembler;
    asm
      MOV  AL, TextAttr
      MOV  AH, Color
      AND  AL, 70h
      TEST AH, 0F0h
      JZ   @@SetColor
      OR   AL, 80h
    @@SetColor:
      AND  AH, 0Fh
      OR   AL, AH
      MOV  TextAttr, AL
    end;

  procedure TextBackground(Color: Byte); assembler;
    asm
      MOV  AL, TextAttr
      MOV  AH, Color
      AND  AL, 8Fh
      AND  AH, 07h
      MOV  CL, 04h
      SHL  AH, CL
      OR   AL, AH
      MOV  TextAttr, AL
    end;
    
  procedure Color(Fore, Back : byte);
  begin
  	TextColor(Fore);
  	TextBackground(Back);
  end;

  procedure LowVideo; assembler;
    asm
      AND  TextAttr, 11110111b
    end;

  procedure HighVideo; assembler;
    asm
      OR  TextAttr, 00001000b
    end;

  procedure NormVideo; assembler;
    asm
      MOV  AL, FirstAttr
      MOV  TextAttr, AL
    end;

{$IFOPT G+}
  procedure Delay(MS : Word); assembler;
    asm
      JMP @@Delay2
    @@DelayFlag:
      DB  0
    @@Delay2:
      MOV  AX, 8301h { Cancel Event Wait }
      INT  15h
      MOV  AX, 8300h
      MOV  CS:[OFFSET @@DelayFlag], AL
      MOV  DX, MS
      MOV  CX, DX
      SHL  DX, 0Ah
      SHR  CX, 06h
      PUSH CS; POP ES { MOV  ES, CS }
      MOV  BX, OFFSET @@DelayFlag
      INT  15h
    @@Delay3:
      CALL IdleMouseProc
      MOV  AL, CS:[OFFSET @@DelayFlag]
      TEST AL, 80h
      JZ   @@Delay3
    end;
{$ELSE}
  procedure Delay( MS : Word ); assembler;
    var
      LastTick : Word;
    asm
      XOR  DX, DX
      MOV  LastTick, DX
      MOV  AX, MS
      MOV  CX, 55
      DIV  CX
      MOV  CX, AX
      MOV  DI, Seg0040
      MOV  ES, DI
      MOV  DI, 006Ch
      CMP  DX, 0
      JE   @@Delay2
      INC  CX
    @@Delay2:
      CMP  CX, 0
      JE   @@Done
    @@DelayLoop:
      PUSH CX
      PUSH DI
      PUSH ES
      CALL IdleMouseProc
      POP  ES
      POP  DI
      POP  CX
      MOV  AX, ES:[DI]
      CMP  LastTick, AX
      JE   @@DelayLoop
      MOV  LastTick, AX
      LOOP @@DelayLoop
    @@Done:
    end;
{$ENDIF}
  procedure Sound(Hz: Word); assembler;
    asm
      MOV  DX, 0012h
      MOV  AX, 34DCh
      MOV  CX, Hz
      CMP  CX, 0
      JE   @@Sound1
      DIV  CX
      JMP  @@Sound2
    @@Sound1:
      XOR  AX, AX
    @@Sound2:
      PUSH AX
      MOV  AL, 10110110b
      MOV  DX, 043h
      OUT  DX, AL
      MOV  DX, 042h
      POP  AX
      OUT  DX, AL
      MOV  AL, AH
      OUT  DX, AL
      MOV  DX, 061h
      IN   AL, DX
      { AND  AL, 11111111b
      OR   AL, 00000011b }
      MOV	AL, 03h 
      OUT  DX, AL
    end;

  procedure NoSound; assembler;
    asm
      MOV  DX, 061h
      IN   AL, DX
      AND  AL, 11111101b
      OR   AL, 00000001b
      OUT  DX, AL
      MOV  AL, 10110110b
      MOV  DX, 043h
      OUT  DX, AL
      MOV  DX, 042h
      MOV  AL, 0
      OUT  DX, AL
      OUT  DX, AL
    end;

  procedure PrintScreen; assembler;
    asm
      PUSH DS
      PUSH SP
      PUSH BP
      PUSHF
      CALL DWORD PTR OldInt05
      POP  BP
      POP  SP
      POP  DS
    end;

  procedure FWriteChar ( Letter : Char ); assembler;
    asm
      CALL PreWrite
      CMP  DirectVideo, True
      JE   @@Direct
      MOV  CX, WindMax
      MOV  DX, CursorXY
      MOV  BH, ActivePage
      MOV  BL, TextAttr
    @@Write:
      MOV  AH, 02h
      INT  10h
      PUSH CX
      MOV  CX, 0001h
      MOV  AH, 09h
      MOV  AL, Letter
      INT  10h
      POP  CX
      INC  DL
      CMP  DL, CL
      JNG  @@Write1
      MOV  CursorXY, DX
      CALL Return
      JMP  @@Done2
    @@Write1:
      MOV  CursorXY, DX
      JMP  @@Done
    @@Direct:
      MOV  DX, CursorXY
      MOV  CX, WindMax
      PUSH DX; CALL GetVideoOfs
      MOV  DI, AX
      MOV  AX, VideoSeg
      MOV  ES, AX
      MOV  AH, TextAttr
      MOV  AL, Letter
    @@Direct1:
      STOSW
      INC  DL
      CMP  DL, CL
      JNG  @@Direct2
      MOV  CursorXY, DX
      CALL Return
      JMP  @@Done2
    @@Direct2:
      MOV  CursorXY, DX
    @@Done:
    @@Done2:
      CALL UpdateCursor
      CALL PostWrite
    end;

  procedure FWrite ( Str : String ); assembler;
    asm
      LES  DI, Str
      MOV  AH, ES:[DI]
      CMP  AH, 0
      JE   @@Done2
      CALL PreWrite
      CMP  DirectVideo, True
      JE   @@Direct
      MOV  CX, WindMax
      MOV  DX, CursorXY
      MOV  BH, ActivePage
      MOV  BL, TextAttr
    @@Write:
      PUSH AX
      PUSH CX
      MOV  AH, 02h
      INT  10h
      INC  DI
      MOV  CX, 0001h
      MOV  AH, 09h
      MOV  AL, ES:[DI]
      INT  10h
      POP  CX
      POP  AX
      INC  DL
      CMP  DL, CL
      JNG  @@Write1
      PUSH AX
      PUSH CX
      PUSH BX
      PUSH ES
      PUSH DI
      MOV  CursorXY, DX
      CALL Return
      MOV  DX, AX
      POP  DI
      POP  ES
      POP  BX
      POP  CX
      POP  AX
    @@Write1:
      DEC  AH
      CMP  AH, 0
      JNE  @@Write
      MOV  CursorXY, DX
      CALL UpdateCursor
      JMP  @@Done
    @@Direct:
      MOV  BL, AH
      MOV  DX, CursorXY
      MOV  CX, WindMax
      PUSH DX; CALL GetVideoOfs
      MOV  DI, AX
      MOV  AX, VideoSeg
      MOV  ES, AX
      MOV  AH, TextAttr
      PUSH DS
      LDS  SI, Str
      INC  SI
      CLD
    @@Direct1:
      LODSB
      STOSW
      INC  DL
      CMP  DL, CL
      JNG  @@Direct2
      PUSH DS
      PUSH ES
      PUSH SI
      PUSH AX
      PUSH BX
      PUSH CX
      MOV  AX, SEG @DATA
      MOV  DS, AX
      MOV  CursorXY, DX
      CALL Return
      MOV  DX, AX
      PUSH DX; CALL GetVideoOfs
      MOV  DI, AX
      CLD
      POP  CX
      POP  BX
      POP  AX
      POP  SI
      POP  ES
      POP  DS
    @@Direct2:
      DEC  BL
      CMP  BL, 0
      JNE  @@Direct1
      POP  DS
      MOV  CursorXY, DX
    @@Done:
      CALL UpdateCursor
      CALL PostWrite
    @@Done2:
    end;

  procedure FWriteLn ( Str : String ); assembler;
    asm
      LES DI, Str
      PUSH ES
      PUSH DI
      CALL FWrite
      CALL Return
    end;

  procedure FWriteAttr; 
  var
  	HoldScroll : boolean;
  	X, Y : integer;
  	C : Char;
  begin
  	HoldScroll := CheckScroll;
  	CheckScroll := false;
  	X := WhereX;
  	Y := WhereY;
  	C := FReadChar;
  	GotoXY(X, Y);
  	CheckScroll := HoldScroll;
  	FWriteChar(C);
  end;

  function FReadChar : Char; assembler;
    asm
      CALL PreWrite
      CMP  DirectVideo, True
      JE   @@Direct
      MOV  CX, WindMax
      MOV  DX, CursorXY
      MOV  BH, ActivePage
      MOV  BL, TextAttr
    @@Write:
      MOV  AH, 02h
      INT  10h
      MOV  AH, 08h
      INT  10h
      PUSH AX
      INC  DL
      CMP  DL, CL
      JNG  @@Write1
      MOV  CursorXY, DX
      CALL Return
      JMP  @@Done2
    @@Write1:
      MOV  CursorXY, DX
      JMP  @@Done
    @@Direct:
      MOV  DX, CursorXY
      MOV  CX, WindMax
      PUSH DX; CALL GetVideoOfs
      MOV  DI, AX
      MOV  AX, VideoSeg
      MOV  ES, AX
    @@Direct1:
      MOV  AX, ES:[DI]
      PUSH AX
      INC  DL
      CMP  DL, CL
      JNG  @@Direct2
      MOV  CursorXY, DX
      CALL Return
      JMP  @@Done2
    @@Direct2:
      MOV  CursorXY, DX
    @@Done:
    @@Done2:
      CALL UpdateCursor
      CALL PostWrite
      POP  AX
    end;

  function FReadAttr : byte; assembler;
    asm
      CALL PreWrite
      CMP  DirectVideo, True
      JE   @@Direct
      MOV  CX, WindMax
      MOV  DX, CursorXY
      MOV  BH, ActivePage
      MOV  BL, TextAttr
    @@Write:
      MOV  AH, 02h
      INT  10h
      MOV  AH, 08h
      INT  10h
      PUSH AX
      INC  DL
      CMP  DL, CL
      JNG  @@Write1
      MOV  CursorXY, DX
      CALL Return
      JMP  @@Done2
    @@Write1:
      MOV  CursorXY, DX
      JMP  @@Done
    @@Direct:
      MOV  DX, CursorXY
      MOV  CX, WindMax
      PUSH DX; CALL GetVideoOfs
      MOV  DI, AX
      MOV  AX, VideoSeg
      MOV  ES, AX
    @@Direct1:
      MOV  AX, ES:[DI]
      PUSH AX
      INC  DL
      CMP  DL, CL
      JNG  @@Direct2
      MOV  CursorXY, DX
      CALL Return
      JMP  @@Done2
    @@Direct2:
      MOV  CursorXY, DX
    @@Done:
    @@Done2:
      CALL UpdateCursor
      CALL PostWrite
      POP  AX
      MOV  AL, AH
    end;

  function FReadCharAttr : word; assembler;
    asm
      CALL PreWrite
      CMP  DirectVideo, True
      JE   @@Direct
      MOV  CX, WindMax
      MOV  DX, CursorXY
      MOV  BH, ActivePage
      MOV  BL, TextAttr
    @@Write:
      MOV  AH, 02h
      INT  10h
      MOV  AH, 08h
      INT  10h
      PUSH AX
      INC  DL
      CMP  DL, CL
      JNG  @@Write1
      MOV  CursorXY, DX
      CALL Return
      JMP  @@Done2
    @@Write1:
      MOV  CursorXY, DX
      JMP  @@Done
    @@Direct:
      MOV  DX, CursorXY
      MOV  CX, WindMax
      PUSH DX; CALL GetVideoOfs
      MOV  DI, AX
      MOV  AX, VideoSeg
      MOV  ES, AX
    @@Direct1:
      MOV  AX, ES:[DI]
      PUSH AX
      INC  DL
      CMP  DL, CL
      JNG  @@Direct2
      MOV  CursorXY, DX
      CALL Return
      JMP  @@Done2
    @@Direct2:
      MOV  CursorXY, DX
    @@Done:
    @@Done2:
      CALL UpdateCursor
      CALL PostWrite
      POP  AX
    end;

  function WindowSize ( X1, Y1, X2, Y2 : byte ) : word;
    var
      T : Byte;
    begin
      if X2 < X1 then begin T := X1; X1 := X2; X2 := T; end;
      if Y2 < Y1 then begin T := Y1; Y1 := Y2; Y2 := T; end;
      WindowSize := Sizeof(Word) +
       (Word(X2) - Word(X1) + 1) * (Word(Y2) - Word(Y1) + 1) * 2;
    end;

  procedure GetWindow ( X1, Y1, X2, Y2 : byte; var Window );
    var
      TX, TY, X, Y : byte;
      TScroll : boolean;
      P : word;
    begin
      PreWrite;
      if X2 < X1 then begin TX := X1; X1 := X2; X2 := TX; end;
      if Y2 < Y1 then begin TY := Y1; Y1 := Y2; Y2 := TY; end;
      WindowType(Window).Width := X2 - X1;
      WindowType(Window).Height := Y2 - Y1;
      TX := WhereX;
      TY := WhereY;
      TScroll := CheckScroll;
      CheckScroll := False;
      P := 0;
      for Y := Y1 to Y2 do
        begin
          GotoXY ( X1, Y );
          for X := X1 to X2 do
            begin
              if (X <= GetMaxX) and (Y <= GetMaxY) then
                WindowType(Window).Data[P] := FReadCharAttr
              else
                WindowType(Window).Data[P] := Word(TextAttr) * $0100 + Byte(TextChar);
              Inc(P);
            end;
        end;
      GotoXY ( TX, TY );
      CheckScroll := TScroll;
      PostWrite;
    end;

  procedure PutWindow ( X1, Y1 : byte; var Window );
    var
      TScroll : boolean;
      TAttr, TX, TY, X, Y : byte;
      P : word;
    begin
      PreWrite;
      TX := WhereX;
      TY := WhereY;
      TAttr := TextAttr;
      TScroll := CheckScroll;
      CheckScroll := False;
      P := 0;
      for Y := 0 to WindowType(Window).Height  do
        if Y <= GetMaxY then
           begin
             GotoXY ( X1, Y + Y1 );
             for X := 0 to WindowType(Window).Width  do
               begin
                 if X <= GetMaxX then
                   begin
                     TextAttr := Hi(WindowType(Window).Data[P]);
                     FWriteChar (Char(Lo(WindowType(Window).Data[P])));
                   end;
                 Inc(P);
               end;
           end;
      GotoXY ( TX, TY );
      TextAttr := TAttr;
      CheckScroll := TScroll;
      PostWrite;
    end;

  procedure MoveCursor; assembler;
    asm
      CALL SetCursorLoc
    end;

{ Cusor size and shape functions }

  function GetCursor : word; assembler;
    asm
      MOV AH, $03
      MOV BH, ActivePage
      INT $10
      MOV AX, CX
    end;

  procedure SetCursor ( Cursor : word ); assembler;
    asm
      MOV  AH, $01
      MOV  CX, Cursor
      INT  $10
    end;

  procedure NormalCursor; assembler;
    asm
      PUSH FirstCursor
      CALL SetCursor
    end;

  procedure HideCursor; assembler;
    asm
      MOV  AX, $2000
      PUSH AX
      CALL SetCursor
    end;

  procedure SmallCursor;
    begin
      SetCursor ( (MemW[$0:$0485]) shl 8 + (MemW[$0:$0485] - 2)  );
    end;

  procedure HalfCursor;
    begin
      SetCursor ( (MemW[$0:$0485] div 2) shl 8 + Lo(FirstCursor));
    end;

  procedure FullCursor;
    begin
      SetCursor ( MemW[$0:$0485] );
    end;

{ Blink Functions }
  function GetBlink : boolean; assembler;
    asm
      XOR  AX, AX
      MOV  ES, AX
      MOV  CL, ES:[$0465] { CRT_Mode }
      AND  CL, $20
      MOV  AL, True
      CMP  CL, $20
      JE   @@Done
      MOV  AL, False
    @@Done:
    end;

  procedure SetBlink ( Blink : boolean ); assembler;
    asm
      MOV AX, $1003
      MOV BL, Blink
      INT $10
    end;

{ Mouse procedures }
  procedure TurnMouseOff;
    begin
      if MouseAvail then HideMouse;
      CheckMouse   := False;
      MouseAvail   := False;
      MouseButtons := 0;
    end;

  procedure TurnMouseOn;
    begin
      if MouseAvail then HideMouse;
      CheckMouse := True;
      ResetMouse(MouseAvail, MouseButtons);
      if MouseAvail then
         begin
           HandlerMask := $FFFF;
           HandlerProc := @MouseHandler;
           SwapMouseHandler(HandlerMask, HandlerProc);
         end;
      if MouseAvail then ShowMouse;
    end;

	procedure ReleaseButtons;
	   var
		 BtnStatus            : integer;
		 X, Y                 : Integer;
	begin
		BtnStatus := 1;
		While BtnStatus>0 do
		   GetMousePos (X, Y, BtnStatus);
	end;

	procedure MouseHome;
	begin
	  SetMousePos (MouseHomeX,MouseHomeY);
	  ReleaseButtons;
	end;


{ Box and line functions }
  procedure DrawBox ( X1, Y1, X2, Y2, Style : byte );
    var
      CursorX, CursorY : byte;
      RangeX,  RangeY  : byte;
      I                : byte;
    begin
      RangeX := GetMaxX;
      RangeY := GetMaxY;
      if (X1 > RangeX) or (Y1 > RangeY) then Exit;
      PreWrite;
      CursorX := WhereX;
      CursorY := WhereY;
      { Draw Top }
      GotoXY( X1, Y1 );
      Write(BoxData[Style][0] );
      for I := X1 + 1 to X2 - 1 do
        if I > RangeX then Break else
          Write( BoxData[Style][4] );
      if (X2 <= RangeX) and (X1 <> X2) then
        Write( BoxData[Style][1] );
      { Draw Sides }
      for I := Y1 + 1 to Y2 - 1 do
        if I > RangeY then Break else
          begin
            GotoXY ( X1, I );
            Write(BoxData[Style][5]);
            if (X2 <= RangeX) and (X1 <> X2) then
              begin
                GotoXY ( X2, I );
                Write(BoxData[Style][5]);
              end;
          end;
      { Draw Bottom }
      if (Y2 <= RangeY) and (Y2 <> Y1) then
        begin
          GotoXY( X1, Y2 );
          Write(BoxData[Style][2] );
          for I := X1 + 1 to X2 - 1 do
            if I > RangeX then Break else
              Write( BoxData[Style][4] );
          if (X2 <= RangeX) and (X1 <> X2) then
            Write( BoxData[Style][3] );
        end;
      GotoXY ( CursorX, CursorY );
      PostWrite;
    end;

  procedure DrawShadow ( X1, Y1, X2, Y2, Style : byte );
    var
      CursorX, CursorY : byte;
      RangeX,  RangeY  : byte;
      C                : Char;
      Attr             : byte;
      I                : byte;
    begin
      RangeX := GetMaxX;
      RangeY := GetMaxY;
      Inc(X1,1);
      Inc(Y1,1);
      Inc(X2,1);
      Inc(Y2,1);
      if Style and bsDoubleWide = bsDoubleWide then
        Inc(X1, 1);
      if (X1 > RangeX) or (Y1 > RangeY) then Exit;
      PreWrite;
      CursorX  := WhereX;
      CursorY  := WhereY;
      Attr     := TextAttr;
      TextAttr := $08;
      { Draw Sides }
      if (X2 <= RangeX) and (X1 <> X2) then
        for I := Y1 to Y2 do
          if I > RangeY then Break else
            begin
              GotoXY ( X2, I );
              C := FReadChar;
              GotoXY ( X2, I );
              FWrite( C );
              if (X2 + 1 <= RangeX) and (Style and bsDoubleWide = bsDoubleWide) then
                begin
                  C := FReadChar;
                  GotoXY ( X2 + 1, I );
                  FWrite( C );
                end;
            end;
      { Draw Bottom }
      if (Y2 <= RangeY) and (Y2 <> Y1) then
        for I := X1 to X2 - 1 do
          if I > RangeX then Break else
            begin
              GotoXY ( I, Y2 );
              C := FReadChar;
              GotoXY ( I, Y2 );
              FWrite( C );
            end;
      TextAttr := Attr;
      GotoXY ( CursorX, CursorY );
      PostWrite;
    end;

  procedure DrawLine ( X1, Y1, Len, Style : byte );
    var
      CursorX, CursorY : byte;
      RangeX,  RangeY  : byte;
      I,       TPos    : byte;
    begin
      CursorX := WhereX;
      CursorY := WhereY;
      RangeX  := GetMaxX;
      RangeY  := GetMaxY;
      GotoXY ( X1, Y1 );
      TPos := Pos(Chr(Lo(FReadCharAttr)), LineData.Current);
      GotoXY ( WhereX - 1, WhereY );
      if (TPos = 0) or (Style and lnNoEnds = lnNoEnds) then
        TPos := 41;
      FWriteChar ( LineData.Style[Style and lnDouble, Style and lnVertical = lnVertical, Top][TPos]);
      Case Style and lnVertical = lnVertical of
        False : begin
          for I := X1 + 1 to X1 + Len - 2 do
            if I > RangeX then Break else
              begin
                if (Style and lnNoCenter = lnNoCenter) then
                  TPos := 41
                else
                  begin
                    TPos := Pos(Chr(Lo(FReadCharAttr)), LineData.Current);
                    GotoXY ( WhereX - 1, WhereY );
                    if TPos = 0 then TPos := 41;
                  end;
                FWriteChar ( LineData.Style[Style and lnDouble, False, Middle][TPos]);
              end;
          if X1 + Len - 1 <= RangeX then
            begin
              TPos := Pos(Chr(Lo(FReadCharAttr)), LineData.Current);
              GotoXY ( WhereX - 1, WhereY );
              if (TPos = 0) or (Style and lnNoEnds = lnNoEnds) then
                TPos := 41;
              FWriteChar ( LineData.Style[Style and lnDouble, False, Bottom][TPos]);
            end;
        end;
        True : begin
          for I := Y1 + 1 to Y1 + Len - 2 do
            if I > RangeY then Break else
              begin
                GotoXY ( X1, I );
                if (Style and lnNoCenter = lnNoCenter) then
                  TPos := 41
                else
                  begin
                    TPos := Pos(Chr(Lo(FReadCharAttr)), LineData.Current);
                    GotoXY(X1, I);
                    if TPos = 0 then TPos := 41;
                  end;
                FWriteChar ( LineData.Style[Style and lnDouble, True, Middle][TPos]);
              end;
          if Y1 + Len - 1 <= RangeY then
            begin
              GotoXY ( X1, Y1 + Len - 1 );
              TPos := Pos(Chr(Lo(FReadCharAttr)), LineData.Current);
              GotoXY ( X1, Y1 + Len - 1 );
              if (TPos = 0) or (Style and lnNoEnds = lnNoEnds) then
                TPos := 41;
              FWriteChar ( LineData.Style[Style and lnDouble, True, Bottom][TPos]);
            end;
        end;
      end;
      GotoXY ( CursorX, CursorY );
    end;

{ Event Functions }
  procedure PurgeEvents;
    var
    	Temp : TEvent;
    begin
      CommandBuf.Purge;
      KeyBuf.Purge;
      MouseBuf.Purge;
      repeat
      	GetEvent(Temp);
      until Temp.What = evNothing; 
      EventFlag := False;
    end;

  procedure ClearEvent(var Event : TEvent);
    begin
      Event.What := evNothing;
      Event.Command := cmClearedEvent;
    end;

  procedure GetEvent(var Event : TEvent);
    begin
      ClearEvent(Event);
      if BreakCount <> 0 then
        begin
          Event.What     := evSystem;
          Event.Command  := cmBreak;
          Event.InfoWord := BreakCount;
          BreakCount := 0;
        end
      else
      if SysReqMake <> 0 then
        begin
          Event.What    := evSystem;
          Event.Command := cmMakeSysReq;
          Event.InfoWord := 1;
          Dec(SysReqMake);
        end
      else
      if SysReqBreak <> 0 then
        begin
          Event.What    := evSystem;
          Event.Command := cmBreakSysReq;
          Event.InfoWord := 1;
          Dec(SysReqBreak);
        end
      else
      if PrintCount <> 0 then
        begin
          Event.What    := evSystem;
          Event.Command := cmPrintScreen;
          Event.InfoWord := PrintCount;
          PrintCount := 0;
        end
      else
      if Not CommandBuf.GetEvent(Event) then
      if Not KeyBuf.GetEvent(Event)     then
      if Not MouseBuf.GetEvent(Event)   then
        IdleMouseProc;
      if Event.What <> evNothing then
      	EventFlag := True;      
    end;

  function PutEvent(var Event : TEvent) : boolean;
    begin
      PutEvent := CommandBuf.PutEvent(Event);
    end;
{ Direct Video Memory procedures }

  function GetVideoSize : word;
    begin
    	if LastMode and $00FF <= 7 then
    	  GetVideoSize := ( Lo(ScreenMax) + 1 ) * ( Hi(ScreenMax) + 1 ) * 2
    	else
		  GetVideoSize := BIOSptr^.RegenSize;
    end;

  function  GetVideoPtr : Pointer;
    begin
      GetVideoPtr := Ptr(VideoSeg, ActiveOfs);
    end;

  procedure SetVideoPtr ( P : Pointer );
    begin
      VideoSeg := Seg(P^);
      ActiveOfs := Ofs(P^);
    end;

  procedure CopyToVideo;
    var
      TP : Pointer;
    begin
      if DirectVideo then
        begin
		  If MouseAvail then HideMouse;
          TP := Ptr(VideoSeg, ActiveOfs);
          Move ( TP^, VideoPtr^, GetVideoSize);
	      If MouseAvail then ShowMouse;
        end;
    end;

  procedure CopyFromVideo;
    var
      TP : Pointer;
    begin
      if DirectVideo then
        begin
	      If MouseAvail then HideMouse;
          TP := Ptr(VideoSeg, ActiveOfs);
          Move ( VideoPtr^, TP^, GetVideoSize);
	      If MouseAvail then ShowMouse;
        end;
    end;

  function RTrim ( Str : String ) : String;
    begin
      While (Str[Length(Str)] = #32) and (Str <> '') do Str[0] := Chr(Length(Str) - 1);
      RTrim := Str;
    end;

  function Space ( N : byte ) : String;
    var
      T : String;
    begin
      FillChar ( T[1], N, 32);
      T[0] := Chr(N);
      Space := T;
    end;

  function RSpace ( Str : String; N : Byte ) : String;
    begin
      if Length(Str) < N then
        RSpace := Str + Space( N - Length(Str) )
      else
        RSpace := Str;
    end;

  procedure AddChr ( var S : String; C : Char );
    begin
      Inc(Byte(S[0]));
      S[Byte(S[0])] := C;
    end;

{ EditLn Function }
  function DoEditLn ( var Dest : String; First : boolean; MaxLen, MaxWide : Byte; var Event : TEvent) : boolean;
    const
      Source : String = '';
      OfsX   : byte = 0;
      CurX   : byte = 255;
    var
      OrgX,  OrgY : Byte;
      Cursor      : word;
      DoneFlag    : Boolean;

    procedure EditCursor;
      begin
        case InsertMode of
          True  : SmallCursor;
          False : HalfCursor;
        end;
      end;

    procedure DisplayStr;
      var
        TAttr : byte;
      begin
        TAttr := TextAttr;
        GotoXY ( OrgX, OrgY );
        if OfsX > 0 then
          FWriteChar(#17)
        else
          FWriteChar(#32);
        FWrite(RSpace(Copy(Dest, OfsX + 1, MaxWide - 2), MaxWide - 2));
        if OfsX + MaxWide - 2 < Length((Dest)) then
          FWriteChar(#16)
        else
          FWriteChar(#32);
        TextAttr := TAttr;
        GotoXY(OrgX + CurX, OrgY);
        MoveCursor;
      end;

    function Left(Display : boolean) : boolean;
      begin
        if CurX + OfsX > 1 then
          begin
            Dec(CurX);
            if CurX < 1 then
            begin
              CurX := 1;
              Dec(OfsX);
            end;
          end;
        if Display then DisplayStr;
        Left := CurX + OfsX > 1;
      end;

    function Right(Display : boolean) : boolean;
      begin
        if CurX + OfsX <= Length(Dest) then
          begin
            Inc(CurX);
            if CurX > MaxWide - 2 then
              begin
                Dec(CurX);
                Inc(OfsX);
              end;
          end;
        if Display then DisplayStr;
        Right := CurX + OfsX <= Length(Dest);
      end;

    procedure Home;
      begin
        OfsX := 0;
        CurX := 1;
        DisplayStr;
      end;

    procedure EndLine;
      begin
        while (CurX + OfsX > Length(Dest)) and (CurX + OfsX > 1) do
          begin
            Dec(CurX);
            if CurX < 1 then
            begin
              CurX := 1;
              Dec(OfsX);
            end;
          end;
        while Right(False) do;
        DisplayStr;
      end;

    procedure LeftWord;
      begin
        if Dest = '' then
          Home
        else
          begin
            Left(False);
            While (CurX + OfsX > Length(Dest)) and Left(False) do;
            if Dest[CurX + OfsX] = #32 then
              while (Dest[CurX + OfsX] = #32) and Left(False) do;
            while (Dest[CurX + OfsX] <> #32) and Left(False) do;
            if CurX + OfsX > 1 then Right(False);
          end;
        DisplayStr;
      end;

    procedure RightWord;
      begin
        if Dest = '' then
          Home
        else
          begin
            if CurX + OfsX < MaxLen then
              begin
                if Dest[CurX + OfsX] = #32 then
                  while (Dest[CurX + OfsX] = #32) and Right(False) do
                else
                  while (Dest[CurX + OfsX] <> #32) and Right(False) do;
                if Dest[CurX + OfsX] = #32 then
                  while (Dest[CurX + OfsX] = #32) and Right(False) do;
              end;
          end;
        DisplayStr;
      end;

    procedure VerifyPosition;
      begin
        if (Dest = '')  or (OfsX + CurX - 1 > Length(Dest)) then
          begin
            CurX := 1;
            OfsX := 0;
            EndLine;
          end;
        DisplayStr;
      end;
	
    procedure HandleKeyboard;
      begin
      	{ Fix Key Case and what not }
      	case Event.KeyCode of
      		$0041..$005A : 	if Event.ShiftCode and (kbEitherShift or kbCapsLock) = 0 then 
				Event.KeyCode := Event.Keycode + $0020;    		
      		$0061..$007A : 	if Event.ShiftCode and (kbEitherShift or kbCapsLock) <> 0 then 
				Event.KeyCode := Event.Keycode - $0020;    		
      	end;

		
        Case Event.KeyCode of
          $0000..$00FF : case InsertMode of
            True : if (Length(RTrim(Dest)) < MaxLen) and (CurX + OfsX <= MaxLen) then begin
              while Length(Dest) < CurX + OfsX - 1 do AddChr(Dest, #32);
              Insert(Event.CharCode, Dest, CurX + OfsX);
              Right(True);
            end;
            False : if (Length(RTrim(Dest)) <= MaxLen) and (CurX + OfsX <= MaxLen) then begin
              while Length(Dest) < CurX + OfsX do AddChr(Dest, #32);
              Dest[CurX + OfsX] := Event.CharCode;
              Right(True);
            end;
          end;
        else
          DoneFlag := True;
        end;
      end;

	procedure HandleCommand;
	begin
		case Event.Command of
          cmStop,
          cmBreak : DoneFlag := True;
          cmBackspace : if OfsX + CurX > 1 then begin
            Delete(Dest, OfsX + CurX - 1, 1);
            Left(True);
          end;
          cmTab,
          cmUntab : begin
          	if CheckTab then begin
				Home;
				DoneFlag := True;
			end else begin
				Event.What := evKeyboard;
				Event.KeyCode := $20;
				HandleKeyboard;
          	end;
          end;
          cmEnter, cmReturn : begin { Enter }
            Home;
            DoneFlag := True;
          end;
          cmEscape : begin { Escape }
            Dest := Source;
            Home;
            DoneFlag := True;
          end;
          cmPrevious : LeftWord;
          cmLeft : Left(True);
          cmNext : RightWord;
          cmRight : Right(True);
          cmInsert : begin
            InsertMode := Not InsertMode;
            DisplayStr;
          end;
          cmDelete : if OfsX + CurX <= MaxLen then begin
            Delete(Dest, OfsX + CurX, 1);
            DisplayStr;
          end;
          cmLineStart, cmTextStart : Home;
          cmLineEnd, cmTextEnd : EndLine;
        else
        	DoneFlag := True;
        end;
	end;

    procedure HandleMouse;
      function Convert(var X, Y : byte) : boolean;
        begin
          Convert := False;
          if (X < Lo(WindMin) + 1) or (X > Lo(WindMax) + 1) or
          (Y < Hi(WindMin) + 1) or (Y > Hi(WindMax) + 1) then
            Exit;
          X := X - Lo(WindMin);
          Y := Y - Hi(WindMin);
          if (Y <> OrgY) or (X < OrgX) or (X > OrgX + MaxWide - 1) then
            Exit;
          X := X - OrgX;
          Y := Y - OrgY + 1;
          { GotoXY ( 2, 2); }
          Convert := True;
        end;

      var
        X, Y : Byte;

      begin
        if Event.What and evMouseDown = evMouseDown then
          begin
            X := Event.Where.X;
            Y := Event.Where.Y;
            if Convert(X, Y) then
              begin
                if Event.Double or ((X > 0) and (X < MaxWide - 1)) then
                  begin
                    while (CurX > X) and Left(False) do;
                    while (CurX < X) and Right(False) do;
                  end
                else
                if X = 0 then
                  begin
                    if OfsX > 0 then
                      Dec(OfsX)
                    else
                    if CurX > 1 then
                      Dec(CurX);
                  end
                else
                  begin
                    if OfsX + MaxWide - 3 < Length(Dest) then
                      Inc(OfsX)
                    else
                    if CurX + OfsX <= Length(Dest) then
                      Inc(CurX);
                  end;
                DisplayStr;
              end
            else
              DoneFlag := True;
          end;
      end;

    begin
      Cursor := GetCursor;
      OrgX := WhereX;
      OrgY := WhereY;
      if First then
        begin
          OfsX   := 0;
          CurX   := 1;
          Source := Dest;
          EndLine;
        end
      else
        VerifyPosition;
      repeat
        DoneFlag := False;
        EditCursor; 
        repeat
          GetEvent(Event);
        until Event.What <> evNothing;
        HideCursor; 
        if Event.What and evCommand = evCommand then HandleCommand else { Command is in evSystem }
        if Event.What and evSystem <> evNothing then DoneFlag := True else
        if Event.What and evMouse  <> evNothing then HandleMouse else
        if Event.What and evKeyDown = evKeyDown then HandleKeyboard;
      until DoneFlag;
      SetCursor(Cursor);
      GotoXY(OrgX, OrgY);
      DoEditLn := (Event.What = evCommand) and (
      	(Event.Command = cmEnter) or (Event.Command = cmReturn) or 
      	(Event.Command = cmTab) or (Event.Command = cmUnTab)
      );
    end;
    
  function EditLn( var Dest : String; MaxLen, MaxWide : Byte) : boolean;
  var 
  	Event : TEvent;
  begin
  	ReleaseButtons;
  	PurgeEvents;
  	ClearEvent(Event);
  	EditLn := DoEditLn(Dest, true, MaxLen, MaxWide, Event);
  	if (Event.What = evCommand) and 
  	(Event.Command <> cmEnter) and (Event.Command <> cmReturn) then begin
  		if Event.InfoPtr = nil then
  			PutCommand(Event.Command, EditSender)
  		else
  			PutEvent(Event);
  	end;
  end;

  function ReadLn(var BufPtr : PTextBuf; Size : word) : word;
    var
      Cursor  : Word;
      ESize   : word;
      TempStr : String;
      Event   : TEvent;
      Wide    : byte;
      X, Y    : Byte;
      I       : word;
    begin
      ReadLn := 0;
      if Size < 2 then Exit;
      ESize := Size - 2;
      if ESize > Sizeof(TempStr) then
        ESize := Sizeof(TempStr) - 1;
      X := WhereX;
      Y := WhereY;
      Cursor  := GetCursor;
      HideCursor;
      TempStr := '';
      if CheckScroll then
        Wide := GetMaxX - X
      else
        Wide := GetMaxX - X + 1;
      if Not DoEditLn(TempStr, True, ESize, Wide, Event) then
        repeat
          {GotoXY ( X, Y );}
        until DoEditLn(TempStr, False, ESize, Wide, Event);
      SetCursor ( Cursor );
      FWriteLn('');
      if Length(TempStr) > ESize then
        TempStr[1] := Chr(ESize);
      AddChr(TempStr, #13);
      AddChr(TempStr, #10);
      Move(TempStr[1], BufPtr^[0], Length(TempStr));
      ReadLn := Length(TempStr);
    end;
    
  function IntStr( I : integer) : String;
  var 
    temp : String;
  begin
  	Str(I, temp);
  	IntStr := temp;
  end;
    
  function Pause : string;
  var	
  	X, Y, B : integer;
  	temp : string;
  begin
  	temp := '';
  	ReleaseButtons;
  	repeat
  		GetMousePos(X, Y, B);
  		if (B <> 0) then
  			temp := 'Mouse X' + IntStr(X) + ', Y' + IntStr(Y) + ', B' + IntStr(B)
  		else if KeyPressed then
  			temp := ReadKey;
  	until temp <> '';
  	ReleaseButtons;
  	Pause := temp;
  end;

{ Redirection of System's Read, ReadLn, Write, and WriteLn procedures }
  function InOutCRT( var F ) : integer;
    var
      I     : word;
      Event : TEvent;
      Chars : Boolean;
    begin
      with TTextRec(F) do
        case Mode of
          {fmInput}  $D7B1 : begin
             BufEnd := ReadLn(BufPtr, BufSize);
             BufPos := 0;
          end;
          {fmOutput} $D7B2 : begin
            I := BufEnd;
            while I < BufPos do
              begin
                if CheckChar = False then
                  FWrite ( BufPtr^[I] )
                else
                  case Byte(BufPtr^[I]) of
                    10 : LineFeed;
                    13 : GotoXY ( 1, WhereY );
                     8 : GotoXY ( WhereX - 1, WhereY );
                  else
                    FWrite ( BufPtr^[I] )
                  end;
                Inc ( I );
                if I >= BufSize then I := 0;
              end;
            BufPos := 0;
          end; { * }
        end;
      InOutCrt := 0;
    end;

  function FlushCRT( var F ) : integer;
    begin
      with TTextRec(F) do
        case Mode of
          fmOutput : InOutCrt ( F );
          fmInput : begin
            BufEnd := 0;
            BufPos := 0;
          end;
        end;
      FlushCrt := 0;
    end;

  function CloseCRT( var F ) : integer;
    begin
      TTextRec(F).Mode := fmClosed;
      CloseCrt := 0;
    end;

  function OpenCRT( var F ) : integer;
    begin
      with TTextRec(F) do
        begin
          if Mode = fmInOut then
            Mode := fmOutPut;
        end;
      OpenCrt := 0;
    end;

  procedure AssignCrt(var F: Text);
    const
      CRTName : Array[0..4] of char = 'CRT'#0;
    begin
      with TTextRec(F) do
        begin
          Handle    := $FFFF;
          Mode      := fmClosed;
          BufSize   := Sizeof(Buffer);
          BufPtr    := @Buffer;
          OpenFunc  := OpenCrt;
          InOutFunc := InOutCrt;
          FlushFunc := FlushCrt;
          CloseFunc := CloseCrt;
          Move(CrtName, Name, Sizeof(Name));
        end;
    end;
    
{ Procedures to preserve and restore settings in QCrt }
  procedure GetQCrtState(var ASettings : TQCrtSettings);
  begin
	ASettings.CheckBreak := CheckBreak;
    ASettings.CheckPrint := CheckPrint;
    ASettings.CheckSysReq := CheckSysReq;
    ASettings.DirectVideo := DirectVideo;
    ASettings.CheckCursor := CheckCursor;
    ASettings.CheckScroll := CheckScroll;
    ASettings.CheckChar := CheckChar;
    ASettings.CheckMouse := CheckMouse;
    ASettings.Check101 := Check101;
    ASettings.CheckTab := CheckTab;
    ASettings.LastMode := LastMode;
    ASettings.TextAttr := TextAttr;
    ASettings.TextChar := TextChar;
    ASettings.InsertMode := InsertMode;
    ASettings.WindMin := WindMin;
    ASettings.WindMax := WindMax;
    ASettings.ScreenMax := ScreenMax;
    ASettings.FontHeight := UserFontSize;
    ASettings.UserFontSize := UserFontSize;
    ASettings.UserFontPtr := UserFontPtr;
    ASettings.MouseAvail := MouseAvail;
    ASettings.MouseShift := MouseShift;
    ASettings.MouseDouble := MouseDouble;
    ASettings.MouseHomeX := MouseHomeX;
    ASettings.MouseHomeY := MouseHomeY;
    ASettings.Cursor := GetCursor;
    ASettings.Blink := GetBlink;
    ASettings.X := WhereX;
    ASettings.Y := WhereY;
  end;
  
  procedure SetQCrtState(var ASettings : TQCrtSettings);
  begin
    if ASettings.LastMode <> LastMode then TextMode(ASettings.LastMode);
    DirectVideo := ASettings.DirectVideo;
    ScreenMax := ASettings.ScreenMax;
    if (WindMin <> ASettings.WindMin) or (WindMax <> ASettings.WindMax) then
    	Window(Lo(WindMin) + 1, Hi(WindMin) + 1, Lo(WindMax) + 1, Hi(WindMax) + 1);
    if (ASettings.X <> WhereX) or (ASettings.Y <> WhereY) then
    	GotoXY(ASettings.X, ASettings.Y);
    if GetCursor <> ASettings.Cursor then SetCursor(ASettings.Cursor);    
    if ASettings.Blink <> GetBlink then SetBlink(ASettings.Blink);

    if (CheckCursor <> ASettings.CheckCursor) then
    begin
      CheckCursor := ASettings.CheckCursor;
	  if CheckCursor then MoveCursor;
    end;    
	CheckBreak := ASettings.CheckBreak;
    CheckPrint := ASettings.CheckPrint;
    CheckSysReq := ASettings.CheckSysReq;
    CheckScroll := ASettings.CheckScroll;
    CheckChar := ASettings.CheckChar;
    CheckMouse := ASettings.CheckMouse;
    Check101 := ASettings.Check101;
    CheckTab := ASettings.CheckTab;
    TextAttr := ASettings.TextAttr;
    TextChar := ASettings.TextChar;
    InsertMode := ASettings.InsertMode;
    FontHeight := ASettings.UserFontSize;
    UserFontSize := ASettings.UserFontSize;
    UserFontPtr := ASettings.UserFontPtr;
    MouseAvail := ASettings.MouseAvail;
    MouseShift := ASettings.MouseShift;
    MouseDouble := ASettings.MouseDouble;
    MouseHomeX := ASettings.MouseHomeX;
    MouseHomeY := ASettings.MouseHomeY;
  end;
  
function GetShiftCode : word;
begin
	GetShiftCode := MemW[Seg0040:$0017];
end;

procedure SetShiftCode(AValue : word);
begin
	MemW[Seg0040:$0017] := AValue;
end;
    
begin
	InitQCrtUnit; 
end.
