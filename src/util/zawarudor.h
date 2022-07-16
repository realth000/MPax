#ifndef ZAWARUDOR_H
#define ZAWARUDOR_H
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

namespace util {
static void zawarudo(const int &milliseconds) {
  QEventLoop loop;
  QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
  loop.exec();
}
}  // namespace util
#endif  // ZAWARUDOR_H