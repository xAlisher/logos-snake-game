# Logos Development Context (Claude Code)

## This Project

- **Name:** snake_game
- **Type:** UI App (Basecamp plugin)
- **Description:** Logos UI app

<!-- BEGIN:logos-dev-boost -->

## Logos Development Guidelines

# Code Generator (logos-cpp-generator)

## Overview

`logos-cpp-generator` bridges pure C++ module implementations to the Logos runtime's Qt plugin system. Module authors write standard C++ and the generator produces all Qt boilerplate automatically.

## The --from-header Pipeline

This is the primary mode for universal modules:

```
C++ impl header (your code)
       │
       ▼
parseImplHeader() — extracts public methods, maps C++ types to LIDL types
       │
       ▼
ModuleDecl (internal AST)
       │
       ├──► <name>_qt_glue.h      — Plugin class + ProviderObject with typed wrappers
       │
       └──► <name>_dispatch.cpp    — callMethod() dispatch + getMethods() metadata
```

### Command

```bash
logos-cpp-generator --from-header src/<name>_impl.h \
  --backend qt \
  --impl-class <ImplClassName> \
  --impl-header <name>_impl.h \
  --metadata metadata.json \
  --output-dir ./generated_code
```

| Flag | Description |
|------|-------------|
| `--from-header <path>` | Path to the pure C++ impl header |
| `--backend qt` | Generate Qt plugin glue (currently the only backend) |
| `--impl-class <name>` | Name of the C++ implementation class (PascalCase + Impl) |
| `--impl-header <name>` | Header filename (for include directives in generated code) |
| `--metadata <path>` | Path to metadata.json (provides name, version, description) |
| `--output-dir <path>` | Directory for generated files |

### Generated Files

**`<name>_qt_glue.h`** — Contains two classes:
1. **ProviderObject** — Inherits `LogosProviderBase`. Holds `m_impl` (your impl class). Each public method gets a typed wrapper that converts Qt params to C++ std params, calls `m_impl.method(...)`, and converts the return value back.
2. **Plugin** — `QObject` subclass with `Q_PLUGIN_METADATA` and `Q_INTERFACES`. Factory method `createProviderObject()` returns a new ProviderObject.

**`<name>_dispatch.cpp`** — Implements two methods on the ProviderObject:
1. `callMethod(methodName, args)` — String-based dispatch table mapping method names to typed calls
2. `getMethods()` — Returns `QJsonArray` of method metadata (name, signature, returnType, parameters)

### Type Mapping Table

| C++ type | LIDL type | Qt mapping |
|----------|-----------|------------|
| `std::string` / `const std::string&` | `tstr` | `QString` |
| `bool` | `bool` | `bool` |
| `int64_t` | `int` | `int` |
| `uint64_t` | `uint` | `int` |
| `double` | `float64` | `double` |
| `void` | `void` | `void` |
| `std::vector<std::string>` | `[tstr]` | `QStringList` |
| `std::vector<uint8_t>` | `bstr` | `QByteArray` |
| `std::vector<int64_t>` | `[int]` | `QVariantList` |
| Anything else | `any` | `QVariant` |

## LIDL (Alternative Input Format)

LIDL is a lightweight Interface Definition Language. Instead of parsing a C++ header, you write a `.lidl` file:

```
module crypto_utils {
    version "1.0.0"
    description "Cryptographic utilities"

    method hash(input: tstr) -> tstr
    method verify(input: tstr, hash: tstr) -> bool
    method generateKey(bits: int) -> tstr
    method listAlgorithms() -> [tstr]
}
```

Both paths (C++ header and LIDL) produce identical generated output. Use `--from-header` for most modules; use LIDL when you want to define the interface before writing the implementation.

## Common Issues

- **Unknown type warning**: If the generator encounters a C++ type not in the mapping table, it maps to `any` (`QVariant`). Prefer explicit types from the table.
- **Class not found**: `--impl-class` must exactly match the class name in the header (case-sensitive).
- **metadata.json mismatch**: The `name` in metadata.json must match the expected plugin binary name.
- **Generated files not found by CMake**: Ensure `generated_code/` files are listed in `CMakeLists.txt` SOURCES and the directory is in INCLUDE_DIRS.

## In CMakeLists.txt

```cmake
logos_module(
    NAME my_module
    SOURCES
        src/my_module_impl.h
        src/my_module_impl.cpp
        generated_code/my_module_qt_glue.h
        generated_code/my_module_dispatch.cpp
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/generated_code
)
```

## In flake.nix

The generator runs in `preConfigure`, before CMake:

```nix
preConfigure = ''
  logos-cpp-generator --from-header src/my_module_impl.h \
    --backend qt --impl-class MyModuleImpl \
    --impl-header my_module_impl.h \
    --metadata metadata.json --output-dir ./generated_code
'';
```

---

# Logos Core Conventions

## Two Component Types

Logos has two fundamentally different types of components. Always identify which you are building before writing code.

**Logos Modules** (`"type": "core"`) — Process-isolated backend services. Pure C++ implementation using standard types. No Qt types in user code. All Qt glue is generated at build time. Loaded by `logoscore` or `liblogos_core`. Each runs in its own `logos_host` subprocess.

**UI Apps** (`"type": "ui"`) — Qt plugins loaded directly by Basecamp. Provide a graphical widget in the MDI workspace. Use `IComponent` for C++ plugins or QML packages. Run in the Basecamp process.

**Rule:** Never mix these. A module is either core (headless, universal interface) or UI (visual, IComponent). If something needs both backend logic and a UI, create a core module for the logic and a separate UI app that calls it via LogosAPI.

## Module Naming

- Module names use `snake_case`: `crypto_utils`, `accounts_module`, `storage_module`
- Impl class is `PascalCase` + `Impl`: `CryptoUtilsImpl`, `AccountsModuleImpl`
- Impl header is `<name>_impl.h`: `crypto_utils_impl.h`
- Plugin binary is `<name>_plugin.so/.dylib`: `crypto_utils_plugin.so`
- The `name` in `metadata.json` must match the binary name prefix exactly

## File Structure

Universal module:
```
my_module/
├── src/
│   ├── my_module_impl.h          # Public API (pure C++ types)
│   └── my_module_impl.cpp        # Implementation
├── metadata.json                 # "interface": "universal", "type": "core"
├── CMakeLists.txt                # logos_module() macro
├── flake.nix                     # preConfigure runs logos-cpp-generator
└── tests/
```

UI app:
```
my_app/
├── src/
│   ├── MyAppPlugin.h/cpp         # IComponent implementation
│   ├── MyAppBackend.h/cpp        # QObject backend
│   └── qml/Main.qml              # QML UI
├── metadata.json                 # "type": "ui"
├── CMakeLists.txt
└── flake.nix
```

## metadata.json Is the Source of Truth

Every module and UI app must have a `metadata.json`. It declares identity, type, interface, dependencies, and build configuration. The `name` field must match the binary name prefix. The `dependencies` array must list exact `name` values from dependent modules' own `metadata.json` files.

## Inter-Module Communication

All cross-module calls go through `LogosAPI`:
```cpp
LogosResult result = api->callModule("module_name", "method_name", {arg1, arg2});
if (result.success()) {
    QVariant data = result.data();
}
```

Always handle the case where a target module is not loaded. Always declare dependencies in `metadata.json`.

---

# metadata.json Schema

## Full Schema

```json
{
  "name": "my_module",
  "version": "1.0.0",
  "description": "What this module does",
  "author": "Author Name",
  "type": "core",
  "interface": "universal",
  "category": "general",
  "main": "my_module_plugin",
  "dependencies": [],
  "include": [],
  "capabilities": [],

  "nix": {
    "packages": {
      "build": [],
      "runtime": []
    },
    "external_libraries": [],
    "cmake": {
      "find_packages": [],
      "extra_sources": [],
      "extra_include_dirs": [],
      "extra_link_libraries": []
    }
  }
}
```

## Required Fields

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Module identifier. Must match binary prefix: `my_module` -> `my_module_plugin.so` |
| `version` | string | Semantic version (`"1.0.0"`) |
| `type` | string | `"core"` for modules, `"ui"` for UI apps |
| `main` | string | Plugin binary name without extension: `"my_module_plugin"` |

## Universal Module Fields

| Field | Value | Description |
|-------|-------|-------------|
| `interface` | `"universal"` | Signals that this module uses pure C++ impl + code generation |

When `"interface": "universal"` is set, the build system expects `logos-cpp-generator --from-header` to run in `preConfigure` and produce Qt glue files.

## Dependencies

```json
"dependencies": ["storage_module", "crypto_module"]
```

Values must match the `name` field in the dependency module's own `metadata.json`. The runtime loads dependencies before the module.

Flake input attribute names should also match the dependency module names when possible. Example: if you depend on `storage_module` from repo `logos-storage-module`, the flake input should be named `logos-storage-module`.

## External Libraries

```json
"nix": {
  "external_libraries": [
    {
      "name": "mylib",
      "build_command": "make static-library",
      "output_pattern": "build/libmylib.*"
    }
  ]
}
```

For Go libraries, add `"go_build": true`. The external library source is provided as a non-flake input in `flake.nix` and mapped via `externalLibInputs`.

## Nix Packages

```json
"nix": {
  "packages": {
    "build": ["pkg-config"],
    "runtime": ["nlohmann_json", "openssl"]
  }
}
```

`build` packages are available during compilation only. `runtime` packages are linked and available at runtime.

## CMake Configuration

```json
"nix": {
  "cmake": {
    "find_packages": ["Threads", "OpenSSL"],
    "extra_sources": ["src/helper.cpp"],
    "extra_include_dirs": ["include"],
    "extra_link_libraries": ["Threads::Threads"]
  }
}
```

These values are passed to CMake by the `logos_module()` macro. They supplement, not replace, the automatic SDK and Qt dependencies.

## UI App Specific Fields

```json
{
  "type": "ui",
  "icon": "icon.png",
  "category": "tools"
}
```

UI apps do not use `"interface": "universal"` — they are hand-written Qt plugins with `IComponent`.

---

# Nix Build Patterns

## All Builds Go Through Nix

Never run raw `cmake` without `nix develop` or `ws develop`. The Nix build system provides Qt, the SDK, the code generator, and all dependencies. Running `cmake --build` outside Nix will fail.

## Flake Structure for Universal Modules

```nix
{
  description = "My Logos Module";

  inputs = {
    logos-module-builder.url = "github:logos-co/logos-module-builder";
    nix-bundle-lgx.url = "github:logos-co/nix-bundle-lgx";
  };

  outputs = inputs@{ logos-module-builder, ... }:
    logos-module-builder.lib.mkLogosModule {
      src = ./.;
      configFile = ./metadata.json;
      flakeInputs = inputs;
      preConfigure = ''
        logos-cpp-generator --from-header src/<name>_impl.h \
          --backend qt \
          --impl-class <ImplClassName> \
          --impl-header <name>_impl.h \
          --metadata metadata.json \
          --output-dir ./generated_code
      '';
    };
}
```

The `preConfigure` hook runs the code generator before CMake. This is required for universal modules.

## Flake Structure for Modules with External Libraries

Add the external library as a non-flake input and pass it via `externalLibInputs`:

```nix
inputs = {
  logos-module-builder.url = "github:logos-co/logos-module-builder";
  nix-bundle-lgx.url = "github:logos-co/nix-bundle-lgx";
  my-lib = {
    url = "github:org/my-lib/commit-hash";
    flake = false;
  };
};

outputs = inputs@{ logos-module-builder, ... }:
  logos-module-builder.lib.mkLogosModule {
    src = ./.;
    configFile = ./metadata.json;
    flakeInputs = inputs;
    externalLibInputs = {
      mylib = inputs.my-lib;
    };
    preConfigure = ''
      logos-cpp-generator --from-header src/<name>_impl.h \
        --backend qt --impl-class <ClassName> \
        --impl-header <name>_impl.h \
        --metadata metadata.json --output-dir ./generated_code
    '';
  };
```

## Build Commands

```bash
nix build                          # Build the module
nix build .#lib                    # Build just the shared library
nix flake check -L                 # Run tests
nix develop                        # Enter dev shell with build tools
```

Inside the workspace (multi-repo):
```bash
ws build my-module                 # Build
ws build my-module --auto-local    # Build with local dirty dep overrides
ws test my-module                  # Run tests
ws test my-module --auto-local     # Test with local overrides
ws develop my-module               # Enter dev shell
```

## Key Rules

- Flake inputs must be tracked by git. Run `git add <file>` before Nix can see new files.
- `nixpkgs` follows `logos-cpp-sdk` via `logos-module-builder`. Never pin a separate nixpkgs.
- Qt version is fixed by `logos-cpp-sdk`. All repos must use the same Qt to avoid version conflicts.
- Use `-L` flag to stream build logs: `nix build -L`
- Use `--override-input` to test with local dependency changes (the `ws` CLI does this for you with `--auto-local`).

## Dev Shell for CMake Iteration

```bash
nix develop
cmake -B build -GNinja && cmake --build build
```

The dev shell provides all build dependencies. Use this for rapid C++ iteration without full Nix rebuilds.

---

# Testing Logos Modules

## Two Testing Approaches

### 1. Unit Tests (Universal Modules)

Universal modules have a plain C++ impl class with no framework dependencies. Test it directly:

```cpp
#include "my_module_impl.h"
#include <cassert>

int main() {
    MyModuleImpl impl;
    assert(impl.hash("hello") == "expected_hash");
    assert(impl.verify("hello", "expected_hash") == true);
    return 0;
}
```

For the SDK test framework, use `LOGOS_TEST_MAIN()`:

```cpp
#include "my_module_impl.h"
#include "logos_test.h"

LOGOS_TEST_MAIN()

TEST(MyModule, HashWorks) {
    MyModuleImpl impl;
    EXPECT_FALSE(impl.hash("hello").empty());
}
```

Add unit tests to the flake by including a `checks` output or a `tests/` directory with its own `CMakeLists.txt`.

### 2. Integration Tests with logoscore

Test the module as a loaded plugin via the headless runtime:

```bash
# Load module and call a method
logoscore -m ./result/lib -l my_module \
  -c "my_module.doSomething(test_input)"

# Multiple sequential calls
logoscore -m ./result/lib -l my_module \
  -c "my_module.init(config)" \
  -c "my_module.process(data)"

# Load multiple modules (deps resolved automatically)
logoscore -m ./result/lib -l my_module,other_module \
  -c "my_module.callOther(hello)"
```

logoscore arguments:
- `-m <path>` — Directory to scan for module plugins (repeatable)
- `-l <mod1,mod2>` — Comma-separated modules to load
- `-c "<module>.<method>(args)"` — Call a method (repeatable, sequential)
- `--quit-on-finish` — Exit after calls complete (for CI)

Type auto-detection in `-c` args: `true`/`false` -> bool, `42` -> int, `3.14` -> double, else -> string. Use `@filename` to load file content as an argument.

### TEST_GROUPS

The test runner supports groups for selective testing:

```bash
TEST_GROUPS=basic ws test logos-test-modules --auto-local
TEST_GROUPS=ipc ws test logos-test-modules --auto-local
TEST_GROUPS=basic,ipc,errors ws test logos-test-modules --auto-local
```

### Running Tests via Nix

```bash
# Run all checks defined in the flake
nix flake check -L

# In the workspace
ws test my-module
ws test my-module --auto-local     # with local dep overrides
ws test --all --type cpp           # all C++ repos
```

## Key Testing Rules

- Unit tests should NOT require logoscore — instantiate the impl class directly
- Integration tests verify the full plugin lifecycle (load, call, response)
- Always test with `--quit-on-finish` in CI to ensure the process exits
- 30-second timeout per `-c` call; exit code 1 on failure
- After adding `checks` to a repo's `flake.nix`, run `ws sync-graph` so the workspace discovers them

---

# UI App Development

## IComponent Pattern

UI Apps are Qt plugins loaded directly by Basecamp. They implement `IComponent`:

```cpp
class IComponent {
public:
    virtual ~IComponent() = default;
    virtual QWidget* createWidget(LogosAPI* logosAPI = nullptr) = 0;
    virtual void destroyWidget(QWidget* widget) = 0;
};
```

The plugin class inherits both `QObject` and `IComponent`, and uses `Q_PLUGIN_METADATA`:

```cpp
class MyPlugin : public QObject, public IComponent {
    Q_OBJECT
    Q_INTERFACES(IComponent)
    Q_PLUGIN_METADATA(IID IComponent_iid FILE "metadata.json")
public:
    QWidget* createWidget(LogosAPI* logosAPI = nullptr) override;
    void destroyWidget(QWidget* widget) override;
};
```

## C++/QML Boundary Rules

This is the most important convention for UI apps. Every piece of logic must go in the right layer:

| Concern | C++ (backend class) | QML |
|---------|---------------------|-----|
| Data models, state | `Q_PROPERTY` on `QObject` | Bind to `backend.property` |
| Business logic | Methods on backend class | Never — no JS business logic |
| Module calls | `LogosAPI::callModule()` | `logos.callModule()` (thin wrapper) |
| File I/O, networking | Always C++ | Never |
| UI layout, styling | Never | Always use `Logos.Theme`, `Logos.Controls` |
| User interactions | `Q_INVOKABLE` slots | `onClicked: backend.doThing()` |
| Plugin lifecycle | `IComponent::createWidget/destroyWidget` | N/A |

## Backend Class Pattern

The backend is a `QObject` subclass exposed to QML as a context property:

```cpp
class MyBackend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
public:
    explicit MyBackend(LogosAPI* api, QObject* parent = nullptr);
    QVariantList items() const;

    Q_INVOKABLE void addItem(const QString& name);
    Q_INVOKABLE void removeItem(int index);

signals:
    void itemsChanged();

private:
    LogosAPI* m_api;
    QVariantList m_items;
};
```

In `createWidget()`, set the backend as a context property on the QML engine:

```cpp
QWidget* MyPlugin::createWidget(LogosAPI* logosAPI) {
    auto* widget = new QQuickWidget;
    auto* backend = new MyBackend(logosAPI, widget);
    widget->rootContext()->setContextProperty("backend", backend);
    widget->setSource(QUrl("qrc:/qml/Main.qml"));
    return widget;
}
```

## QML Conventions

- Entry point is always `Main.qml`
- Use `Logos.Theme` for all colors: `Logos.Theme.backgroundColor`, `Logos.Theme.textColor`
- Use `Logos.Controls` for interactive elements: `LogosButton`, `LogosText`
- Never hardcode colors — always use theme properties
- Access the backend via the `backend` context property
- Use declarative bindings over imperative JavaScript
- Root element should use `anchors.fill: parent`

## Calling Logos Modules

From C++ backend:
```cpp
QVariant result = m_api->callModule("storage", "save", {key, value});
```

From QML (via LogosQmlBridge):
```qml
logos.callModule("storage", "save", [key, value])
```

Always declare module dependencies in `metadata.json` so they are loaded before the UI app.

## metadata.json for UI Apps

```json
{
  "name": "my_app",
  "type": "ui",
  "version": "1.0.0",
  "description": "My UI application",
  "icon": "icon.png",
  "category": "tools",
  "dependencies": ["storage_module"]
}
```

---

# Universal Module Development

## The Universal Interface Pattern

Universal modules use **pure C++** for their implementation. You write a single implementation class using standard C++ types. The build system generates all Qt/plugin infrastructure automatically via `logos-cpp-generator --from-header`.

**You write:** A C++ class with `std::string`, `int64_t`, `bool`, `std::vector<T>`.
**The generator produces:** Qt plugin class, method dispatch, introspection metadata.

## Rules

- **NO Qt types** in your impl header or implementation: no `QString`, `QObject`, `Q_INVOKABLE`, `QVariant`
- **NO Qt includes** in your impl header (Qt headers in `.cpp` are OK if needed for internal use, but the public API must be pure C++)
- Set `"interface": "universal"` in `metadata.json`
- Name the impl class `<PascalCaseName>Impl` (e.g., `CryptoUtilsImpl`)
- Name the impl header `<name>_impl.h` (e.g., `crypto_utils_impl.h`)
- Only `public` methods become module API methods. Private/protected are ignored by the generator.
- Constructors, destructors, typedefs, and using declarations are skipped by the generator.

## Type Mapping

| Use this in your C++ | Generator maps to | Qt type produced |
|----------------------|-------------------|-----------------|
| `std::string` / `const std::string&` | `tstr` | `QString` |
| `bool` | `bool` | `bool` |
| `int64_t` | `int` | `int` |
| `uint64_t` | `uint` | `int` |
| `double` | `float64` | `double` |
| `void` | `void` | `void` |
| `std::vector<std::string>` | `[tstr]` | `QStringList` |
| `std::vector<uint8_t>` | `bstr` | `QByteArray` |
| `std::vector<int64_t>` | `[int]` | `QVariantList` |
| `std::vector<double>` | `[float64]` | `QVariantList` |
| `std::vector<bool>` | `[bool]` | `QVariantList` |

If you use a type not in this table, the generator maps it to `any` (`QVariant`). Prefer explicit types from the table for type safety.

## Impl Header Template

```cpp
#pragma once
#include <string>
#include <vector>
#include <cstdint>

class MyModuleImpl {
public:
    MyModuleImpl();
    ~MyModuleImpl();

    std::string doSomething(const std::string& input);
    bool validate(const std::string& data);
    int64_t count();
    std::vector<std::string> listItems();

private:
    // Private members are not exposed as module API
};
```

## Build Pipeline

The `flake.nix` `preConfigure` hook runs the generator before CMake:

```bash
logos-cpp-generator --from-header src/<name>_impl.h \
  --backend qt \
  --impl-class <ImplClassName> \
  --impl-header <name>_impl.h \
  --metadata metadata.json \
  --output-dir ./generated_code
```

This produces `generated_code/<name>_qt_glue.h` and `generated_code/<name>_dispatch.cpp`. These files are listed in `CMakeLists.txt` under the `SOURCES` of `logos_module()`.

## Testing

Unit tests instantiate the impl class directly — it is a plain C++ class:

```cpp
#include "my_module_impl.h"
// No Qt test framework needed for basic tests
MyModuleImpl impl;
assert(impl.doSomething("test") == "expected");
```

Integration tests use `logoscore`:
```bash
logoscore -m ./result/lib -l my_module -c "my_module.doSomething(test)"
```

## Quick Reference

### Build Commands

```bash
nix build                    # Build the module/app
nix build -L                 # Build with streaming logs
nix flake check -L           # Run tests
nix develop                  # Enter dev shell
```

### Testing

```bash
# Integration test with logoscore
logoscore -m ./result/lib -l snake_game -c "snake_game.methodName(args)"

# Inspect module metadata and methods
lm ./result/lib/snake_game_plugin.so
lm methods ./result/lib/snake_game_plugin.so --json
```

### Packaging

```bash
lgx create snake_game
lgx add snake_game.lgx -v linux-x86_64 -f ./result/lib/snake_game_plugin.so
lgx verify snake_game.lgx
```

### For detailed documentation, see the logos-dev-boost docs/ directory
### or activate a skill: create-universal-module, wrap-external-lib,
### create-ui-app, package-lgx, inter-module-comm, testing-modules,
### nix-flake-setup, add-to-workspace

<!-- END:logos-dev-boost -->

## Claude Code Skills

The following skills are available in `.claude/skills/`:

- **add-to-workspace** — Activate when registering a new module in the logos-workspace. Covers adding git submodules, flake.nix inputs with follows declarations, scripts/ws REPOS array, repo groups, and dep-graph.nix regeneration.
- **create-ui-app** — Activate when creating a Logos Basecamp UI app with IComponent, C++ backend, and QML frontend. Covers the plugin class, QObject backend, QML entry point, and the C++/QML boundary.
- **create-universal-module** — Activate when creating a new Logos module with the universal C++ interface. Covers scaffolding, metadata.json, flake.nix with code generator, CMakeLists.txt, and the pure C++ impl header.
- **inter-module-comm** — Activate when implementing inter-module communication in Logos. Covers LogosAPI::callModule(), LogosResult handling, dependency declaration in metadata.json, generated client stubs, and the QML bridge.
- **nix-flake-setup** — Activate when configuring Nix flake.nix for a Logos module. Covers flake inputs, follows declarations, preConfigure for code generation, mkLogosModule, overrides for local development, and workspace integration.
- **package-lgx** — Activate when packaging a Logos module or UI app for distribution as an LGX package. Covers lgx create, adding platform variants, verification, portable builds, and installation via lgpm.
- **testing-modules** — Activate when writing tests for Logos modules. Covers unit testing universal modules (direct impl class testing), logoscore integration tests, TEST_GROUPS, mock transport, and Nix check configuration.
- **wrap-external-lib** — Activate when wrapping an external C or C++ library as a Logos universal module. Covers external_libraries in metadata.json, flake input configuration, extern C includes, and the Go library special case.

## MCP Server

If configured in `.mcp.json`, the `logos-dev-boost` MCP server provides:
- `logos_project_info` — Project metadata and build targets
- `logos_search_docs` — Search Logos documentation
- `logos_api_reference` — API reference for LogosAPI, LogosResult, type system
- `logos_build_help` — Context-aware build commands and troubleshooting
- `logos_scaffold` — Generate new module/app from template