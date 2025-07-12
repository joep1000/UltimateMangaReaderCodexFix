#include "mangainfowidget.h"

#include <QResizeEvent>
#include <QScrollBar>

#include <QStandardItemModel>
#include <QToolButton>
#include <QMenu>
#include <QComboBox>
#include "ui_mangainfowidget.h"

MangaInfoWidget::MangaInfoWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::MangaInfoWidget), currentmanga(), manageDownloadsButton(nullptr),
      languageBox(nullptr), selectedChapter(-1)
{
    ui->setupUi(this);
    adjustUI();
}

MangaInfoWidget::~MangaInfoWidget()
{
    delete ui;
}

void MangaInfoWidget::adjustUI()
{
    ui->pushButtonReadContinue->setProperty("type", "borderless");
    ui->pushButtonReadFirst->setProperty("type", "borderless");
    ui->pushButtonReadLatest->setProperty("type", "borderless");

    ui->pushButtonReadContinue->setFixedHeight(SIZES.buttonSize);
    ui->pushButtonReadFirst->setFixedHeight(SIZES.buttonSize);
    ui->pushButtonReadLatest->setFixedHeight(SIZES.buttonSize);

    ui->toolButtonAddFavorites->setFixedSize(SIZES.buttonSizeToggleFavorite, SIZES.buttonSizeToggleFavorite);
    ui->toolButtonAddFavorites->setIconSize(
        QSize(SIZES.buttonSizeToggleFavorite * 0.8, SIZES.buttonSizeToggleFavorite * 0.8));
    ui->toolButtonDownload->setFixedSize(SIZES.buttonSizeToggleFavorite, SIZES.buttonSizeToggleFavorite);
    ui->toolButtonDownload->setIconSize(
        QSize(SIZES.buttonSizeToggleFavorite * 0.8, SIZES.buttonSizeToggleFavorite * 0.8));

    if (!manageDownloadsButton)
    {
        manageDownloadsButton = new QToolButton(this);
        manageDownloadsButton->setIcon(QIcon(":/images/icons/file-error.png"));
        manageDownloadsButton->setFixedSize(SIZES.buttonSizeToggleFavorite, SIZES.buttonSizeToggleFavorite);
        manageDownloadsButton->setIconSize(QSize(SIZES.buttonSizeToggleFavorite * 0.8,
                                                SIZES.buttonSizeToggleFavorite * 0.8));
        manageDownloadsButton->setPopupMode(QToolButton::InstantPopup);
        QMenu *menu = new QMenu(manageDownloadsButton);
        menu->addAction("Delete Chapter", this, SLOT(on_actionDeleteChapter()));
        menu->addAction("Delete All", this, SLOT(on_actionDeleteAll()));
        menu->addAction("Delete Read", this, SLOT(on_actionDeleteRead()));
        manageDownloadsButton->setMenu(menu);
        ui->horizontalLayout_6->addWidget(manageDownloadsButton);
    }

    if (!languageBox)
    {
        languageBox = new QComboBox(this);
        languageBox->setVisible(false);
        languageBox->setFixedHeight(SIZES.buttonSize);
        connect(languageBox, SIGNAL(currentIndexChanged(int)), this,
                SLOT(on_comboBoxLanguage_currentIndexChanged(int)));
        ui->horizontalLayout_6->insertWidget(2, languageBox);
    }

    ui->labelMangaInfoCover->setScaledContents(true);

    ui->labelMangaInfoTitle->setStyleSheet("font-size: 16pt");

    // set labels bold
    ui->labelMangaInfoLabelAuthor->setProperty("type", "mangainfolabel");
    ui->labelMangaInfoLabelArtist->setProperty("type", "mangainfolabel");
    ui->labelMangaInfoLabelGenres->setProperty("type", "mangainfolabel");
    ui->labelMangaInfoLabelStaus->setProperty("type", "mangainfolabel");

    ui->labelMangaInfoLabelAuthorContent->setProperty("type", "mangainfocontent");
    ui->labelMangaInfoLabelArtistContent->setProperty("type", "mangainfocontent");
    ui->labelMangaInfoLabelGenresContent->setProperty("type", "mangainfocontent");
    ui->labelMangaInfoLabelStausContent->setProperty("type", "mangainfocontent");

    activateScroller(ui->scrollAreaMangaInfoSummary);
    activateScroller(ui->listViewChapters);
    ui->listViewChapters->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

inline void updateLabel(QLabel *caption, QLabel *content, const QString &text)
{
    bool hide = text.length() <= 1;
    caption->setHidden(hide);
    content->setHidden(hide);

    content->setText(text);
}

void MangaInfoWidget::setManga(QSharedPointer<MangaInfo> manga)
{
    if (currentmanga != manga)
    {
        currentmanga.clear();
        currentmanga = manga;

        QObject::connect(currentmanga.get(), &MangaInfo::updatedSignal, this, &MangaInfoWidget::updateManga);

        QObject::connect(currentmanga.get(), &MangaInfo::coverLoaded, this, &MangaInfoWidget::updateCover);
    }

    selectedLanguage = "All";
    updateInfos();
    updateCover();
}

void MangaInfoWidget::updateManga(bool)
{
    qDebug() << "updated" << currentmanga->title;

    updateInfos();
}

void MangaInfoWidget::updateCover()
{
    if (!QFile::exists(currentmanga->coverPath))
    {
        ui->labelMangaInfoCover->clear();
    }
    else
    {
        QPixmap img = QPixmap::fromImage(loadQImageFast(currentmanga->coverPath));
        double r = (double)img.height() / img.width();

        if (r >= ((double)SIZES.coverHeight / SIZES.coverWidth))
            ui->labelMangaInfoCover->setFixedSize(SIZES.coverHeight / r, SIZES.coverHeight);
        else
            ui->labelMangaInfoCover->setFixedSize(SIZES.coverWidth, SIZES.coverWidth * r);

        ui->labelMangaInfoCover->setPixmap(img);
    }
}

void MangaInfoWidget::refreshInfos()
{
    updateInfos();
}

void MangaInfoWidget::updateInfos()
{
    selectedChapter = -1;
    QStandardItemModel *model = new QStandardItemModel(this);
    ReadingProgress progress(currentmanga->hostname, currentmanga->title);

    QStringList langList = currentmanga->languages;
    languageBox->clear();
    if (langList.size() > 1)
    {
        languageBox->addItem("All");
        languageBox->addItems(langList);
        languageBox->setVisible(true);
        int idx = languageBox->findText(selectedLanguage);
        if (idx < 0)
            idx = 0;
        languageBox->setCurrentIndex(idx);
    }
    else
    {
        languageBox->setVisible(false);
        selectedLanguage = "All";
    }

    for (int i = 0; i < currentmanga->chapters.size(); i++)
    {
        const auto &ch = currentmanga->chapters[i];
        if (!selectedLanguage.isEmpty() && selectedLanguage != "All" && ch.language != selectedLanguage)
            continue;

        QString text = QString("%1: %2").arg(i + 1).arg(ch.chapterTitle);
        DownloadImageDescriptor dd("", currentmanga->title, i, 0);
        if (QFile::exists(currentmanga->mangaSource->getImagePath(dd)))
            text = QChar(0x2713) + QString(" ") + text;

        QStandardItem *item = new QStandardItem(text);
        QFont f = item->font();
        if (i >= progress.index.chapter)
            f.setBold(true);
        item->setFont(f);
        item->setData(i); // store chapter index
        model->insertRow(0, item);
    }

    if (ui->listViewChapters->model() != nullptr)
        ui->listViewChapters->model()->deleteLater();

    ui->listViewChapters->setModel(model);

    ui->labelMangaInfoTitle->setText(currentmanga->title);

    updateLabel(ui->labelMangaInfoLabelAuthor, ui->labelMangaInfoLabelAuthorContent, currentmanga->author);
    updateLabel(ui->labelMangaInfoLabelArtist, ui->labelMangaInfoLabelArtistContent, currentmanga->artist);
    updateLabel(ui->labelMangaInfoLabelGenres, ui->labelMangaInfoLabelGenresContent, currentmanga->genres);
    updateLabel(ui->labelMangaInfoLabelStaus, ui->labelMangaInfoLabelStausContent, currentmanga->status);

    ui->labelMangaInfoLabelSummaryContent->setText(currentmanga->summary);

    ui->scrollAreaMangaInfoSummary->verticalScrollBar()->setValue(0);
    ui->listViewChapters->verticalScrollBar()->setValue(0);

    bool enable = currentmanga->chapters.count() > 0;

    ui->pushButtonReadContinue->setEnabled(enable);
    ui->pushButtonReadFirst->setEnabled(enable);
    ui->pushButtonReadLatest->setEnabled(enable);
    ui->toolButtonDownload->setVisible(enable);
    ui->toolButtonAddFavorites->setVisible(enable);
}

void MangaInfoWidget::setFavoriteButtonState(bool state)
{
    ui->toolButtonAddFavorites->setChecked(state);
}

void MangaInfoWidget::on_toolButtonAddFavorites_clicked()
{
    if (!currentmanga.isNull())
        emit toggleFavoriteClicked(currentmanga);
}

void MangaInfoWidget::on_listViewChapters_clicked(const QModelIndex &index)
{
    selectedChapter = index.data().toInt();
    emit readMangaClicked({selectedChapter, 0});
}

void MangaInfoWidget::on_pushButtonReadLatest_clicked()
{
    emit readMangaClicked({static_cast<int>(currentmanga->chapters.count() - 1), 0});
}

void MangaInfoWidget::on_pushButtonReadContinue_clicked()
{
    emit readMangaContinueClicked();
}

void MangaInfoWidget::on_pushButtonReadFirst_clicked()
{
    emit readMangaClicked({0, 0});
}

void MangaInfoWidget::on_toolButtonDownload_clicked()
{
    if (!currentmanga.isNull())
        emit downloadMangaClicked();
}

void MangaInfoWidget::on_actionDeleteChapter()
{
    if (!currentmanga.isNull() && selectedChapter >= 0)
        emit deleteChapterClicked(selectedChapter);
}

void MangaInfoWidget::on_actionDeleteAll()
{
    if (!currentmanga.isNull())
        emit deleteAllClicked();
}

void MangaInfoWidget::on_actionDeleteRead()
{
    if (!currentmanga.isNull())
        emit deleteReadClicked();
}

void MangaInfoWidget::on_comboBoxLanguage_currentIndexChanged(int index)
{
    if (!languageBox->isVisible())
        return;
    selectedLanguage = languageBox->itemText(index);
    updateInfos();
}
