#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBuffer>
#include <QUrl>

namespace Ui {
    class MainWindow;
}

class Session;
class Server;
class Channel;
class QNetworkAccessManager;
class QNetworkReply;
class QTextDocument;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    Session *session;
    QNetworkAccessManager *networkAccessManager;
    QTextDocument *document;

    QBuffer currentBuffer;    // TODO:
    QMovie *currentMovie;     // TODO:  Make this a structure
    QUrl currentMovieUrl;     // TODO:  or something

    void changeToServer(Server *newServer);
    void changeToChannel(Channel *newChannel);
    void scrollToBottom();

private slots:
    void sendMessage();
    void treeItemClicked(const QModelIndex& index);
    void handleMessage(QString server, QString channel, QString message);
    void imageDownloaded(QNetworkReply* networkReply);
    void gifAnimated(int frame);
};

#endif // MAINWINDOW_H
