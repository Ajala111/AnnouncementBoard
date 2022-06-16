#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QDate>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("SAP Board (AJALA)");

    /* Set up a list/ table view without having a pre-number of rows, this will allow users to have
    ability to enter entries as many as wanted. The table would have 3 major columns (Announcement, Delete, and
    Date of entry), but the last column which keep track of "Date of entries" would be set to invisible(hidden)
    */
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setColumnHidden(2,true);
    ui->tableWidget->setStyleSheet("QTableWidget{alternate-background-color: #C0C0C0; selection-background-color: #606060;}"); //background-color: #C0C0C0; #282828
    ui->tableWidget->setColumnWidth(0,490);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // connecting each button to specific slot (function) to achieve smooth performace
    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(cellClicked(int,int)));
    connect(ui->btnCreate, SIGNAL(clicked()), this, SLOT(btnCreate_clicked()));
    connect(ui->btnAll, SIGNAL(clicked()), this, SLOT(btnAll_clicked()));
    connect(ui->btnToday, SIGNAL(clicked()), this, SLOT(btnToday_clicked()));
    connect(ui->btnLast7Days, SIGNAL(clicked()), this, SLOT(btnLast7Days_clicked()));
    connect(ui->btnLast30Days, SIGNAL(clicked()), this, SLOT(btnLast30Days_clicked()));

    // function/method that set similar CSS for four 'filter' buttons
    setButtonCSS();

    /* This function was carefully used to read-in the data based on last time visited, so as to start from
     * where it stops, readSettings() is used to complement writeSettings(), to avoid using any form of dataBase
    */
    readSettings();
}


MainWindow::~MainWindow()
{
    delete ui;
}

/* CREATE button is used to enter input data to the display table.
 * If no date is entered in the text field, nothing should happen.
 */
void MainWindow::btnCreate_clicked()
{
    QString text = ui->txtEnterText->toPlainText();
    if (text.isEmpty() || text.isNull()) return;

    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignLeft);

    QTableWidgetItem *item2 = new QTableWidgetItem("Delete");
    item2->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *item3 = new QTableWidgetItem(QDate::currentDate().toString());
    item3->setTextAlignment(Qt::AlignCenter);

    /* in order to achieve reverse order chronological entries, new row is inserted at the top (index:0)
    * every time a CREATE button is clciked. However, the text in the input Textfield is deleted (clear()) whenever
    * an announcement has been successfully added.
    */
    ui->tableWidget->insertRow(0);
    ui->tableWidget->setItem(0,0,item);
    ui->tableWidget->setItem(0,1,item2);
    ui->tableWidget->setItem(0,2,item3);
    ui->txtEnterText->clear();
}

/* detect if a user clicked on "Delete" (any cell in column 1), this function is however connected with a
* SIGNAL that can capture the corresponding row of the cell clicked.
* If the user agree to delete, the corresponding row will be removed from the announcement lists
*/
void MainWindow::cellClicked(int row, int column)
{
    if (column == 1) {
        QMessageBox::StandardButtons reply = QMessageBox::question(this, "Delete Announcement", "Are you sure ?");
        if (reply == QMessageBox::Yes) ui->tableWidget->removeRow(row);
    }
}


/* To filter the created announcement, the current date is compared with the third column (date of
* announcement creation. if the difference is gretaer than user's desired date such column would be hidden.
* this function is called for each of the filtered button
*/
void MainWindow::filterCreatedAnnouncement(int NumberOfDays)
{
    if (ui->tableWidget->rowCount() < 1) return;
    QDate today = QDate::currentDate();
    for (int i =0; i< ui->tableWidget->rowCount(); i++){
        QString createdDate = ui->tableWidget->item(i,2)->text();
        QDate created_Date = QDate::fromString(createdDate);

        if ((created_Date.daysTo(today)) > NumberOfDays) ui->tableWidget->hideRow(i);
        else ui->tableWidget->showRow(i);
    }
}


// All/Any previously hidden rows are shown (to display all announcement entries)
void MainWindow::btnAll_clicked()
{
    setButtonCSS();
    ui->btnAll->setStyleSheet("background-color: rgba(46, 204, 113, 0.4); border: none;");
    if (ui->tableWidget->rowCount() < 1) return;
    for (int i =0; i< ui->tableWidget->rowCount(); i++){
        ui->tableWidget->showRow(i);
    }
}

void MainWindow::btnToday_clicked()
{
    setButtonCSS();
    ui->btnToday->setStyleSheet("background-color: rgba(46, 204, 113, 0.4); border: none;");
    filterCreatedAnnouncement(0);
}

void MainWindow::btnLast7Days_clicked()
{
    setButtonCSS();
    ui->btnLast7Days->setStyleSheet("background-color: rgba(46, 204, 113, 0.4); border: none;");
    filterCreatedAnnouncement(7);
}

void MainWindow::btnLast30Days_clicked()
{
    setButtonCSS();
    ui->btnLast30Days->setStyleSheet("background-color: rgba(46, 204, 113, 0.4); border: none;");
    filterCreatedAnnouncement(30);
}

/* This Function is used to set internal data for capturing last updated table before the user close the application
* the two most important data (Announcement and DateOfEntry) were captured, and carefully stored in chronological order
* This function would be triggered any time the user tries to close the application (see closeEvent(*event))
*/
void MainWindow::writeSettings()
{
    QSettings *settings = new QSettings("SAP", "AnnouncementAPP");
    settings->clear();
    if (ui->tableWidget->rowCount() < 1) return;

    settings->beginGroup("Announcement");
    for (int i =0; i< ui->tableWidget->rowCount(); i++){
        settings->setValue(QString::number(i), ui->tableWidget->item(i,0)->text());
    }
    settings->endGroup();

    settings->beginGroup("Date");
    for (int j =0; j< ui->tableWidget->rowCount(); j++){
        settings->setValue(QString::number(j), ui->tableWidget->item(j,2)->text());
    }
    settings->endGroup();
}

/* Read setting every time the application is launched/ everytime the website is visited, the last known
* setting will be loaded, and Announvement table will be filled in an orderly manner.
*/
void MainWindow::readSettings()
{
    QList<QString> announcement;
    QList<QString> date;
    QSettings settings ("SAP", "AnnouncementAPP");
    settings.beginGroup("Announcement");
    if (settings.allKeys().count() <1) return;

    foreach (QString key, settings.allKeys()) announcement.push_front(settings.value(key).toString());
    settings.endGroup();

    settings.beginGroup("Date");
    foreach (QString key, settings.allKeys()) date.push_front(settings.value(key).toString());
    settings.endGroup();

    for (int i=0; i< announcement.size(); i++){
        QTableWidgetItem *item = new QTableWidgetItem(announcement.at(i));
        item->setTextAlignment(Qt::AlignLeft);

        QTableWidgetItem *item2 = new QTableWidgetItem("Delete");
        item2->setTextAlignment(Qt::AlignCenter);

        QDate dateVal = QDate::fromString(date.at(i));
        QTableWidgetItem *item3 = new QTableWidgetItem(dateVal.toString());
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0,0,item);
        ui->tableWidget->setItem(0,1,item2);
        ui->tableWidget->setItem(0,2,item3);
    }
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    writeSettings();
    event->accept();
}

/* To improve the user experience, After input text into the text field, user can use (CTRL + ReturnKey) to
*  call CREATE buton
*/
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
        if (event->key() == Qt::Key_Return)
            btnCreate_clicked();
    QMainWindow::keyPressEvent(event);
}


void MainWindow::setButtonCSS()
{
    QString defaultFilterBtnStyle = "QPushButton {background-color: #3cbaa2; border: 1px solid black;border-radius: 5px;}";
    ui->btnToday->setStyleSheet(defaultFilterBtnStyle);
    ui->btnAll->setStyleSheet(defaultFilterBtnStyle);
    ui->btnLast30Days->setStyleSheet(defaultFilterBtnStyle);
    ui->btnLast7Days->setStyleSheet(defaultFilterBtnStyle);
}
