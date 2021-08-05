{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{ --- TDlgAbout --- }

{$IFDEF INTERFACE}
const
	class_TDlgAbout = 'TDlgAbout';

type
	PDlgAbout = ^TDlgAbout;
	TDlgAbout = object (TMessageBox)
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl);
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TDlgAbout.ObjectClass(AName : String) : String;
begin
	if (AName = '') or (AName = class_TDlgAbout) then
		ObjectClass := class_TDlgAbout
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TDlgAbout.Create;
{const
	ColorOK = $2E;}
var
	B : TBounds;
begin
	inherited Create(AParent, 'DIALOG.ABOUT');
	Bounds(1,1,50,16, B);
	SetStyle(bxSingle);
	SetBounds(B);
	FAutoEndModal := True;
	SetMessage(' ');
	AppendMessage('The Program Manager Eternity Edition');
	AppendMessage('Copyright 2015-' + IntStr(Version.Year) + '.');
	AppendMessage(' ');
	AppendMessage('The QuickCRT Framework');
	AppendMessage('Copyright 1990-' + IntStr(Version.Year)  + '.');
	AppendMessage(' ');
	AppendMessage('Both PGME and QuickCRT are opensource and');
	AppendMessage('can be found on sourceforge at:');
	AppendMessage(' ');
	AppendMessage(Version.URL);
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
{$ENDIF}
