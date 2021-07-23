{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$IFDEF TEMPLATES}
procedure AddComments(var AConfig : TConfigFile; Comments : Pointer);
var
	P, T : PListItem;
	C : ^Char;
	S : String;
begin
	AConfig.OpenSection('#');
	S := AConfig.GetValue('#', '');
	if Assigned(AConfig.FSectionKey) and (S = '') then begin
		AConfig.SetValue('#', 'Notes:');
		AConfig.FSectionKey^.Sorted := False;
		P := AConfig.FindKey('#');
		S := '';
		C := Ptr(Seg(Comments^), Ofs(Comments^)); {first is RETF, but it will be skipped}
		repeat
			C := Ptr(Seg(C^), Ofs(C^) + 1);
			case C^ of
				#$00 : begin end; { Ignore }
				#$0D : begin
					T := New(PListItem, Create(S));
					P^.Add(T);
					S := '';
				end;
			else
				S := S + C^;
			end;			
		until C^ = #$00;
		{AConfig.FSectionKey^.Add(P);}
	end;
end;

procedure KeyMapComments; far; assembler; 
asm
	RETF
	DB 'This is a keyboard scancode and keycode mapping file.',0Dh
	DB 'The file is not required. However, when it is present, all keys and commands',0Dh
	DB 'that are used must be mapped. They are in the ASSIMLATE and COMMANDS sections.',0Dh
	DB 'Other sections and keys are not required. Unless, you need to override their',0Dh
	DB 'default values.',0Dh
	(* DB 0Dh
	DB 9,'Example: ',0Dh
	DB 0Dh
	DB 9,'If you wish Delete, Ctrl+Delete, Alt+Delete to all behave',0Dh
	DB 9,'like Delete, then this is where you map them. See the $0008',0Dh
	DB 9,'entry. (Names of keys are in the language file)',0Dh
	DB 0Dh
	DB 9,'Format is:',0Dh
	DB 0Dh
	DB 9,'TO_KEY=FROM_KEY:(A | M | C)+SHIFT_CODE,....',0Dh
	DB 0Dh
	DB 9,'TO_KEY is the key code that is desired.',0Dh
	DB 9,'FROM_KEY is the original pressed keycode.',0Dh
	DB 9,':A is match any shift code at keypress. (Assumed if not present)',0Dh
	DB 9,':M is must match shift code exactly.',0Dh
	DB 9,':C is must contain shift code, but can have additional shifts.',0Dh
	DB 9,'+SHIFT_CODE is shift code if present.',0Dh
	DB 9,9,'SHIFT CODES are bit values and can be added together for any combination.',0Dh
	DB 9,9,9,'kbRightShift  = $0001',0dh
    DB 9,9,9,'kbLeftShift   = $0002',0dh
    DB 9,9,9,'kbEitherShift = kbRightShift or kbLeftShift',0dh
    DB 9,9,9,'kbEitherCtrl  = $0004',0dh
    DB 9,9,9,'kbEitherAlt   = $0008',0dh
    DB 9,9,9,'kbScrollLock  = $0010',0dh
    DB 9,9,9,'kbNumsLock    = $0020',0dh
    DB 9,9,9,'kbCapsLock    = $0040',0dh
    DB 9,9,9,'kbInsertLock  = $0080',0dh
    DB 9,9,9,'kbLeftCtrl    = $0100',0dh
    DB 9,9,9,'kbLeftAlt     = $0200',0dh
    DB 9,9,9,'kbSysDown     = $0400',0dh
    DB 9,9,9,'kbPauseFlag   = $0800',0dh
    DB 9,9,9,'kbScrollDown  = $1000',0dh
    DB 9,9,9,'kbNumsDown    = $2000',0dh
    DB 9,9,9,'kbCapsDown    = $4000',0dh
    DB 9,9,9,'kbInsertDown  = $8000',0dh
 	DB 0Dh
	DB 9,',.... additional FROM_KEY sequences.',0Dh *)
	DB 0
end;
	
procedure LanguageComments; far; assembler; 
asm
	RETF
	DB 'This is a Program Manager Eternity language file.',0Dh
	DB 'These are the default values and are not required unless '
	DB 'changes are needed.',0Dh
	DB 0
end;
	
procedure ThemeComments; far; assembler; 
asm
	RETF
	DB 'This is a Program Manager Eternity theme file.',0Dh
	DB 'These are the default values and are not required unless '
	DB 'changes are needed.',0Dh
	DB 0
end;
{$ENDIF}
