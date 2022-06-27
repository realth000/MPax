#include <QApplication>

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
  MainUI w;
  w.show();
  return a.exec();
}
