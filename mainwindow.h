#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ircmessage.h"
#include <QMainWindow>

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

private slots:
    void sendMessage();
    void displayMessage(QString message);
};

#endif // MAINWINDOW_H