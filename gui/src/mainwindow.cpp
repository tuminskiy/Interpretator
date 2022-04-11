#include "mainwindow.hpp"

#include "lexer.hpp"
#include "parser.hpp"

const std::string MainWindow::m_format =
  "<b>Язык</b> = \"Начало\" Определения Опер ... Опер \"Конец\"<br>"
  "<b>Определения</b> = Определение \";\" ... Определение<br>"
  "<b>Определение</b> = [\"Анализ\" ! \"Синтез\"] Вещ ... Вещ [\"Конец анализа\" ! \"Конец синтеза\"]<br>"
  "<b>Опер</b> = Метка ... Метка \":\" Перем \"=\" Прав. часть<br>"
  "<b>Метка</b> = Цел<br>"
  "<b>Прав. часть</b> = &lt;/\"-\"/&gt; Кусок Знак1 ... Кусок<br>"
  "<b>Знак1</b> = \"-\" ! \"+\"<br>"
  "<b>Кусок</b> = Часть Знак2 ... Часть<br>"
  "<b>Знак2</b> = \"*\" ! \"/\"<br>"
  "<b>Часть</b> = Частичка Знак3 ... Частичка<br>"
  "<b>Знак3</b> = \"&\" ! \"|\"<br>"
  "<b>Частичка</b> = &lt;/\"!\"/&gt; Кусочек<br>"
  "<b>Кусочек</b> = &lt;/Фун ... Фун/&gt; Частица<br>"
  "<b>Фун</b> = \"cos\" ! \"sin\" ! \"tan\"<br>"
  "<b>Частица</b> = Перем ! Цел<br>"
  "<b>Перем</b> = Буква Буква &lt;/Цел/&gt;<br>"
  "<b>Вещ</b> = Цел \".\" Цел<br>"
  "<b>Цел</b> = Цифра ... Цифра<br>"
  "<b>Буква</b> = \"А\" ! \"Б\" ! ... ! \"Я\"<br>"
  "<b>Цел</b> = \"0\" ! \"1\" ! ... ! \"9\"";


MainWindow::MainWindow(QWidget* parent)
  : QMainWindow{ parent }
{
  m_ui.setupUi(this);

  m_ui.tbFormat->setText(QString::fromStdString(m_format));

  const QString template_programm =
      "Начало\n"
      "Анализ 1.23 Конец анализа\n"
      "1 : яя1 = 1 + 3\n"
      "Конец";

  m_ui.teInput->setPlainText(template_programm);

  connect(m_ui.bRun, &QPushButton::clicked,
          this, &MainWindow::on_clicked_run);
}

void MainWindow::on_clicked_run()
{
  const auto programm = m_ui.teInput->toPlainText().toStdString();

  if (programm.empty())
    return;

  const auto tokens = Lexer::tokenize(programm);
  const auto analyze = Lexer::analyze(tokens);

  if (!analyze.success) {
    QString msg;

    if (analyze.token_it != tokens.end()) {
      std::visit([&msg](const TokenBase& token) mutable {
        msg += QString::fromStdString("Встречен токен: " + std::string{ token.token() });
      }, *analyze.token_it);
    }

    msg += "\n" + QString::fromStdString(analyze.msg);

    m_ui.tbOutput->setText(msg);
    return;
  }

  auto exprs = Parser::parse(tokens);

  std::unordered_map<std::string_view, double> var_value;

  std::function<std::optional<double>(std::string_view)> get_var_value =
    [&var_value](std::string_view var) mutable {
      const auto it = var_value.find(var);
      std::optional<double> result;

      if (it != var_value.cend())
        result = it->second;

      return result;
    };

  QString msg;

  try {
    for (auto& expr : exprs) {
      expr.set_var_value_getter(get_var_value);
      const auto value = expr.eval();
      var_value[expr.variable()] = value;
    }

    for (const auto& [var, value] : var_value) {
      msg += QString::fromStdString(std::string(var)) + " = " + QString::number(value) + "\n";
    }

  } catch (const std::logic_error& e) {
    msg = QString::fromStdString(e.what());
  }

  m_ui.tbOutput->setText(msg);
}


