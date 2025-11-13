# Analyzer Fix Instructions

The analyzer errors you're seeing are **false positives** - your code compiles correctly. The default IntelliSense engine has trouble with complex template code.

## Quick Fix Steps:

1. **Generate compile_commands.json** (if not already done):
   ```bash
   cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
   cmake --build build
   ```

2. **Reload VS Code**:
   - Press `Ctrl+Shift+P`
   - Type "Developer: Reload Window"
   - Press Enter

3. **If errors persist, try one of these:**

### Option A: Use clangd (Recommended - Better template support)
1. Install "clangd" extension from VS Code marketplace
2. In `.vscode/settings.json`, uncomment the clangd section
3. Reload VS Code

### Option B: Ignore false positives
The errors are cosmetic - your code builds fine. You can:
- Ignore the red squiggles
- Or add `// NOLINT` comments to suppress specific lines

### Option C: Rebuild IntelliSense database
1. Press `Ctrl+Shift+P`
2. Type "C/C++: Reset IntelliSense Database"
3. Wait for it to rebuild

## Why this happens:
- `DWT::_CYCCNT::read()` is correct - it's a template instantiation
- `GPIO_ConfigPin` is correct - it's a template function
- The analyzer can't always resolve complex template types
- Your actual compiler (GCC) handles it fine

