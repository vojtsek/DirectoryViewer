#ifndef MAINHANDLER_H
#define MAINHANDLER_H

#include "types.h"
#include "openedlisthandle.h"
#include <QObject>
#include <string>

extern int size_in, max_lists, col_count, init_count;
extern std::string init_dir, home_path;
class OpenedListHandle;

class MainHandler : public QObject
{
  Q_OBJECT
  void prepare_cmd(cmd_info_T &, bool &, bool &, bool, OpenedListHandle *&);
public:
  static enum {B, KB, MB, GB} sz;
  std::map<Qt::Key, ButtonHandle<MainHandler>> tool_btts;
  std::vector<OpenedListHandle *> opened_lists;
  QGridLayout *main_grid;
  explicit MainHandler(QObject *parent = 0);
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
  void list_added();
  void list_removed();
};

#endif // MAINHANDLER_H
