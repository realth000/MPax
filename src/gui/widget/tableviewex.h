#ifndef MPAX_TABLEVIEWEX_H
#define MPAX_TABLEVIEWEX_H

#include <QtGui/QResizeEvent>
#include <QtWidgets/QTableView>

class TableViewEx : public QTableView {
  Q_OBJECT
 public:
  explicit TableViewEx(QWidget *parent = nullptr);
  void setColumnWidthRatio(const QList<qreal> &ratioList);
  void setColumnWidthRatio(const int row, const qreal &ratio);

 protected:
  void resizeEvent(QResizeEvent *event) override;
  QList<qreal> m_columnWidthRatio;

 private slots:
  void resizeColumnWidth(int logicalIndex, int oldSize, int newSize);
};

#endif  // MPAX_TABLEVIEWEX_H
