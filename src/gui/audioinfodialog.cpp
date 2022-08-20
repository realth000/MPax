#include "audioinfodialog.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "audio/audioinfo.h"
#include "ui_audioinfodialog.h"
#include "util/fileutil.h"

AudioInfoDialog::AudioInfoDialog(PlayContent *playContent, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::AudioInfoDialog),
      m_content(playContent),
      m_modified(false),
      m_headerList(QStringList{tr("Name"), tr("Value")}),
      m_titleItem(new QTableWidgetItem()),
      m_artistItem(new QTableWidgetItem()),
      m_albumTitleItem(new QTableWidgetItem()),
      m_albumArtistItem(new QTableWidgetItem()),
      m_albumYearItem(new QTableWidgetItem()),
      m_trackNumberItem(new QTableWidgetItem()),
      m_genreItem(new QTableWidgetItem()),
      m_commentItem(new QTableWidgetItem()) {
  ui->setupUi(this);
  initUi();
  initConnections();
}

AudioInfoDialog::~AudioInfoDialog() { delete ui; }

QLineEdit *AudioInfoDialog::readonlyLineEdit(const QString &text) {
  QLineEdit *lineEdit = new QLineEdit(text);
  lineEdit->setReadOnly(true);
  lineEdit->setStyleSheet("background: transparent;");
  lineEdit->setCursorPosition(0);
  return lineEdit;
}

QLineEdit *AudioInfoDialog::readonlyLineEdit(const int &text) {
  return readonlyLineEdit(QString::number(text));
}

QLineEdit *AudioInfoDialog::readonlyLineEdit(const int &text,
                                             const QString &suffix) {
  return readonlyLineEdit(QString::number(text) + suffix);
}

void AudioInfoDialog::initUi() {
  this->setWindowTitle(tr("Property - ") + m_content->title);
  ui->tab->setCurrentIndex(0);
  ui->tab->setTabText(0, tr("Meta data"));
  ui->tab->setTabText(1, tr("Detail"));
  ui->tab->setTabText(2, tr("Cover"));

  ui->toolButton->setEnabled(false);
  ui->applyButton->setEnabled(false);

  initMetadataTable();
  initDetailTable();

  ui->okButton->setFocus();
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
  ui->metadataTable->setRowCount(8);

  ui->metadataTable->setCellWidget(0, 0, readonlyLineEdit(tr("Title")));
  ui->metadataTable->setCellWidget(1, 0, readonlyLineEdit(tr("Artist")));
  ui->metadataTable->setCellWidget(2, 0, readonlyLineEdit(tr("Album title")));
  ui->metadataTable->setCellWidget(3, 0, readonlyLineEdit(tr("Album artist")));
  ui->metadataTable->setCellWidget(4, 0, readonlyLineEdit(tr("Album year")));
  ui->metadataTable->setCellWidget(5, 0, readonlyLineEdit(tr("Track number")));
  ui->metadataTable->setCellWidget(6, 0, readonlyLineEdit(tr("Genre")));
  ui->metadataTable->setCellWidget(7, 0, readonlyLineEdit(tr("Comment")));

  ui->metadataTable->setItem(0, 1, m_titleItem);
  ui->metadataTable->setItem(1, 1, m_artistItem);
  ui->metadataTable->setItem(2, 1, m_albumTitleItem);
  ui->metadataTable->setItem(3, 1, m_albumArtistItem);
  ui->metadataTable->setItem(4, 1, m_albumYearItem);
  ui->metadataTable->setItem(5, 1, m_trackNumberItem);
  ui->metadataTable->setItem(6, 1, m_genreItem);
  ui->metadataTable->setItem(7, 1, m_commentItem);

  m_titleItem->setText(m_content->title);
  m_artistItem->setText(m_content->artist);
  m_albumTitleItem->setText(m_content->albumTitle);
  m_albumArtistItem->setText(m_content->albumArtist);
  m_albumYearItem->setText(QString::number(m_content->albumYear));
  m_trackNumberItem->setText(QString::number(m_content->trackNumber));
  m_genreItem->setText(m_content->genre);
  m_commentItem->setText(m_content->comment);
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

  ui->detailTable->setCellWidget(0, 0, readonlyLineEdit(tr("File name")));
  ui->detailTable->setCellWidget(1, 0, readonlyLineEdit(tr("Folder name")));
  ui->detailTable->setCellWidget(2, 0, readonlyLineEdit(tr("File path")));
  // TODO: What's this index.
  ui->detailTable->setCellWidget(3, 0, readonlyLineEdit(tr("Index")));
  ui->detailTable->setCellWidget(4, 0, readonlyLineEdit(tr("File size")));
  ui->detailTable->setCellWidget(5, 0, readonlyLineEdit(tr("Modified time")));
  ui->detailTable->setCellWidget(6, 0, readonlyLineEdit(tr("Duration")));
  ui->detailTable->setCellWidget(7, 0, readonlyLineEdit(tr("Sample rate")));
  ui->detailTable->setCellWidget(8, 0, readonlyLineEdit(tr("Channels")));
  ui->detailTable->setCellWidget(9, 0, readonlyLineEdit(tr("Bit rate")));

  QFileInfo info(m_content->contentPath);
  ui->detailTable->setCellWidget(0, 1,
                                 readonlyLineEdit(m_content->contentName));
  ui->detailTable->setCellWidget(1, 1,
                                 readonlyLineEdit(info.dir().absolutePath()));
  ui->detailTable->setCellWidget(2, 1,
                                 readonlyLineEdit(m_content->contentPath));
  // TODO: What's this index.
  ui->detailTable->setCellWidget(3, 1, readonlyLineEdit(0));
  ui->detailTable->setCellWidget(
      4, 1,
      readonlyLineEdit(
          Util::toFileSize(m_content->contentSize, Util::FileSizeLevel::MB),
          QString("MB (%1 %2)")
              .arg(QString::number(m_content->contentSize), tr("bytes"))));
  ui->detailTable->setCellWidget(
      5, 1,
      readonlyLineEdit(info.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
  // TODO: need record duration in state.
  ui->detailTable->setCellWidget(
      6, 1,
      readonlyLineEdit(
          Util::toFormatTime(m_content->length) +
          QString(" (%1 %2)")
              .arg(QString::number(m_content->length), tr("seconds"))));
  ui->detailTable->setCellWidget(
      7, 1, readonlyLineEdit(m_content->sampleRate, " Hz"));
  ui->detailTable->setCellWidget(8, 1, readonlyLineEdit(m_content->channels));
  ui->detailTable->setCellWidget(9, 1,
                                 readonlyLineEdit(m_content->bitRate, " kbps"));
}

void AudioInfoDialog::saveAudioInfo() {
  m_content->title = m_titleItem->text();
  m_content->artist = m_artistItem->text();
  m_content->albumTitle = m_albumTitleItem->text();
  m_content->albumArtist = m_albumArtistItem->text();
  m_content->albumYear = m_albumYearItem->text().toInt();
  m_content->trackNumber = m_trackNumberItem->text().toInt();
  m_content->genre = m_genreItem->text();
  m_content->comment = m_commentItem->text();

  if (AudioInfo::writeAudioInfo(m_content->contentPath, m_content)) {
    emit updatePlayContentRequested(m_content);
  }
}