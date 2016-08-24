#ifndef CHECKOUTDIALOG_H
#define CHECKOUTDIALOG_H

#include "lgit.h"

#include <QString>
#include <QDialog>

#include "git2.h"
#include "git2/checkout.h"

namespace Ui {
class CheckoutDialog;
}

class CheckoutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckoutDialog(QWidget *parent = 0);
    ~CheckoutDialog();

    void addNotification( git_checkout_notify_t why, const QString & path );
    int addBranch( const QString & name, const QString & sha_tip );
    int addTag( const QString & name, const QString & sha_tip );

    void setLGit( lgit *_lgit ) { lgit_ = _lgit; }
private:
    int addNonselectable( const QString & title );
    int startBranches();
    int startTags();

    bool inserting_branches_;
    bool inserting_tags_;

    lgit *lgit_;

    Ui::CheckoutDialog *ui;
};

#endif // CHECKOUTDIALOG_H
