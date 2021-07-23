{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QErrors; { QuickCrt Errors }
{$I QCRT.DEF}

interface

uses QCrt;

{

Note that when QErrors is used. It will automatically clear the screen and display 
and error message if the program exits with an error code. if the errorcode is 
greater than $F000 then no message is displayed and it is reduced by $F000 before exit.

}

const
	{ Standard Error Codes }
	erNone								   = 0;
	erNo_Error                             = erNone;
	erInvalid_Function_Number              = 1;
	erFile_Not_Found                       = 2;
	erPath_Not_Found                       = 3;
	erToo_Many_Open_Files                  = 4;
	erFile_Access_Denied                   = 5;
	erInvalid_File_Handle                  = 6;
	erInsufficient_Memory                  = 8;
	erInvalid_File_Access_Code             = 12;
	erInvalid_Drive_Number                 = 15;
	erCannot_Remove_Current_Directory      = 16;
	erCannot_Rename_Across_Drives          = 17;
	erNo_More_Files                        = 18;
	erDisk_Read_Error                      = 100;
	erDisk_Write_Error                     = 101;
	erFile_Not_Assigned                    = 102;
	erFile_Not_Open                        = 103;
	erFile_Not_Open_For_Input              = 104;
	erFile_Not_Open_For_Output             = 105;
	erInvalid_Numeric_Format               = 106;
	erDisk_Is_Writeprotected               = 150;
	erBad_Drive_Request_Struct_Length      = 151;
	erDrive_Not_Ready                      = 152;
	erCRC_Error_In_Data                    = 154;
	erDisk_Seek_Error                      = 156;
	erUnknown_Media_Type                   = 157;
	erSector_Not_Found                     = 158;
	erPrinter_Out_Of_Paper                 = 159;
	erDevice_Write_Fault                   = 160;
	erDevice_Read_Fault                    = 161;
	erHardware_Failure                     = 162;
	erDivision_By_Zero                     = 200;
	erRange_Check_Error                    = 201;
	erStack_Overflow                       = 202;
	erHeap_Overflow                        = 203;
	erInvalid_Pointer_Operation            = 204;
	erFloating_Point_Overflow              = 205;
	erFloating_Point_Underflow             = 206;
	erInvalid_Floating_Point_Operation     = 207;
	erOverlay_Manager_Not_Installed        = 208;
	erOverlay_File_Read_Error              = 209;
	erObject_Not_Initialized               = 210;
	erCall_To_Abstract_Method              = 211;
	erStream_Registration_Error            = 212;
	erCollection_Index_Out_Of_Range        = 213;
	erCollection_Overflow                  = 214;
	erArithmetic_Overflow                  = 215;
	erGeneral_Protection_Fault             = 216;

	{ OverlayErrors := OvrResult + erOverlay_Ofs }
	erOverlay_Ofs                          = 300;
	erOverlay_Ok                           = erOverlay_Ofs + 0;
	erOverlay_Error                        = erOverlay_Ofs + 1;
	erOverlay_Not_Found                    = erOverlay_Ofs + 2;
	erOverlay_No_Memory                    = erOverlay_Ofs + 3;
	erOverlay_IO_Error                     = erOverlay_Ofs + 4;
	erOverlay_No_EMS_Driver                = erOverlay_Ofs + 5;
	erOverlay_No_EMS_Memory                = erOverlay_Ofs + 6;
	erOverlay_No_XMS_Driver                = erOverlay_Ofs + 7;
	erOverlay_No_XMS_Memory                = erOverlay_Ofs + 8;

	{ BIOS ERROR CODES }
	erBad_Address_Mark                 = 1000;
	erReset_Failure                    = 1001; { erHardware_Failure; }
	erDMA_Failure                      = 1002;
	erDMA_Overrun                      = 1003;
	erBad_Track                        = 1004;
	erBad_CRC                          = 1005; { erCRC_Error_In_Data; }
	erData_Corrected                   = 1006;
	erController_Failure               = 1007; { erHardware_Failure; }
	erBad_Seek                         = 1008; { erDisk_Seek_Error; }
	erTime_Out                         = 1009;
	erSense_Failure                    = 1010;
	
	{ Custom Errors }		
	erParamError					= 2001;
	
const
	ErrorsOnExit : boolean = true;
	ErrorClrScr : boolean = true;

function ErrorStr(StrID : word) : String;

implementation

function ErrorStr(StrID : word) : String;
var
	S : String;
begin
	case StrId of
		0 : S := 'No error';
		1 : S := 'Invalid function number';
		2 : S := 'File not found';
		3 : S := 'Path not found';
		4 : S := 'Too many files open';
		5 : S := 'File access denied';
		6 : S := 'Invalid file handle';
		7 : S := 'User terminated';
		8 : S := 'Insufficient memory';
		9 : S := 'Mouse not found';
		10 : S := 'Internal error';
		12 : S := 'Invalid file access code';
		15 : S := 'Invalid drive number';
		16 : S := 'Cannot remove current directory';
		17 : S := 'Cannot rename across drives';

		19 : S := 'File already exists';

		100 : S := 'Disk read error';
		101 : S := 'Disk write error';
		102 : S := 'File not assigned';
		103 : S := 'File not open';
		104 : S := 'File not open for input';
		105 : S := 'File not open for output';
		106 : S := 'Invalid numeric format';
		149 : S := 'Disk full';
		150 : S := 'Disk is write-protected';
		151 : S := 'Unknown unit';
		152 : S := 'Drive not Ready';
		153 : S := 'Unknown command';
		154 : S := 'CRC error in data';
		155 : S := 'Bad drive request structure length';
		156 : S := 'Disk seek error';
		157 : S := 'Unknown media type';
		158 : S := 'Sector not found';
		159 : S := 'Printer out of paper';
		160 : S := 'Device write fault';
		161 : S := 'Device read fault';
		162 : S := 'Hardware failure';
		200 : S := 'Division by zero';
		201 : S := 'Range check error';
		202 : S := 'Stack overflow error';
		203 : S := 'Heap overflow error';
		204 : S := 'Invalid pointer operation';
		205 : S := 'Floating point overflow';
		206 : S := 'Floating point underflow';
		207 : S := 'Invalid floating point operation';
		208 : S := 'Overlay manager not installed';
		209 : S := 'Overlay file read error';
		210 : S := 'Object not initialized';
		211 : S := 'Call to abstract method';
		212 : S := 'Stream regestration error';
		213 : S := 'Collection index out of range';
		214 : S := 'Collection overflow error';

		erOverlay_Ok            : S := 'No overlay error';
		erOverlay_Error         : S := 'Overlay error';
		erOverlay_Not_Found     : S := 'Overlay file not found';
		erOverlay_No_Memory     : S := 'Insufficient memory for overlay';
		erOverlay_IO_Error      : S := 'Overlay I/O error';
		erOverlay_No_EMS_Driver : S := 'No EMS Driver installed for overlay';
		erOverlay_No_EMS_Memory : S := 'Insufficient EMS memory available for overlay';
		erOverlay_No_XMS_Driver : S := 'No XMS Driver installed for overlay';
		erOverlay_No_XMS_Memory : S := 'Insufficient XMS memory available for overlay';

		{XMS Errors}
		$180 : S := 'XMS function not implemented';
		$181 : S := 'VDISK device driver detected';
		$182 : S := 'A20 error';
		$18E : S := 'General XMM driver error';
		$18F : S := 'Unrecoverable XMM driver error';
		$190 : S := 'HMA does not exists';
		$191 : S := 'HMA already in use';
		$192 : S := 'Insufficient XMM size';
		$193 : S := 'HMA not allocated';
		$194 : S := 'A20 line still enabled';
		$1A0 : S := 'All XMS is allocated';
		$1A1 : S := 'XMS handles exhaused';
		$1A2 : S := 'Invalid XMS handle';
		$1A3 : S := 'Invalid XMS source handle';
		$1A4 : S := 'Invalid XMS source offset';
		$1A5 : S := 'Invalid XMS destination handle';
		$1A6 : S := 'Invalid XMS destination offset';
		$1A7 : S := 'Invalid XMS byte count';
		$1A8 : S := 'Invalid XMS overlap in move';
		$1A9 : S := 'XMS Parity Error';
		$1AA : S := 'XMS Block not locked';
		$1AB : S := 'XMS Block is locked';
		$1AC : S := 'XMS lock count overflow';
		$1AD : S := 'XMS lock failure';
		$1B0 : S := 'Smaller UMB available';
		$1B1 : S := 'No UMB''s available';
		$1B2 : S := 'Invalid UMB segment';
		
		erParamError : S := 'Invalid or missing command line parameter';

	else
		S := 'unknown error code';
	end;
	ErrorStr := S;
end;

var
	OldExitProc :  Pointer;

procedure ErrorExitProc; far;
begin
	ExitProc := OldExitProc;
	if not ErrorsOnExit then begin
		{$IFDEF TEMPLATES}
		if ExitCode <> 0 then begin
			WriteLn;
			WriteLn('MEMORY: ', MaxAvail, '/', MemAvail);
		end;

		{$ENDIF}
		exit;
	end;
	if ExitCode = 0 then exit;
	if ExitCode and $F000 = $F000 then
	begin
		ExitCode := ExitCode and $0FFF;
		exit;
	end;
	TextAttr := $07;
	if LastMode and CO80 <> CO80 then TextMode(CO80);
	if ErrorClrScr then 
		ClrScr
	else
		WriteLn;
	TextAttr := $4F;
	Write(' ERROR: #', ExitCode, ', "', ErrorStr(ExitCode), '" ');
	ClrEol;
	TextAttr := $07;
	WriteLn;
	WriteLn;
	WriteLn(' Sorry, but the application has suffered a severe error and has been terminated.');
	MoveCursor;		
end;


begin
	OldExitProc := ExitProc;
	ExitProc := @ErrorExitProc;
	{$IFDEF TEMPLATES}	
		ErrorsOnExit := False; 
	{$ENDIF}
end.

