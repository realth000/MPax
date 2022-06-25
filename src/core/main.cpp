#include <QApplication>

#include "gui/mainui.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainUI w;
  w.show();
  return a.exec();
}
