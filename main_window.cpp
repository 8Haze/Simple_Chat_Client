#include "main_window.h"
#include "ui_main_window.h"

#include <QDebug>

Main_Window::Main_Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_Window)
{
    ui->setupUi(this);

    ui->send_button->setEnabled(false);
    ui->message_line->setEnabled(false);

    connect(&socket, &QTcpSocket::connected, this, &Main_Window::on_socket_connected);
    connect(&socket, &QTcpSocket::disconnected, this, &Main_Window::on_socket_disconnected);
    connect(&socket, &QTcpSocket::readyRead, this, &Main_Window::on_socket_ready_read);

    setWindowTitle("Simple Chat Client");
}

void Main_Window::print_message(const QString& message)
{
    ui->chat_display->append(message);
}

void Main_Window::add_new_user(const QString& name)
{
    members.insert(name);

    ui->members_list->clear();
    for (const auto& member : members)
        ui->members_list->addItem(member);

    print_message("<font color=\"Purple\">" + name + " has joined the chat room.</font>");
}

void Main_Window::add_existing_users(const QString& list)
{
    auto received_names = list.split('#');

    for (const auto& name : received_names)
        if (!name.isEmpty())
            members.insert(name);

    for (const auto& member : members)
        ui->members_list->addItem(member);
}

void Main_Window::remove_user(const QString& name)
{
    members.erase(name);

    ui->members_list->clear();
    for (const auto& member : members)
        ui->members_list->addItem(member);

    print_message("<font color=\"Orange\">" + name + " has left the chat room.</font>");
}

void Main_Window::on_socket_connected()
{
    print_message("<font color=\"Green\">Connected to server.</font>");
    socket.write(ui->name_line->text().toUtf8() + 'J');
    ui->connect_button->setText("Disconnect");
    ui->send_button->setEnabled(true);
    ui->message_line->setEnabled(true);
    ui->name_line->setEnabled(false);
    connected_to_host = true;
}

void Main_Window::on_socket_disconnected()
{
    print_message("<font color=\"Red\">Disconnected from server.</font>");
    ui->connect_button->setText("Connect");
    ui->send_button->setEnabled(false);
    ui->message_line->setEnabled(false);
    ui->name_line->setEnabled(true);
    connected_to_host = false;
}

void Main_Window::on_socket_ready_read()
{
    QString message = socket.readAll();
    char message_type = message.toStdString().back();
    message.chop(1);

    switch (message_type)
    {
    case('M'): print_message(message); break;
    case('J'): add_new_user(message); break;
    case('L'): remove_user(message); break;
    case('E'): add_existing_users(message); break;
    default: qDebug() << "Unrecognized message type. Received text: " + message + message_type; break;
    }
}

void Main_Window::on_connect_button_clicked()
{
    if (!connected_to_host)
    {
        if (ui->name_line->text().isEmpty() || ui->name_line->text().contains('#'))
            return;

        socket.connectToHost("127.0.0.1", 8001);
    }
    else
    {
        socket.write(ui->name_line->text().toUtf8() + 'L');
        socket.disconnectFromHost();
        members.clear();
        ui->members_list->clear();
    }
}

void Main_Window::on_name_line_returnPressed()
{
    on_connect_button_clicked();
}

void Main_Window::on_send_button_clicked()
{
    QString name = ui->name_line->text();
    QString message = ui->message_line->text();

    if (message.isEmpty())
        return;

    socket.write("<font color=\"Blue\">" + name.toUtf8() + "</font>: " + message.toUtf8() + 'M');
    ui->message_line->clear();
}

void Main_Window::on_message_line_returnPressed()
{
    on_send_button_clicked();
}

Main_Window::~Main_Window()
{
    delete ui;
}
