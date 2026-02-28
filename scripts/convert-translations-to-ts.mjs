#!/usr/bin/env node

import fs from 'node:fs';
import path from 'node:path';

const repoRoot = path.resolve(process.argv[2] ?? path.join(process.cwd(), '..'));
const translationsRoot = path.join(repoRoot, 'src', 'translations');
const outputDir = path.join(repoRoot, 'escc', 'src', 'i18n');

const namespaces = ['common', 'groups', 'hints', 'log', 'settings'];

function xmlEscape(value) {
  return value
    .replaceAll('&', '&amp;')
    .replaceAll('<', '&lt;')
    .replaceAll('>', '&gt;')
    .replaceAll('"', '&quot;')
    .replaceAll("'", '&apos;');
}

function readJson(filePath) {
  const content = fs.readFileSync(filePath, 'utf8');
  return JSON.parse(content);
}

function localeToQt(locale) {
  return locale.toLowerCase().replace('-', '_');
}

function buildTsForLocale(locale, sourcesByNamespace) {
  const localeDir = path.join(translationsRoot, locale);
  const localeCode = localeToQt(locale);
  const lines = [];

  lines.push('<?xml version="1.0" encoding="utf-8"?>');
  lines.push(`<TS version="2.1" language="${xmlEscape(localeCode)}">`);

  for (const ns of namespaces) {
    const localeFile = path.join(localeDir, `${ns}.json`);
    if (!fs.existsSync(localeFile)) {
      continue;
    }

    const localeJson = readJson(localeFile);
    const sourceJson = sourcesByNamespace.get(ns) ?? {};

    lines.push('  <context>');
    lines.push(`    <name>${xmlEscape(`esc.${ns}`)}</name>`);

    const keys = Object.keys(sourceJson);
    for (const key of keys) {
      const sourceText = String(sourceJson[key] ?? '');
      const translatedText = localeJson[key] !== undefined ? String(localeJson[key]) : sourceText;

      lines.push('    <message>');
      lines.push(`      <source>${xmlEscape(sourceText)}</source>`);
      lines.push(`      <comment>${xmlEscape(`${ns}.${key}`)}</comment>`);
      lines.push(`      <translation>${xmlEscape(translatedText)}</translation>`);
      lines.push('    </message>');
    }

    lines.push('  </context>');
  }

  lines.push('</TS>');
  lines.push('');
  return lines.join('\n');
}

function main() {
  if (!fs.existsSync(translationsRoot)) {
    throw new Error(`Translations folder not found: ${translationsRoot}`);
  }

  fs.mkdirSync(outputDir, { recursive: true });

  const locales = fs
    .readdirSync(translationsRoot, { withFileTypes: true })
    .filter((entry) => entry.isDirectory())
    .map((entry) => entry.name)
    .sort((a, b) => a.localeCompare(b));

  if (!locales.includes('en')) {
    throw new Error('Base locale "en" is required for source strings.');
  }

  const sourcesByNamespace = new Map();
  for (const ns of namespaces) {
    const sourceFile = path.join(translationsRoot, 'en', `${ns}.json`);
    if (!fs.existsSync(sourceFile)) {
      throw new Error(`Missing base source file: ${sourceFile}`);
    }
    sourcesByNamespace.set(ns, readJson(sourceFile));
  }

  for (const locale of locales) {
    const tsContent = buildTsForLocale(locale, sourcesByNamespace);
    const outputFile = path.join(outputDir, `escc_${localeToQt(locale)}.ts`);
    fs.writeFileSync(outputFile, tsContent, 'utf8');
  }

  console.log(`Generated TS files for ${locales.length} locales in ${outputDir}`);
}

main();
