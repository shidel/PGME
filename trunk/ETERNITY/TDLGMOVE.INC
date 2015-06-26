{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TDlgMove --- }

{$IFDEF INTERFACE}
const
	class_TDlgMove = 'TDlgMove';

type 
	PDlgMove = ^TDlgMove;
	TDlgMove = object (TMessageBox)
    public { protected }
{    	FOK : PButton;}
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
function TDlgMove.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TDlgMove) then 
		ObjectClass := class_TDlgMove
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgMove.Create;
{const
	ColorOK = $2E;}
var
	B : TBounds;
begin	
	inherited Create(AParent, 'DIALOG.MOVE');
	Bounds(1,1,50,16, B);
	SetStyle(bxSingle);
	SetBounds(B);
	FAutoEndModal := True;
	SetMessage(' ');
	AppendMessage('The Program Manager Eternity Edition');
	AppendMessage('Copyright 2015.');
	AppendMessage(' ');
	AppendMessage('The QuickCRT Framework');
	AppendMessage('Copyright 1990-2015.');
	AppendMessage(' ');
	AppendMessage('Both PGME and QuickCRT are opensource and');
	AppendMessage('can be found on sourceforge at:');	
	AppendMessage(' ');
	AppendMessage('http://pgme.sourceforge.net');
	AppendMessage('http://quickcrt.sourceforge.net');
	SetTitle('About PGM Eternity');
	SetHelpText('The "About" dialog box.');
	AddHelpText('');
	AddHelpText('This is dialog box provides some information regarding PGME.');
	AddHelpText('Additional information is available in the BLAHBLAH.TXT file');
	AddHelpText('and online at http://pgme.sourceforge.net');
{	TextAttr := ColorOK;
	FOK := New(PButton, Create(@Self, 'BUTTON.OK'));
	with PButton(FOK)^ do begin
		Bounds(B.Left + (B.Width - 5) div 2 - 6 , B.Height - 4, 14, 1, B);
		SetBounds(B);
		SetAnchors(abLeft or abRight or abBottom);
		SetCaption('OK');
		SetCommand(cmOK);		
		SetHelpText('5;Use the "%0" button to close the dialog box.');
	end;}
end;

destructor TDlgMove.Destroy;
begin
	inherited Destroy;
end;

procedure TDlgMove.CheckLocal(var AEvent : TEvent);
begin
	inherited CheckLocal(AEvent);
end;

procedure TDlgMove.BeforeShow;
begin
	inherited BeforeShow;
end;
{$ENDIF}
