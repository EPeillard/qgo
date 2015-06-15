#include "ragowidget.h"
#include "ui_ragowidget.h"

RAGoWidget::RAGoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RAGoWidget)
{
    ui->setupUi(this);
    
    ragoWidget=this;
    
    connect(ui->checkBox,SIGNAL(stateChanged(int)),this,SLOT(activateRAGo(int)));
    connect(ui->changeCam,SIGNAL(pressed()),this,SLOT(changeCam()));
    connect(ui->buttonCalib,SIGNAL(pressed()),this,SLOT(startCalib()));
    
}

RAGoWidget::~RAGoWidget()
{
    delete ui;
}

void RAGoWidget::activateRAGo(int state)
{
  if(state==Qt::Checked)
  {
    ui->errorLabel->setText("Initialisation ...");
    qApp->processEvents();
    
    try{
      proj = new Projector();
      vg = new VirtualGoban(proj);
      camera = new Camera();
      goban = new Goban(vg);
      core = new Core(camera, proj, goban);
      ui->errorLabel->setText("RAGo prêt a être configuré. ");
      ui->label->setText("Placez la fenêtre sur l'écran correspondant au projecteur et la maximiser. ");
    
      ///Drawing a white image on the goban to improve the detection of the corners
      proj->draw(PROJ_MOD_1 , PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
      waitKey(10);
    }
    catch(exception)
    {
      ui->errorLabel->setText("Pas de caméra détectée");
      ui->checkBox->setCheckState(Qt::Unchecked);
    } 
  }
  else
  {
    ui->errorLabel->setText("Patientez, arrêt en cours ...");
    
    delete proj;
    delete vg;
    delete camera;
    delete goban;
    delete core;
    
    ui->errorLabel->setText("RAGo désactivé");
  }
}

void RAGoWidget::changeCam()
{
  camera->nextCam();
}

void RAGoWidget::startCalib()
{
  ui->beforeCalib->setEnabled(false);
  core->init();
}

