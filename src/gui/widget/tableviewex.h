#ifndef MPAX_TABLEVIEWEX_H
#define MPAX_TABLEVIEWEX_H

#include <QtWidgets/QTableView>

class TableViewEx : public QTableView {
 Q_OBJECT
 public:
  explicit TableViewEx(QWidget *parent = nullptr);

 protected:
  QList<qreal> m_columnWidthRatio;
};

#endif  // MPAX_TABLEVIEWEX_H
