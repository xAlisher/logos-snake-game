# Project Knowledge: logos-dev-boost Evaluation Pilot

## Purpose
Snake game module built using logos-dev-boost tooling to evaluate the full development pipeline. Findings documented here for the comprehensive report to Logos core devs.

---

## Finding #1: Node 18 Incompatible
**Severity:** Blocker
**Step:** Scaffold (`logos-dev-boost init`)
**Details:** `import.meta.dirname` requires Node 20.11+. System Node 18 fails with: `The "paths[0]" argument must be of type string. Received undefined`
**Fix:** Use `nix run 'nixpkgs#nodejs_22'` to run the CLI
**Recommendation:** Document minimum Node version. Add runtime check in CLI entry point.

## Finding #2: Missing flake.nix and CMakeLists.txt
**Severity:** High
**Step:** Scaffold
**Details:** `logos-dev-boost init snake_game --type ui-app` generates source files, metadata.json, AI context files, but NO build configuration (flake.nix, CMakeLists.txt). Module cannot be built without manually creating these.
**Generated:** metadata.json, src/*.h/cpp, src/qml/Main.qml, AGENTS.md, CLAUDE.md, .mcp.json, .claude/skills/
**Missing:** flake.nix, CMakeLists.txt
**Recommendation:** Scaffold should generate complete buildable project.

## Finding #3: IComponent Interface Does Not Exist in SDK
**Severity:** Critical
**Step:** Scaffold / Build
**Details:** UI app scaffold generates `#include "IComponent.h"` and `Q_INTERFACES(IComponent)`, but the SDK only provides `PluginInterface` at `<module_lib/interface.h>`. The generated code is **unbuildable**.
**SDK reality:** `PluginInterface` with `name()`, `version()`, `logosAPI` member
**Scaffold generates:** `IComponent` with `createWidget()`, `destroyWidget()` — QWidget factory pattern that doesn't exist
**Recommendation:** Fix scaffold template to use `PluginInterface` or document that `IComponent` is a future/planned interface.

## Finding #4: CMake Include Path Discovery
**Severity:** Medium
**Step:** Build
**Details:** `logos-module-builder` passes `LOGOS_CPP_SDK_ROOT` and `LOGOS_MODULE_ROOT` as CMake variables. However:
- `interface.h` is in `${LOGOS_MODULE_ROOT}/include/module_lib/`
- `interface.h` includes `"logos_api.h"` which is in `${LOGOS_CPP_SDK_ROOT}/include/cpp/`
- Must add BOTH `${LOGOS_MODULE_ROOT}/include` AND `${LOGOS_CPP_SDK_ROOT}/include/cpp` to include paths
- The scaffold doesn't generate CMakeLists.txt, so developer must figure this out manually
**Recommendation:** If scaffold generated CMakeLists.txt, it should set these paths. Or logos-module-builder should provide a CMake helper function (like `logos_module()` macro mentioned in docs but not found).

## Finding #5: Library Prefix Mismatch
**Severity:** Medium
**Step:** Build
**Details:** CMake default output is `libsnake_game_plugin.so`, but logos-module-builder packaging expects `snake_game_plugin.so` (no `lib` prefix). Build compiles successfully but packaging fails with "No plugin library file found".
**Fix:** Add `set_target_properties(snake_game_plugin PROPERTIES PREFIX "")` to CMakeLists.txt
**Recommendation:** Document this requirement, or have the builder search for both patterns.

## Finding #6: AI Context Files Are Excellent
**Severity:** Positive
**Step:** Scaffold
**Details:** The generated AGENTS.md and CLAUDE.md are comprehensive (2800+ lines). They include:
- Type mapping tables
- Code generator pipeline docs
- Build commands
- Testing patterns
- 8 Claude Code skills auto-installed
- MCP server config
- Correct conventions and patterns
**Assessment:** This is the strongest feature of logos-dev-boost. AI agents get accurate Logos knowledge without hallucinating APIs.

## Finding #7: logos-module-builder Works Well
**Severity:** Positive
**Step:** Build
**Details:** `logos-module-builder.lib.mkLogosModule` successfully:
- Resolves Qt 6, logos-cpp-sdk, logos-module dependencies
- Provides correct CMake variables (LOGOS_CPP_SDK_ROOT, LOGOS_MODULE_ROOT)
- Runs the build pipeline (configure, build, install, package)
- Produces proper .so output
**Assessment:** The Nix build infrastructure is solid once you know the correct CMake setup.

## Finding #8: LogosAPI Undefined Symbol Warning
**Severity:** Info
**Step:** Build
**Details:** After successful build, the builder tries to load the .so and run `logos-cpp-generator` on it. This fails with `undefined symbol: _ZN8LogosAPI16staticMetaObjectE` because the module doesn't link against the full Logos runtime. Builder treats this as a warning ("this may be expected if the module has no public API") and continues. Build succeeds.
**Assessment:** Expected for standalone module builds. Not a problem.

---

## Architecture Notes

### Plugin Interface Patterns (Real vs Documented)
| What | logos-dev-boost says | SDK actually has |
|------|---------------------|------------------|
| Core module | PluginInterface + Q_INVOKABLE | PluginInterface + Q_INVOKABLE ✓ |
| UI app | IComponent + QWidget factory | PluginInterface + QML (no IComponent) ✗ |
| UI QML-only | Not documented | Pure QML plugin (manifest.json + metadata.json + qmldir) |

### Build Variables from logos-module-builder
| Variable | Path | Contains |
|----------|------|----------|
| `LOGOS_CPP_SDK_ROOT` | `/nix/store/...-logos-cpp-sdk` | SDK headers, logos_api.h |
| `LOGOS_MODULE_ROOT` | `/nix/store/...-logos-module-0.1.0` | module_lib/interface.h |

### Include Paths Needed for PluginInterface Module
```cmake
target_include_directories(my_plugin PRIVATE
    ${LOGOS_CPP_SDK_ROOT}/include
    ${LOGOS_CPP_SDK_ROOT}/include/cpp
    ${LOGOS_MODULE_ROOT}/include
    ${LOGOS_MODULE_ROOT}/include/module_lib
)
```

## Finding #9: Scaffold QML References Nonexistent Backend
**Severity:** Medium
**Step:** Scaffold
**Details:** Generated Main.qml references `backend.items` and `backend.addItem()`, which come from the `SnakeGameBackend` class. However, the backend class is part of the IComponent pattern (Finding #3) which doesn't exist in the SDK. If you remove the backend (because IComponent doesn't exist), the QML breaks.
**Recommendation:** Fix scaffold to generate working QML that doesn't depend on the broken IComponent pattern.

## Finding #10: Pure QML UI Plugin Pattern Not Documented
**Severity:** High
**Step:** Implementation
**Details:** logos-dev-boost documents two patterns: universal module (core) and UI app (IComponent). But in practice, Basecamp uses a THIRD pattern: pure QML UI plugins. These are:
- manifest.json + metadata.json + Main.qml (no C++)
- Type: `ui_qml` in manifest
- Loaded from `LogosBasecamp/plugins/<name>/`
- No IComponent, no QWidget, no C++ backend needed

This is how keycard-ui, notes_ui, auth_showcase-ui all work. logos-dev-boost doesn't document or scaffold this pattern at all.
**Recommendation:** Add `--type qml-plugin` scaffold option for pure QML UI modules.

## Finding #12: logoscore Not Applicable for QML Plugins
**Severity:** Info
**Step:** Testing (Step 5)
**Details:** logoscore is designed for testing universal C++ modules via their method dispatch. Pure QML UI plugins have no C++ methods to test — they're visual components. logos-dev-boost's testing documentation only covers core module testing, not QML UI testing.
**Assessment:** Testing gap for UI plugins. QML testing would need Qt's QML test framework or manual testing.
**Recommendation:** Document that logoscore doesn't apply to QML-only plugins. Suggest QML testing alternatives.

## Finding #13: LGX Packaging Not Applicable for QML Plugins
**Severity:** Info
**Step:** Packaging (Step 6)
**Details:** LGX packaging (`lgx create/add/verify`) is designed for compiled modules (.so/.dylib). Pure QML plugins are just files (manifest.json + metadata.json + *.qml) copied to the plugins directory. No compilation, no platform variants.
**Assessment:** The LGX workflow doesn't cover the most common UI plugin deployment pattern.
**Recommendation:** Document QML plugin deployment as a separate pattern. Consider LGX support for QML bundles.

## Finding #14: Code Generator Pipeline Works
**Severity:** Positive
**Step:** Build (high_score universal module)
**Details:** The full pipeline works:
1. `logos-cpp-generator --from-header` parses pure C++ impl header
2. Generates `high_score_qt_glue.h` (Qt plugin class, ProviderObject) and `high_score_dispatch.cpp` (method dispatch)
3. CMake builds with generated files
4. Output: `high_score_plugin.so` (72KB)
**Friction:** Must copy `metadata.json` to `generated_code/` directory because `Q_PLUGIN_METADATA(FILE "metadata.json")` resolves relative to the glue header location.
**Assessment:** The universal interface pattern is viable. This is the path that logos-dev-boost documents well.

## Finding #15: Runtime Requires Compatible Basecamp Version
**Severity:** High
**Step:** Build / Deploy
**Details:** The generated glue inherits `LogosProviderBase` from logos-cpp-sdk. At runtime, Basecamp's `logos_host` must provide this symbol. The SDK version pinned by `logos-module-builder` must match the Basecamp version. Our current Basecamp AppImage (March 22, 2026) may not have `LogosProviderBase` — it uses the older `PluginInterface` pattern.
**Implication:** Universal interface modules require a newer Basecamp than what we have. Need to update the AppImage to test.
**Recommendation:** Document minimum Basecamp version for universal modules.

## Finding #16: metadata.json Location for Generated Glue
**Severity:** Medium
**Step:** Build
**Details:** The generated `high_score_qt_glue.h` contains `Q_PLUGIN_METADATA(IID "..." FILE "metadata.json")`. moc resolves this relative to the header's directory (`generated_code/`), not the project root. Must copy metadata.json to `generated_code/` in preConfigure.
**Recommendation:** Code generator should use absolute path or the builder should handle this automatically.

## Finding #17: Latest Basecamp Doesn't Discover File-Dropped Modules
**Severity:** High
**Step:** Deploy / Test
**Details:** Latest Basecamp (pre-release-39804ed-111, April 2) does not discover modules installed by copying files to `LogosApp/modules/` or `LogosBasecamp/modules/`. Only `package_manager` and `capability_module` load (bundled with AppImage). The old Basecamp (March 22) discovers file-dropped modules fine.
**Implication:** Latest Basecamp likely requires LGX package installation via `lgpm` or `package_manager`. Direct file copy is no longer supported.
**Assessment:** This is the biggest gap in our workflow — we've been deploying by file copy. Need to learn LGX packaging.

## Finding #19: LGX Build Works via nix build .#lgx
**Severity:** Positive
**Step:** LGX Packaging (Step 6)
**Details:** `logos-module-builder` provides `packages.x86_64-linux.lgx` output automatically. Running `nix build .#lgx` produces a proper `.lgx` file (gzipped, with variant metadata). Also provides `.#install` which runs `lgpm` to install from the LGX.
**Assessment:** LGX packaging is well-integrated into the Nix build system — no separate `lgx` CLI needed.

## Finding #20: Nix Store Read-Only Permissions Break Install Copy
**Severity:** Low
**Step:** Deploy
**Details:** Files in nix store are read-only. Copying from `nix build .#install` output to data directory fails with `Permission denied` if target files already exist. Must `rm -rf` target first then copy, or use `chmod` after copy.

## Finding #21: Latest Basecamp Doesn't Accept File-Dropped Modules (Confirmed)
**Severity:** Critical for evaluation
**Step:** Deploy / Test
**Details:** Tested with both file copy AND lgpm-installed modules. Latest Basecamp (pre-release-39804ed-111) does NOT discover ANY user-installed modules in `LogosBasecamp/modules/`. Only bundled modules (package_manager, capability_module) load. This blocks testing the universal interface module in runtime.
**Assessment:** The latest Basecamp has a fundamentally different module discovery mechanism. We cannot test the high_score module without understanding how the new Basecamp discovers modules (possibly requires `package_manager` API call, or different data directory).
**Action needed:** Investigate logos-co/logos-basecamp source for module discovery changes.

## Finding #18: Hundreds of Stale AppImage Mount Points
**Severity:** Medium
**Step:** Operations
**Details:** `/tmp/.mount_logos-*` directories accumulate from crashed AppImage processes. Found 200+ stale mounts. These consume disk space and clutter temp directory.
**Recommendation:** Clean up script: `rm -rf /tmp/.mount_logos-*` (when no Logos is running)

## Finding #11: Deploy Target Confusion
**Severity:** Medium
**Step:** Implementation
**Details:** logos-dev-boost scaffold suggests deploying to `~/.local/share/Logos/LogosBasecampDev/plugins/`. In practice:
- Core modules: `LogosApp/modules/<name>/`
- UI QML plugins: `LogosBasecamp/plugins/<name>/`
- NOT `LogosBasecampDev` (this doesn't exist or isn't used)
**Recommendation:** Fix deploy path in scaffold output.
