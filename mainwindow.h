#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void filterCreatedAnnouncement(int NumberOfDays);
    void readSettings();
    void writeSettings();
    void setButtonCSS();

private slots:
    void btnCreate_clicked();
    void cellClicked(int row, int column);

    void btnAll_clicked();
    void btnToday_clicked();
    void btnLast7Days_clicked();
    void btnLast30Days_clicked();

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

};
#endif // MAINWINDOW_H
