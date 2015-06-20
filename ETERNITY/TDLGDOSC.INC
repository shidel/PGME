{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgDOSCommand --- }

{$IFDEF INTERFACE}
const
	class_TDlgDOSCommand = 'TDlgDOSCommand';

type 
	PDlgDOSCommand = ^TDlgDOSCommand;
	TDlgDOSCommand = object (TDialog)
    public { protected }
    	FOK : PButton;
    	FCmdLine : PEdit;
		function  ObjectClass ( AName : String ) : String; virtual;
    public { protected }
		procedure CheckLocal(var AEvent : TEvent); virtual;
		procedure BeforeShow; virtual;
	public
		constructor Create(AParent : PControl);
		destructor Destroy; virtual;
	end;   

{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgDOSCommand.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgDOSCommand) then 
		ObjectClass := class_TDlgDOSCommand
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgDOSCommand.Create;
const
	ColorLabel = $47;
	ColorEdit = $0E;
	ColorOK = $2E;
var
	B, T : TBounds;
begin
	TextAttr := ColorLabel;
	inherited Create(AParent, 'DIALOG.DOSCOMMAND');
	SetBackGround(ColorLabel shr 4);
	Bounds(1,1,55,10, B);
	FAutoEndModal := True;
	SetStyle(bxSingle);
	SetBounds(B);
	SetTitle('Add DOS Command');
	SetHelpText('The "DOS Command" dialog box.');
	AddHelpText('');
	AddHelpText('This is where you manually enter a DOS command to the program');
	AddHelpText('execution process.');
	TextAttr := ColorLabel;
	Bounds(3,2,B.Width - 8, 1, T);
	with PLabel(New(PLabel, Create(@Self, 'LABEL')))^ do begin
		SetBounds(T);
		SetAnchors(abLeft or abRight or abTop);
		SetCaption('Custom DOS Command:');
		FHAlign := AlignLeft;
	end;
	Inc(T.Top, 1);
	TextAttr := ColorEdit;
	FCmdLine := New(PEdit, Create(@Self, 'EDIT'));
	with FCmdLine^ do begin
		SetBounds(T);
		SetAnchors(abLeft or abRight or abBottom);
		SetCaption('');		
	end;
	
	TextAttr := ColorOK;
	FOK := New(PButton, Create(@Self, 'BUTTON.OK'));
	with PButton(FOK)^ do begin
		Bounds(B.Left + (B.Width - 5) div 2 - 6 , B.Height - 4, 14, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abRight or abBottom);
		SetCaption('OK');
		SetCommand(cmOK);		
		SetHelpText('5;Use the "%0" button to add the DOS command');
		AddHelpText('and close the dialog box.');
	end;
end;

destructor TDlgDOSCommand.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgDOSCommand.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgDOSCommand.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
