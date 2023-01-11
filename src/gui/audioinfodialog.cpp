#include "audioinfodialog.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtWidgets/QLabel>

#include "audio/audioinfo.h"
#include "ui_audioinfodialog.h"
#include "util/cssloader.h"
#include "util/fileutil.h"

AudioInfoDialog::AudioInfoDialog(PlayContent *playContent, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::AudioInfoDialog),
      m_content(playContent),
      m_modified(false),
      m_headerList(QStringList{tr("Name"), tr("Value")}),
      m_coverHeaderList(QStringList{tr("Name"), tr("Image"), tr("Info")}) {
  ui->setupUi(this);
  initUi();
  initConnections();
}

AudioInfoDialog::~AudioInfoDialog() { delete ui; }

void AudioInfoDialog::initUi() {
  this->setWindowTitle(tr("Property - ") + m_content->title);
  Util::loadCssFromFile(":/css/base.css");

  ui->detailTable->setShowGrid(false);
  ui->metadataTable->setShowGrid(false);
  ui->coverTable->setShowGrid(false);

  setTabOrder(ui->okButton, ui->cancelButton);
  setTabOrder(ui->cancelButton, ui->okButton);

  ui->statusBar->setVisible(false);

  ui->metadataTab->setFocusPolicy(Qt::NoFocus);
  ui->metadataTable->setFocusPolicy(Qt::NoFocus);
  ui->detailTable->setFocusPolicy(Qt::NoFocus);
  ui->coverTab->setFocusPolicy(Qt::NoFocus);
  ui->coverTable->setFocusPolicy(Qt::NoFocus);
  //  ui->cancelButton->setFocusPolicy(Qt::NoFocus);
  ui->okButton->setFocus();

  ui->tab->setCurrentIndex(0);
  ui->tab->setTabText(0, tr("Meta data"));
  ui->tab->setTabText(1, tr("Detail"));
  ui->tab->setTabText(2, tr("Cover"));

  ui->toolButton->setEnabled(false);
  ui->applyButton->setEnabled(false);

  initMetadataTable();
  initDetailTable();
  initCoverTable();
}

void AudioInfoDialog::initConnections() {
  connect(ui->okButton, &QPushButton::clicked, this, [this]() {
    this->saveAudioInfo();
    this->close();
  });
  connect(ui->cancelButton, &QPushButton::clicked, this,
          [this]() { this->close(); });
  connect(ui->applyButton, &QPushButton::clicked, this,
          &AudioInfoDialog::saveAudioInfo);
}

void AudioInfoDialog::initMetadataTable() {
  ui->metadataTable->verticalHeader()->setVisible(false);
  ui->metadataTable->setAlternatingRowColors(true);
  ui->metadataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->metadataTable->setSelectionMode(QAbstractItemView::SingleSelection);
  //  ui->metadataTable->setEditTriggers(QAbstractItemView::SelectedClicked |
  //                                     QAbstractItemView::DoubleClicked);

  ui->metadataTable->setColumnCount(2);
  ui->metadataTable->setColumnWidth(0, ui->metadataTable->width() / 3);
  //  ui->metadataTable->setColumnWidth(1, ui->metadataTable->width() / 3);
  ui->metadataTable->horizontalHeader()->setStretchLastSection(true);

  ui->metadataTable->setHorizontalHeaderLabels(m_headerList);
  ui->metadataTable->setRowCount(9);

  ui->metadataTable->setItem(0, 0, new QTableWidgetItem(tr("Title")));
  ui->metadataTable->setItem(1, 0, new QTableWidgetItem(tr("Artist")));
  ui->metadataTable->setItem(2, 0, new QTableWidgetItem(tr("Album title")));
  ui->metadataTable->setItem(3, 0, new QTableWidgetItem(tr("Album artist")));
  ui->metadataTable->setItem(4, 0, new QTableWidgetItem(tr("Album year")));
  ui->metadataTable->setItem(5, 0, new QTableWidgetItem(tr("Track number")));
  ui->metadataTable->setItem(6, 0,
                             new QTableWidgetItem(tr("Album total tracks")));
  ui->metadataTable->setItem(7, 0, new QTableWidgetItem(tr("Genre")));
  ui->metadataTable->setItem(8, 0, new QTableWidgetItem(tr("Comment")));

  for (int i = 0; i < ui->metadataTable->rowCount(); i++) {
    ui->metadataTable->item(i, 0)->setFlags(
        ui->metadataTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
  }

  ui->metadataTable->setItem(0, 1, new QTableWidgetItem(m_content->title));
  ui->metadataTable->setItem(1, 1, new QTableWidgetItem(m_content->artist));
  ui->metadataTable->setItem(2, 1, new QTableWidgetItem(m_content->albumTitle));
  ui->metadataTable->setItem(3, 1,
                             new QTableWidgetItem(m_content->albumArtist));
  ui->metadataTable->setItem(
      4, 1,
      new QTableWidgetItem(m_content->albumYear > 0
                           ? QString::number(m_content->albumYear)
                           : ""));
  ui->metadataTable->setItem(
      5, 1,
      new QTableWidgetItem(m_content->trackNumber > 0
                           ? QString::number(m_content->trackNumber)
                           : ""));
  ui->metadataTable->setItem(
      6, 1,
      new QTableWidgetItem(m_content->albumTrackCount > 0
                           ? QString::number(m_content->albumTrackCount)
                           : ""));

  ui->metadataTable->setItem(7, 1, new QTableWidgetItem(m_content->genre));
  ui->metadataTable->setItem(8, 1, new QTableWidgetItem(m_content->comment));
}

void AudioInfoDialog::initDetailTable() {
  ui->detailTable->verticalHeader()->setVisible(false);
  ui->detailTable->setAlternatingRowColors(true);
  ui->detailTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->detailTable->setSelectionMode(QAbstractItemView::SingleSelection);
  //  ui->detailTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

  ui->detailTable->setColumnCount(2);
  ui->detailTable->setColumnWidth(0, ui->detailTable->width() / 3);
  //  ui->detailTable->setColumnWidth(1, ui->detailTable->width() / 3);
  ui->detailTable->horizontalHeader()->setStretchLastSection(true);

  ui->detailTable->setHorizontalHeaderLabels(m_headerList);
  ui->detailTable->setRowCount(10);

  auto *t = new QTableWidgetItem(tr("File name"));
  ui->detailTable->setItem(0, 0, t);
  ui->detailTable->setItem(1, 0, new QTableWidgetItem(tr("Folder name")));
  ui->detailTable->setItem(2, 0, new QTableWidgetItem(tr("File path")));
  // TODO: What's this index.
  ui->detailTable->setItem(3, 0, new QTableWidgetItem(tr("Index")));
  ui->detailTable->setItem(4, 0, new QTableWidgetItem(tr("File size")));
  ui->detailTable->setItem(5, 0, new QTableWidgetItem(tr("Modified time")));
  ui->detailTable->setItem(6, 0, new QTableWidgetItem(tr("Duration")));
  ui->detailTable->setItem(7, 0, new QTableWidgetItem(tr("Sample rate")));
  ui->detailTable->setItem(8, 0, new QTableWidgetItem(tr("Channels")));
  ui->detailTable->setItem(9, 0, new QTableWidgetItem(tr("Bit rate")));

  QFileInfo info(m_content->contentPath);
  ui->detailTable->setItem(0, 1, new QTableWidgetItem(m_content->contentName));
  ui->detailTable->setItem(1, 1,
                           new QTableWidgetItem(info.dir().absolutePath()));
  ui->detailTable->setItem(2, 1, new QTableWidgetItem(m_content->contentPath));
  // TODO: What's this index.
  ui->detailTable->setItem(3, 1, new QTableWidgetItem(QString::number(0)));
  ui->detailTable->setItem(
      4, 1,
      new QTableWidgetItem(
          QString::number(Util::toFileSize(m_content->contentSize,
                                           Util::FileSizeLevel::MB)) +
              QString("MB (%1 %2)")
                  .arg(QString::number(m_content->contentSize), tr("bytes"))));
  ui->detailTable->setItem(5, 1,
                           new QTableWidgetItem(info.lastModified().toString(
                               "yyyy-MM-dd hh:mm:ss")));
  // TODO: need record duration in state.
  ui->detailTable->setItem(
      6, 1,
      new QTableWidgetItem(
          Util::toFormatTime(m_content->length) +
              QString(" (%1 %2)")
                  .arg(QString::number(m_content->length), tr("seconds"))));
  ui->detailTable->setItem(
      7, 1,
      new QTableWidgetItem(QString::number(m_content->sampleRate) + " Hz"));
  ui->detailTable->setItem(
      8, 1, new QTableWidgetItem(QString::number(m_content->channels)));
  ui->detailTable->setItem(
      9, 1,
      new QTableWidgetItem(QString::number(m_content->bitRate) + " kbps"));
  for (int i = 0; i < ui->detailTable->rowCount(); i++) {
    ui->detailTable->item(i, 0)->setFlags(ui->detailTable->item(i, 0)->flags() &
        ~Qt::ItemIsEditable);
    ui->detailTable->item(i, 1)->setFlags(ui->detailTable->item(i, 1)->flags() &
        ~Qt::ItemIsEditable);
  }
}

void AudioInfoDialog::initCoverTable() {
  ui->coverTable->verticalHeader()->setVisible(false);
  ui->coverTable->setAlternatingRowColors(true);
  ui->coverTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->coverTable->setSelectionMode(QAbstractItemView::SingleSelection);

  ui->coverTable->setColumnCount(3);
  ui->coverTable->setColumnWidth(0, 70);
  ui->coverTable->setColumnWidth(1, 100);
  ui->coverTable->horizontalHeader()->setStretchLastSection(true);

  ui->coverTable->setHorizontalHeaderLabels(m_coverHeaderList);
  ui->coverTable->setRowCount(5);
  for (int i = 0; i < ui->coverTable->rowCount(); i++) {
    ui->coverTable->setRowHeight(i, 100);
    ui->coverTable->item(i, 0)->setFlags(ui->coverTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
  }

  ui->coverTable->setItem(0, 0, new QTableWidgetItem(tr("Front Cover")));
  ui->coverTable->setItem(1, 0, new QTableWidgetItem(tr("Back Cover")));
  ui->coverTable->setItem(2, 0, new QTableWidgetItem(tr("Artist")));
  ui->coverTable->setItem(3, 0, new QTableWidgetItem(tr("Disc")));
  ui->coverTable->setItem(4, 0, new QTableWidgetItem(tr("Icon")));

  for (auto cover : m_content->coverList) {
    if (cover == nullptr || cover->image == nullptr) {
      continue;
    }
    auto img = *cover->image;
    auto coverLabel = new QLabel();
    coverLabel->setPixmap(QPixmap::fromImage(img).scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    auto infoString = QString("%1 B\n").arg(img.sizeInBytes());
    infoString.append(QString("%1 x %2\n").arg(img.width()).arg(img.height()));
    infoString.append(cover->mimeType);

    switch (cover->type) {
      case CoverType::FrontCover: {
        ui->coverTable->setCellWidget(0, 1, coverLabel);
        ui->coverTable->setItem(0, 2, new QTableWidgetItem(infoString));
        continue;
      }
      case CoverType::BackCover: {
        ui->coverTable->setCellWidget(1, 1, coverLabel);
        ui->coverTable->setItem(1, 2, new QTableWidgetItem(infoString));
        continue;
      }
      case CoverType::Artist: {
        ui->coverTable->setCellWidget(2, 1, coverLabel);
        ui->coverTable->setItem(2, 2, new QTableWidgetItem(infoString));
        continue;
      }
      case CoverType::Media: {
        ui->coverTable->setCellWidget(3, 1, coverLabel);
        ui->coverTable->setItem(3, 2, new QTableWidgetItem(infoString));
        continue;
      }
      case CoverType::OtherFileIcon: {
        ui->coverTable->setCellWidget(4, 1, coverLabel);
        ui->coverTable->setItem(4, 2, new QTableWidgetItem(infoString));
        continue;
      }
      default: {
        break;
      }
    }
    delete coverLabel;
  }
  for (int i = 0; i < ui->coverTable->rowCount(); i++) {
    ui->coverTable->item(i, 2)->setFlags(ui->coverTable->item(i, 2)->flags() & ~Qt::ItemIsEditable);
  }
}

void AudioInfoDialog::saveAudioInfo() {
  m_content->title = ui->metadataTable->item(0, 1)->text();
  m_content->artist = ui->metadataTable->item(1, 1)->text();
  m_content->albumTitle = ui->metadataTable->item(2, 1)->text();
  m_content->albumArtist = ui->metadataTable->item(3, 1)->text();
  if (ui->metadataTable->item(4, 1)->text().toInt() > 0) {
    m_content->albumYear = ui->metadataTable->item(4, 1)->text().toInt();
  }
  if (ui->metadataTable->item(5, 1)->text().toInt() > 0) {
    m_content->trackNumber = ui->metadataTable->item(5, 1)->text().toInt();
  }
  if (ui->metadataTable->item(6, 1)->text().toInt() > 0) {
    m_content->albumTrackCount = ui->metadataTable->item(6, 1)->text().toInt();
  }
  m_content->genre = ui->metadataTable->item(7, 1)->text();
  m_content->comment = ui->metadataTable->item(7, 1)->text();

  if (AudioInfo::writeAudioInfo(m_content->contentPath, m_content)) {
    emit updatePlayContentRequested(m_content);
  }
}
