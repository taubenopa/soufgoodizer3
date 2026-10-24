# Soufgoodizer

Ein-Button-Multiband-Kompressor als **VST3** (Windows). Das ganze Plugin ist dein Kopf.

| Aktion | Wirkung |
|---|---|
| Linksklick | nächstes Preset (A → B → C → D → E → F → A) |
| Shift + Linksklick | vorheriges Preset |
| Rechtsklick | Plugin ein / aus (Kopf wird dunkel) |

Presets: **A Smooth · B Punch · C Loud · D Warm Bass · E Air · F Crush**
(Werte in `Presets.h` – dort kannst du alles anpassen.)

## Bauen über GitHub
1. Alles in ein GitHub-Repo pushen.
2. Tab **Actions** → Workflow „Build Soufgoodizer + soufinstaller“ läuft automatisch.
3. Unter **Artifacts** liegt `soufinstaller.exe`.
4. Für ein Release: `git tag v1.0.0 && git push --tags` – der Installer wird dann automatisch an das Release gehängt.

## Lokal bauen (Windows, Visual Studio 2022 + CMake)
```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target Soufgoodizer_VST3
```
Installer: Inno Setup installieren, dann `soufinstaller.iss` kompilieren.

Installiert wird nach `C:\Program Files\Common Files\VST3\Soufgoodizer.vst3`.
Danach im Host (z. B. FL Studio) den Plugin-Scan starten.
