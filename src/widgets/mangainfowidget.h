#ifndef MANGAINFOWIDGET_H
#define MANGAINFOWIDGET_H

#include <QIcon>
#include <QWidget>

#include "imageprocessingnative.h"
#include "mangaindex.h"
#include "mangainfo.h"
#include "sizes.h"
#include "utils.h"

namespace Ui
{
class MangaInfoWidget;
}

class MangaInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MangaInfoWidget(QWidget *parent = nullptr);
    ~MangaInfoWidget();

    void setManga(QSharedPointer<MangaInfo> manga);
    void setFavoriteButtonState(bool state);

signals:
    void toggleFavoriteClicked(QSharedPointer<MangaInfo> manga);
    void readMangaClicked(const MangaIndex &index);
    void readMangaContinueClicked();
    void downloadMangaClicked();
    void deleteChapterClicked(int chapter);
    void deleteAllClicked();
    void deleteReadClicked();

public slots:
    void updateManga(bool newchapters);
    void updateCover();
    void refreshInfos();

private slots:
    void on_toolButtonAddFavorites_clicked();

    void on_listViewChapters_clicked(const QModelIndex &index);

    void on_pushButtonReadLatest_clicked();
    void on_pushButtonReadContinue_clicked();
    void on_pushButtonReadFirst_clicked();

    void on_toolButtonDownload_clicked();
    void on_actionDeleteChapter();
    void on_actionDeleteAll();
    void on_actionDeleteRead();
    void on_comboBoxLanguage_currentIndexChanged(int index);

private:
    Ui::MangaInfoWidget *ui;

    QSharedPointer<MangaInfo> currentmanga;
    QToolButton *manageDownloadsButton;
    QComboBox *languageBox;
    int selectedChapter;
    QString selectedLanguage;

    void adjustUI();

    void updateInfos();
};

#endif  // MANGAINFOWIDGET_H
