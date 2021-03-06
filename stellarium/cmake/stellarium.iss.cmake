; Stellarium installer
; Run "make install" first to generate the executable and translation files.
; @ISS_AUTOGENERATED_WARNING@

[Setup]
@ISS_ARCHITECTURE_SPECIFIC@
DisableStartupPrompt=yes
DisableDirPage=no
DisableProgramGroupPage=no
WizardSmallImageFile=@CMAKE_SOURCE_DIR@\data\icon.bmp
WizardImageFile=@CMAKE_SOURCE_DIR@\data\splash.bmp
WizardImageStretch=no
WizardImageBackColor=clBlack

AppName=Stellarium-PlanetC
AppVersion=@PLANETC_PLUGIN_VERSION@
AppVerName=Stellarium-PlanetC @PLANETC_PLUGIN_VERSION@
AppCopyright=Copyright (C) 2016-2020 Giorgio Calderone
AppPublisher=Giorgio Calderone
AppPublisherURL=https://github.com/gcalderone/PlanetC
AppSupportURL=https://github.com/gcalderone/PlanetC
AppUpdatesURL=https://github.com/gcalderone/PlanetC
VersionInfoVersion=@PLANETC_PLUGIN_VERSION@
MinVersion=6
SetupIconFile=@CMAKE_SOURCE_DIR@\data\@PACKAGE_ICON@.ico
OutputBaseFilename=PlanetC-v@PLANETC_PLUGIN_VERSION@-@PACKAGE_VERSION@-@ISS_PACKAGE_PLATFORM@
OutputDir=@CMAKE_SOURCE_DIR@\installers
; In 64-bit mode, {pf} is equivalent to {pf64},
; see http://www.jrsoftware.org/ishelp/index.php?topic=32vs64bitinstalls
DefaultDirName={pf}\PlanetC
DefaultGroupName=PlanetC
UninstallDisplayIcon={app}\data\stellarium.ico
LicenseFile=@CMAKE_SOURCE_DIR@\COPYING
ChangesAssociations=yes
; LZMA2/max required 95 MB RAM for compression and 8 MB RAM for decompression
; Using LZMA2/max algorithm reduces size of package on ~10%
Compression=lzma2/max

[Files]
Source: "@CMAKE_INSTALL_PREFIX@\bin\stellarium.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "@CMAKE_INSTALL_PREFIX@\bin\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "@CMAKE_SOURCE_DIR@\data\stellarium.url"; DestDir: "{app}"; Flags: ignoreversion
Source: "@CMAKE_SOURCE_DIR@\data\stellarium-devdocs.url"; DestDir: "{app}"; Flags: ignoreversion
Source: "README-PLANETC"; DestDir: "{app}"; Flags: isreadme; DestName: "README-PLANETC.txt"
; Source: "@CMAKE_SOURCE_DIR@\README.md"; DestDir: "{app}"; Flags: isreadme ignoreversion; DestName: "README.txt"
; Source: "@CMAKE_SOURCE_DIR@\INSTALL"; DestDir: "{app}"; Flags: ignoreversion; DestName: "INSTALL.txt"
Source: "@CMAKE_SOURCE_DIR@\COPYING"; DestDir: "{app}"; Flags: ignoreversion; DestName: "GPL.txt"
; Source: "@CMAKE_SOURCE_DIR@\AUTHORS"; DestDir: "{app}"; Flags: ignoreversion; DestName: "AUTHORS.txt"
Source: "@CMAKE_SOURCE_DIR@\ChangeLog"; DestDir: "{app}"; Flags: ignoreversion; DestName: "ChangeLog.txt"
; Qt5 stuff
Source: "@CMAKE_INSTALL_PREFIX@\qt5stuff\*"; DestDir: "{app}\"; Flags: recursesubdirs ignoreversion
; Stellarium's stuff
Source: "@CMAKE_INSTALL_PREFIX@\share\stellarium\*"; DestDir: "{app}\"; Flags: recursesubdirs ignoreversion
@ISS_OPENSSL_LIBS@
@ISS_STELLARIUM_STUFF@

[Tasks]
Name: desktopicon; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: desktopicon\common; Description: "{cm:ForAllUsers}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: exclusive
Name: desktopicon\user; Description: "{cm:ForCurrentUserOnly}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: exclusive unchecked
Name: removecache; Description: "{cm:RemoveCache}"; GroupDescription: "{cm:RemoveFromPreviousInstallation}"
Name: removeconfig; Description: "{cm:RemoveMainConfig}"; GroupDescription: "{cm:RemoveFromPreviousInstallation}"
Name: removeplugins; Description: "{cm:RemovePluginsConfig}"; GroupDescription: "{cm:RemoveFromPreviousInstallation}"; Flags: unchecked
Name: removesolar; Description: "{cm:RemoveSolarConfig}"; GroupDescription: "{cm:RemoveFromPreviousInstallation}"

[Run]
; An option to start Stellarium after setup has finished
Filename: "{app}\stellarium.exe"; Description: "{cm:LaunchProgram,Stellarium}"; Flags: postinstall nowait skipifsilent unchecked

[InstallDelete]
;The old log file in all cases
Type: files; Name: "{userappdata}\PlanetC\log.txt"
Type: files; Name: "{userappdata}\PlanetC\config.ini"; Tasks: removeconfig
Type: files; Name: "{userappdata}\PlanetC\data\ssystem_minor.ini"; Tasks: removesolar
Type: filesandordirs; Name: "{userappdata}\PlanetC\modules"; Tasks: removeplugins
Type: filesandordirs; Name: "{localappdata}\stellarium\stellarium"; Tasks: removecache

[UninstallDelete]

[Icons]
Name: "{group}\{cm:ProgramOnTheWeb,Stellarium}"; Filename: "{app}\stellarium.url"; IconFilename: "{app}\data\stellarium.ico"
Name: "{group}\{cm:DevelopersDocsOnTheWeb}"; Filename: "{app}\stellarium-devdocs.url"; IconFilename: "{app}\data\stellarium.ico"
Name: "{group}\PlanetC"; Filename: "{app}\stellarium.exe"; Parameters: "-platform windows:altgr"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"
; Name: "{group}\PlanetC {cm:FallbackMode}"; Filename: "{app}\stellarium.exe"; Parameters: "--safe-mode -platform windows:altgr"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"
Name: "{group}\PlanetC {cm:DebugMode}"; Filename: "{app}\stellarium.exe"; Parameters: "--dump-opengl-details -platform windows:altgr"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"
; Name: "{group}\PlanetC {cm:AngleMode}"; Filename: "{app}\stellarium.exe"; Parameters: "--angle-mode -platform windows:altgr"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"
; Name: "{group}\PlanetC {cm:AngleD3D9Mode}"; Filename: "{app}\stellarium.exe"; Parameters: "--angle-d3d9 -platform windows:altgr"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"
Name: "{group}\PlanetC {cm:AngleD3D11Mode}"; Filename: "{app}\stellarium.exe"; Parameters: "--angle-d3d11 -platform windows:altgr"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"
; Name: "{group}\PlanetC {cm:AngleWarpMode}"; Filename: "{app}\stellarium.exe"; Parameters: "--angle-warp -platform windows:altgr"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"
Name: "{group}\PlanetC {cm:AngleMode}"; Filename: "{app}\stellarium.exe"; Parameters: "--angle-d3d9 -platform windows:altgr"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"
Name: "{group}\PlanetC {cm:MesaMode}"; Filename: "{app}\stellarium.exe"; Parameters: "--mesa-mode -platform windows:altgr"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"
@ISS_SPOUT@
Name: "{group}\{cm:UninstallProgram,PlanetC}"; Filename: "{uninstallexe}"
Name: "{group}\config.ini"; Filename: "{userappdata}\PlanetC\config.ini"
Name: "{group}\{cm:LastRunLog}"; Filename: "{userappdata}\PlanetC\log.txt"
Name: "{group}\{cm:OutputDataFile}"; Filename: "{userappdata}\PlanetC\output.txt"
Name: "{group}\{cm:ChangeLog}"; Filename: "{app}\ChangeLog.txt"
@ISS_GUIDE@
Name: "{commondesktop}\PlanetC"; Filename: "{app}\stellarium.exe"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"; Tasks: desktopicon\common
Name: "{userdesktop}\PlanetC"; Filename: "{app}\stellarium.exe"; WorkingDir: "{app}"; IconFilename: "{app}\data\stellarium.ico"; Tasks: desktopicon\user

[Registry]
; Set file associations for Stellarium scripts
Root: HKCR; Subkey: ".ssc"; ValueType: string; ValueName: ""; ValueData: "Stellarium.Script"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Stellarium.Script"; ValueType: string; ValueName: ""; ValueData: "Stellarium Script"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Stellarium.Script\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\stellarium.exe,0"
Root: HKCR; Subkey: "Stellarium.Script\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\stellarium.exe"" --startup-script ""%1"""

; Recommended use Inno Setup 5.5.3+
[Languages]
; Official translations of GUI of Inno Setup + translation Stellarium specific lines
Name: "en"; MessagesFile: "compiler:Default.isl,@CMAKE_SOURCE_DIR@\util\ISL\EnglishCM.isl"

[Code]
procedure CurUninstallStepChanged (CurUninstallStep: TUninstallStep);
var
  mres : integer;
begin
  case CurUninstallStep of
    usPostUninstall:
      begin
        mres := MsgBox(ExpandConstant('{cm:DeleteUserData}'), mbConfirmation, MB_YESNO or MB_DEFBUTTON2)
        if mres = IDYES then
          DelTree(ExpandConstant('{userappdata}\PlanetC'), True, True, True);
          DelTree(ExpandConstant('{userdocs}\PlanetC'), True, True, True);
          DelTree(ExpandConstant('{localappdata}\stellarium'), True, True, True);
      end;  
  end;
end;
