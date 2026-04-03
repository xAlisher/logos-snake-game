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

## Finding #11: Deploy Target Confusion
**Severity:** Medium
**Step:** Implementation
**Details:** logos-dev-boost scaffold suggests deploying to `~/.local/share/Logos/LogosBasecampDev/plugins/`. In practice:
- Core modules: `LogosApp/modules/<name>/`
- UI QML plugins: `LogosBasecamp/plugins/<name>/`
- NOT `LogosBasecampDev` (this doesn't exist or isn't used)
**Recommendation:** Fix deploy path in scaffold output.
