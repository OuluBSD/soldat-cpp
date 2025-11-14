# Roadmap for soldat-cpp

## Important: Pascal Code Comparison

### Overview
- Original Pascal code (old version): `/home/sblo/Dev/CodexSandbox/soldat/pascal/`
- Current Pascal code (new version): `./pascal/`
- C++ code (from old conversion): Copied to `include/` and `src/` directories

### File Mapping
The original C++ files in `~/Dev/CodexSandbox/soldat/stdsrc/` used CapitalCase names that match the Pascal units. These have been copied to:
- Header files (.h) → `include/` directory
- Source files (.cpp) → `src/` directory

### Comparison Process
1. Compare each Pascal file from `/home/sblo/Dev/CodexSandbox/soldat/pascal/` with corresponding file in `./pascal/`
2. Identify differences between old and new Pascal code
3. Update C++ implementation in `src/` and `include/` to reflect same changes

### Initial Comparison Findings
- **Constants.pas**: Changed HEADSTYLE_HELMET from 34 to 1, HEADSTYLE_HAT from 124 to 2; minor formatting changes in aim distance constants
- **Util.pas**: Removed TStringArray type definition and several utility functions (CharCount, SplitStr)
- **Server.pas**: Removed IdCompilerDefines.inc include; reorganized uses clause with Steam-related changes
- **Net.pas**: Simplified network interface implementation; replaced TSteamNetworkingSockets with PISteamNetworkingSockets; changed GameNetworkingSockets to Steam in uses clause
- **Game.pas**: Minor formatting changes; removed MapsList.Clear call from ChangeMap procedure

## Phase 1: Project Restructuring
- [x] Record original Pascal commit hash in VERSION.md
- [x] Update QWEN.md with version tracking instructions
- [x] Create `pascal/` directory
- [x] Move Pascal source directories to `pascal/` directory
- [x] Create C++ source directory structure
- [ ] Set up initial CMakeLists.txt for new structure

## Phase 2: Code Conversion Reference
- [ ] Identify corresponding files from old conversion at ~/Dev/CodexSandbox/soldat
- [ ] Note CapitalCase file renaming in old conversion (stdsrc/ directory)
- [ ] Create reference mapping between Pascal and C++ files
- [ ] Document conversion patterns and techniques used in old conversion

## Phase 3: Pascal Code Removal
- [ ] Verify all necessary code has been converted or is ready for conversion
- [ ] Remove original Pascal directories from project root (after migration to pascal/)
- [ ] Clean up any Pascal-specific build configurations

## Phase 4: C++ Implementation
- [ ] Implement C++ versions of Pascal modules using old conversion as reference
- [ ] Ensure compatibility with existing CMake build system
- [ ] Add proper error handling and modern C++ practices
- [ ] Implement unit tests for converted components

## Phase 5: Testing and Validation
- [ ] Build the complete C++ project
- [ ] Run all available tests
- [ ] Verify functionality matches original Pascal implementation
- [ ] Performance testing

## Phase 6: Documentation and Cleanup
- [ ] Update README.md with new project structure
- [ ] Document any deviations from original Pascal functionality
- [ ] Clean up temporary files and conversion artifacts
- [ ] Final review and verification