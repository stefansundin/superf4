/*
	SuperF4 - en-US localization by Stefan Sundin (recover89@gmail.com)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Note: For some reason &A will not work as a shortcut.
*/

struct strings en_US = {
 /* === translation info === */
 /* code               */ L"en-US",
 /* lang_english       */ L"English",
 /* lang               */ L"English",
 /* author             */ L"Stefan Sundin",

 /* === app === */
 /* tray_enabled       */ APP_NAME,
 /* tray_disabled      */ APP_NAME " (disabled)",
 { /* menu */
   /* enable           */ L"Enable",
   /* disable          */ L"Disable",
   /* hide             */ L"Hide tray",
   /* update           */ L"Update available!",
   /* config           */ L"Configure",
   /* about            */ L"About",
   /* exit             */ L"Exit",
 },
 { /* update */
   /* balloon          */ L"New version found!",
   /* dialog           */ L"A new version is available. Go to website?",
   /* nonew            */ L"No update available.",
 },

 /* === config === */
 /* title              */ APP_NAME L" Configuration",
 { /* tabs */
   /* general          */ L"General",
   /* advanced         */ L"Advanced",
   /* about            */ L"About",
 },
 { /* general tab */
   /* box              */ L"General settings",
   /* language         */ L"Language:",
   /* autostart_box    */ L"Autostart",
   /* autostart        */ L"S&tart "APP_NAME" when logging on",
   /* autostart_hide   */ L"&Hide tray",
   /* autostart_elevate*/ L"&Elevate to administrator privileges",
   /*       elevate_tip*/ L"Note that a UAC prompt will appear every time you log in, unless you disable UAC completely.",
   /* elevate          */ L"E&levate",
   /* elevated         */ L"Elevated",
   /* elevate_tip      */ L"This will create a new instance of "APP_NAME" which is running with administrator privileges. This allows "APP_NAME" to manage other programs which are running with administrator privileges.\n\nYou will have to approve a UAC prompt from Windows to allow "APP_NAME" to run with administrator privileges.",
   /* elevation_aborted*/ L"Elevation aborted.",
   /* autosave         */ L"Note: Settings are saved and applied instantly!",
 },
 { /* advanced tab */
   /* box              */ L"Advanced settings",
   /* checkonstartup   */ L"A&utomatically check for updates",
   /* beta             */ L"Check for &beta versions",
   /* checknow         */ L"&Check now",
   /* ini              */ L"The settings are saved in "APP_NAME".ini. There are a few things that you can only configure by editing the file manually.",
   /* openini          */ L"Open &ini file",
 },
 { /* about tab */
   /* box              */ L"About "APP_NAME,
   /* version          */ L"Version "APP_VERSION,
   /* author           */ L"Created by Stefan Sundin",
   /* license          */ APP_NAME L" is free and open source software!\nFeel free to redistribute!",
   /* donate           */ L"&Donate",
   /*translation_credit*/ L"Translation credit",
 },

 /* === misc === */
 /* unhook_error       */ L"There was an error disabling "APP_NAME". This was most likely caused by Windows having already disabled "APP_NAME"'s hooks.\n\nIf this is the first time this has happened, you can safely ignore it and continue using "APP_NAME".\n\nIf this is happening repeatedly, you can read on the website wiki how to prevent this from happening again (look for '"APP_NAME" mysteriously stops working' on the About page).",
};
