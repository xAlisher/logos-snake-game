# logos-dev-boost Evaluation Report

**Project:** Snake game module built as logos-dev-boost tooling pilot
**Date:** 2026-04-03
**Author:** Fergie (implementer agent) + Senty (security reviewer)
**Repo:** [xAlisher/logos-snake-game](https://github.com/xAlisher/logos-snake-game)
**Findings:** 25 total (documented in PROJECT_KNOWLEDGE.md)

---

## Top-Level Finding: Tooling/Runtime Mismatch

In the environments and releases we tested, we could not validate end-to-end runtime loading of a universal interface module. The tooling, the public releases, and our working runtime each target different interfaces:

| | **logos-dev-boost** (tooling) | **logos-co/logos-basecamp** (GitHub releases) | **Our logos-app** (local Nix build) |
|---|---|---|---|
| **Plugin interface** | `LogosProviderBase` (universal/codegen) | Unknown (module discovery blocked) | `PluginInterface` (legacy) |
| **Module discovery** | Assumes file-drop or LGX | Not observed in releases tested | File-drop works |
| **Binary name** | N/A | `LogosBasecamp` | `LogosApp` |

This may reflect a platform transition in progress rather than a permanent state. We recommend the Basecamp team clarify the current module loading contract.

---

## Evidence Matrix

How each major conclusion was validated:

| Conclusion | Local `logos-app` | Public Basecamp releases | Build-only | Inferred from code/docs |
|-----------|:-:|:-:|:-:|:-:|
| Code generator works | | | ✓ | |
| Nix builder works | | | ✓ | |
| LGX packaging works | | | ✓ | |
| AI context files are accurate | | | | ✓ |
| UI scaffold is broken (IComponent) | | | ✓ | ✓ |
| Module discovery fails in public releases | | ✓ (tested #80-#111) | | |
| `LogosProviderBase` incompatible with our runtime | ✓ (logoscore crash) | | | |
| Pure QML plugin pattern undocumented | | | | ✓ |
| Snake game QML works | ✓ | | | |

---

## Tool-by-Tool Breakdown

### 1. CLI: `logos-dev-boost init`

**What it does:** Scaffolds a new Logos module or UI app project.

| Aspect | Verdict | Details |
|--------|---------|---------|
| CLI works | PARTIAL | Requires Node 20.11+. Fails silently on Node 18 (`import.meta.dirname` undefined). No version check. |
| Module scaffold (`--type module`) | NOT TESTED | We tested UI app first, then built module manually |
| UI app scaffold (`--type ui-app`) | BROKEN | Generates `IComponent` interface that doesn't exist in SDK. Unbuildable code. |
| Generated source files | BROKEN | `SnakeGamePlugin.h` includes `"IComponent.h"` — SDK has `PluginInterface` at `<module_lib/interface.h>` |
| Generated QML | BROKEN | References `backend.items` and `backend.addItem()` from the IComponent pattern that doesn't exist |
| flake.nix generation | MISSING | Not generated. Must create manually. |
| CMakeLists.txt generation | MISSING | Not generated. Must create manually. |
| Deploy path suggestion | WRONG | Suggests `LogosBasecampDev` which doesn't exist. Real paths: `LogosApp/modules/` or `LogosBasecamp/plugins/` |

**Useful?** No for UI apps — produces unbuildable code. Could be useful for universal modules if it generated build files.

**Recommendation:** Fix IComponent → PluginInterface. Generate flake.nix and CMakeLists.txt. Add `--type qml-plugin` for pure QML modules. Add Node version check.

---

### 2. CLI: `logos-dev-boost install`

**What it does:** Configures AI tools for an existing project. Auto-detects IDE, generates context files.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Runs automatically during init | WORKS | Integrated into scaffold flow |
| IDE auto-detection | NOT TESTED STANDALONE | Only tested via init |

**Useful?** Not tested independently. Runs as part of init.

**Recommendation:** Test standalone on existing project.

---

### 3. CLI: `logos-dev-boost generate`

**What it does:** Regenerates AI context files (AGENTS.md, CLAUDE.md, cursor rules, llms.txt).

| Aspect | Verdict | Details |
|--------|---------|---------|
| Standalone regeneration | NOT TESTED | Only tested via init auto-generation |

**Useful?** Not tested independently.

---

### 4. Context File: AGENTS.md

**What it does:** Always-loaded AI context file with compressed Logos SDK documentation.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Generated correctly | EXCELLENT | 2800+ lines of accurate Logos conventions |
| Type mapping table | ACCURATE | C++ → LIDL → Qt mappings match SDK reality |
| Code generator docs | ACCURATE | `--from-header` pipeline documented correctly |
| Build commands | ACCURATE | `nix build`, `nix flake check`, `nix develop` all correct |
| Plugin patterns | PARTIALLY WRONG | Documents IComponent (doesn't exist). Doesn't document pure QML pattern. |
| Inter-module comm | ACCURATE | `LogosAPI::callModule()` pattern matches reality |

**Useful?** YES — this is the strongest feature of logos-dev-boost. AI agents get accurate Logos knowledge immediately. The few inaccuracies (IComponent) are inherited from the scaffold bug.

**Recommendation:** Fix IComponent references. Add pure QML UI plugin documentation. Otherwise excellent — adopt now.

---

### 5. Context File: CLAUDE.md

**What it does:** Claude Code-specific context file that imports AGENTS.md content and adds skill references.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Generated correctly | WORKS | Imports all AGENTS.md content plus skill list |
| Skill references | WORKS | Lists all 8 skills with activation triggers |
| MCP server reference | WORKS | Documents available MCP tools |

**Useful?** YES — direct Claude Code integration works well.

**Recommendation:** Adopt as-is. Fix inherits from AGENTS.md fixes.

---

### 6. MCP Server: `logos_project_info`

**What it does:** Analyzes project metadata without running Nix evaluation.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Project type detection | WORKS | Correctly identified `ui-app` type |
| Metadata parsing | WORKS | Returns name, version, dependencies, build targets |
| Flake detection | WORKS | Detects flake.nix presence |
| Nix input listing | WORKS | Lists flake inputs correctly |

**Useful?** YES — quick project inspection without slow Nix evaluation.

**Recommendation:** Adopt. Works well.

---

### 7. MCP Server: `logos_search_docs`

**What it does:** Full-text fuzzy search over bundled documentation.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Search functionality | NOT TESTED | Did not invoke via MCP protocol |

**Useful?** Not tested.

---

### 8. MCP Server: `logos_api_reference`

**What it does:** Returns type system and LogosAPI reference.

| Aspect | Verdict | Details |
|--------|---------|---------|
| API reference | NOT TESTED | Did not invoke via MCP protocol |

**Useful?** Not tested.

---

### 9. MCP Server: `logos_build_help`

**What it does:** Context-aware build commands and troubleshooting.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Package commands | WORKS | Correctly suggested `lgx create`, `lgx add`, `lgpm install` |
| Build commands | WORKS | Correct `nix build` commands |
| Troubleshooting | NOT TESTED | Did not test with build errors |

**Useful?** YES — gave correct LGX packaging workflow.

**Recommendation:** Adopt. Test troubleshooting path.

---

### 10. MCP Server: `logos_scaffold`

**What it does:** Same as CLI init, but via MCP tool.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Functionality | NOT TESTED SEPARATELY | CLI init was used directly |

**Useful?** Not tested separately from CLI.

---

### 11. Skill: `create-universal-module`

**What it does:** Step-by-step guide for creating a universal C++ module.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Impl header pattern | ACCURATE | Pure C++ types, naming conventions match |
| Type mapping | ACCURATE | Matches code generator's actual behavior |
| Build pipeline | ACCURATE | preConfigure + logos-cpp-generator works as documented |
| Testing pattern | PARTIALLY TESTED | logoscore commands are correct but runtime crashes (incompatibility) |

**Useful?** YES — guided the high_score module creation successfully. Build pipeline works exactly as described.

**Recommendation:** Adopt. Best skill for module development.

---

### 12. Skill: `create-ui-app`

**What it does:** Guide for creating Basecamp UI app with IComponent + QML.

| Aspect | Verdict | Details |
|--------|---------|---------|
| IComponent pattern | BROKEN | IComponent doesn't exist in SDK |
| Backend class pattern | BROKEN | Depends on IComponent |
| QML conventions | PARTIALLY USEFUL | Layout patterns are fine, but theme imports (Logos.Theme, Logos.Controls) only exist in latest Basecamp |

**Useful?** NO — based on non-existent interface. Cannot follow this skill.

**Recommendation:** Rewrite for PluginInterface or pure QML pattern.

---

### 13. Skill: `package-lgx`

**What it does:** Guide for LGX packaging and distribution.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Manual lgx commands | NOT TESTED | Used `nix build .#lgx` instead (built-in to builder) |
| `nix build .#lgx` | WORKS | Produces proper .lgx file automatically |
| `nix build .#install` | WORKS | Runs lgpm, installs to nix store path |
| Platform variant naming | BUG | Builder produces `linux-amd64-dev` but runtime expects `linux-amd64` |

**Useful?** PARTIALLY — the Nix-integrated path works better than the manual lgx commands documented.

**Recommendation:** Document `nix build .#lgx` as primary path. Fix variant naming.

---

### 14. Skill: `nix-flake-setup`

**What it does:** Guide for configuring Nix flake.nix for Logos modules.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Minimal flake template | WORKS | `mkLogosModule` with `src`, `configFile`, `flakeInputs` works |
| preConfigure for codegen | WORKS | Code generator runs correctly in this hook |
| Build commands | WORKS | `nix build`, `nix develop` correct |
| CMake variables | NOT DOCUMENTED | Doesn't mention `LOGOS_CPP_SDK_ROOT`, `LOGOS_MODULE_ROOT` — must discover manually |

**Useful?** YES — got the flake working with this guide. Missing CMake variable documentation was the main friction.

**Recommendation:** Add CMake variable reference. Document `set_target_properties(PREFIX "")` requirement.

---

### 15. Skill: `testing-modules`

**What it does:** Guide for unit and integration testing.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Unit test pattern | ACCURATE | Direct impl class instantiation works |
| logoscore commands | ACCURATE | Correct flags and syntax |
| logoscore runtime | CRASHES | Module loads but `LogosProviderBase` incompatible with runtime |
| QML testing | NOT COVERED | No guidance for testing QML UI plugins |

**Useful?** PARTIALLY — patterns are correct but runtime incompatibility prevents actual testing.

**Recommendation:** Add QML testing guidance. Fix runtime compatibility first.

---

### 16. Skill: `inter-module-comm`

**What it does:** Guide for inter-module communication via LogosAPI.

| Aspect | Verdict | Details |
|--------|---------|---------|
| LogosAPI patterns | NOT TESTED AT RUNTIME | Couldn't run module to test inter-module calls |
| Documentation | APPEARS ACCURATE | `logos.callModule()` QML pattern matches our keycard-basecamp usage |

**Useful?** Not tested. Documentation looks correct based on our keycard experience.

---

### 17. Skill: `wrap-external-lib`

**What it does:** Guide for wrapping external C/C++ libraries as Logos modules.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Functionality | NOT TESTED | Not applicable to snake game pilot |

**Useful?** Not tested.

---

### 18. Skill: `add-to-workspace`

**What it does:** Guide for adding modules to logos-workspace multi-repo setup.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Functionality | NOT TESTED | Not applicable to standalone pilot |

**Useful?** Not tested.

---

### 19. Guidelines (7 files in AGENTS.md)

| Guideline | Verdict | Details |
|-----------|---------|---------|
| `core.md` — Two component types | PARTIALLY WRONG | Documents IComponent that doesn't exist. Doesn't cover pure QML pattern. |
| `universal-module.md` — Pure C++ pattern | ACCURATE | Type mapping, naming, build pipeline all correct |
| `ui-app.md` — IComponent pattern | BROKEN | Entire guideline based on non-existent interface |
| `nix-build.md` — Flake structure | ACCURATE | Build commands, override patterns correct |
| `testing.md` — Unit/integration tests | ACCURATE | Patterns correct, runtime incompatibility separate issue |
| `metadata-json.md` — Full schema | ACCURATE | Field descriptions match reality |
| `codegen.md` — Code generator | ACCURATE | Pipeline, type mapping, LIDL format all correct |

---

### 20. Code Generator: `logos-cpp-generator`

**What it does:** Parses pure C++ impl header, generates Qt plugin glue and method dispatch.

| Aspect | Verdict | Details |
|--------|---------|---------|
| Header parsing | WORKS | Correctly parsed `HighScoreImpl` class with 4 methods |
| Qt glue generation | WORKS | Produced `high_score_qt_glue.h` with proper Q_OBJECT, Q_PLUGIN_METADATA |
| Dispatch generation | WORKS | Produced `high_score_dispatch.cpp` with method dispatch table |
| Type mapping | WORKS | `std::string` → `QString`, `int64_t` → `int` correctly mapped |
| Nix integration | WORKS | Runs in `preConfigure` hook as documented |
| metadata.json location | BUG | Q_PLUGIN_METADATA references metadata.json relative to generated file — must copy |

**Useful?** YES — the code generator works exactly as documented. This is a solid tool.

**Recommendation:** Fix metadata.json path resolution. Otherwise adopt.

---

### 21. Build System: `logos-module-builder`

**What it does:** Nix flake helper that wraps CMake/Ninja build with Logos SDK dependencies.

| Aspect | Verdict | Details |
|--------|---------|---------|
| `mkLogosModule` | WORKS | Resolves Qt 6, SDK, module headers correctly |
| CMake variable passing | WORKS | `LOGOS_CPP_SDK_ROOT`, `LOGOS_MODULE_ROOT` passed correctly |
| LGX output (`nix build .#lgx`) | WORKS | Integrated LGX packaging |
| Install output (`nix build .#install`) | WORKS | Integrated lgpm installation |
| Dev shell | NOT TESTED | Did not use `nix develop` |
| Platform variant naming | BUG | Produces `linux-amd64-dev` instead of `linux-amd64` |

**Useful?** YES — solid build infrastructure once you know the CMake variables.

**Recommendation:** Document CMake variables. Fix variant naming. Add `logos_module()` CMake macro.

---

## Summary by Category

### What Worked Well
1. **AI context generation** (AGENTS.md, CLAUDE.md) — comprehensive, accurate, immediately useful
2. **Code generator** (`logos-cpp-generator`) — full pipeline works end-to-end
3. **Nix build infrastructure** (`logos-module-builder`) — solid dependency resolution
4. **LGX packaging** — integrated into Nix build, produces proper packages
5. **Claude Code skills** — 8 task-specific guides, well-written
6. **MCP project-info tool** — quick project inspection
7. **Universal module documentation** — type mapping, naming, pipeline all correct

### What Didn't Work
1. **UI app scaffold** — generates code using non-existent `IComponent` interface
2. **Missing build files** — no flake.nix or CMakeLists.txt generated
3. **Runtime incompatibility** — `LogosProviderBase` (codegen) vs `PluginInterface` (runtime)
4. **Basecamp module discovery** — all GitHub releases reject user-installed modules
5. **Platform variant naming** — `-dev` suffix breaks module discovery
6. **Deploy path** — scaffold suggests non-existent directory

### What Was Not Tested
1. `logos-dev-boost install` (standalone)
2. `logos-dev-boost generate` (standalone)
3. MCP `logos_search_docs` tool
4. MCP `logos_api_reference` tool
5. MCP `logos_scaffold` tool (used CLI instead)
6. Skill: `wrap-external-lib`
7. Skill: `add-to-workspace`
8. Skill: `inter-module-comm` (at runtime)
9. `nix develop` shell
10. Module scaffold (`--type module` without `--type ui-app`)

---

## Recommendations by Owner

### Owner: logos-dev-boost team — Priority: High

These are bugs/gaps in the tooling that we observed directly:

| # | Action | Evidence |
|---|--------|----------|
| 1 | Replace IComponent with PluginInterface in UI app scaffold | SDK has no IComponent (build failure) |
| 2 | Generate flake.nix and CMakeLists.txt from scaffold | Missing from scaffold output |
| 3 | Add Node 20.11+ version check in CLI | Silent crash on Node 18 |
| 4 | Fix deploy path in scaffold output | `LogosBasecampDev` doesn't exist |
| 5 | Fix metadata.json path in code generator | Q_PLUGIN_METADATA can't find it in generated_code/ |
| 6 | Fix platform variant naming | `-dev` suffix breaks module discovery |

### Owner: logos-dev-boost team — Priority: Medium

Gaps that reduce usability:

| # | Action | Rationale |
|---|--------|-----------|
| 7 | Add `--type qml-plugin` scaffold | Pure QML is the most common UI pattern in production |
| 8 | Document three plugin patterns | core module, compiled UI, pure QML UI all exist |
| 9 | Document CMake variables (`LOGOS_CPP_SDK_ROOT`, `LOGOS_MODULE_ROOT`) | Developers must discover these manually |
| 10 | Add QML testing guidance | logoscore only covers core modules |
| 11 | Provide `logos_module()` CMake macro | Mentioned in docs but not in SDK |

### Owner: Basecamp/platform team — Needs investigation

These may be intentional architecture changes, not bugs. We flag them for clarification:

| # | Question | What we observed |
|---|----------|------------------|
| 12 | How should external modules be loaded in public Basecamp releases? | Releases #80-#111 (Mar 22 – Apr 2) did not discover file-dropped or lgpm-installed modules |
| 13 | What is the current module interface contract? | Tooling generates `LogosProviderBase`, our runtime uses `PluginInterface` |
| 14 | Which build is canonical — `logos-app` (Nix) or `logos-basecamp` (GitHub)? | They have different binaries, data dirs, and module discovery behavior |

### Owner: Our team (keycard-basecamp) — Adopt now

| # | Action | Value |
|---|--------|-------|
| 15 | Run `logos-dev-boost install` on keycard-basecamp | Get AGENTS.md/CLAUDE.md for accurate AI context |
| 16 | Reference Nix flake patterns | nixpkgs unification, follows declarations |
| 17 | Prepare for logoscore testing | Patterns ready, blocked on runtime alignment |

---

## Conclusion

### What was genuinely validated
- **Code generator** (`logos-cpp-generator`): parses C++ headers correctly, produces valid Qt glue, integrates into Nix build
- **Nix builder** (`logos-module-builder`): resolves dependencies, builds modules, produces LGX packages
- **AI context generation**: comprehensive, accurate documentation that eliminates API hallucination
- **Pure QML UI plugins**: work in our local `logos-app` runtime (snake game runs)

### What was blocked
- **End-to-end runtime loading** of universal interface modules: blocked by interface mismatch (`LogosProviderBase` vs `PluginInterface`) and module discovery differences between builds
- **Testing via logoscore**: module loads and launches in `logos_host` but crashes due to interface mismatch
- **Testing via public Basecamp releases**: module discovery not observed in any tested release (#80-#111)

### What this means
The pilot produced both real tooling wins and real platform blockers. The blockers are not logos-dev-boost bugs — they reflect a broader platform transition where the universal interface exists in the tooling but has not yet been validated against a matching runtime.

### For keycard-basecamp
- **Now:** Adopt AI context generation (AGENTS.md/CLAUDE.md)
- **Now:** Continue with `PluginInterface` + our local Nix build
- **Later:** Re-evaluate universal interface when platform team confirms runtime alignment
