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
    void handleNumericMessage(IrcNumericMessage* message);

private slots:
    void processMessage(IrcMessage * message);

};

#endif // MAINWINDOW_H
