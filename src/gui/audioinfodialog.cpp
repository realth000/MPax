#include "audioinfodialog.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "audio/audioinfo.h"
#include "ui_audioinfodialog.h"
#include "util/cssloader.h"
#include "util/fileutil.h"

AudioInfoDialog::AudioInfoDialog(PlayContent *playContent, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::AudioInfoDialog),
      m_content(playContent),
      m_modified(false),
      m_headerList(QStringList{tr("Name"), tr("Value")}) {
  ui->setupUi(this);
  initUi();
  initConnections();
}

AudioInfoDialog::~AudioInfoDialog() { delete ui; }

void AudioInfoDialog::initUi() {
  this->setWindowTitle(tr("Property - ") + m_content->title);

  setTabOrder(ui->okButton, ui->cancelButton);
  setTabOrder(ui->cancelButton, ui->okButton);

  ui->statusBar->setVisible(false);

  ui->metadataTab->setFocusPolicy(Qt::NoFocus);
  ui->metadataTable->setFocusPolicy(Qt::NoFocus);
  ui->detailTable->setFocusPolicy(Qt::NoFocus);
  ui->coverTab->setFocusPolicy(Qt::NoFocus);
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

  ui->metadataTable->setItem(0, 1, new QTableWidgetItem(m_content->title));
  ui->metadataTable->setItem(1, 1, new QTableWidgetItem(m_content->artist));
  ui->metadataTable->setItem(2, 1, new QTableWidgetItem(m_content->albumTitle));
  ui->metadataTable->setItem(3, 1,
                             new QTableWidgetItem(m_content->albumArtist));
  if (m_content->albumYear > 0) {
    ui->metadataTable->setItem(
        4, 1, new QTableWidgetItem(QString::number(m_content->albumYear)));
  }
  if (m_content->trackNumber > 0) {
    ui->metadataTable->setItem(
        5, 1, new QTableWidgetItem(QString::number(m_content->trackNumber)));
  }
  if (m_content->albumTrackCount > 0) {
    ui->metadataTable->setItem(
        6, 1,
        new QTableWidgetItem(QString::number(m_content->albumTrackCount)));
  }
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
  ui->detailTable->setItem(3, 1, new QTableWidgetItem(0));
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
  ui->detailTable->setItem(8, 1, new QTableWidgetItem(m_content->channels));
  ui->detailTable->setItem(
      9, 1,
      new QTableWidgetItem(QString::number(m_content->bitRate) + " kbps"));
  qDebug("sample rate=%d, bit rate=%d, time=%d\n", m_content->sampleRate,
         m_content->bitRate, m_content->length);
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
