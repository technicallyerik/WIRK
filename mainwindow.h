#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QPixmap>

namespace Ui {
    class MainWindow;
}

class Session;
class Server;
class Channel;
class QNetworkAccessManager;
class QNetworkReply;
class QTextDocument;
class AnimationViewModel;

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
    QList<AnimationViewModel *> animations;

    void changeToServer(Server *newServer);
    void changeToChannel(Channel *newChannel);
    void scrollToBottom();

private slots:
    void sendMessage();
    void treeItemClicked(const QModelIndex& index);
    void handleMessage(QString server, QString channel, QString message);
    void imageDownloaded(QNetworkReply* networkReply);
    void anchorClicked(QUrl url);
    void movieAnimated(QPixmap pixels, QUrl url);

};

#endif // MAINWINDOW_H
