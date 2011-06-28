// /////////////////////////////////////////////////////////////////
// Generated by medPluginGenerator
// /////////////////////////////////////////////////////////////////


#include "%1.h"
#include "%1ToolBox.h"

#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>


#include <medCore/medAbstractView.h>
#include <medCore/medRunnableProcess.h>
#include <medCore/medJobManager.h>

#include <medData/medAbstractDataImage.h>

#include <medGui/medToolBoxFactory.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medProgressionStack.h>

#include <rpiCommonTools.hxx>

class %1ToolBoxPrivate
{
public:

    medProgressionStack * progression_stack;
};

%1ToolBox::%1ToolBox(QWidget *parent) : medToolBoxRegistrationCustom(parent), d(new %1ToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QPushButton *runButton = new QPushButton(tr("Run"), this);

    QFormLayout *layout = new QFormLayout(widget);

    this->setTitle("%2");
    //layout->addRow(new QLabel(tr(""),this),d->iterationsBox);

    // progression stack
    d->progression_stack = new medProgressionStack(widget);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);

    this->addWidget(widget);
    this->addWidget(runButton);
    this->addWidget(d->progression_stack);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

%1ToolBox::~%1ToolBox(void)
{
    delete d;

    d = NULL;
}

bool %1ToolBox::registered(void)
{
    return medToolBoxFactory::instance()->
            registerCustomRegistrationToolBox("%1ToolBox",
                                 create%3ToolBox);
}

void %1ToolBox::run(void)
{

    if(!this->parent())
        return;
    dtkAbstractProcess * process;

    if (this->parent()->process())
    {
        process = this->parent()->process();

    }
    else
    {
        process = dtkAbstractProcessFactory::instance()->create("%1");
        this->parent()->setProcess(process);
    }
    dtkAbstractData *fixedData = this->parent()->fixedData();
    dtkAbstractData *movingData = this->parent()->movingData();


    if (!fixedData || !movingData)
        return;


    %1 *process_Registration = dynamic_cast<%1 *>(process);

    // process_Registration->setMyWonderfullParameter(fronTheGui);
    // process_Registration->setMyWonderfullParameter(fronTheGui);

    process->setInput(fixedData,  0);
    process->setInput(movingData, 1);

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (process);

    d->progression_stack->addJobItem(runProcess, "Progress:");

    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));

    medJobManager::instance()->registerJobItem(runProcess);
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}

medToolBoxRegistrationCustom *create%3ToolBox(QWidget *parent)
{
    return new %1ToolBox (parent);
}
