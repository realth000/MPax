#include "aboutdialog.h"

#include <QtCore/QDebug>
#include <QtWidgets/QScrollBar>

#include "ui_aboutdialog.h"
#include "util/cssloader.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutDialog) {
  ui->setupUi(this);
  this->setStyleSheet(
      Util::loadCssFromFile({":/css/base.css", ":/css/aboutdialog.css"}));
  ui->infoTextBrowser->verticalScrollBar()->setStyleSheet(
      Util::loadCssFromFile({":/css/base.css", ":/css/aboutdialog.css"}));
  QString aboutText;
  const QString name = QString(
      "<h1><a href=\"https://github.com/realth000/MPax\">"
      "<font color=#007b50>MPax</font></a></h1>");
  const QString introduction =
      QString(tr("MPax is a simple and easy-to-use music player.") + "<br>");
  const QString license =
      QString(tr("MPax is licensed under the term of GPL v3 license."));
  const QString infoHeader = QString("<h2>" + tr("Version info") + "</h2>");
  const QString versionTagUrl =
      QString(
          tr("Version: ") +
              "<a "
              "href=\"https://github.com/realth000/MPax/releases/tag/v%1\"><font "
              "color=#007b50>%1</font></a><br>")
          .arg(QString(APP_VERSION_TAG).replace("v", ""));
  const QString commitUrl =
      QString(tr("Build: ") +
          "<a href=\"https://github.com/realth000/MPax/commit/%1\">"
          "<font color=#007b50>%2%3</font></a><br>")
          .arg(APP_COMMIT_LONG, APP_COMMIT,
               QString(APP_RELEASE_COUNT) != "1"
               ? "-release" + QString(APP_RELEASE_COUNT)
               : "");
  const QString commitTime =
      QString(tr("Build time: ") + "%1").arg(APP_COMMIT_TIME);
  const QString depHead =
      QString("<h2>" + tr("MPax uses the libraries bellow:") + "</h2>");
  const QString depQt =
      QString(
          "<h3><a href=\"https://www.qt.io\">"
          "<font color=#007b50>Qt" +
              tr(": ") +
              "%1</font></a></h3>Qt is the fastest and "
              "smartest way to produce industry-leading "
              "software that users love.<br>Qt is licensed under GNU (L)GPL.")
          .arg(QT_VERSION_STR);
  const QString depQHotKey = QString(
      "<h3><a href=\"https://github.com/Skycoder42/QHotkey\">"
      "<font color=#007b50>Skycoder42/QHotkey</font></a></h3> A global "
      "shortcut/hotkey for Desktop Qt-Applications.<br>Licensed under "
      "the BSD-3-Clause license.");
  const QString depTagLib = QString(
      "<h3><a href=\"https://taglib.org/\">"
      "<font color=#007b50>TagLib</font></a></h3>TagLib is a library for "
      "reading and editing the meta-data of several popular audio "
      "formats.<br>TagLib is distributed under the GNU Lesser General Public "
      "License "
      "(LGPL) and Mozilla Public License (MPL).");
  const QString depQSS = QString(
      "<h3><a href=\"https://github.com/GTRONICK/QSS\"><font "
      "color=#007b50>GTRONICK/QSS</font></a></h3>QT Style Sheets "
      "templates.<br>Licensed under the "
      "MIT license.");
  ui->infoTextBrowser->setReadOnly(true);
  ui->infoTextBrowser->setContextMenuPolicy(Qt::NoContextMenu);
  ui->infoTextBrowser->setOpenExternalLinks(true);
  aboutText += name;
  aboutText += introduction;
  aboutText += license;
  aboutText += infoHeader;
  aboutText += versionTagUrl;
  aboutText += commitUrl;
  aboutText += commitTime;
  aboutText += depHead;
  aboutText += depQt;
  aboutText += depQHotKey;
  aboutText += depTagLib;
  aboutText += depQSS;
  ui->infoTextBrowser->setText(aboutText);
}

AboutDialog::~AboutDialog() { delete ui; }
