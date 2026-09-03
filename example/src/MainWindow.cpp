// Demo
#include <DiagnosticListItem.hpp>
#include <MainWindow.hpp>

// QCodeEditor
#include <QCXXHighlighter>
#include <QCodeEditor>
#include <QGLSLCompleter>
#include <QGLSLHighlighter>
#include <QJSHighlighter>
#include <QJSONHighlighter>
#include <QJavaHighlighter>
#include <QLuaCompleter>
#include <QLuaHighlighter>
#include <QPythonCompleter>
#include <QPythonHighlighter>
#include <QSyntaxStyle>
#include <QXMLHighlighter>

// Qt
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStyle>
#include <QVBoxLayout>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_setupLayout(nullptr), m_codeSampleCombobox(nullptr), m_highlighterCombobox(nullptr),
      m_completerCombobox(nullptr), m_styleCombobox(nullptr), m_readOnlyCheckBox(nullptr), m_wordWrapCheckBox(nullptr),
      m_tabReplaceEnabledCheckbox(nullptr), m_tabReplaceNumberSpinbox(nullptr), m_autoIndentationCheckbox(nullptr),
      m_codeEditor(nullptr), m_diagSeverity(0), m_diagCode(nullptr), m_diagMessage(nullptr), m_diagnostics(nullptr),
      m_completers(), m_highlighters(), m_styles()
{
    initData();
    createWidgets();
    setupWidgets();
    performConnections();
}

void MainWindow::initData()
{
    m_codeSamples = {{"C++", loadCode(":/code_samples/cxx.cpp")}, {"GLSL", loadCode(":/code_samples/shader.glsl")},
                     {"XML", loadCode(":/code_samples/xml.xml")}, {"Java", loadCode(":/code_samples/java.java")},
                     {"JS", loadCode(":/code_samples/js.js")},    {"JSON", loadCode(":/code_samples/json.json")},
                     {"LUA", loadCode(":/code_samples/lua.lua")}, {"Python", loadCode(":/code_samples/python.py")}};

    m_completers = {
        {"None", nullptr},
        {"GLSL", new QGLSLCompleter(this)},
        {"LUA", new QLuaCompleter(this)},
        {"Python", new QPythonCompleter(this)},
    };

    m_highlighters = {
        {"None", nullptr},
        {"C++", new QCXXHighlighter},
        {"GLSL", new QGLSLHighlighter},
        {"XML", new QXMLHighlighter},
        {"Java", new QJavaHighlighter},
        {"JS", new QJSHighlighter},
        {"JSON", new QJSONHighlighter},
        {"LUA", new QLuaHighlighter},
        {"Python", new QPythonHighlighter},
    };

    m_styles = {{"Default", QSyntaxStyle::defaultStyle()}};

    // Loading styles
    loadStyle(":/styles/drakula.xml");
}

QString MainWindow::loadCode(QString path)
{
    QFile fl(path);

    if (!fl.open(QIODevice::ReadOnly))
    {
        return QString();
    }

    return fl.readAll();
}

void MainWindow::loadStyle(QString path)
{
    QFile fl(path);

    if (!fl.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto style = new QSyntaxStyle(this);

    if (!style->load(fl.readAll()))
    {
        delete style;
        return;
    }

    m_styles.append({style->name(), style});
}

void MainWindow::createWidgets()
{
    // Layout
    auto container = new QWidget(this);
    setCentralWidget(container);

    auto hBox = new QHBoxLayout(container);

    auto setupGroup = new QGroupBox("Setup", container);
    hBox->addWidget(setupGroup);

    m_setupLayout = new QVBoxLayout(setupGroup);
    setupGroup->setLayout(m_setupLayout);
    setupGroup->setMaximumWidth(300);

    // Diagnostic
    auto diagContainer = new QWidget(container);
    diagContainer->setFixedWidth(200);
    auto diagLayout = new QVBoxLayout(diagContainer);
    diagLayout->addWidget(new QLabel("Diagnostics", diagContainer));
    m_diagnostics = new QListWidget(diagContainer);
    diagLayout->addWidget(m_diagnostics, 1);

    connect(m_diagnostics, &QListWidget::doubleClicked, this, [this](const QModelIndex &index) {
        auto item = dynamic_cast<DiagnosticListItem *>(m_diagnostics->item(index.row()));
        auto cursor = m_codeEditor->textCursor();
        cursor.setPosition(item->m_span.start);
        cursor.setPosition(item->m_span.end, QTextCursor::KeepAnchor);
        m_codeEditor->setTextCursor(cursor);
    });

    auto diagBtnLayout = new QGridLayout();
    auto diagBtnGroup = new QButtonGroup(diagContainer);

    auto btnHint = new QRadioButton("Hint", diagContainer);
    btnHint->setChecked(true);
    m_diagSeverity = 0;
    diagBtnGroup->addButton(btnHint, 0);
    diagBtnLayout->addWidget(btnHint, 0, 0);

    auto btnInfo = new QRadioButton("Information", diagContainer);
    diagBtnGroup->addButton(btnInfo, 1);
    diagBtnLayout->addWidget(btnInfo, 0, 1);

    auto btnWarn = new QRadioButton("Warning", diagContainer);
    diagBtnGroup->addButton(btnWarn, 2);
    diagBtnLayout->addWidget(btnWarn, 1, 0);

    auto btnError = new QRadioButton("Error", diagContainer);
    diagBtnGroup->addButton(btnError, 3);
    diagBtnLayout->addWidget(btnError, 1, 1);

    diagLayout->addLayout(diagBtnLayout);

    connect(diagBtnGroup, &QButtonGroup::idClicked, [this](int id) { m_diagSeverity = id; });

    auto diagMsgLayout = new QFormLayout;
    m_diagCode = new QLineEdit(diagContainer);
    diagMsgLayout->addRow("Code", m_diagCode);

    m_diagMessage = new QLineEdit(diagContainer);
    diagMsgLayout->addRow("Message", m_diagMessage);

    diagLayout->addLayout(diagMsgLayout);

    auto btnAddDiag = new QPushButton("Add", diagContainer);
    diagLayout->addWidget(btnAddDiag);
    btnAddDiag->setEnabled(false);
    connect(m_diagMessage, &QLineEdit::textChanged, btnAddDiag, [btnAddDiag, this]() {
        btnAddDiag->setEnabled(m_codeEditor->textCursor().hasSelection() && !m_diagMessage->text().isEmpty());
    });
    connect(btnAddDiag, &QPushButton::clicked, this, &MainWindow::addDiagnostic);

    auto btnRemDiag = new QPushButton("Remove", diagContainer);
    btnRemDiag->setEnabled(false);
    diagLayout->addWidget(btnRemDiag);
    connect(m_diagnostics, &QListWidget::currentRowChanged, btnRemDiag,
            [btnRemDiag](int row) { btnRemDiag->setEnabled(row >= 0); });
    connect(btnRemDiag, &QPushButton::clicked, this, &MainWindow::removeDiagnostic);

    hBox->addWidget(diagContainer);

    // CodeEditor
    m_codeEditor = new QCodeEditor(this);
    hBox->addWidget(m_codeEditor);

    connect(m_codeEditor, &QCodeEditor::selectionChanged, btnAddDiag, [btnAddDiag, this]() {
        btnAddDiag->setEnabled(m_codeEditor->textCursor().hasSelection() && !m_diagMessage->text().isEmpty());
    });

    m_codeSampleCombobox = new QComboBox(setupGroup);
    m_highlighterCombobox = new QComboBox(setupGroup);
    m_completerCombobox = new QComboBox(setupGroup);
    m_styleCombobox = new QComboBox(setupGroup);

    m_readOnlyCheckBox = new QCheckBox("Read Only", setupGroup);
    m_wordWrapCheckBox = new QCheckBox("Word Wrap", setupGroup);
    m_tabReplaceEnabledCheckbox = new QCheckBox("Tab Replace", setupGroup);
    m_tabReplaceNumberSpinbox = new QSpinBox(setupGroup);
    m_autoIndentationCheckbox = new QCheckBox("Auto Indentation", setupGroup);

    m_actionToggleComment = new QAction("Toggle comment", this);
    m_actionToggleBlockComment = new QAction("Toggle block comment", this);

    m_actionToggleComment->setShortcut(QKeySequence("Ctrl+/"));
    m_actionToggleBlockComment->setShortcut(QKeySequence("Shift+Ctrl+/"));

    connect(m_actionToggleComment, &QAction::triggered, m_codeEditor, &QCodeEditor::toggleComment);
    connect(m_actionToggleBlockComment, &QAction::triggered, m_codeEditor, &QCodeEditor::toggleBlockComment);

    m_mainMenu = new QMenu("Actions", this);
    m_mainMenu->addAction(m_actionToggleComment);
    m_mainMenu->addAction(m_actionToggleBlockComment);
    menuBar()->addMenu(m_mainMenu);

    // Adding widgets
    m_setupLayout->addWidget(new QLabel(tr("Code sample"), setupGroup));
    m_setupLayout->addWidget(m_codeSampleCombobox);
    m_setupLayout->addWidget(new QLabel(tr("Completer"), setupGroup));
    m_setupLayout->addWidget(m_completerCombobox);
    m_setupLayout->addWidget(new QLabel(tr("Highlighter"), setupGroup));
    m_setupLayout->addWidget(m_highlighterCombobox);
    m_setupLayout->addWidget(new QLabel(tr("Style"), setupGroup));
    m_setupLayout->addWidget(m_styleCombobox);
    m_setupLayout->addWidget(m_readOnlyCheckBox);
    m_setupLayout->addWidget(m_wordWrapCheckBox);
    m_setupLayout->addWidget(m_tabReplaceEnabledCheckbox);
    m_setupLayout->addWidget(m_tabReplaceNumberSpinbox);
    m_setupLayout->addWidget(m_autoIndentationCheckbox);
    m_setupLayout->addSpacerItem(new QSpacerItem(1, 2, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void MainWindow::setupWidgets()
{
    setWindowTitle("QCodeEditor Demo");

    // CodeEditor
    m_codeEditor->setPlainText(m_codeSamples[0].second);
    m_codeEditor->setSyntaxStyle(m_styles[0].second);
    m_codeEditor->setCompleter(m_completers[0].second);
    m_codeEditor->setHighlighter(new QCXXHighlighter);

    QStringList list;
    // Code samples
    for (auto &&el : m_codeSamples)
    {
        list << el.first;
    }

    m_codeSampleCombobox->addItems(list);
    list.clear();

    // Highlighter
    for (auto &&el : m_highlighters)
    {
        list << el.first;
    }

    m_highlighterCombobox->addItems(list);
    list.clear();

    // Completer
    for (auto &&el : m_completers)
    {
        list << el.first;
    }

    m_completerCombobox->addItems(list);
    list.clear();

    // Styles
    for (auto &&el : m_styles)
    {
        list << el.first;
    }

    m_styleCombobox->addItems(list);
    list.clear();

    m_tabReplaceEnabledCheckbox->setChecked(m_codeEditor->tabReplace());
    m_tabReplaceNumberSpinbox->setValue(m_codeEditor->tabReplaceSize());
    m_tabReplaceNumberSpinbox->setSuffix(tr(" spaces"));
    m_autoIndentationCheckbox->setChecked(m_codeEditor->autoIndentation());

    m_wordWrapCheckBox->setChecked(m_codeEditor->wordWrapMode() != QTextOption::NoWrap);
}

void MainWindow::performConnections()
{
    connect(m_codeSampleCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) { m_codeEditor->setPlainText(m_codeSamples[index].second); });

    connect(m_highlighterCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) { m_codeEditor->setHighlighter(m_highlighters[index].second); });

    connect(m_completerCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) { m_codeEditor->setCompleter(m_completers[index].second); });

    connect(m_styleCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) { m_codeEditor->setSyntaxStyle(m_styles[index].second); });

    connect(m_readOnlyCheckBox, &QCheckBox::stateChanged, [this](int state) { m_codeEditor->setReadOnly(state != 0); });

    connect(m_wordWrapCheckBox, &QCheckBox::stateChanged, [this](int state) {
        if (state != 0)
        {
            m_codeEditor->setWordWrapMode(QTextOption::WordWrap);
        }
        else
        {
            m_codeEditor->setWordWrapMode(QTextOption::NoWrap);
        }
    });

    connect(m_tabReplaceEnabledCheckbox, &QCheckBox::stateChanged,
            [this](int state) { m_codeEditor->setTabReplace(state != 0); });

    connect(m_tabReplaceNumberSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) { m_codeEditor->setTabReplaceSize(value); });

    connect(m_autoIndentationCheckbox, &QCheckBox::stateChanged,
            [this](int state) { m_codeEditor->setAutoIndentation(state != 0); });
}

void MainWindow::addDiagnostic()
{
    QCodeEditor::DiagnosticSeverity severity;
    switch (m_diagSeverity)
    {
    case 0:
        severity = QCodeEditor::DiagnosticSeverity::Hint;
        break;
    case 1:
        severity = QCodeEditor::DiagnosticSeverity::Information;
        break;
    case 2:
        severity = QCodeEditor::DiagnosticSeverity::Warning;
        break;
    case 3:
        severity = QCodeEditor::DiagnosticSeverity::Error;
        break;
    }

    auto cursor = m_codeEditor->textCursor();

    m_diagnostics->addItem(new DiagnosticListItem(severity, {cursor.selectionStart(), cursor.selectionEnd()},
                                                  m_diagCode->text(), m_diagMessage->text()));
    updateDiagnostics();
}

void MainWindow::removeDiagnostic()
{
    delete m_diagnostics->takeItem(m_diagnostics->currentRow());
    updateDiagnostics();
}

void MainWindow::updateDiagnostics()
{
    m_codeEditor->clearDiagnostics();

    for (int i = 0; i < m_diagnostics->count(); i++)
    {
        auto item = dynamic_cast<const DiagnosticListItem *>(m_diagnostics->item(i));
        m_codeEditor->addDiagnostic(item->m_severity, item->m_span, item->m_message, item->m_code);
    }
}
