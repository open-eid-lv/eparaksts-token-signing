; -- eParaksts Token Signing --
; x86 and x64 using a single installer

#define ProductName "eParaksts Token Signing"
#define ProductVersion "1.0.2"
#define GeneratedDir "./"
;#define GeneratedDir "./../../generated"
#define VcInstallDir "C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC"

[Setup]
; AppId is required for uninstall, key = '{E463B1A1-0331-4358-9CA5-532FF83AE151}_is1'
AppId={{E463B1A1-0331-4358-9CA5-532FF83AE151}
AppName={#ProductName}
AppVersion={#ProductVersion}
AppPublisher=EUSO
AppCopyright=Copyright (c) 2015 EUSO
VersionInfoVersion={#ProductVersion}
DefaultDirName={pf}\{#ProductName}
DefaultGroupName={#ProductName}
SetupIconFile={#GeneratedDir}/eparaksts.ico
LicenseFile={#GeneratedDir}/license.lgpl.rtf
ChangesEnvironment=yes
PrivilegesRequired=admin
UninstallDisplayIcon={uninstallexe}
Compression=lzma2
SolidCompression=yes
SignTool=SignTool
SignedUninstaller=yes
OutputDir={#GeneratedDir}
OutputBaseFilename=eparaksts-token-signing-{#ProductVersion}-setup
; MinVersion: Windows XP
MinVersion=5.1
; ArchitecturesInstallIn64BitMode: always "32-bit mode" (blank)
ArchitecturesInstallIn64BitMode=
; ArchitecturesAllowed: x86 and x64
ArchitecturesAllowed=x86 x64

[Files]
; PluginIE x64
Source: "{#GeneratedDir}/eparaksts-plugin/ie/x64/Release/eparaksts-plugin-ie.dll"; DestDir: "{app}/x64"; Check: IsWin64; Flags: 64bit ignoreversion sharedfile regserver

; PluginIE x86
Source: "{#GeneratedDir}/eparaksts-plugin/ie/Release/eparaksts-plugin-ie.dll"; DestDir: "{app}"; Flags: 32bit ignoreversion sharedfile regserver

; PluginNP
Source: "{#GeneratedDir}/eparaksts-plugin/np-eparaksts-plugin.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#GeneratedDir}/eparaksts-plugin/libeparaksts-module.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\OpenSSL-Win32\bin\libeay32.dll"; DestDir: "{app}"; Flags: ignoreversion

; ExtensionChrome
Source: "{#GeneratedDir}/eparaksts-chrome-extension/host-windows/Release/eparaksts-chrome-extension.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#GeneratedDir}/eparaksts-chrome-extension/host-windows/lv.eparaksts.eparaksts_chrome_extension.json"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#VcInstallDir}/redist/x86/Microsoft.VC120.CRT/msvcp120.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#VcInstallDir}/redist/x86/Microsoft.VC120.CRT/msvcr120.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#VcInstallDir}/redist/x86/Microsoft.VC120.CRT/vccorlib120.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#VcInstallDir}/redist/x86/Microsoft.VC120.MFC/mfc120u.dll"; DestDir: "{app}"; Flags: ignoreversion

[Registry]
; PluginNP
Root: HKLM; Subkey: "SOFTWARE\MozillaPlugins\@eParaksts/eParaksts Plugin,version=1.0.2"; ValueType: string; ValueName: "Path"; ValueData: "{app}\np-eparaksts-plugin.dll"; Flags: uninsdeletekey

; ExtensionChrome
Root: HKLM; Subkey: "SOFTWARE\Google\Chrome\NativeMessagingHosts\lv.eparaksts.eparaksts_chrome_extension"; ValueType: string; ValueName: ""; ValueData: "{app}\lv.eparaksts.eparaksts_chrome_extension.json"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Google\Chrome\Extensions\dfablgdffinpaeiilgjpebchbacimpoa"; ValueType: string; ValueName: "update_url"; ValueData: "https://clients2.google.com/service/update2/crx"; Flags: uninsdeletekey

[Tasks]
; Task to update PATH
Name: modifypath; Description: Update Environment Variable 'PATH'

[Code]
// Code to update PATH
const
  ModPathName = 'modifypath';
  ModPathType = 'system';
function ModPathDir(): TArrayOfString;
begin
  setArrayLength(Result, 1)
  Result[0] := ExpandConstant('{app}');
end;
#include "modpath.iss"
