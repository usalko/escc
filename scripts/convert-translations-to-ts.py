#!/usr/bin/env python3

import json
from pathlib import Path
from xml.sax.saxutils import escape

ROOT = Path(__file__).resolve().parents[2]
TRANSLATIONS_ROOT = ROOT / "src" / "translations"
OUTPUT_DIR = ROOT / "escc" / "src" / "i18n"
NAMESPACES = ("common", "groups", "hints", "log", "settings")


def locale_to_qt(locale: str) -> str:
    return locale.lower().replace("-", "_")


def load_json(file_path: Path):
    with file_path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def main() -> None:
    if not TRANSLATIONS_ROOT.exists():
        raise RuntimeError(f"Translations folder not found: {TRANSLATIONS_ROOT}")

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    locales = sorted(path.name for path in TRANSLATIONS_ROOT.iterdir() if path.is_dir())
    if "en" not in locales:
        raise RuntimeError("Base locale 'en' is required")

    sources = {ns: load_json(TRANSLATIONS_ROOT / "en" / f"{ns}.json") for ns in NAMESPACES}

    for locale in locales:
        locale_dir = TRANSLATIONS_ROOT / locale
        lines = [
            '<?xml version="1.0" encoding="utf-8"?>',
            f'<TS version="2.1" language="{escape(locale_to_qt(locale))}">',
        ]

        for namespace in NAMESPACES:
            locale_file = locale_dir / f"{namespace}.json"
            if not locale_file.exists():
                continue

            translated = load_json(locale_file)
            lines.append("  <context>")
            lines.append(f"    <name>{escape(f'esc.{namespace}')}</name>")

            for key, source_text in sources[namespace].items():
                source_value = str(source_text)
                translation_value = str(translated.get(key, source_value))
                lines.append("    <message>")
                lines.append(f"      <source>{escape(source_value)}</source>")
                lines.append(f"      <comment>{escape(f'{namespace}.{key}')}</comment>")
                lines.append(f"      <translation>{escape(translation_value)}</translation>")
                lines.append("    </message>")

            lines.append("  </context>")

        lines.append("</TS>")
        lines.append("")
        output_file = OUTPUT_DIR / f"escc_{locale_to_qt(locale)}.ts"
        output_file.write_text("\n".join(lines), encoding="utf-8")

    print(f"Generated {len(locales)} TS files into {OUTPUT_DIR}")


if __name__ == "__main__":
    main()
