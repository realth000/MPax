#include "tableviewex.h"

#include <QtCore/QDebug>
#include <QtWidgets/QHeaderView>

TableViewEx::TableViewEx(QWidget *parent)
    : QTableView(parent), m_columnWidthRatio(QList<qreal>{}) {
  horizontalHeader()->setStretchLastSection(true);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setShowGrid(false);
}
