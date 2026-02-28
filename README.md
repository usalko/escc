# ESC Configurator C++ (`escc`)

`escc` is the native C++/Qt version of ESC Configurator.
It is being built as a replacement for the web version, with a focus on better performance, improved stability, and no browser limitations.

## Who this app is for

In short, `escc` is for pilots and hobbyists who want to:

- connect to a flight controller over serial;
- read and update ESC settings;
- flash BLHeli_S / Bluejay / AM32 firmware;
- use logs, startup melody tools, and core maintenance features.

## Current status

The project is under active development.
Core UI and major logic are already migrated, with some tasks still in progress.

## Quick start (Linux)

### 1) Install dependencies

At minimum, you need:

- CMake 3.21+
- a C++17 compiler (`g++` or `clang++`)
- Qt 6 modules: `Core`, `Widgets`, `Network`, `SerialPort`, `LinguistTools`

`Qt Multimedia` is optional (used for melody preview).

### 2) Build

From the `escc` directory:

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j$(nproc)
```

### 3) Run

```bash
./escc
```

## Run tests

From `escc/build`:

```bash
ctest --output-on-failure
```

## Translations (i18n)

The app supports runtime language switching.

- Source JSON translations: `../src/translations/<locale>/*.json`
- Qt `.ts` files: `src/i18n/*.ts`

### Update `.ts` from JSON

From repository root:

```bash
python3 escc/scripts/convert-translations-to-ts.py
```

### Build `.qm` files

From `escc/build`:

```bash
cmake --build . --target release_translations
```

## Useful notes

- App settings are stored using `QSettings`.
- Serial features depend on the `Qt SerialPort` module.
- If `Qt LinguistTools` is not available, the app still builds, but `.qm` generation is disabled.

## Feedback

If you find a bug or want to suggest an improvement, please open an issue in the main repository.

