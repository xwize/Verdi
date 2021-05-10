#include "workspacewidget.h"
#include "ui_workspacewidget.h"
#include "application.h"

WorkspaceWidget::WorkspaceWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::WorkspaceWidget)
{
    setTitleBarWidget( new QWidget(parent) );
    ui->setupUi(this);
}

void WorkspaceWidget::updateFromModel()
{
    Verdi::WorkspaceVariableNameMap vars
            = vApp->workspace->getVariables();

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount( vars.size() );

    int row = 0;
    Verdi::WorkspaceVariableNameMap::iterator i = vars.begin();
    for( ; i != vars.end(); i++)
    {
        QTableWidgetItem* newItem = NULL;

        newItem = new QTableWidgetItem( QString(i->first.c_str()) );
        ui->tableWidget->setItem(row, 0, newItem);

        newItem = new QTableWidgetItem( QString(i->second->getType().c_str()) );
        ui->tableWidget->setItem(row, 1, newItem);

        newItem = new QTableWidgetItem( QString(i->second->getDataSummary().c_str()) );
        ui->tableWidget->setItem(row, 2, newItem);

        Verdi::String fname = i->second->getFileName();
        if(fname.empty())
            fname = "N/A";
        newItem = new QTableWidgetItem( QString(fname.c_str()));
        ui->tableWidget->setItem(row, 3, newItem);

        row++;
    }
}

WorkspaceWidget::~WorkspaceWidget()
{
    delete ui;
}
