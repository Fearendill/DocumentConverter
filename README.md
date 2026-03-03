# DocumentConverter

![Qt Version](https://img.shields.io/badge/Qt-6.10+-green.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20%20%7C%20-lightgrey.svg)

Une application desktop multi-plateforme pour convertir des documents (TXT, DOCX, ODT, ODS, ODP) en PDF. Développée avec Qt 6.10 et C++17.

## ✨ Fonctionnalités

- **Conversion par lot** : Traitez plusieurs fichiers simultanément
- **Formats supportés** :
  - ✅ TXT / TEXT
  - ✅ DOCX / DOC (via LibreOffice)
  - ✅ ODT / ODS / ODP (via LibreOffice)
  - ✅ EPUB 
- **Interface intuitive** : Glisser-déposer et sélection multiple
- **Progression en temps réel** : Barre de progression et journal détaillé
- **Sans écrasement** : Numérotation automatique des fichiers existants
- **Multi-plateforme** : Windows, Linux, macOS

## 📋 Prérequis

### Obligatoires
- Qt 6.10+ avec modules : `core`, `gui`, `widgets`, `printsupport`
- Compilateur C++17 compatible (GCC, Clang, MSVC, MinGW)

### Recommandés
- **LibreOffice** 7.0+ (pour DOCX/ODT/ODS/ODP)
  - [Télécharger LibreOffice](https://www.libreoffice.org/download/download/)

> 💡 **Note** : Sans LibreOffice, seule la conversion TXT → PDF fonctionne (via QPrinter natif Qt).

## 🚀 Installation

[![GitHub Release](https://img.shields.io/github/v/release/Fearendill/DocumentConverter?style=for-the-badge&logo=github&color=blue&label=Release)](https://github.com/Fearendill/DocumentConverter/releases)
