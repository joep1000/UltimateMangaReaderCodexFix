#include "mangachapter.h"

MangaChapter::MangaChapter()
    : chapterTitle(), chapterUrl(), pagesLoaded(false), pageUrlList(), imageUrlList(),
      language()
{
}

MangaChapter::MangaChapter(const QString &title, const QString &url)
    : chapterTitle(title), chapterUrl(url), pagesLoaded(false), pageUrlList(), imageUrlList(),
      language()
{
}

QDataStream &operator<<(QDataStream &str, const MangaChapter &m)
{
    str << m.chapterTitle << m.chapterUrl << m.pagesLoaded << m.pageUrlList << m.imageUrlList
        << m.chapterNumber << m.language;

    return str;
}

QDataStream &operator>>(QDataStream &str, MangaChapter &m)
{
    str >> m.chapterTitle >> m.chapterUrl >> m.pagesLoaded >> m.pageUrlList >> m.imageUrlList >>
        m.chapterNumber;

    if (!str.atEnd())
        str >> m.language;
    else
        m.language.clear();

    return str;
}
