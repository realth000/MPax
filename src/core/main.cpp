#include <QtCore/QTranslator>
#include <QtWidgets/QApplication>

#include "gui/mainui.h"
#ifdef Q_OS_WINDOWS
#include <QtGui/QFont>
#endif

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
#ifdef Q_OS_WINDOWS
  QFont appFont;
  appFont.setFamily("Microsoft Yahei");
  a.setFont(appFont);
#endif
  // Setup locale.
  QLocale locale = QLocale::system();
  QTranslator appTranslator;
  switch (locale.script()) {
    case QLocale::SimplifiedChineseScript:
#if 1
      appTranslator.load(QLatin1String("./translation/zh_CN.qm"));
#else
      appTranslator.load(QLatin1String("./translation/en.qm"));
#endif
      break;
    default:
      appTranslator.load(QLatin1String("./translation/en.qm"));
  }
  QCoreApplication::installTranslator(&appTranslator);
  MainUI w;
  w.show();
  return a.exec();
}
