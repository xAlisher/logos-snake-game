# logos-snake-game

A Snake game module for Logos Basecamp, built as a **logos-dev-boost evaluation pilot**.

## Goal

Evaluate the full logos-dev-boost development toolchain by building a real module end-to-end:
- Scaffold → Build → Test → Package → Deploy → Run

The snake game is intentionally simple — the value is in documenting what works, what doesn't, and why.

## What's Here

| Component | Pattern | Status |
|-----------|---------|--------|
| **snake-ui** | Pure QML UI plugin | Working in Basecamp |
| **high-score-module** (universal) | logos-cpp-generator + LogosProviderBase | Builds, but incompatible with our runtime |
| **high-score-module** (PluginInterface) | Manual Q_INVOKABLE (keycard pattern) | In progress |

## Key Finding

There is a three-way incompatibility between the tooling, public releases, and our working runtime. See [REPORT.md](REPORT.md) for the full evaluation (21 tools assessed, 25 findings).

## Reports

- **[REPORT.md](REPORT.md)** — Tool-by-tool evaluation report for Logos core devs
- **[PROJECT_KNOWLEDGE.md](PROJECT_KNOWLEDGE.md)** — All 25 findings with severity, details, and recommendations

## How to Run

```bash
# Snake game (QML UI — works in our Basecamp)
cp plugins/snake-ui/* ~/.local/share/Logos/LogosBasecamp/plugins/snake-ui/
# Launch Basecamp, click "snake" in sidebar

# High score module (universal interface — builds but can't run)
cd high-score-module
nix build        # Builds .so
nix build .#lgx  # Produces LGX package
```

## Issues

See [issues](https://github.com/xAlisher/logos-snake-game/issues) for step-by-step progress (Steps 1-9).
