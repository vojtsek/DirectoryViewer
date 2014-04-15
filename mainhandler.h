#ifndef MAINHANDLER_H
#define MAINHANDLER_H

#include "types.h"
#include "openedlisthandle.h"
#include <map>
#include <QObject>
#include <string>

class OpenedListHandle;

class MainHandler : public QObject
{
  Q_OBJECT
  void prepare_cmd(cmd_info_T &, bool &, bool &, bool, OpenedListHandle *&);
public:
  std::map<Qt::Key, ButtonHandle<MainHandler>> tool_btts;
  std::vector<OpenedListHandle *> opened_lists;
  explicit MainHandler(QObject *parent = 0);
  void loadSettings(std::shared_ptr<Data>);
  void loadApps(std::shared_ptr<Data>);
  void refreshLists(OpenedListHandle *, bool);

signals:
  void ch_list(bool);
  void confirm1(std::string, cmd_info_T &);
  void confirm2(std::string, cmd_info_T &);
  void error(std::string &);

public slots:
  void copy();
  void remove();
  void move();
  void rename();
  void create();
  void refresh(OpenedListHandle *);
  void list_added();
  void list_removed();
};

#endif // MAINHANDLER_H
