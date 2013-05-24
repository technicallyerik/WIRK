#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ircmessage.h"
#include "irc_server.h"
#include <QMainWindow>
#include <QList>
#include <QStandardItemModel>
#include "parsed_message.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QList<irc_server*> m_servers;
    QStandardItemModel* generateTree();
    QStandardItemModel* generateUsers(QStringList users);
    void changeToChannel(irc_channel* newChannel);

private slots:
    void channelChanged();
    void sendMessage();
    void displayMessage(parsed_message *message);
    void usersChanged(QStringList users);
    void treeItemClicked(const QModelIndex& index);

};

#endif // MAINWINDOW_H
