# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

Visual Studio 2022 (v143 toolset), x64 only, C11 standard, precompiled headers.

```bash
# Build from command line (requires VS Developer Command Prompt)
msbuild fuck-shift-space.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild fuck-shift-space.vcxproj /p:Configuration=Debug /p:Platform=x64
```

No tests. No package manager. Only external lib: `imm32.lib` (Windows IME).

## Architecture

Single-file Windows desktop app (`fuck-shift-space.cpp`, ~657 lines). No UI window — runs as message-only window with system tray icon.

**Message pump** (`wWinMain` → `WndProc`) drives everything via three event sources:

1. **Global hotkeys** (`WM_HOTKEY` via `RegisterHotKey`):
   - `Shift+Space` → releases Shift first, sends Space, re-presses Shift. Prevents accidental IME toggle.
   - `Win+Space` → sends `Ctrl+Space` instead. Overrides Windows IME switcher.

2. **Window focus hook** (`SetWinEventHook` on `EVENT_OBJECT_FOCUS`):
   - `WinEventProc` reads foreground window's process name, looks up in `chineseApps`/`englishApps` sets, calls `SwitchToEnglish` to flip IME conversion mode via `ImmGetDefaultIMEWnd` + `WM_IME_CONTROL`.

3. **Timer** (`WM_TIMER` every 3s → `KillSomeApp` + `ClearTempDir`):
   - `KillSomeApp`: snapshots all processes, kills `msedge.exe` exceeding memory limit (10s grace), kills orphaned `yundetectservice.exe`, kills `oxc_language_server.exe` not parented by `zed.exe`/`code.exe`, kills `node.exe` not parented by terminal/VS Code/Zed.
   - `ClearTempDir`: if free disk space < min(6.25%, 1GB), recursively nukes temp directory.

**Config** stored in registry: `HKCU\Software\karoyqiu\fuck-shift-space` — `ChineseApps`/`EnglishApps` (REG_MULTI_SZ), `MemoryLimitMB` (DWORD, default 2GB).

**Key helpers**: `HasParentProcess` walks parent PID chain to determine legitimate parentage. `SwitchToEnglish` flips IME mode with a 40ms `Sleep` for reliability.
