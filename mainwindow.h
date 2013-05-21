#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ircmessage.h"
#include "irc_server.h"
#include <QMainWindow>
#include <QList>
#include <QStandardItemModel>

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

private slots:
    void sendMessage();
    void displayMessage(QString message);
};

#endif // MAINWINDOW_H
