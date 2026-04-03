# logos-dev-boost Evaluation Report

**Project:** Snake game module built as logos-dev-boost tooling pilot
**Date:** 2026-04-03
**Author:** Fergie (implementer agent) + Senty (security reviewer)
**Repo:** [xAlisher/logos-snake-game](https://github.com/xAlisher/logos-snake-game)

---

## Executive Summary

logos-dev-boost is a promising AI-assisted development accelerator with **excellent documentation generation** but **significant gaps in scaffolding and build setup**. The strongest feature is the AI context file generation (AGENTS.md, CLAUDE.md, skills) which gives AI agents accurate Logos knowledge. The weakest areas are the UI app scaffold (generates unbuildable code) and missing build configuration files.

**Verdict:** Adopt selectively. Use the AI context generation and documentation patterns. Do not rely on the scaffold for UI apps until the IComponent/PluginInterface mismatch is fixed upstream.

---

## Step-by-Step Walkthrough

### Step 1: Scaffold (`logos-dev-boost init`)

**Command:** `logos-dev-boost init snake_game --type ui-app`

| Aspect | Result | Notes |
|--------|--------|-------|
| CLI execution | FAIL then PASS | Requires Node 20.11+, system Node 18 fails |
| Source files generated | PASS | Plugin.h/cpp, Backend.h/cpp, Main.qml |
| AI context files | EXCELLENT | AGENTS.md (2800+ lines), CLAUDE.md, 8 skills |
| MCP config | PASS | .mcp.json generated correctly |
| Build config (flake.nix) | MISSING | Not generated — must create manually |
| Build config (CMakeLists.txt) | MISSING | Not generated — must create manually |
| Buildable out of the box | FAIL | Generated code references IComponent which doesn't exist in SDK |

**Critical:** The UI app scaffold generates `#include "IComponent.h"` but the SDK only has `PluginInterface`. The generated code cannot compile.

### Step 2: AI Tooling (`logos-dev-boost install`)

Not tested separately — install runs automatically during init. The generated files are comprehensive and accurate. The 8 Claude Code skills cover all common tasks (create module, testing, packaging, etc.).

**Assessment:** This is the best feature of logos-dev-boost. AI agents get accurate Logos SDK knowledge without hallucinating APIs.

### Step 3: Nix Build

**4 manual fixes required after scaffold:**

1. Create `flake.nix` from scratch (not generated)
2. Create `CMakeLists.txt` from scratch (not generated)
3. Add multiple include paths for SDK headers (`LOGOS_CPP_SDK_ROOT/include/cpp`, `LOGOS_MODULE_ROOT/include`)
4. Set `PREFIX ""` on CMake target (builder expects no `lib` prefix)

Once fixed, `logos-module-builder.lib.mkLogosModule` works well — resolves Qt 6, SDK, and all dependencies correctly.

### Step 4: Implementation

Used the **pure QML UI plugin pattern** (manifest.json + metadata.json + Main.qml) — the same pattern used by keycard-ui, notes_ui, and auth_showcase-ui in production. logos-dev-boost does not document or scaffold this pattern at all.

Snake game works in Basecamp: arrow keys to move, space to start/restart, score tracking, wrap-around walls, self-collision detection.

### Step 5: Testing (logoscore)

**Not applicable.** logoscore tests universal C++ module methods. Pure QML plugins have no C++ dispatch table. logos-dev-boost's testing documentation only covers core modules.

### Step 6: LGX Packaging

**Not applicable.** LGX packages compiled .so/.dylib files. Pure QML plugins are just files copied to the plugins directory. No compilation step, no platform variants.

---

## Findings Summary

| # | Severity | Finding |
|---|----------|---------|
| 1 | Blocker | Node 18 incompatible — requires Node 20.11+ |
| 2 | High | Missing flake.nix and CMakeLists.txt in scaffold |
| 3 | **Critical** | IComponent interface doesn't exist in SDK — scaffold produces unbuildable code |
| 4 | Medium | CMake include path discovery requires 4 separate paths |
| 5 | Medium | Library prefix mismatch (lib prefix vs no prefix) |
| 6 | **Positive** | AI context files (AGENTS.md, CLAUDE.md) are excellent |
| 7 | **Positive** | logos-module-builder Nix infrastructure works well |
| 8 | Info | LogosAPI undefined symbol warning — expected for stub modules |
| 9 | Medium | Scaffold QML references nonexistent backend |
| 10 | High | Pure QML UI plugin pattern not documented |
| 11 | Medium | Deploy target path wrong in scaffold output (LogosBasecampDev doesn't exist) |
| 12 | Info | logoscore not applicable for QML plugins |
| 13 | Info | LGX packaging not applicable for QML plugins |

---

## Architecture Mismatch

logos-dev-boost documents two component types:
1. **Universal Module** (core, C++) — well-documented, works
2. **UI App** (IComponent, QWidget) — documented but **doesn't exist in SDK**

In practice, Basecamp has THREE patterns:
1. **Core Module** (PluginInterface, C++) — what universal modules actually are
2. **Compiled UI Plugin** (PluginInterface, C++ + QML) — like keycard-ui's debug panel
3. **Pure QML UI Plugin** (manifest.json + metadata.json + QML files) — most common for UIs

logos-dev-boost's scaffold and documentation only cover pattern 1 (partially correct) and pattern 2 (with wrong interface). Pattern 3 (the most common) is completely absent.

---

## Recommendations for Core Devs

### Fix Now
1. **Fix UI app scaffold** — replace IComponent with PluginInterface, or add pure QML template
2. **Generate flake.nix and CMakeLists.txt** — scaffold should produce buildable projects
3. **Add Node version check** — fail gracefully on Node < 20.11
4. **Fix deploy path** — LogosBasecampDev → LogosBasecamp

### Add
5. **Add `--type qml-plugin` scaffold** — for pure QML UI modules (most common pattern)
6. **Document the three plugin patterns** — core module, compiled UI, pure QML UI
7. **Add QML testing guidance** — logoscore doesn't cover UI testing
8. **Document CMake include paths** — `LOGOS_CPP_SDK_ROOT`, `LOGOS_MODULE_ROOT` and what headers live where

### Consider
9. **LGX for QML bundles** — package QML plugins for distribution
10. **logos_module() CMake macro** — mentioned in docs but not found in SDK; would eliminate include path friction

---

## What Works Well

1. **AI context generation** — AGENTS.md/CLAUDE.md are comprehensive, accurate, and immediately useful
2. **Claude Code skills** — 8 task-specific guides auto-installed
3. **MCP server** — live project introspection tools
4. **logos-module-builder** — solid Nix build infrastructure once configured
5. **Documentation quality** — guidelines and skill content is well-written and accurate (for core modules)

---

## What Needs Work

1. **UI app scaffold is broken** — generates unbuildable code
2. **Missing build files** — scaffold doesn't generate flake.nix or CMakeLists.txt
3. **Documentation gap** — pure QML UI pattern (most common) is undocumented
4. **Testing gap** — no guidance for QML/UI testing
5. **Packaging gap** — LGX doesn't cover QML plugins

---

## Conclusion

logos-dev-boost is a strong foundation with the right architecture (three-layer AI integration). The AI context generation is production-ready and should be adopted. The scaffolding and build setup need significant fixes before they can be used reliably for UI development. The core module (universal interface) path appears more mature than the UI path.

**Our recommendation:** Adopt AI context patterns now. File upstream issues for findings #1-5 and #10-11. Wait for scaffold fixes before recommending logos-dev-boost for new UI development.
