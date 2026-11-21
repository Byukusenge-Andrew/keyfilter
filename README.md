# keyfilter

A tiny Windows tool that works around a shorted keyboard where pressing **G** also types **5** and **H** also types **6** (and the reverse).

It installs a low-level keyboard hook (`WH_KEYBOARD_LL`). The two keys of a shorted pair arrive within a few milliseconds of each other, so whichever arrives second within 25 ms is dropped. Injected input (e.g. the on-screen keyboard) is ignored.

This is a workaround, not a repair. The hardware fault remains and may spread, so replace the keyboard when you can.

## Build (MinGW)

```
g++ -O2 -mwindows keyfilter.cpp -o keyfilter.exe
```

## Use

- Run `keyfilter.exe`. It runs silently in the background.
- Stop it: `Stop-Process -Name keyfilter` (PowerShell) or end it in Task Manager.
- Start at login: put a shortcut to `keyfilter.exe` in `shell:startup`.
- To handle other shorted pairs, edit `partner()` in `keyfilter.cpp`; tune `WINDOW_MS` if ghosts slip through or real keys get dropped.

## Limits

- Does not work in the BIOS, login screen, or some elevated/secure windows (run as admin for elevated windows).
- Antivirus may flag keyboard hooks. The source is short; read it first.
