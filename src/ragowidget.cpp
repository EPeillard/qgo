
#include "ragowidget.h"
#include "ui_ragowidget.h"
#include <qgoboard.h>

using namespace cv;
using namespace std;
using namespace rago;

RAGoWidget::RAGoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RAGoWidget)
{
    ui->setupUi(this);
    
    ragoWidget=this;
    
    phase=disabled; 
    
    timer=new QTimer(this);
    timer->start(100); 
    
    connect(timer, SIGNAL(timeout()), this, SLOT(updateFrame())); 
    connect(timer, SIGNAL(timeout()), this, SLOT(searchNewMove()));
    
    connect(ui->checkBox,SIGNAL(stateChanged(int)),this,SLOT(activateRAGo(int)));
    connect(ui->changeCam,SIGNAL(pressed()),this,SLOT(changeCam()));
    connect(ui->buttonCalib,SIGNAL(pressed()),this,SLOT(startCalib()));
    connect(ui->buttonCalibY, SIGNAL(pressed()), this, SLOT(goodCalib())); 
    connect(ui->buttonCalibN, SIGNAL(pressed()), this, SLOT(wrongCalib())); 
    connect(ui->lumSlider, SIGNAL(valueChanged(int)), this, SLOT(changeLum(int)));
    connect(ui->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
    
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
      ui->errorLabel->setText("RAGo prêt à être configuré. ");
      ui->label->setText("Placez la fenêtre sur l'écran correspondant au projecteur et la maximiser. \nChoisir la caméra à utiliser. ");
    
      ///Drawing a white image on the goban to improve the detection of the corners
      proj->draw(PROJ_MOD_1 , PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
      waitKey(10);
      
      phase=waitCalib; 
      ui->checkConfig->setEnabled(false);
    }
    catch(exception)
    {
      ui->errorLabel->setText("Pas de caméra détectée");
      ui->checkBox->setCheckState(Qt::Unchecked);
      
      phase=disabled; 
    } 
  }
  else
  {
    phase=disabled; 

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
  ui->label->setText("Patienter pendant la calibration de la caméra... ");
  phase=calibCamera;
  ui->buttonCalib->setEnabled(false);
  
  ui->checkConfig->setEnabled(false);
  core->init();
  ui->checkConfig->setEnabled(true);
  phase=calibCameraEnd;
  QString txt = QString("Un goban de taille ") + QString(to_string(core->getGobanSize()).c_str()) + QString(" a été détecté. \nLa calibration est correcte si les points s'affichent sur les coins du goban. ");
  ui->label->setText(txt);
}

void RAGoWidget::updateFrame()
{
  if(phase!=disabled)
  {
    Mat frame = camera->getFrame();
    cvtColor(frame, frame, CV_BGR2RGB);
    
    if(phase!=calibCamera && phase != waitCalib)
    {
	for(int i=0;i<4;i++)
	{
	  circle(frame,(*(core->getList_corner_markers()[i])),5,Scalar(0,0,128),-1,8);
	}
    }
    
    QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    ui->labelFrame->setPixmap(QPixmap::fromImage(image));
  }
}

void RAGoWidget::goodCalib()
{
  if(phase==calibCameraEnd)
  {
    ui->beforeCalib->setEnabled(false);
    ui->label->setText("Patienter pendant la calibration du projecteur... ");
    phase=calibProjector;
    
    ui->checkConfig->setEnabled(false);
    core->detection();
    core->genConvMat();
    proj->setVG2P(core->getVG2PMat());
    try{
      vg->drawBorders(); //Il arrive que l'appli crash ici, en attendant on refait la calibration si ca arrive FIXME
    }
    catch(exception)
    {
      phase=calibCameraEnd;
      goodCalib();
    }
    ui->checkConfig->setEnabled(true);
    
    phase=calibProjectorEnd;
    ui->label->setText("La calibration est correcte si le cadre est projeté correctement sur le goban. ");
  }
  else if(phase==calibProjectorEnd)
  {
    phase=enabled;
    ui->checkConfig->setEnabled(false);
    ui->label->setText("La calibration est maintenant terminée. ");
    ui->errorLabel->setText("RAGo est configuré. ");
    
    vg->removeBorders();
  }
}

void RAGoWidget::wrongCalib()
{
  if(phase==calibCameraEnd)
  {
    ui->label->setText("Patienter pendant la calibration de la caméra... ");
    phase=calibCamera;
    ui->checkConfig->setEnabled(false);
    core->init();
    ui->checkConfig->setEnabled(true);
    phase=calibCameraEnd;
  QString txt = QString("Un goban de taille ") + QString(to_string(core->getGobanSize()).c_str()) + QString(" a été détecté. \nLa calibration est correcte si les points s'affichent sur les coins du goban. ");
  ui->label->setText(txt);
  }
  else if(phase==calibProjectorEnd)
  {
    ui->label->setText("Patienter pendant la calibration du projecteur... ");
    phase=calibProjector;
    ui->checkConfig->setEnabled(false);
    vg->removeBorders();
    core->detection();
    core->genConvMat();
    proj->setVG2P(core->getVG2PMat());
    vg->drawBorders();
    ui->checkConfig->setEnabled(true);
    phase=calibProjectorEnd;
    ui->label->setText("La calibration est correcte si le cadre est projeté correctement sur le goban. ");
  }
}

void RAGoWidget::changeLum(int newLum)
{
  lum=newLum/100.0;
  ui->lumLabel->setNum(lum);
  
  if(phase == waitCalib || calibCamera || calibCameraEnd)
  {
    proj->draw(PROJ_MOD_1 , PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
    waitKey(10);
  }
  if(phase==calibProjectorEnd||phase==enabled)
  {
    vg->draw();
    waitKey(10);
  }
}

void RAGoWidget::changeZoom(int newZoom)
{
  if(newZoom/10.0>=1)
  {
    ui->zoomLabel->setNum(newZoom/10.0);
    camera->changeZoom(newZoom/10.0);
  }
}

float RAGoWidget::getLum()
{
  return lum; 
}

int RAGoWidget::getGobanSize()
{
  return core->getGobanSize();
}

RAGoPhase RAGoWidget::getPhase()
{
  return phase;
}

void RAGoWidget::addNewMoveMark(StoneColor c, int x, int y)
{
  
  if(newStone.getPlayer()!=PLAYER_NONE && goban->isSomething(newStone.getX(),newStone.getY()))
    vg->addStone(newStone.getPlayer(),newStone.getX(),newStone.getY());
  
  qDebug("Make new move : %i,%i",x,y);
  int color;
  if(c==stoneWhite)
  {
      color=PLAYER_WHITE;
      vg->addNewMark(c,x,y);
  }
  else if(c==stoneBlack)
  {
      color=PLAYER_BLACK;
      vg->addNewMark(c,x,y);
  }
  
  newStone.setDraw(x,y,color);
}


void RAGoWidget::makeMove(StoneColor c, int x, int y)
{
  qDebug("Make move : %i,%i",x,y);
  int color;
  if(c==stoneWhite)
  {
      color=PLAYER_WHITE;
      goban->play(color, x, y);
  }
  else if(c==stoneBlack)
  {
      color=PLAYER_BLACK;
      goban->play(color, x, y);	
  }
  else if(c==stoneErase || c==stoneNone)
  {
    goban->remove(x,y);
  }
  
  //resetReferenceFrame();
}

void RAGoWidget::initGoban()
{
  goban->setGoban();
  waitKey(500);
  resetReferenceFrame();
}

void RAGoWidget::resetReferenceFrame()
{
  core->generateBeginningTurnMat();
}

//Called every 100ms
void RAGoWidget::searchNewMove()
{
  try{
    if(phase==enabled && attachedBoard &&!block)
    {
      int* result;
      int player;
      if(attachedBoard->getBlackTurn())
	player=1;
      else
	player=0;
      
      result = core->imagediff(player);
      
      if(precMove.first==result[0] && precMove.second==result[1]
	&& precMove.first>=1 && precMove.second>=1
	&& precMove.first<=getGobanSize() && precMove.second<=getGobanSize())
      {
	count++;
      {
      }
      else
	precMove.first=result[0];
	precMove.second=result[1];
	count=0;
      }
      
      if(count>=5)
      {
	emit playMove(player, precMove.first, precMove.second);
	resetReferenceFrame();
      }
    }
  }
  catch(exception)
  {
    qDebug("** RAGO : Search move error");
  }
}

void RAGoWidget::attachBoard(qGoBoard* b)
{
  attachedBoard=b;
}

void RAGoWidget::detachBoard(qGoBoard* b)
{
  if(b==attachedBoard)
    attachedBoard=nullptr;
}

void RAGoWidget::blockRago(bool b)
{
  if(b)
    block=true;
  else
  {
    resetReferenceFrame();
    block=false;
  }
}


