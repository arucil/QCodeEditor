#pragma once

// QCodeEditor
#include <internal/QCodeEditor.hpp>

// Qt
#include <QMap>
#include <QWidget> // Required for inheritance

class QSyntaxStyle;
class QPaintEvent;

/**
 * @brief Class, that describes line number area widget.
 */
class QLineNumberArea : public QWidget
{
    Q_OBJECT

  public:
    /**
     * @brief Constructor.
     * @param parent Pointer to parent QTextEdit widget.
     */
    explicit QLineNumberArea(QCodeEditor *parent = nullptr);

    // Disable copying
    QLineNumberArea(const QLineNumberArea &) = delete;
    QLineNumberArea &operator=(const QLineNumberArea &) = delete;

    /**
     * @brief Method for setting syntax style object.
     * @param style Pointer to syntax style.
     */
    void setSyntaxStyle(QSyntaxStyle *style);

    /**
     * @brief Method for getting syntax style.
     * @return Pointer to syntax style.
     */
    QSyntaxStyle *syntaxStyle() const;

    /**
     * @brief Add a marker to the line number area, indicating the severity of
     *        diagnostics in a line. If there is already a marker at the same line,
     *        the higher severity will take priority.
     *
     * @param startLine 0-indexed, inclusive.
     * @param endLine 0-indexed, exclusive.
     */
    void addDiagnosticMarker(QCodeEditor::DiagnosticSeverity severity, int startLine, int endLine);

    void clearDiagnosticMarkers();

  public slots:
    void updateEditorLineCount();

  protected:
    void paintEvent(QPaintEvent *event) override;

  private:
    QFont::Weight intToFontWeight(int v);


    QSyntaxStyle *m_syntaxStyle;

    QCodeEditor *m_codeEditParent;

    QMap<int, QCodeEditor::DiagnosticSeverity> m_diagnosticMarkers;
};
