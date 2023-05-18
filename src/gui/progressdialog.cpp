#include "progressdialog.h"

#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>

#include "ui_progressdialog.h"
#include "util/cssloader.h"
#include "util/zawarudor.h"

ProgressDialog::ProgressDialog(const QString &workName, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ProgressDialog),
      m_cancelable(true),
      m_infinite(false),
      m_max(100) {
  ui->setupUi(this);
  this->setStyleSheet(Util::loadCssFromFile(
      {":/css/external/MaterialDark.css", ":/css/base.css"}));
  ui->progressBar->setMaximum(m_max);
  ui->cancelButton->setFocusPolicy(Qt::NoFocus);
  this->setWindowTitle(tr("Processing"));
  ui->workLabel->setText(workName);
  connect(ui->cancelButton, &QPushButton::clicked, this,
          &ProgressDialog::quitCheck);
}

ProgressDialog::~ProgressDialog() { delete ui; }

void ProgressDialog::setCancelable(bool cancelable) {
  m_cancelable = cancelable;
  ui->cancelButton->setVisible(cancelable);
}

bool ProgressDialog::cancelable() const { return m_cancelable; }

void ProgressDialog::setInfinite(bool infinite) {
  m_infinite = infinite;
  if (infinite) {
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);
  } else {
    ui->progressBar->setMaximum(100);
  }
}

bool ProgressDialog::infinite() const { return m_infinite; }

void ProgressDialog::setMax(const int &max) {
  ui->progressBar->setMaximum(100);
  m_max = max;
}

void ProgressDialog::setFinish() {
  if (!m_infinite) {
    ui->progressBar->setValue(100);
  }
  successAndQuit();
}

void ProgressDialog::setWorkName(const QString &workName) {
  ui->workLabel->setText(workName);
}

void ProgressDialog::updateProgress(const int &finishedCount) {
  if (m_infinite || m_max == 0) {
    ui->progressLabel->setText(QString::number(finishedCount));
    return;
  }
  ui->progressBar->setValue(100 * finishedCount / m_max);
  ui->progressLabel->setText(QString::number(finishedCount) + "/" +
      QString::number(m_max));
  if (finishedCount >= m_max) {
    successAndQuit();
  }
}

void ProgressDialog::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape) {
    quitCheck();
    return;
  }
  qDebug() << event->modifiers() << event->key();
  QDialog::keyPressEvent(event);
}

void ProgressDialog::successAndQuit() {
  ui->cancelButton->setEnabled(false);
  emit succeed();
  Util::zawarudo(300);
  // this->close();
}

void ProgressDialog::quitCheck() {
  const int choose = QMessageBox::warning(this, tr("Cancel"),
                                          tr("Sure to cancel this progress?"),
                                          tr("No"), tr("Yes"));
  if (choose == 1) {
    emit canceled();
    this->close();
  }
}
