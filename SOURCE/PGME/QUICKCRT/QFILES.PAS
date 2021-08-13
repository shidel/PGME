{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

unit QFiles; { QuickCrt Files }
{$I QCRT.DEF}
{$D-,Y-,L-}

interface

uses QClass;

  const
	class_TAbstractFile : TObjectClass = 'TAbstractFile';
	class_TFile : TObjectClass = 'TFile';

  const
    { File status flags }
    flInvalid  = $0000; { Is Not Valid }
    flAssigned = $0001; { IsAssigned }
    flOpened   = $0002; { IsOpen }
    flFlush    = $0004; { Needs Flushed }
    flErase    = $0008; { Erase when disposed (not advised) }
    flError    = $0010; { Is Error Code }
    flRead     = $0020; { File can be read }
    flWrite    = $0040; { File can be written }
    flRandom   = $0060; { File can be read or written }

    { SetIOResult return commands }
    fcOK       = $00;
    fcIgnore   = $01;
    fcRetry    = $02;
    fcAbort    = $03;

  type
    PAbstractFile = ^TAbstractFile;
    TAbstractFile = object (TObject)
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
        Ptr    : LongInt;
        Mode   : word;
        Flags  : word;
        Name   : String;
        Result : integer;
      constructor Create;
      destructor  Destroy; virtual;
      procedure Assign( AFileName : String ); virtual;
      procedure BlockRead(const ABuf; ACount: Word; var AResult: Word); virtual;
      procedure BlockWrite(const ABuf; ACount: Word; var AResult: Word); virtual;
      procedure Close; virtual;
      procedure Erase; virtual;
      function  FileSize: Longint; virtual;
      procedure Flush; virtual;
      procedure Rename( ANewName : String ); virtual;
      procedure Reset; virtual;
      procedure Rewrite; virtual;
      procedure Seek(AFilePos: Longint); virtual;
      procedure Truncate; virtual;
      function  SetIOResult ( AIOResult : integer ) : byte; virtual;
    { the following procedures/functions should not need to be overridden }
      procedure Append;
      procedure Copy(var AFile : PAbstractFile; ACount: LongInt);
      function  Sof: Boolean;
      function  Eof: Boolean;
      function  FilePos: Longint;
      function  IOResult: Integer;
      function  SeekSof: Boolean;
      function  SeekEof: Boolean;
      procedure SetFileMode ( AMode : word );
    { Should never, ever need overridden (they all call BlockRead) }
      procedure Insert(ACount : longint);
      procedure Delete(ACount : longint);
      procedure ReadRecord    (const AAdr; ASize : word);
      procedure WriteRecord   (const AAdr; ASize : word);
      procedure ReadChar      (const AChr );
      procedure WriteChar     (const AChr );
      procedure ReadBoolean   (const ABoolean);
      procedure WriteBoolean  (const ABoolean);
      procedure ReadByteBool  (const AByteBool);
      procedure WriteByteBool (const AByteBool);
      procedure ReadWordBool  (const AWordBool);
      procedure WriteWordBool (const AWordBool);
      procedure ReadLongBool  (const ALongBool);
      procedure WriteLongBool (const ALongBool);
      procedure ReadByte      (const AByte);
      procedure WriteByte     (const AByte);
      procedure ReadWord      (const AWord);
      procedure WriteWord     (const AWord);
      procedure ReadShortInt  (const AShortInt);
      procedure WriteShortInt (const AShortInt);
      procedure ReadInteger   (const AInteger);
      procedure WriteInteger  (const AInteger);
      procedure ReadLongInt   (const ALongInt);
      procedure WriteLongInt  (const ALongInt);
      procedure ReadReal      (const AReal);
      procedure WriteReal     (const AReal);
      procedure ReadSingle    (const ASingle);
      procedure WriteSingle   (const ASingle);
      procedure ReadDouble    (const ADouble);
      procedure WriteDouble   (const ADouble);
      procedure ReadComp      (const AComp);
      procedure WriteComp     (const AComp);
      procedure ReadExtended  (const AExtended);
      procedure WriteExtended (const AExtended);
      procedure ReadString  (const AString; AStrSize : word);
      procedure WriteString (const AString; AStrSize : word);
      function  GetString   : String;
      procedure PutString   (const AString);
    { Text files }
      function  Soln: boolean;
      function  Eoln: Boolean;
      function  SeekSoln: boolean;
      function  SeekEoln: Boolean;
      function  SeekLn ( ANumber : LongInt ) : boolean;
      procedure Read  ( const AStr; AMax : word );
      procedure Write ( const AStr );
      procedure ReadLn  ( const AStr; AMax : word );
      procedure WriteLn ( const AStr );
      function  LengthLn : word;
      procedure InsertLn ( ALen : word );
      procedure DeleteLn;
      procedure ResizeLn ( ALen : word );
    end;

    PFile = ^TFile;
    TFile = object( TAbstractFile )
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
        FRec : File;
      constructor Create;
      destructor  Destroy; virtual;
      procedure Assign( AFileName : String ); virtual;
      procedure BlockRead(const ABuf; ACount: Word; var AResult: Word); virtual;
      procedure BlockWrite(const ABuf; ACount: Word; var AResult: Word); virtual;
      procedure Close; virtual;
      procedure Erase; virtual;
      function FileSize: Longint; virtual;
      procedure Flush; virtual;
      procedure Rename( ANewName : String ); virtual;
      procedure Reset; virtual;
      procedure Rewrite; virtual;
      procedure Seek(AFilePos: Longint); virtual;
      procedure Truncate; virtual;
      function GetName : String;
    end;

implementation

  uses QDos;

{$I ERRCONST.INC}

  const
    CRLF : String[2] = #13#10;

  var
    Buf : array[0..511] of byte;

  function TAbstractFile.ObjectClass(AName : String) : String;
  begin
	if (AName = '') or (AName = class_TAbstractFile) then
		ObjectClass := class_TAbstractFile
	else
		ObjectClass := inherited ObjectClass(AName);
  end;

  constructor TAbstractFile.Create;
    begin
      Ptr := 0;
      Mode := 0;
      Name := '';
      Flags := flInvalid;
      SetIOResult(erNone);
    end;

  destructor TAbstractFile.Destroy;
    begin
      if Flags and flOpened = flOpened then Close;
      if Flags and flErase = flErase then Erase;
    end;

  procedure TAbstractFile.Assign( AFileName : String );
    begin
      if (Flags and flOpened = flOpened) then
        SetIOResult(erFile_access_denied)
      else
        begin
          SetIOResult ( erNone );
          Name := AFileName;
        end;
    end;

  procedure TAbstractFile.BlockRead(const ABuf; ACount: Word; var AResult: Word);
    begin
      AResult := 0;
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
      if (Flags and flOpened <> flOpened ) then
        SetIOResult(erFile_not_open)
      else
      if (Flags and flRead <> flRead) then
        SetIOResult(erFile_not_open_for_input)
      else
        SetIOResult ( erNone );
    end;

  procedure TAbstractFile.BlockWrite(const ABuf; ACount: Word; var AResult: Word);
    begin
      AResult := 0;
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
      if (Flags and flOpened <> flOpened) then
        SetIOResult(erFile_not_open)
      else
      if (Flags and flWrite <> flWrite) then
        SetIOResult(erFile_not_open_for_output)
      else
        SetIOResult ( erNone );
    end;

  procedure TAbstractFile.Close;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
      if (Flags and flOpened <> flOpened) then
        SetIOResult(erFile_not_open)
      else
        begin
          SetIOResult ( erNone );
          if Flags and flFlush = flFlush then Flush;
        end;
    end;

  procedure TAbstractFile.Erase;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
      if (Flags and flOpened = flOpened) then
        SetIOResult(erFile_access_denied)
      else
        SetIOResult ( erNone );
    end;

  function TAbstractFile.FilePos: Longint;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
      if (Flags and flOpened <> flOpened) then
        SetIOResult(erFile_not_open)
      else
        begin
          SetIOResult( erNone );
          FilePos := Ptr;
        end;
    end;

  function TAbstractFile.FileSize: Longint;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
      if (Flags and flOpened <> flOpened) then
        SetIOResult(erFile_not_open)
      else
        begin
          SetIOResult ( erNone );
          FileSize := 0;
        end;
    end;

  procedure TAbstractFile.Flush;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
      if (Flags and flOpened <> flOpened) then
        SetIOResult(erFile_not_open)
      else
      if (Flags and flWrite <> flWrite) then
        SetIOResult(erFile_not_open_for_output)
      else
        SetIOResult ( erNone );
    end;

  function TAbstractFile.IOResult: Integer;
    begin
      IOResult := Result;
      SetIOResult(erNone);
    end;

  function TAbstractFile.SetIOResult ( AIOResult : integer ) : byte;
    begin
      if AIOResult <> erNone then
        Flags := Flags or flError
      else
        Flags := Flags and (not flError);
      Result := AIOResult;
      case AIOResult of
        erNone : SetIOResult := fcOK;
      else
        SetIOResult := fcAbort;
        {$IFOPT I+}
        RunError(AIOResult);
        {$ENDIF}
      end;
    end;

  procedure TAbstractFile.Rename;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
        SetIOResult ( erNone );
    end;

  procedure TAbstractFile.Reset;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
        begin
          SetIOResult ( erNone );
          if (Flags and flOpened = flOpened) then
            Close;
          if (Flags and flError <> flError) then
            SetFileMode ( FileMode );
        end;
    end;

  procedure TAbstractFile.Rewrite;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
        begin
          SetIOResult ( erNone );
          if (Flags and flOpened = flOpened) then
            Close;
          if (Flags and flError <> flError) then
            SetFileMode ( (FileMode and (not 3)) or 2 );
        end;
    end;

  procedure TAbstractFile.Seek(AFilePos: Longint);
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
      if (Flags and flOpened <> flOpened) then
        SetIOResult(erFile_not_open)
      else
        SetIOResult ( erNone );
    end;

  procedure TAbstractFile.Truncate;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
      if (Flags and flOpened <> flOpened) then
        SetIOResult(erFile_not_open)
      else
        SetIOResult ( erNone );
    end;

  procedure TAbstractFile.Append;
    begin
      if (Flags and flAssigned <> flAssigned) then
        SetIOResult(erFile_not_assigned)
      else
        begin
          SetIOResult ( erNone );
          if (Flags and flOpened = flOpened) then
            Close;
          if (Flags and flError <> flError) then
            SetFileMode ( (FileMode and (not 3)) or 1 );
        end;
    end;

  procedure TAbstractFile.Copy(var AFile : PAbstractFile; ACount: LongInt);
    var
      RCount, WCount : word;
    begin
      repeat
        BlockRead(Buf, Sizeof(Buf), RCount);
        if (Flags and flError <> flError) then
          AFile^.BlockWrite(Buf, RCount, WCount);
      until Eof or (Flags and flError = flError) or
        (AFile^.Flags and flError = flError) or (RCount <> Sizeof(Buf)) or
        (RCount <> WCount);
    end;

  function TAbstractFile.Sof: Boolean;
    begin
      Sof := (Ptr = 0 );
    end;

  function TAbstractFile.Eof: Boolean;
    begin
      Eof := (FileSize <= Ptr );
    end;

  function TAbstractFile.SeekSof: Boolean;
    begin
      Seek( 0 );
      SeekSof := Flags and flError <> flError;
    end;

  function TAbstractFile.SeekEof: Boolean;
    var
      Size : LongInt;
    begin
      Size := FileSize;
      if (Flags and flError <> flError) then
        Seek( Size );
      SeekEof := Flags and flError <> flError;
    end;

  procedure TAbstractFile.SetFileMode ( AMode : word );
    begin
      Mode := AMode;
      Flags := Flags and (Not flRandom);
      case Mode and 2 of
        0 : Flags := Flags or flRead;
        1 : Flags := Flags or flWrite;
        2 : Flags := Flags or flRandom;
      end;
    end;

  procedure TAbstractFile.Insert(ACount : longint);
    var
      Size,
      Count  : word;
      OldPtr : LongInt;
      CurPtr : LongInt;
    begin
      OldPtr := FilePos;
      if (Flags and flError = flError) or (ACount < 1) then Exit;
      CurPtr := FileSize;
      if (Flags and flError <> flError) then
        repeat
          Size := Sizeof(Buf);
          if CurPtr - Size < OldPtr then
            Size := CurPtr - OldPtr;
          Dec(CurPtr, Size);
          Seek(CurPtr);
          if (Flags and flError <> flError) then BlockRead(Buf, Size, Count);
          if (Flags and flError <> flError) then Seek(CurPtr + ACount);
          if (Flags and flError <> flError) then BlockWrite(Buf, Size, Count);
        until (CurPtr = OldPtr) or (Flags and flError = flError);
      if (Flags and flError <> flError) then Seek(OldPtr);
    end;

  procedure TAbstractFile.Delete(ACount : longint);
    var
      Size,
      Count  : word;
      OldPtr : LongInt;
      CurPtr : LongInt;
      FSize  : LongInt;
    begin
      OldPtr := FilePos;
      if (Flags and flError = flError) or (ACount < 1) then Exit;
      FSize := FileSize;
      CurPtr := OldPtr + ACount;
      if (Flags and flError <> flError) then
        repeat
          Size := Sizeof(Buf);
          if CurPtr + Size > FSize then
            Size := FSize - CurPtr;
          Seek(CurPtr);
          if (Flags and flError <> flError) then BlockRead(Buf, Size, Count);
          if (Flags and flError <> flError) then Seek(CurPtr - ACount);
          if (Flags and flError <> flError) then BlockWrite(Buf, Size, Count);
          Inc(CurPtr, Size);
        until (CurPtr >= FSize) or (Flags and flError = flError);
      if (Flags and flError <> flError) then Truncate;
      if (Flags and flError <> flError) then Seek(OldPtr);
    end;

  procedure TAbstractFile.ReadRecord (const AAdr; ASize : word);
    var
      Count : word;
    begin
      BlockRead ( AAdr, ASize, Count );
      if (Flags and flError <> flError) and (Count <> ASize) then
        SetIOResult(erDevice_Read_Fault);
    end;

  procedure TAbstractFile.WriteRecord (const AAdr; ASize : word);
    var
      Count : word;
    begin
      BlockWrite ( AAdr, ASize, Count );
      if (Flags and flError <> flError) and (Count <> ASize) then
        SetIOResult(erDevice_Write_Fault);
    end;

  procedure TAbstractFile.ReadChar ( const AChr );
    begin
      ReadRecord(AChr, Sizeof(Char));
    end;

  procedure TAbstractFile.WriteChar ( const AChr );
    begin
      WriteRecord(AChr, Sizeof(Char));
    end;

  procedure TAbstractFile.ReadBoolean   (const ABoolean);
    begin
      ReadRecord(ABoolean, Sizeof(Boolean));
    end;

  procedure TAbstractFile.WriteBoolean  (const ABoolean);
    begin
      WriteRecord(ABoolean, Sizeof(Boolean));
    end;

  procedure TAbstractFile.ReadByteBool  (const AByteBool);
    begin
      ReadRecord(AByteBool, Sizeof(ByteBool));
    end;

  procedure TAbstractFile.WriteByteBool (const AByteBool);
    begin
      WriteRecord(AByteBool, Sizeof(ByteBool));
    end;

  procedure TAbstractFile.ReadWordBool  (const AWordBool);
    begin
      ReadRecord(AWordBool, Sizeof(WordBool));
    end;

  procedure TAbstractFile.WriteWordBool (const AWordBool);
    begin
      WriteRecord(AWordBool, Sizeof(WordBool));
    end;

  procedure TAbstractFile.ReadLongBool  (const ALongBool);
    begin
      ReadRecord(ALongBool, Sizeof(LongBool));
    end;

  procedure TAbstractFile.WriteLongBool (const ALongBool);
    begin
      WriteRecord(ALongBool, Sizeof(LongBool));
    end;

  procedure TAbstractFile.ReadByte      (const AByte);
    begin
      ReadRecord(AByte, Sizeof(Byte));
    end;

  procedure TAbstractFile.WriteByte     (const AByte);
    begin
      WriteRecord(AByte, Sizeof(Byte));
    end;

  procedure TAbstractFile.ReadWord      (const AWord);
    begin
      ReadRecord(AWord, Sizeof(Word));
    end;

  procedure TAbstractFile.WriteWord     (const AWord);
    begin
      WriteRecord(AWord, Sizeof(Word));
    end;

  procedure TAbstractFile.ReadShortInt  (const AShortInt);
    begin
      ReadRecord(AShortInt, Sizeof(ShortInt));
    end;

  procedure TAbstractFile.WriteShortInt (const AShortInt);
    begin
      WriteRecord(AShortInt, Sizeof(ShortInt));
    end;

  procedure TAbstractFile.ReadInteger   (const AInteger);
    begin
      ReadRecord(AInteger, Sizeof(Integer));
    end;

  procedure TAbstractFile.WriteInteger  (const AInteger);
    begin
      WriteRecord(AInteger, Sizeof(Integer));
    end;

  procedure TAbstractFile.ReadLongInt   (const ALongInt);
    begin
      ReadRecord(ALongInt, Sizeof(LongInt));
    end;

  procedure TAbstractFile.WriteLongInt  (const ALongInt);
    begin
      WriteRecord(ALongInt, Sizeof(LongInt));
    end;

  procedure TAbstractFile.ReadReal      (const AReal);
    begin
      ReadRecord(AReal, Sizeof(Real));
    end;

  procedure TAbstractFile.WriteReal     (const AReal);
    begin
      WriteRecord(AReal, Sizeof(Real));
    end;

  procedure TAbstractFile.ReadSingle    (const ASingle);
    begin
      ReadRecord(ASingle, Sizeof(Single));
    end;

  procedure TAbstractFile.WriteSingle   (const ASingle);
    begin
      WriteRecord(ASingle, Sizeof(Single));
    end;

  procedure TAbstractFile.ReadDouble    (const ADouble);
    begin
      ReadRecord(ADouble, Sizeof(Double));
    end;

  procedure TAbstractFile.WriteDouble   (const ADouble);
    begin
      WriteRecord(ADouble, Sizeof(Double));
    end;

  procedure TAbstractFile.ReadComp      (const AComp);
    begin
      ReadRecord(AComp, Sizeof(Comp));
    end;

  procedure TAbstractFile.WriteComp     (const AComp);
    begin
      WriteRecord(AComp, Sizeof(Comp));
    end;

  procedure TAbstractFile.ReadExtended  (const AExtended);
    begin
      ReadRecord(AExtended, Sizeof(Extended));
    end;

  procedure TAbstractFile.WriteExtended (const AExtended);
    begin
      WriteRecord(AExtended, Sizeof(Extended));
    end;

  procedure TAbstractFile.ReadString  (const AString; AStrSize : word);
    begin
      ReadRecord(AString, Sizeof(String));
    end;

  procedure TAbstractFile.WriteString (const AString; AStrSize : word);
    begin
      WriteRecord(AString, Sizeof(String));
    end;

  function TAbstractFile.GetString : String;
    var
      Str : String;
    begin
      ReadByte(Str[0]);
      if (Flags and flError <> flError) then
        ReadRecord(Str[1], Byte(Str[0]));
      if (Flags and flError <> flError) then
        GetString := Str
      else
        GetString := '';
    end;

  procedure TAbstractFile.PutString (const AString);
    begin
      WriteRecord(AString, Length(String(AString)) + 1);
    end;

  { Text files }
  function TAbstractFile.Soln: Boolean;
    var
      Test  : word;
      Count : word;
      P     : LongInt;
    begin
      Soln := False;
      P := FilePos;
      if (Flags and flError = flError) then Exit;
      case P of
        1 : Exit;
        0 : Soln := True;
      else
        Seek(P - 2);
        if (Flags and flError <> flError) then
          BlockRead ( Test, Sizeof(Test), Count );
        Soln := (Count = 2) and (Test = $0A0D) and
        (Flags and flError <> flError);
      end;
    end;

  function TAbstractFile.EoLn: Boolean;
    var
      Test  : word;
      Count : word;
      P     : LongInt;
    begin
      EoLn := False;
      P := FilePos;
      if (Flags and flError = flError) then Exit;
      BlockRead ( Test, Sizeof(Test), Count );
      Eoln := (Count = 2) and (Test = $0A0D) and
        (Flags and flError <> flError);
      Seek(P);
    end;

  function TAbstractFile.SeekSoln: Boolean;
    var
      SLn : boolean;
    begin
      SetIOResult ( erNone );
      repeat
        SLn := Soln;
        if (Flags and flError <> flError) and (Not SLn) then
          Seek(Ptr - 1);
      until SLn or (Flags and flError = flError);
      SeekSoln := Soln;
    end;

  function TAbstractFile.SeekEoln: Boolean;
    var
      EndLn, EndF : boolean;
    begin
      SetIOResult ( erNone );
      repeat
        EndLn := Eoln;
        if (Flags and flError <> flError) then EndF  := Eof;
        if (Flags and flError <> flError) and (Not EndLn) and (Not EndF) then
          Seek(Ptr + 1);
      until EndLn or EndF or (Flags and flError = flError);
      SeekEoln := Eoln;
    end;

 { Old version
 procedure TAbstractFile.Read ( const AStr; AMax : word );
    var
      P : ^String;
      I : word;
      C : Char;
    begin
      P := @AStr;
      I := 0;
      P^[0] := #0;
      while (Not EOF) and (Not Eoln) and (Flags and flError <> flError) and (I < AMax) do
        begin
          ReadRecord(C, Sizeof(C));
          if (Flags and flError <> flError) then
            begin
              Inc(I);
              if I <= 255 then
                begin
                  P^[0] := Char(I);
                  P^[I] := C;
                end;
            end;
        end;
    end; }

 procedure TAbstractFile.Read ( const AStr; AMax : word );
    var
      P, T : ^String;
      Max, Count : word;
      I : integer;
      L : LongInt;
    begin
       P := @AStr;
       P^[0] := #0;
       T := @Buf;
       L := FilePos;
       if Max > 255 then Max := 255;
       if (AMax > 0) and (Flags and flError <> flError) then begin
			BlockRead(Buf[1], Max, Count);   { Buffer is 512, Max String is 255 }
			if (Flags and flError <> flError) then
				begin
					Buf[0] := Count;
					I := Pos(#$0A#$0D, T^);
					if I > 0 then begin
						Buf[0] := I - 1;
						Seek(L + I + 1);
{		   			end else begin
						Seek(L + Count + 1);}
		   			end;
					P^ := T^;
				end;
		end;
    end;

  procedure TAbstractFile.Write ( const AStr );
    begin
      WriteRecord(String(AStr)[1], Length(String(AStr)));
    end;

 { Old Version
  procedure TAbstractFile.ReadLn ( const AStr; AMax : word );
    var
      P : ^String;
      I : word;
      C : Char;

    begin
      P := @AStr;
      I := 0;
      P^[0] := #0;
      while (Not EOF) and (Not Eoln) and (Flags and flError <> flError) do
        begin
          ReadRecord(C, Sizeof(C));
          if (Flags and flError <> flError) and (I < AMax) then
            begin
              Inc(I);
              if I <= 255 then
                begin
                  P^[0] := Char(I);
                  P^[I] := C;
                end;
            end;
        end;
      if (Flags and flError <> flError) and (Not EOF) then ReadRecord ( I, Sizeof(I) );
    end; }

  procedure TAbstractFile.ReadLn ( const AStr; AMax : word );
    var
      P, T : ^String;
      Max, Count : word;
      I : integer;
      L : LongInt;
    begin
       P := @AStr;
       P^[0] := #0;
       T := @Buf;
       L := FilePos;
       I := 0;
       if Max > 255 then Max := 255;
       if (AMax > 0) and (Flags and flError <> flError) then begin
			BlockRead(Buf[1], Max, Count);   { Buffer is 512, Max String is 255 }
			if (Flags and flError <> flError) then
				begin
					Buf[0] := Count;
					I := Pos(#$0D#$0A, T^);
					if I > 0 then
						Buf[0] := I - 1;
		   			P^ := T^;
				end;
		end;

		while (FilePos > 0) and ( I = 0 ) and (Not EOF) and (Flags and flError <> flError) do begin
			Seek(FilePos - 1);
			BlockRead(Buf[1], 255, Count);
			Buf[0] := Count;
			I := Pos(#$0A#$0D, T^);
		end;
		if I <> 0 then
			Seek(L + I + 1);
    end;

  procedure TAbstractFile.WriteLn ( const AStr );
    const
      CRLF : word = $0A0D;
    begin
      WriteRecord(String(AStr)[1], Length(String(AStr)));
      if Flags and flError <> flError then WriteWord ( CRLF );
    end;

  function TAbstractFile.LengthLn : word;
    var
      OPtr : longInt;
      SPtr : longInt;
      EPtr : longInt;
    begin
      LengthLn := 0;
      OPtr := FilePos;
      if (Flags and flError <> flError) then SeekSoln;
      if (Flags and flError <> flError) then SPtr := FilePos;
      if (Flags and flError <> flError) then SeekEoln;
      if (Flags and flError <> flError) then EPtr := FilePos;
      if (Flags and flError <> flError) then Seek(OPtr);
      if (Flags and flError <> flError) then LengthLn := EPtr - SPtr;
    end;

  procedure TAbstractFile.InsertLn ( ALen : word );
    var
      OPtr : longInt;
    begin
      SeekSoln;
      if (Flags and flError <> flError) then OPtr := FilePos;
      if (Flags and flError <> flError) then Insert(ALen + 2);
      if (Flags and flError <> flError) then Seek(OPtr + ALen);
      if (Flags and flError <> flError) then Write(CRLF);
      if (Flags and flError <> flError) then Seek(OPtr);
    end;

  procedure TAbstractFile.DeleteLn;
    var
      OLen : word;
      OPtr : LongInt;
    begin
      SeekSoln;
      if (Flags and flError <> flError) then OPtr := FilePos;
      if (Flags and flError <> flError) then OLen := LengthLn;
      if (Flags and flError <> flError) then Delete(OLen + 2);
      if (Flags and flError <> flError) then Seek(OPtr);
    end;

  procedure TAbstractFile.ResizeLn ( ALen : word );
    var
      OLen : word;
      OPtr : LongInt;
    begin
      OLen := LengthLn;
      if (Flags and flError <> flError) then
        begin
          if OLen < ALen then
            begin
              SeekSoln;
              if (Flags and flError <> flError) then OPtr := FilePos;
              if (Flags and flError <> flError) then Seek(OPtr + OLen);
              if (Flags and flError <> flError) then Insert(ALen - OLen);
              if (Flags and flError <> flError) then SeekSoln;
            end
          else
          if OLen > ALen then
            begin
              SeekSoln;
              if (Flags and flError <> flError) then OPtr := FilePos;
              if (Flags and flError <> flError) then Seek(OPtr + ALen);
              if (Flags and flError <> flError) then Delete(OLen - ALen);
              if (Flags and flError <> flError) then Seek(OPtr);
            end
          else
            SeekSoln;
        end;
    end;

  function TAbstractFile.SeekLn ( ANumber : LongInt ) : boolean;
    var
      I : LongInt;
      S : String[1];
    begin
      SeekSof;
      for I := 1 to ANumber - 1 do
        if (Flags and flError <> flError) then
          ReadLn ( S, 0 );
      SeekLn := (Flags and flError <> flError);
    end;

{ ------------ TFile ------------ }

  function TFile.ObjectClass(AName : String) : String;
  begin
	if (AName = '') or (AName = class_TFile) then
		ObjectClass := class_TFile
	else
		ObjectClass := inherited ObjectClass(AName);
  end;

  constructor TFile.Create;
    begin
      inherited Create;
    end;

  destructor  TFile.Destroy;
    begin
      inherited Destroy;
    end;

  procedure TFile.Assign( AFileName : String );
    var
      Action : byte;
    begin
      inherited Assign(AFileName);
      if (Flags and flError <> flError) then
        begin
          repeat
            System.Assign(FRec, AFileName);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
          if (Action <> fcAbort) then
            Flags := Flags or flAssigned;
        end;
    end;

  procedure TFile.BlockRead(const ABuf; ACount: Word; var AResult: Word);
    var
      P : Pointer;
      Action : byte;
    begin
      inherited BlockRead(ABuf, ACount, AResult);
      if (Flags and flError <> flError) then
        begin
          repeat
            P := @ABuf;
            System.BlockRead(FRec, P^, ACount, AResult);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
          if (Action <> fcAbort) then Inc(Ptr, AResult)
        end;
    end;

  procedure TFile.BlockWrite(const ABuf; ACount: Word; var AResult: Word);
    var
      P : Pointer;
      Action : byte;
    begin
      inherited BlockWrite(ABuf, ACount, AResult);
      if (Flags and flError <> flError) then
        begin
          repeat
            P := @ABuf;
            System.BlockWrite(FRec, P^, ACount, AResult);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
          if (Action <> fcAbort) then Inc(Ptr, AResult);
        end;
    end;

  procedure TFile.Close;
    var
      Action : byte;
    begin
      inherited Close;
      if (Flags and flError <> flError) then
        begin
          repeat
            System.Close(FRec);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
          if (Action <> fcAbort) then
            Flags := Flags and (Not flOpened);
          if (Action <> fcAbort) then Ptr := 0;
        end;
    end;

  procedure TFile.Erase;
    var
      Action : byte;
    begin
      inherited Erase;
      if (Flags and flError <> flError) then
        begin
          repeat
            System.Erase(FRec);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
        end;
    end;

  function TFile.FileSize: Longint;
    var
      Action : byte;
    begin
      inherited FileSize;
      if (Flags and flError <> flError) then
        begin
          repeat
            FileSize := System.FileSize(FRec);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
        end;
    end;

  procedure TFile.Flush;
    begin
      inherited Flush;
      if (Flags and flError <> flError) then
        begin
          Flags := Flags and (Not flFlush);
        end;
    end;


  procedure TFile.Rename( ANewName : String );
    var
      Action : byte;
    begin
      inherited Rename(ANewName);
      if (Flags and flError <> flError) then
        begin
          repeat
            System.Rename(FRec, ANewName);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
          if (Action <> fcAbort) then
            Name := ANewName;
        end;
    end;

  procedure TFile.Reset;
    var
      Action : byte;
    begin
      inherited Reset;
      if (Flags and flError <> flError) then
        begin
          repeat
            System.Reset(FRec, 1);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
          if (Action <> fcAbort) then
            Flags := Flags or flOpened;
          if (Action <> fcAbort) then Ptr := 0;
        end;
    end;

  procedure TFile.Rewrite;
    var
      Action : byte;
    begin
      inherited Rewrite;
      if (Flags and flError <> flError) then
        begin
          repeat
            System.Rewrite(FRec, 1);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
          if (Action <> fcAbort) then
            Flags := Flags or flOpened;
          if (Action <> fcAbort) then Ptr := 0;
        end;
    end;

  procedure TFile.Seek(AFilePos: Longint);
    var
      Action : byte;
    begin
      inherited Seek(AFilePos);
      if (Flags and flError <> flError) then
        begin
          repeat
            System.Seek(FRec, AFilePos);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
          if (Action <> fcAbort) then
            Flags := Flags or flOpened;
          if (Action <> fcAbort) then Ptr := AFilePos;
        end;
    end;

  procedure TFile.Truncate;
    var
      Action : byte;
    begin
      inherited Truncate;
      if (Flags and flError <> flError) then
        begin
          repeat
            System.Truncate(FRec);
            Action := SetIOResult(System.IOResult);
          until (Action <> fcRetry);
          if (Action <> fcAbort) then
            Flags := Flags or flOpened;
        end;
    end;

  function TFile.GetName : String;
    var
      I : word;
      S : String;
    begin
      S := '';
      for I := 0 to 79 do
        if TFileRec(FRec).Name[I] = #0 then
          Break
        else
          S := S + (TFileRec(FRec).Name[I]);
      GetName := S;
    end;

end.

