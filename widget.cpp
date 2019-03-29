#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QList>
#include <QQueue>
#include <stdlib.h>
#include <qformlayout.h>
#include "mytransmission.h"
#include <qstylefactory.h>


/*设置不同功能按键的样式表*/
QString open_button_style = "QPushButton{background-color:green;\
                                   color: white;   border-radius: 10px;  border: 2px groove gray;\
                                   border-style: outset;}";
QString close_button_style = "QPushButton{background-color:red;\
                             color: white;   border-radius: 10px;  border: 2px groove gray;\
                             border-style: outset;}";

                                 //  "QPushButton:hover{background-color:white; color: black;}"\
                                     "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                                     border-style: inset; }";

 QString set_button_style = "QPushButton{background-color:green;\
                              color: white;   border-radius: 10px;  border: 2px groove gray;\
                              border-style: outset;}"\
                              "QPushButton:hover{background-color:white; color: black;}"\
                              "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                              border-style: inset; }";



 QString mode_select = "QPushButton{background-color:blue;\
                             color: white;\
                               }";
 QString mode_deselect = "QPushButton{background-color:gray;\
                             color: black;\
                               }";


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->key_mouse_enable,&QPushButton::clicked,this,&Widget::onKey_Mouse_Ctrl_Mode_Enable_Click);
    connect(ui->set_angle_speed,&QPushButton::clicked,this,&Widget::Onset_angle_speed_Click);
    connect(ui->set_line_speed,&QPushButton::clicked,this,&Widget::Onset_line_speed_Click);
    connect(ui->clean_battery_low_vol_error,&QPushButton::clicked,this,&Widget::Onclean_battery_low_vol_error_Click);
    connect(ui->clean_battery_over_vol_error,&QPushButton::clicked,this,&Widget::Onclean_battery_over_vol_error_Click);
    connect(ui->clean_motor_over_curr_error,&QPushButton::clicked,this,&Widget::Onclean_motor_over_curr_error_Click);
    connect(ui->clean_motor_over_temp_error,&QPushButton::clicked,this,&Widget::Onclean_motor_over_temp_error_Click);
    connect(ui->clean_motot1_commu_error,&QPushButton::clicked,this,&Widget::Onclean_motot1_commu_error_Click);
    connect(ui->clean_motot2_commu_error,&QPushButton::clicked,this,&Widget::Onclean_motot2_commu_error_Click);
    connect(ui->clean_motot3_commu_error,&QPushButton::clicked,this,&Widget::Onclean_motot3_commu_error_Click);
    connect(ui->clean_motot4_commu_error,&QPushButton::clicked,this,&Widget::Onclean_motot4_commu_error_Click);
    connect(ui->StartOrStopButton_3,&QPushButton::clicked,this,&Widget::OnStartOrStopButton_Click);
    connect(ui->ctrl_mode,SIGNAL(currentIndexChanged(int)),this,SLOT(onCtr_source_switch_currentIndexChanged(int)));

    /*设置界面风格为暗黑*/
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(palette);

    setFixedSize(this->width(),this->height());

    ui->line_speed_feedback->setReadOnly(true);
    ui->angle_speed_feedback->setReadOnly(true);
    ui->car_status_feedback->setReadOnly(true);
    ui->ctrl_mode_feedback->setReadOnly(true);
    ui->vol_feedback->setReadOnly(true);

    Enable_button(false);
    ui->battery_low_vol_error->setEnabled(false);
    ui->battery_over_vol_error->setEnabled(false);
    ui->motor_over_curr_error->setEnabled(false);
    ui->motor_over_temp_error->setEnabled(false);
    ui->motot1_commu_error->setEnabled(false);
    ui->motot2_commu_error->setEnabled(false);
    ui->motot3_commu_error->setEnabled(false);
    ui->motot4_commu_error->setEnabled(false);




    ui->set_angle_speed->setStyleSheet(set_button_style);
    ui->set_line_speed->setStyleSheet(set_button_style);
    ui->clean_battery_low_vol_error->setStyleSheet(set_button_style);
    ui->clean_battery_over_vol_error->setStyleSheet(set_button_style);
    ui->clean_motor_over_curr_error->setStyleSheet(set_button_style);
    ui->clean_motor_over_temp_error->setStyleSheet(set_button_style);
    ui->clean_motot1_commu_error->setStyleSheet(set_button_style);
    ui->clean_motot2_commu_error->setStyleSheet(set_button_style);
    ui->clean_motot3_commu_error->setStyleSheet(set_button_style);
    ui->clean_motot4_commu_error->setStyleSheet(set_button_style);
    ui->key_mouse_enable->setStyleSheet(open_button_style);

    ui->line_speed->setText(tr("请输入线速度值，范围正负100"));
    ui->angle_speed->setText(tr("请输入角速度值，范围正负100"));



    timer = new QTimer(this);//创建一个定时器
    connect(timer,&QTimer::timeout,this,&Widget::ScanfSerialPort);//信号连接
    //开启计时，100ms检测是否有串口设备接入
    timer->start(100);

    transmission = new myTransmission;

     timer1 = new QTimer(this);//创建一个定时器
     connect(timer1,&QTimer::timeout,this,&Widget::Dispaly_FeedBack);//信号连接
     timer1->start(50);

     KeyTimer = new QTimer(this);
      connect(KeyTimer,&QTimer::timeout,this,&Widget::KeyChangeCtrlVal);//信号连接


     memset(&transmission->system_data,0,sizeof(SystemData_t));

     // 获取当前日期
     QDateTime current_date_time = QDateTime::currentDateTime();
     QString current_date = current_date_time.toString("yyyy_MM_dd");

     logfileDir = "./Log/" + current_date;

     uint8_t cnt = 2;
     QString filename = "test_report"+QString::number(cnt)+".txt";

     create_file(logfileDir, filename);

     logfileDir =  logfileDir + "/"+ filename;
      qDebug()<<logfileDir;

     QFile file(logfileDir);

     if(file.open(QIODevice::WriteOnly | QIODevice::Text))
     {
         qDebug()<<"打开文件成功";
         QTextStream stream(&file);
         stream.seek(file.size());
         stream << "set speed \t\t real speed\n";
         stream << "2.500 \t\t 0.500\n";
         file.close();
     }
     else
     {
         qDebug()<<"打开文件失败";
     }




     qDebug()<<"界面设置成功";
}

Widget::~Widget()
{
    delete ui;
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
      case Qt::Key_W:
        key_Flag = 1;
        if(transmission->keyMouse == 1)
        {
           transmission->line_speed += 10;

           if(transmission->line_speed >=  100)
               transmission->line_speed  = 100;

           ui->line_speed->setText(QString::number(transmission->line_speed));
           transmission->Send_cmd(0x12,transmission->line_speed);
           qDebug()<<" line spedd :" <<transmission->line_speed;
        }
        break;

      case Qt::Key_S:
        if(transmission->keyMouse == 1)
        {
           transmission->line_speed -= 10;

           if(transmission->line_speed <= -100)
               transmission->line_speed  = -100;

           ui->line_speed->setText(QString::number(transmission->line_speed));
           transmission->Send_cmd(0x12,transmission->line_speed);
           qDebug()<<" line spedd :" <<transmission->line_speed;
        }
         break;

      case Qt::Key_A:
        if(transmission->keyMouse == 1)
        {
           transmission->angle_speed += 10;

           if(transmission->angle_speed >= 100)
               transmission->angle_speed = 100;

            ui->angle_speed->setText(QString::number(transmission->angle_speed));
            transmission->Send_cmd(0x11,transmission->angle_speed);
            qDebug()<<" angle spedd :" <<transmission->angle_speed;
        }
         break;

      case Qt::Key_D:
        if(transmission->keyMouse == 1)
        {
           transmission->angle_speed -= 10;

           if(transmission->angle_speed <= -100)
               transmission->angle_speed  = -100;

            ui->angle_speed->setText(QString::number(transmission->angle_speed));
            transmission->Send_cmd(0x11,transmission->angle_speed);
            qDebug()<<" angle spedd :" <<transmission->angle_speed;
        }
       case Qt::Key_Q:
           if(transmission->keyMouse == 1)
           {
               transmission->line_speed  = 0;
               transmission->Send_cmd(0x12,transmission->line_speed);
               ui->line_speed->setText(QString::number(transmission->line_speed));
               qDebug()<<" line spedd :" <<transmission->line_speed;
           }
        break;
    case Qt::Key_E:
        if(transmission->keyMouse == 1)
        {
            transmission->angle_speed  = 0;
            ui->angle_speed->setText(QString::number(transmission->angle_speed));
            transmission->Send_cmd(0x11,transmission->angle_speed);
            qDebug()<<" angle spedd :" <<transmission->angle_speed;
        }
     break;


      default:
        break;
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
   switch(event->key())
   {
      case Qt::Key_W:
      case Qt::Key_S:
       if(transmission->keyMouse == 1)
       {
           if(event->isAutoRepeat())
               return;
           transmission->line_speed  = 0;
           transmission->Send_cmd(0x12,transmission->line_speed);
           ui->line_speed->setText(QString::number(transmission->line_speed));
           qDebug()<<" line spedd :" <<transmission->line_speed;
       }

          break;
      case Qt::Key_A:
      case Qt::Key_D:
       if(transmission->keyMouse == 1)
       {
           if(event->isAutoRepeat())
               return;
           transmission->angle_speed  = 0;
           ui->angle_speed->setText(QString::number(transmission->angle_speed));
           transmission->Send_cmd(0x11,transmission->angle_speed);
           qDebug()<<" angle spedd :" <<transmission->angle_speed;
       }

        break;
   }
}

void Widget::Onset_angle_speed_Click()
{
    transmission->angle_speed = ui->angle_speed->text().toInt();
    transmission->Send_cmd(0x11,transmission->angle_speed);
}

void Widget::Onset_line_speed_Click()
{
    transmission->line_speed = ui->line_speed->text().toInt();
    transmission->Send_cmd(0x12,transmission->line_speed);
}

void Widget::Onclean_battery_low_vol_error_Click()
{

    transmission->Send_cmd(0x13,0x01);
}

void Widget::Onclean_battery_over_vol_error_Click()
{
    transmission->Send_cmd(0x13,0x02);
}

void Widget::Onclean_motor_over_curr_error_Click()
{
   transmission->Send_cmd(0x13,0x08);
}

void Widget::Onclean_motor_over_temp_error_Click()
{
    transmission->Send_cmd(0x13,0x07);
}

void Widget::Onclean_motot1_commu_error_Click()
{
    transmission->Send_cmd(0x13,0x03);
}

void Widget::Onclean_motot2_commu_error_Click()
{
    transmission->Send_cmd(0x13,0x04);
}

void Widget::Onclean_motot3_commu_error_Click()
{
    transmission->Send_cmd(0x13,0x05);
}

void Widget::Onclean_motot4_commu_error_Click()
{
    transmission->Send_cmd(0x13,0x06);
}

void Widget::onCtr_source_switch_currentIndexChanged(int index)
{
    qDebug()<<"curr index is "<<index;

    transmission->ctrl_mode = index;

    if(ui->ctrl_mode->currentText() == "指令模式")
         transmission->ctrl_mode = 1;
    else
         transmission->ctrl_mode = 0;

     transmission->Send_cmd(0x14, transmission->ctrl_mode);
}

void Widget::onKey_Mouse_Ctrl_Mode_Enable_Click()
{
    if(ui->key_mouse_enable->text() == "使能")
    {
        transmission->keyMouse = 1;
        ui->key_mouse_enable->setText(tr("失能"));
        ui->key_mouse_enable->setStyleSheet(close_button_style);
        ui->line_speed->setReadOnly(true);
        ui->angle_speed->setReadOnly(true);
        ui->key_note->setText(tr("W增加线速度 S减 Q清零线速度; A增加角速度 D减 E清零角速度"));
    }
    else
    {
        transmission->keyMouse = 0;
        ui->key_mouse_enable->setText(tr("使能"));
        ui->key_mouse_enable->setStyleSheet(open_button_style);
        ui->line_speed->setReadOnly(false);
        ui->angle_speed->setReadOnly(false);

        ui->line_speed->setText(tr("请输入线速度值，范围正负100"));
        ui->angle_speed->setText(tr("请输入角速度值，范围正负100"));

        ui->key_note->clear();
    }

   qDebug()<<"KeyMouse="<<transmission->keyMouse;
}

void Widget::OnStartOrStopButton_Click()
{
    if(ui->StartOrStopButton_3->text() == tr("打开串口"))
       {

            transmission->Open_Serial(ui->PortNumber_3->currentText(),ui->baudBox_3->currentText().toInt(),\
                                      ui->DataBitsBox_3->currentText().toInt(),ui->ParityBox_3->currentIndex(),ui->StopBitsBox_3->currentIndex());

            transmission->serial_stata = 1;

             Enable_button(true);

             ui->baudBox_3->setEnabled(false);
             ui->StopBitsBox_3->setEnabled(false);
             ui->ParityBox_3->setEnabled(false);
             ui->DataBitsBox_3->setEnabled(false);
             ui->PortNumber_3->setEnabled(false);
             ui->StartOrStopButton_3->setText(tr("关闭串口"));
    }
    else
    {
         transmission->Close_Serial();
         transmission->serial_stata = 0;

        Enable_button(false);

        ui->baudBox_3->setEnabled(true);
        ui->StopBitsBox_3->setEnabled(true);
        ui->ParityBox_3->setEnabled(true);
        ui->DataBitsBox_3->setEnabled(true);
        ui->PortNumber_3->setEnabled(true);
        ui->StartOrStopButton_3->setText(tr("打开串口"));
    }

}

void Widget::Dispaly_FeedBack()
{
  ui->line_speed_feedback->setText(QString::number((float)(transmission->system_data.line_speed)/1000.0));
  ui->angle_speed_feedback->setText(QString::number((float)(transmission->system_data.spin_speed)/1000.0));
  ui->car_status_feedback->setText(QString::number(transmission->system_data.car_status));
  ui->vol_feedback->setText(QString::number((float)(transmission->system_data.real_vol)/10.0f));
  ui->ctrl_mode_feedback->setText(QString::number(transmission->system_data.ctrl_mode));

  //qDebug()<<"error:"<<transmission->system_data.error_type;

  if((transmission->system_data.error_type & (1<<0)) != 0)
     ui->battery_low_vol_error->setStyleSheet(close_button_style);
  else
     ui->battery_low_vol_error->setStyleSheet(open_button_style);

  if((transmission->system_data.error_type & (1<<1)) != 0)
     ui->battery_over_vol_error->setStyleSheet(close_button_style);
  else
     ui->battery_over_vol_error->setStyleSheet(open_button_style);

  if((transmission->system_data.error_type & (1<<2)) != 0)
     ui->motot1_commu_error->setStyleSheet(close_button_style);
  else
     ui->motot1_commu_error->setStyleSheet(open_button_style);

  if((transmission->system_data.error_type & (1<<3)) != 0)
     ui->motot2_commu_error->setStyleSheet(close_button_style);
  else
     ui->motot2_commu_error->setStyleSheet(open_button_style);

  if((transmission->system_data.error_type & (1<<4)) != 0)
     ui->motot3_commu_error->setStyleSheet(close_button_style);
  else
     ui->motot3_commu_error->setStyleSheet(open_button_style);

  if((transmission->system_data.error_type & (1<<5)) != 0)
     ui->motot4_commu_error->setStyleSheet(close_button_style);
  else
     ui->motot4_commu_error->setStyleSheet(open_button_style);

  if((transmission->system_data.error_type & (1<<6)) != 0)
     ui->motor_over_temp_error->setStyleSheet(close_button_style);
  else
     ui->motor_over_temp_error->setStyleSheet(open_button_style);

  if((transmission->system_data.error_type & (1<<7)) != 0)
     ui->motor_over_curr_error->setStyleSheet(close_button_style);
  else
      ui->motor_over_curr_error->setStyleSheet(open_button_style);
}

void Widget::KeyChangeCtrlVal()
{

}

void Widget::Enable_button(bool enable)
{
    ui->set_angle_speed->setEnabled(enable);
    ui->set_line_speed->setEnabled(enable);
    ui->clean_battery_low_vol_error->setEnabled(enable);
    ui->clean_battery_over_vol_error->setEnabled(enable);
    ui->clean_motor_over_curr_error->setEnabled(enable);
    ui->clean_motor_over_temp_error->setEnabled(enable);
    ui->clean_motot1_commu_error->setEnabled(enable);
    ui->clean_motot2_commu_error->setEnabled(enable);
    ui->clean_motot3_commu_error->setEnabled(enable);
    ui->clean_motot4_commu_error->setEnabled(enable);

    ui->ctrl_mode->setEnabled(enable);
    ui->key_mouse_enable->setEnabled(enable);
}

Widget::ScanfSerialPort()
{

    static int serila_num_curr = 0,serial_num_last = 0;
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
    {
         ui->PortNumber_3->clear();
         serila_num_curr = 0;
         ui->StartOrStopButton_3->setEnabled(false);
    }
    else
    {
       QStringList list;
       ui->StartOrStopButton_3->setEnabled(true);
       serial_num_last = serila_num_curr;
        foreach(QSerialPortInfo info,infos)
        {
             list << info.portName();
        }
        serila_num_curr = list.length();
         if(serila_num_curr != serial_num_last)
         {
             ui->PortNumber_3->clear();
             qSort(list.begin(),list.end());
             ui->PortNumber_3->addItems(list);
         }

    }

    return 0;

}

void Widget::create_file(QString filePath, QString fileName)
{
  QDir tempDir;

  //临时保存程序当前路径
  QString currentDir = tempDir.currentPath();


  //如果filePath路径不存在，创建它
  if(!tempDir.exists(filePath))
  {
      qDebug()<<QObject::tr("不存在该路径")<<endl;
      tempDir.mkpath(filePath);
  }
  QFile *tempFile = new QFile;

  //将程序的执行路径设置到filePath下
  tempDir.setCurrent(filePath);
  qDebug()<<tempDir.currentPath();

  //检查filePath路径下是否存在文件fileName,如果停止操作。
    if(tempFile->exists(fileName))
    {
        qDebug()<<QObject::tr("文件存在");
        tempDir.setCurrent(currentDir);
        return ;
    }

    //此时，路径下没有fileName文件，使用下面代码在当前路径下创建文件
      tempFile->setFileName(fileName);
      if(!tempFile->open(QIODevice::WriteOnly|QIODevice::Text))
      {
          qDebug()<<QObject::tr("打开失败");
      }
      tempFile->close();
      //将程序当前路径设置为原来的路径
      tempDir.setCurrent(currentDir);
      qDebug()<<tempDir.currentPath();
}
