{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QControls; { QuickCRT Objects Controls}
{$I QCRT.DEF}

interface

{$DEFINE INTERFACE}

uses 
   {$IFDEF DEVLOG} QLog, {$ENDIF} 
	QCrt, QStrings, QInfo, QObjects, QClass;
		
{$I TControl.inc}
{$I TLabel.inc}
{$I TPanel.inc}
{$I TLine.inc}
{$I TBox.inc}
{$I TButton.inc}
{$I TWindow.inc}

{$I TDialog.inc}
{$I TMessageBox.inc}
{$I TProgressBar.inc}
{$I TScrollBar.inc}
{$I TSlider.inc}
{$I TListView.inc}
{$I TTextView.inc}
{$I TCheckBox.inc}
{$I TRadioButton.inc}
{$I TToggle.inc}
{$I TEdit.inc}
{$I TPopUp.inc}
{$I TPopUpList.inc}
{$I TDropList.inc}
{$I TMemo.inc}
{$I THelp.inc}

{$I TApplication.inc}
	
var
	Application : PApplication; { First Created TApplication object }
			
	{ Procedure to Set a Bounds Record }
	procedure Bounds(ALeft, ATop, AWidth, AHeight : integer; var AValue : TBounds);
		
{$UNDEF INTERFACE}

implementation

{$DEFINE IMPLEMENTATION}

uses QErrors, QBIOS, QDos, QFiles, QFmtStr, QSpeaker;

	var
		OldExitProc : pointer;
		OldIdleProc : procedure;
		SavedSettings : TQCrtSettings;

{$I DCOMMENTS.INC}
{$I DKEYNAME.INC}
{$I DKEYMAPS.INC}
{$I DCOMMANDS.INC}

	procedure IdleApplication; far;
	begin
		OldIdleProc;
		if Assigned(Application) then Application^.Idle;
	end;

	procedure Bounds(ALeft, ATop, AWidth, AHeight : integer; var AValue : TBounds);
	begin
		AValue.Left := ALeft;
		AValue.Top := ATop;
		AValue.Width := AWidth;
		AValue.Height := AHeight;
	end;
	
	procedure ExitQControls; far;
	begin
		Application := nil;
		IdleProc := OldIdleProc;
		ExitProc := OldExitProc;
	end;
	
	procedure InitQControls;
	begin		
		Application := nil;
		OldExitProc := ExitProc;
		ExitProc := @ExitQControls;
		OldIdleProc := IdleProc;
	end;
	
{$I TControl.inc}
{$I TLabel.inc}	
{$I TPanel.inc}
{$I TLine.inc}
{$I TBox.inc}
{$I TButton.inc}
{$I TWindow.inc}

{$I TDialog.inc}
{$I TMessageBox.inc}
{$I TProgressBar.inc}
{$I TScrollBar.inc}
{$I TSlider.inc}
{$I TListView.inc}
{$I TTextView.inc}
{$I TCheckBox.inc}
{$I TRadioButton.inc}
{$I TToggle.inc}
{$I TEdit.inc}
{$I TPopUp.inc}
{$I TPopUpList.inc}
{$I TDropList.inc}
{$I TMemo.inc}
{$I THelp.inc}

{$I TApplication.inc}

{$UNDEF IMPLEMENTATION}
begin
	InitQControls;
end.
