# Encoding Workflow

This repository uses UTF-8 (without BOM) and CRLF line endings for source and text files.

## Quick Check

Run:

```powershell
powershell -ExecutionPolicy Bypass -File tools\check-encoding.ps1
```

The script validates `code\*.c` and `code\*.h` for:

- strict UTF-8 decodability
- UTF-8 BOM usage
- mixed line endings (CRLF/LF)
- LF-only or CR-only files

## Patch Safety Rules

- Read files with explicit UTF-8 handling to avoid terminal display confusion.
- Use ASCII anchors (function names, macros, identifiers) when creating patches.
- Avoid using terminal-rendered garbled text as patch context.

