Copyright 1992-2015, Jerome Shidel.

### QuickCrt 

**_QCRT.DEF_** QuickCrt Turbo Pascal 7.0 default compiler directive definition file.

**_QCRT.PAS_** QuickCrt. The Ultimate Text mode CRT unit. It can function as a drop-in 
replacement for the buggy Borland CRT unit. But, it is so much more and many times faster. 
However, it is mostly assembly language wrapped in Pascal. So, it is not very portable to 
different compilers or platforms. It requires none of the other QuickCrt units. But,
most of them are built on-top of QCRT.PAS.

**_QSTRINGS.PAS_** String Utilities. Simple and commonly used string manipulation 
routines.

**_QCLASS.PAS_** Contains the basic object classes that all QuickCRT objects inherit.

**_QOBJECTS.PAS_** QuickCrt Non-visual Objects. Running on the extended functionality 
built into QuickCrt, it brings a little RAD to Pascal. It is not a Turbo Vision clone 
and doesn't want to be one. It is a Pascal native object-oriented and event driven 
framework for building programs quickly and without to much fuss.

**_QMORE.PAS_** More QuickCrt Objects.

**_QFILEDLG.PAS_** Standard File and Directory Dialogs.

**_QCONTROLS.PAS_** QuickCrt Standard Visual Objects

**_QERRORS.PAS_** Simple Error Messages. Simple Error Message Lookup functions.
I may move this into QStrings. It doesn't really deserve it's own unit. 

**_QBIOS.PAS_** BIOS utilities.

**_QDOS.PAS_** Dos Utilities.

**_QFILES.PAS_** Object-Oriented file I/O.

**_QFMTSTR.PAS_** String Formatting Utilities.

**_QINFO.PAS_** QuickCrt Information Object.

**_QSCRNSVR.PAS_** Basic Screen Saver Template Object.

**_QSPEAKER.PAS_** Event Driven Speaker Tone Generator.

**_QTIMER.PAS_** Event Driven Timer Functions.

**_QTEST.PAS_** Debugging and development testing application.

