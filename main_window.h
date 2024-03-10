#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <set>

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Main_Window; }
QT_END_NAMESPACE

class Main_Window : public QMainWindow
{
private:

    Q_OBJECT

    Ui::Main_Window* ui;

    std::set<QString> members;
    QTcpSocket socket;
    bool connected_to_host = false;

    void print_message(const QString& message);
    void add_new_user(const QString& name);
    void add_existing_users(const QString& list);
    void remove_user(const QString& name);

public:

    Main_Window(QWidget* parent = nullptr);

    ~Main_Window();

private slots:

    void on_socket_connected();
    void on_socket_disconnected();
    void on_socket_ready_read();
    void on_connect_button_clicked();
    void on_name_line_returnPressed();
    void on_send_button_clicked();
    void on_message_line_returnPressed();

};

#endif // MAIN_WINDOW_H
