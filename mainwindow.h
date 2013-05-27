#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class Session;
class Server;
class Channel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    Session *session;

    void changeToServer(Server *newServer);
    void changeToChannel(Channel *newChannel);

private slots:
    void sendMessage();
    void treeItemClicked(const QModelIndex& index);
    void handleMessage(QString server, QString channel, QString message);

};

#endif // MAINWINDOW_H
