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

 signals:
  void updatePlayContentRequested(PlayContent *playContent);

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
  QTableWidgetItem *m_albumTotalTrackItem;
  QTableWidgetItem *m_genreItem;
  QTableWidgetItem *m_commentItem;

  QLineEdit *readonlyLineEdit(const QString &text);
  QLineEdit *readonlyLineEdit(const int &text);
  QLineEdit *readonlyLineEdit(const int &text, const QString &suffix);
  void initUi();
  void initConnections();
  void initMetadataTable();
  void initDetailTable();

 private slots:
  void saveAudioInfo();
};

#endif  // AUDIOINFODIALOG_H
