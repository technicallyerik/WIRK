#ifndef NEWSERVER_H
#define NEWSERVER_H

#include <QDialog>

namespace Ui {
class NewServer;
}

class Session;

class NewServer : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewServer(Session &inSession, QWidget *parent = 0);
    ~NewServer();

private slots:
    void accept();

private:
    Ui::NewServer *ui;
    Session *session;
};

#endif // NEWSERVER_H
