#ifndef MANGADEX_H
#define MANGADEX_H

#include <QSet>

#include "abstractmangasource.h"
#include "mangainfo.h"

#define RAPIDJSON_ASSERT(x) \
    if (!(x))               \
        throw QException();

#include "thirdparty/rapidjson.h"

class MangaDex : public AbstractMangaSource
{
public:
    explicit MangaDex(NetworkManager *dm);
    virtual ~MangaDex() = default;

    bool updateMangaList(UpdateProgressToken *token) override;
    Result<MangaChapterCollection, QString> updateMangaInfoFinishedLoading(
        QSharedPointer<DownloadStringJob> job, QSharedPointer<MangaInfo> info) override;
    Result<QStringList, QString> getPageList(const QString &chapterUrl) override;

    void setLanguageFilter(const QString &lang) { languageFilter = lang; }
    QString getLanguageFilter() const { return languageFilter; }

private:
    void login();
    QString apiUrl;

    QString languageFilter;

    QVector<QString> statuses;
    QVector<QString> demographies;
    QMap<int, QString> genreMap;
};

#endif  // MANGADEX_H
