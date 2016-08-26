#include "statusdialog.h"
#include "ui_statusdialog.h"

StatusDialog::StatusDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::StatusDialog )
{
    ui->setupUi( this );
}

StatusDialog::~StatusDialog()
{
    delete ui;
}

void StatusDialog::setText( const QString & richText )
{
    richText_ = richText;
    ui->textEdit->document()->clear();
    ui->textEdit->appendHtml( richText );
}
