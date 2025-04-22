#define MyAppName "Mica4U"
#define MyAppVersion "1.6.9"
#define MyAppPublisher "DRK"
#define MyAppURL "https://github.com/DRKCTRL/Mica4U"
#define MyAppExeName "Mica4U.exe"

; Check for Architecture definition from build script, default to x64
#ifndef Arch
  #define Arch "x64"
#endif

[Setup]
AppId={{2B04C122-1A7E-4BB8-95AB-E2C414D1742C}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=..\LICENSE
OutputDir=output
OutputBaseFilename=Mica4U_Setup_{#Arch}
SetupIconFile=..\icon.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
UninstallDisplayName={#MyAppName}
Compression=lzma
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
CloseApplications=yes
UninstallRestartComputer=no

; Set architecture based on build command
#if Arch == "x64"
  ArchitecturesAllowed=x64
  ArchitecturesInstallIn64BitMode=x64
#endif

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "dist\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs
Source: "..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\ExplorerBlurMica.dll"; DestDir: "{userappdata}\Mica4U"; Flags: ignoreversion
Source: "..\initialise.cmd"; DestDir: "{userappdata}\Mica4U"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Registry]
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{{2B04C122-1A7E-4BB8-95AB-E2C414D1742C}}"; ValueType: string; ValueName: "DisplayIcon"; ValueData: "{app}\{#MyAppExeName}"
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{{2B04C122-1A7E-4BB8-95AB-E2C414D1742C}}"; ValueType: string; ValueName: "Publisher"; ValueData: "{#MyAppPublisher}"
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{{2B04C122-1A7E-4BB8-95AB-E2C414D1742C}}"; ValueType: string; ValueName: "URLInfoAbout"; ValueData: "{#MyAppURL}"
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{{2B04C122-1A7E-4BB8-95AB-E2C414D1742C}}"; ValueType: string; ValueName: "DisplayVersion"; ValueData: "{#MyAppVersion}"

[UninstallRun]
; First unregister the DLL
Filename: "{userappdata}\Mica4U\initialise.cmd"; Parameters: "uninstall"; RunOnceId: "UnregisterDLL"; Flags: runhidden

[UninstallDelete]
Type: files; Name: "{app}\*.*"
Type: dirifempty; Name: "{app}"
Type: files; Name: "{group}\*.*"
Type: dirifempty; Name: "{group}"

[Code]
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
    mRes : integer;
    appDataPath: string;
begin
    case CurUninstallStep of
        usUninstall:
        begin
            mRes := MsgBox('Do you want to remove all settings and files? Click Yes to remove everything, No to keep settings.', 
                          mbConfirmation, MB_YESNO or MB_DEFBUTTON2);
            if mRes = IDYES then
            begin
                appDataPath := ExpandConstant('{userappdata}\Mica4U');
                DelTree(appDataPath, True, True, True);
            end;
        end;
        usPostUninstall:
        begin
            // Clean up Start Menu folder if empty
            DelTree(ExpandConstant('{group}'), False, True, True);
        end;
    end;
end;

[InstallDelete]
Type: files; Name: "{app}\*.*"
Type: dirifempty; Name: "{app}" 
