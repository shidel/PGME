Copyright 1992-2022, Jerome Shidel.
<br />
GNU General Public License, version 2.0

## The Program Manager Eternity

This version of **_Program Manager Eternity_** and related files are released under the GNU GPL v2.0.

## Contributing language translations, document corrections, themes, etc.

Please do not submit language translations, menu updates, document revisions,
font or theme files directly to this project. Those files *(if any)* that may
be in this source tree are **NOT** included in the software releases. If any
exist in this project, they are for my development purposes only. When a
version or release of this project done, all those files are pulled directly
from their actual home, the [PGME](https://github.com/shidel/fd-nls/pgme)
directory in the [FD-NLS](https://github.com/shidel/fd-nls) translations
project.

If you wish to provide an additional translations, make corrections to an
existing files, provide new fonts, etc. please issue a pull request or submit
them to the [FD-NLS](https://github.com/shidel/fd-nls) translations project.## What's Up

## Updates, changes and improvements

### Next release

### 2022-08-21

* Reduced mouse flicker a little. (getting rid of it completely will require changing how the QCrt unit handles it. More work than I want to do now)
* Improved compatibility with non-CTMouse drivers. (Like MS Mouse Driver)
* Found issue causing double keypresses, removed work-around. Returned flag from interrupt 15 hook was not being set/cleared.

### 2022-02-08

* Work-around for double keypresses under some VMs when internation keyboard driver is loaded.
* Fix label for Sound effects toggle using wrong component id.
* Build script improvements and fixes.
* Including updated and additional language translations, fonts and themes.

### 2021-08-20

* Added Window Close characters to themes
* Corrected Config for correct French font filename
* French & Turkish Language basic rough themes
* Some font improvements
* Added delay to font preview in Font Designer for easier viewing.
* Time Screen saver now remembers scale setting.
* QScroll now remembers Smooth scroll settings.
* Fixed new typing in find box goes behind clock UI bug. (minor, self corrects)
* QResFile (and format) updated to support using UPX on EXE files at any time. Now not limited attaching files only after compression.

### 2021-08-15

* Added U keyboard option to QScroll to toggle Smooth upward scrolling.
* Added S keyboard option to TIME Screen Saver to change display scale.
* Added support to uninstall using FreeDOS package managers.
* Changed installer to use QResFile binrary resources.
* Add Support to UPX compress binaries in build process.
* New QResFile, developer oriented executable resource management utility. It replaces the clunky ADD2INS program used in the build process.
* Support to autohide menus without any visible programs.
* Automatic language/font/theme/keymap switching in QCrt based apps (PGME, EFontDesigner and Installer) when defaults are empty.
* New Clock, Conway and Static screen savers.
* Removed Extra spacing in alternate language help text
* Added support to scroll to item in unsorted lists (for the dropdowns)
* Improved startup message (beginning/resuming/updating) display and duration
* Watch for Package (Un)Installs under FreeDOS, improved Auto-Hide.
* Installer ignore drive Z: under DOSBox.
* Auto disable QSCROLL smooth scrolling in common non-DOSBox Virtual Machines
* Added ability to auto-hide uninstalled/missing programs.
* Ignore CPUCache setting in DOSBOX.
* Added suport to use CPUCACHE to disable/enable CPU caching for old games.
* Added CPU Idle/power saving option to Config Dialog.
* Added "<none>" option to Config and Display Dialogs for File based options.
* Fix display options only save when theme was changed bug.
* Mouse flicker reduced some.
* Added Config Dialog option to turn sound on/off
* Added config file option to save power, CPU_IDLE=ANY, APM, HALT or NONE
* Added wait on return for individual programs, WAIT=OFF, FOREVER, DEFAULT or time in seconds.
* Added wait on return default per menu global WAIT=OFF, FOREVER or seconds.
* Assigned Edit Menu popup tp EditItem Command (default Alt+E).
* Changed Edit button mapping from EditFile to EditItem
* Create menu with wildcard character, bug fix.
* Some grammer and spelling corrections.

### 2015-07-26

* Fixed non-FreeDOS/DosBox "Path Not Found" compatibility issue.
* Some grammer and spelling corrections.

### 2015-07-09

* Added Whatsnew.txt (this file)
* QScroll Added Line to separate multiple files when loading more than one.
* QScroll Added command line switch /S- to set smooth scrolling to off. (/S and /S+ turn it on which is the default for now);
* QScroll Pressing "S" key toggles smooth scrolling.
* QScroll Exit on mouse click.
* QScroll Scroll by mouse movement.
* Improved compatibility for keyboards without insert lock key.

### 2015-07-05

* Initial Beta Version. Mostly Functional.

## Building PGME from Source

Simple Instructions for compiling PGME from archive sources.

	First you must have Turbo Pascal 7.0.

	Then either download the latest archive of PGME.

	Extract the PGME archive.

	then from a dos prommpt run make in the pgme directory.

	If all goes well, an Install directory will be created with the installer.

Instructions on compiling from svn sources.

	You still need turbo pascal 7.0.

	svn checkout svn://svn.code.sf.net/p/pgme/svn/trunk pgme
	(or from [github](https://github.com/shidel/PGME))

    Then from a dos prommpt run *build* in the pgme directory.

A QuickCRT snapshot is now included in the main PGME repository tree. This
snapshot is not kept in sync with the main version of QuickCRT. Rather
it should be considered a fork of that project. At present there are a couple
"living" forks of QuickCRT. There is this branch that was derived from
v8.4 and built-out with a object oriented application framework. Then there
is the previous 8.4 version without the OOP framework available at:

    svn checkout svn://svn.code.sf.net/p/quickcrt/svn/trunk pgme/quickcrt

Then there is the newer version 9.x without an OOP framework. But was being
migrated to support assembly only projects as well as Pascal. It is at

    https://github.com/LoopZ/QCrt

None of those living versions are not actually "better" than any of the others.
They are just different from each other with different purposes.

