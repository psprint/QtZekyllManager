#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTimer>
#include <vector>
#include <string>
#include "zkiresize.h"
#include "lzcsde.h"

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
    QString current_repo_;
    QString current_path_;
    int current_index_;
    bool isConsistent_;
    bool isConsistent2_;

    QTimer timer_;

    std::vector<std::string> ZKL_INDEX_ZEKYLLS_;
    Ui::MainWindow *ui;
    ZkIResize *zkiresize_;
    LZCSDE lzcsde_list_;
    LZCSDE lzcsde_consistent_;

    void insertLZCSDTableRow(QTableWidget * tableWidget, int id, const QString & zekyll, bool checked, const QString & section, const QString & description);
    void insertLZSDETableRow(QTableWidget * tableWidget, int id, const QString & zekyll, const QString & section,
                                const QString & description, const QString & error);

signals:
    void repositoryChanged();

public slots:
    void handle_zkiresize_list(int exitCode, QStringList entries);
    void handle_zkiresize_consistent(int exitCode, QStringList entries);
    void updateMessages( const QStringList & messages );
    void stopThirdTabMarking();

private slots:
    void browse();
    void reloadRepository();
    void on_tableWidget_cellDoubleClicked(int row, int column);
};

#endif // MAINWINDOW_H