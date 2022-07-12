#include "tableviewex.h"

#include <QtCore/QDebug>
#include <QtWidgets/QHeaderView>

TableViewEx::TableViewEx(QWidget *parent) : QTableView(parent) {
  connect(horizontalHeader(), &QHeaderView::sectionResized, this,
          &TableViewEx::resizeColumnWidth);
  horizontalHeader()->setStretchLastSection(true);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setShowGrid(false);
}

void TableViewEx::resizeEvent(QResizeEvent *event) {
  if (event->oldSize() == event->size()) {
    return;
  }
  // FIXME: Avoid signal here without reconnect.
  disconnect(horizontalHeader(), &QHeaderView::sectionResized, this,
             &TableViewEx::resizeColumnWidth);
  setColumnWidth(0, m_columnWidthRatio[0] * this->width());
  setColumnWidth(1, m_columnWidthRatio[1] * this->width());
  // FIXME: Do not let need horizontal scrollbar.
  setColumnWidth(2, m_columnWidthRatio[2] * this->width());
  connect(horizontalHeader(), &QHeaderView::sectionResized, this,
          &TableViewEx::resizeColumnWidth);
}

void TableViewEx::setColumnWidthRatio(const QList<qreal> &ratioList) {
  m_columnWidthRatio = ratioList;
}

void TableViewEx::setColumnWidthRatio(const int row, const qreal &ratio) {
  if (m_columnWidthRatio.length() <= row) {
    return;
  }
  m_columnWidthRatio[row] = ratio;
}

void TableViewEx::resizeColumnWidth(int logicalIndex, int oldSize,
                                    int newSize) {
  // FIXME: Temporarily disable resizing here.
  m_columnWidthRatio.clear();
  for (int i = 0; i < this->model()->rowCount(); i++) {
    if (columnWidth(i) > 0)
      m_columnWidthRatio.append((10.0 + columnWidth(i)) / this->width());
  }
}
