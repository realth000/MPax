#ifndef AUDIOINFODIALOG_H
#define AUDIOINFODIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTableWidgetItem>

#include "core/playcontent.h"

namespace Ui {
class AudioInfoDialog;
}

class AudioInfoDialog : public QDialog {
  Q_OBJECT

 public:
  explicit AudioInfoDialog(PlayContent *playContent, QWidget *parent = nullptr);
  ~AudioInfoDialog();

 private:
  Ui::AudioInfoDialog *ui;
  PlayContent *m_content;
  bool m_modified;
  const QStringList m_headerList;

  // Metadata table item
  QTableWidgetItem *m_titleItem;
  QTableWidgetItem *m_artistItem;
  QTableWidgetItem *m_albumTitleItem;
  QTableWidgetItem *m_albumArtistItem;
  QTableWidgetItem *m_albumYearItem;
  QTableWidgetItem *m_trackNumberItem;
  QTableWidgetItem *m_genreItem;
  QTableWidgetItem *m_commentItem;

  QLineEdit *readonlyLineEdit(const QString &text);
  void initUi();
  void initConnections();
  void initMetadataTable();
  void initDetailTable();

 private slots:
  void saveAudioInfo();
};

#endif  // AUDIOINFODIALOG_H
