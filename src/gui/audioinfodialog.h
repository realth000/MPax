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
  const QStringList m_coverHeaderList;

  void initUi();
  void initConnections();
  void initMetadataTable();
  void initDetailTable();
  void initCoverTable();

 private slots:
  void saveAudioInfo();
};

#endif  // AUDIOINFODIALOG_H
