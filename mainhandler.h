#ifndef MAINHANDLER_H
#define MAINHANDLER_H

#include "types.h"
#include "openedlisthandle.h"
#include <QObject>
#include <string>

class OpenedListHandle;

class MainHandler : public QObject
{
  Q_OBJECT
public:
  int col_count;
  unsigned int max_lists;
  std::map<Qt::Key, ButtonHandle<MainHandler>> tool_btts;
  std::vector<OpenedListHandle> opened_lists;
  QGridLayout *main_grid;
  explicit MainHandler(QObject *parent = 0);
signals:
  void ch_list(bool);

public slots:
  void copy();
  void remove();
  void move();
  void rename();
  void view();
  void list_added();
  void list_removed();
};

#endif // MAINHANDLER_H
