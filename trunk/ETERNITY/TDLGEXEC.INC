{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgExecOpt --- }

{$IFDEF INTERFACE}
const
	class_TDlgExecOpt = 'TDlgExecOpt';

type 
	PDlgExecOpt = ^TDlgExecOpt;
	TDlgExecOpt = object (TDialog)
    public { protected }
    		FFileName : PLabel;
    		FCmdLine : PEdit;
    		FOK : PButton;	
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure SetData(ADlg : PObject); virtual;
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgExecOpt.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgExecOpt) then 
		ObjectClass := class_TDlgExecOpt
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgExecOpt.Create;
var
	B, T : TBounds;
begin
	inherited Create(AParent, AName);
	SetCaption('Optional Parameters');
	FStyle := bxSingle;	
	FAutoEndModal := True;
	Bounds(1,1,50,12, B);
	
	SetBounds(B);	
	Bounds(3,2,B.Width - 8,1, T);
	TextAttr := $70;
	with PLabel(New(PLabel, Create(@Self, 'LABEL.EXEC')))^ do begin
		SetBounds(T);
		SetAnchors(abLeft or abRight or abTop);
		FHAlign := AlignLeft;
		SetCaption('Executable Program:');
	end;
	Inc(T.Top, 1);
	TextAttr := $71;
	FFileName := New(PLabel, Create(@Self, 'LABEL.FILENAME'));
	with PLabel(FFileName)^ do begin
		SetHelpText('The "Executable Program" label shows which executable you are');
		AddHelpText('modifying.');
		SetBounds(T);
		FHAlign := AlignLeft;
		SetAnchors(abLeft or abRight or abTop);
		SetCaption('D:\SOME.EXE');
	end;
	Inc(T.Top, 2);
	TextAttr := $70;
	with PLabel(New(PLabel, Create(@Self, 'LABEL.CMDLN')))^ do begin
		SetBounds(T);
		FHAlign := AlignLeft;
		SetAnchors(abLeft or abRight or abTop);
		SetCaption('Command line parameters:');
	end;
	Inc(T.Top, 1);
	TextAttr := $1E;
	FCmdLine := New(PEdit, Create(@Self, 'EDIT.CMDLN'));
	with PEdit(FCmdLine)^ do begin
		SetHelpText('The "Command Line" edit allows you to enter optional command');
		AddHelpText('command line parameters to use when a specific program is.');
		AddHelpText('executed.');
		SetBounds(T);
		SetAnchors(abLeft or abRight or abTop);
		SetCaption('');
	end;

	Bounds(B.Width div 2 - 7, B.Height - 4, 14, 1, T);
	TextAttr := $2E;
	FOK := New(PButton, Create(@Self, 'BUTTON.OK'));
	with FOK^ do begin
		SetBounds(T);
		SetAnchors(abRight or abBottom);
		SetCaption('OK');
		SetCommand(cmOk);
	end;

end;

procedure TDlgExecOpt.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
{	if AEvent.What = evCommand then 
		case AEvent.Command of
			end;
		end;}
end;

procedure TDlgExecOpt.SetData(ADlg : PObject);
begin
	FFileName^.SetCaption('');
	FCmdLine^.SetCaption('');
	if Assigned(ADlg) then begin
		if ADlg^.ClassOf(class_TDlgLaunch) then begin
			FFileName^.SetCaption(PDlgLaunch(ADlg)^.FProc^.FCurrent^.GetNameID);
			FCmdLine^.SetCaption(PLaunchItem(PDlgLaunch(ADlg)^.FProc^.FCurrent)^.FOptions);			
		end else 
			if ADlg^.ClassOf(class_TFileDialog) then
				FFileName^.SetCaption(PFileDialog(ADlg)^.GetFileName);
	end;
end;
{$ENDIF}
