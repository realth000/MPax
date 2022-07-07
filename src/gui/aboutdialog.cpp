#include "aboutdialog.h"

#include <QtCore/QDebug>
#include <QtWidgets/QScrollBar>

#include "ui_aboutdialog.h"
#include "util/cssloader.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutDialog) {
  ui->setupUi(this);
  this->setStyleSheet(util::loadCssFromFile(":/css/aboutdialog.css"));
  ui->infoTextBrowser->verticalScrollBar()->setStyleSheet(
      ":/css/aboutdialog.css");
  qDebug() << APP_COMMIT_TIME << APP_COMMIT << APP_COMMIT_LONG;
  QString aboutText;
  const QString name = QString(
      "<h1><a href=\"https://github.com/realth000/MPax\">"
      "<font color=#007b50>MPax</font></a></h1>");
  const QString introduction =
      QString("<p>" + tr("MPax is a simple music player.") + "</p>");
  const QString commitUrl =
      QString("<p>" + tr("Build commit: ") +
              "<a href=\"https://github.com/realth000/MPax/commit/%1\">"
              "<font color=#007b50>%2</font></a></p>")
          .arg(APP_COMMIT_LONG, APP_COMMIT);
  const QString commitTime =
      QString("<p>" + tr("Build commit time: ") + "%1</p>")
          .arg(APP_COMMIT_TIME);
  const QString depHead =
      QString("<h2>" + tr("MPax uses the libraries bellow:") + "</h2>");
  const QString depQt = QString(
                            "<p><a href=\"https://www.qt.io\">"
                            "<font color=#007b50>Qt</font></a>" +
                            tr(": ") + "%1</p>")
                            .arg(QT_VERSION_STR);
  const QString depQHotKey = QString(
      "<p><a href=\"https://github.com/Skycoder42/QHotkey\">"
      "<font color=#007b50>Skycoder42/QHotkey</font></a></p>");
  ui->infoTextBrowser->setReadOnly(true);
  ui->infoTextBrowser->setContextMenuPolicy(Qt::NoContextMenu);
  ui->infoTextBrowser->setOpenExternalLinks(true);
  aboutText += name;
  aboutText += introduction;
  aboutText += commitUrl;
  aboutText += commitTime;
  aboutText += depHead;
  aboutText += depQt;
  aboutText += depQHotKey;
  ui->infoTextBrowser->setText(aboutText);
}

AboutDialog::~AboutDialog() { delete ui; }
