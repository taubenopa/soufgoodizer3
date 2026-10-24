; Inno Setup Script fuer den Soufgoodizer
; Ergebnis: installer\output\soufinstaller.exe

#define AppName    "Soufgoodizer"
#define AppVersion "1.0.0"
#define VstSource  "build\Soufgoodizer_artefacts\Release\VST3\Soufgoodizer.vst3"

[Setup]
AppId={{B6D1E2A4-5F0C-4C7B-9A53-50F0F600D1AE}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher=Souf
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
DisableReadyPage=no
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir=output
OutputBaseFilename=soufinstaller
SetupIconFile=soufinstaller.ico
UninstallDisplayIcon={commonappdata}\{#AppName}\soufinstaller.ico
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "german";  MessagesFile: "compiler:Languages\German.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#VstSource}\*"; DestDir: "{commoncf64}\VST3\{#AppName}.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "soufinstaller.ico"; DestDir: "{commonappdata}\{#AppName}"; Flags: ignoreversion

[UninstallDelete]
Type: filesandordirs; Name: "{commoncf64}\VST3\{#AppName}.vst3"
Type: filesandordirs; Name: "{commonappdata}\{#AppName}"
