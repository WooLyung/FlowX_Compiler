# Editing rules

- Do not modify any `.vcxproj.filters` file, including its line endings.
- Keep C++ sources, headers, Visual Studio project files, and `.flowx` files in CRLF, as specified in `.gitattributes` and `.editorconfig`.
- After editing these files, normalize their line endings to CRLF without changing their encoding or BOM. Patch tools may insert LF lines; verify the final bytes before finishing.
- Keep generated `.ll` files in LF.
- Limit code changes to the requested scope and preserve user edits.
