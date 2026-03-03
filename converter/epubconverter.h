#ifndef EPUBCONVERTER_H
#define EPUBCONVERTER_H

#include "baseconverter.h"

namespace DocumentConverter {

// Classe de conversion pour les fichiers EPUB.  Selon les outils
// disponibles (Calibre, LibreOffice) la conversion est déléguée ou une
// implémentation basée sur Qt est utilisée.
class EpubConverter : public BaseConverter {
public:
    // Constructeur ; parent est passé à QObject pour la gestion de la
    // mémoire par Qt.
    explicit EpubConverter(QObject *parent = nullptr);

    // Conversion principale.  inputPath est le chemin du fichier EPUB à
    // convertir, outputPath le chemin de destination du document
    // généré.
    ConversionResult convert(const QString &inputPath,
                             const QString &outputPath) override;

    // Vérifie si le chemin désigne un fichier supporté (ici un EPUB).
    bool supports(const QString &filePath) const override;

    // Retourne le format source que gère ce convertisseur.
    Format sourceFormat() const override { return Format::EPUB; }


private:
    // Tentative de conversion en utilisant l'outil calibre en ligne de
    // commande (si installé).
    ConversionResult convertwithCalibre(const QString &inputPath, const QString &outputPath);

    // Fallback utilisant LibreOffice / unoconv.  LibreOffice doit être
    // installé pour que cela fonctionne.
    ConversionResult convertWithLibreOffice(const QString &inputPath, const QString &outputPath);

    // Conversion basique réalisée uniquement avec les bibliothèques Qt.
    ConversionResult convertWithQt(const QString &inputPath, const QString &outputPath);

    // Récupère les métadonnées (titre, auteur) d'un EPUB.  Retourne true
    // si l'opération indique réussite.
    bool extractEpubMetadata(const QString &inputPath, QString &title, QString &author);

    // Vérifie la disponibilité de calibre sur le système.
    bool chechCalibreAvailable();
    // Recherche le chemin vers l'exécutable calibre.
    QString findCalibrePath();

    QString m_calibrePath;          // chemin vers calibre
    bool m_calibreAvailable = false; // true si calibre a été trouvé

};

} // namespace DocumentConverter

#endif // EPUBCONVERTER_H