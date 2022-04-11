#pragma once

#include "ui_mainwindow.h"

#include <QMainWindow>

#include <string>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);

private slots:
  void on_clicked_run();

private:
  Ui::MainWindow m_ui;

  static const std::string m_format;
};
