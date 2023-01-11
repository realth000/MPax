#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QtGui/QKeyEvent>
#include <QtWidgets/QDialog>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog {
 Q_OBJECT

 public:
  explicit ProgressDialog(const QString &workName, QWidget *parent = nullptr);
  ~ProgressDialog();
  void setCancelable(bool cancelable);
  bool cancelable() const;
  void setInfinite(bool infinite);
  bool infinite() const;
  void setMax(const int &max);
  void setFinish();
  void setWorkName(const QString &workName);

 signals:
  void canceled();
  void succeed();

 public slots:
  void updateProgress(const int &finishedCount);

 protected:
  virtual void keyPressEvent(QKeyEvent *event) override;

 private:
  Ui::ProgressDialog *ui;
  bool m_cancelable;
  bool m_infinite;
  int m_max;

  void successAndQuit();

 private slots:
  void quitCheck();
};

#endif  // PROGRESSDIALOG_H
