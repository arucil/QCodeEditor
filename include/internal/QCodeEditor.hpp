#pragma once

#include <interval-tree/interval_tree.hpp>

// Qt
#include <QList>
#include <QRegularExpression>
#include <QString>
#include <QTextEdit> // Required for inheritance
#include <QVector>

class QRect;
class QRegularExpression;
class QKeyEvent;
class QFocusEvent;
class QWheelEvent;
class QEvent;
class QResizeEvent;
class QPaintEvent;
class QMimeData;
class QFont;
class QCompleter;
class QLineNumberArea;
class QSyntaxStyle;
class QStyleSyntaxHighlighter;

/**
 * @brief Class, that describes code editor.
 */
class QCodeEditor : public QTextEdit
{
    Q_OBJECT

  public:
    /**
     * @brief The DiagnosticSeverity enum
     * @note the order should be: the bigger the more important
     */
    enum class DiagnosticSeverity
    {
        Hint,
        Information,
        Warning,
        Error
    };

    struct Span
    {
        /** @brief Start position. Inclusive.
         */
        int start;

        /** @brief End position. Exclusive.
         */
        int end;
    };

    struct Diagnostic
    {
        Diagnostic() = default;

        /**
         * @param start inclusive
         * @param end exclusive
         * @param code optional
         */
        Diagnostic(DiagnosticSeverity severity, const Span &span, const QString &message, const QString &code = "")
            : severity(severity), span(span), message(message), code(code)
        {
        }

        DiagnosticSeverity severity;
        Span span;
        QString message;
        QString code;
    };

    struct Parenthesis
    {
        QChar left, right;
        bool autoComplete, autoRemove, tabJumpOut;

        Parenthesis(QChar l = '(', QChar r = ')', bool complete = true, bool remove = true, bool jumpout = true)
            : left(l), right(r), autoComplete(complete), autoRemove(remove), tabJumpOut(jumpout)
        {
        }
    };

    /**
     * @brief Constructor.
     * @param widget Pointer to parent widget.
     */
    explicit QCodeEditor(QWidget *widget = nullptr);

    // Disable copying
    QCodeEditor(const QCodeEditor &) = delete;
    QCodeEditor &operator=(const QCodeEditor &) = delete;

    /**
     * @brief Method for getting first visible block
     * index.
     * @return Index.
     */
    QTextBlock getFirstVisibleBlock();

    /**
     * @brief Method for setting highlighter.
     * @param highlighter Pointer to syntax highlighter.
     */
    void setHighlighter(QStyleSyntaxHighlighter *highlighter);

    /**
     * @brief Method for setting syntax sty.e.
     * @param style Pointer to syntax style.
     */
    void setSyntaxStyle(QSyntaxStyle *style);

    /**
     * @brief Method for setting tab replacing
     * enabled.
     */
    void setTabReplace(bool enabled);

    /**
     * @brief Method for getting is tab replacing enabled.
     * Default value: true
     */
    bool tabReplace() const;

    /**
     * @brief Method for setting amount of spaces, that will
     * replace tab.
     * @param val Number of spaces.
     */
    void setTabReplaceSize(int val);

    /**
     * @brief Method for getting number of spaces, that will
     * replace tab if `tabReplace` is true.
     * Default: 4
     */
    int tabReplaceSize() const;

    /**
     * @brief Method for setting auto indentation enabled.
     */
    void setAutoIndentation(bool enabled);

    /**
     * @brief Method for setting the parentheses.
     */
    void setParentheses(const QVector<Parenthesis> &parentheses);

    /**
     * @brief Method for setting extra bottom margin enabled.
     */
    void setExtraBottomMargin(bool enabled);

    /**
     * @brief Method for getting is auto indentation enabled.
     * Default: true
     */
    bool autoIndentation() const;

    /**
     * @brief Method for setting completer.
     * @param completer Pointer to completer object.
     */
    void setCompleter(QCompleter *completer);

    /**
     * @brief Method for getting completer.
     * @return Pointer to completer.
     */
    QCompleter *completer() const;

    /**
     * @brief addDiagnostic add a diagnostic to the editor.
     *        The diagnostics will be shown as underlines in the editor.
     * @param severity The severity level of the diagnostic.
     * @param span Span of the diagnostic.
     * @param message The error message to show on hover.
     * @param code Optional. The error code. E.g. "E3001".
     */
    void addDiagnostic(DiagnosticSeverity severity, const Span &span, const QString &message, const QString &code = "");

    void clearDiagnostics();

  signals:
    /**
     * @brief Signal, the font is changed by the wheel event.
     */
    void fontChanged(const QFont &newFont);

    /**
     * @brief Signal, like QLineEdit::editingFinished
     */
    void editingFinished();

    /**
     * @brief CTRL+Enter
     */
    void livecodeTrigger();

  public slots:

    /**
     * @brief Slot, that performs insertion of
     * completion info into code.
     * @param s Data.
     */
    void insertCompletion(const QString &s);

    /**
     * @brief Slot, that performs update of
     * internal editor viewport based on line
     * number area width.
     */
    void updateLineNumberMarginWidth();

    /**
     * @brief Slot, that performs update of some
     * part of line number area.
     * @param rect Area that has to be updated.
     */
    void updateLineNumberArea(const QRect &rect);

    /**
     * @brief Slot, that will proceed extra selection
     * for current cursor position.
     */
    void updateParenthesisAndCurrentLineHighlights();
    void updateWordOccurrenceHighlights();

    /**
     * @brief Slot, that will update editor style.
     */
    void updateStyle();

    /**
     * @brief Slot, that indent the selected lines.
     */
    void indent();

    /**
     * @brief Slot, that unindent the selected lines.
     */
    void unindent();

    /**
     * @brief Slot, that swap the selected lines up.
     */
    void swapLineUp();

    /**
     * @brief Slot, that swap the selected lines down.
     */
    void swapLineDown();

    /**
     * @brief Slot, that delete the selected lines.
     */
    void deleteLine();

    /**
     * @brief Slot, that duplicate the current line or the selection.
     */
    void duplicate();

    /**
     * @brief Slot, that toggle single line comment of the
     * selected lines.
     */
    void toggleComment();

    /**
     * @brief Slot, that toggle block comment of the selection.
     */
    void toggleBlockComment();

  protected:
    /**
     * @brief Method, that's called on any text insertion of
     * mimedata into editor. If it's text - it inserts text
     * as plain text.
     */
    void insertFromMimeData(const QMimeData *source) override;

    /**
     * @brief Method, that's called on editor painting. This
     * method if overloaded for line number area redraw.
     */
    void paintEvent(QPaintEvent *e) override;

    /**
     * @brief Method, that's called on any widget resize.
     * This method if overloaded for line number area
     * resizing.
     */
    void resizeEvent(QResizeEvent *e) override;

    /**
     * @brief Method, update the bottom margin when the font changes.
     */
    void changeEvent(QEvent *e) override;

    /**
     * @brief Method, update the font size when the wheel is rotated with Ctrl pressed
     */
    void wheelEvent(QWheelEvent *e) override;

    /**
     * @brief Method, that's called on any key press, posted
     * into code editor widget. This method is overloaded for:
     *
     * 1. Completion
     * 2. Tab to spaces
     * 3. Low indentation
     * 4. Auto parenthesis
     */
    void keyPressEvent(QKeyEvent *e) override;

    /**
     * @brief Method, that's called on focus into widget.
     * It's required for setting this widget to set
     * completer.
     */
    void focusInEvent(QFocusEvent *e) override;

    /**
     * @brief Reimplemented in order to support editingFinished signal
     */
    void focusOutEvent(QFocusEvent *e) override;

    /**
     * @brief Method for tooltip generation
     */
    bool event(QEvent *e) override;

  private slots:
    /**
     * @brief Slot, that updates the bottom margin.
     */
    void updateBottomMargin();

  private:
    /**
     * @brief Method for initializing default
     * monospace font.
     */
    void initFont();

    /**
     * @brief Method for performing connection
     * of objects.
     */
    void performConnections();

    /**
     * @brief Method for updating geometry of line number area.
     */
    void updateLineNumberAreaGeometry();

    /**
     * @brief Method, that performs completer processing.
     * Returns true if event has to be dropped.
     * @param e Pointer to key event.
     * @return Shall event be dropped.
     */
    bool proceedCompleterBegin(QKeyEvent *e);
    void proceedCompleterEnd(QKeyEvent *e);

    /**
     * @brief Method for getting character under
     * cursor.
     * @param offset Offset to cursor.
     */
    QChar charUnderCursor(int offset = 0) const;

    /**
     * @brief Method for getting word under
     * cursor.
     * @return Word under cursor.
     */
    QString wordUnderCursor() const;

    /**
     * @brief Method, that adds highlighting of
     * currently selected line to extra selection list.
     */
    void highlightCurrentLine();

    /**
     * @brief Method, that adds highlighting of
     * parenthesis if available.
     */
    void highlightParenthesis();

    void highlightWordOccurrences();

    /**
     * @brief Method for remove the first group of regex
     * in each line of the selection.
     * @param regex remove its first group
     * @param force if true, remove regardless of whether
     * all lines are begun with regex; if false remove
     * only when all lines are begun with regex.
     * @return if regex is removed
     */
    bool removeInEachLineOfSelection(const QRegularExpression &regex, bool force);

    /**
     * @brief Method for add the str at the begin of regex
     * in each line of the selection.
     * @param regex add at the begin of its match
     * @param str string to add
     */
    void addInEachLineOfSelection(const QRegularExpression &regex, const QString &str);

    struct InternalSpan
    {
      public:
        using value_type = int;
        using interval_kind = lib_interval_tree::closed;

        constexpr InternalSpan(value_type low, value_type high, int diagIndex = 0)
            : low_{low}, high_{high}, diagIndex(diagIndex)
        {
            assert(low <= high);
        }

        /**
         *  Returns if both intervals equal.
         */
        friend bool operator==(InternalSpan const &lhs, InternalSpan const &other)
        {
            return lhs.low_ == other.low_ && lhs.high_ == other.high_;
        }

        /**
         *  Returns if both intervals are different.
         */
        friend bool operator!=(InternalSpan const &lhs, InternalSpan const &other)
        {
            return lhs.low_ != other.low_ || lhs.high_ != other.high_;
        }

        /**
         *  Returns the lower bound of the interval
         */
        value_type low() const
        {
            return low_;
        }

        /**
         *  Returns the upper bound of the interval
         */
        value_type high() const
        {
            return high_;
        }

        /**
         *  Returns whether the intervals overlap.
         *  For when both intervals are closed.
         */
        bool overlaps(value_type l, value_type h) const
        {
            return low_ <= h && l <= high_;
        }

        /**
         *  Returns whether the intervals overlap, excluding border.
         *  For when at least one interval is open (l&r).
         */
        bool overlaps_exclusive(value_type l, value_type h) const
        {
            return low_ < h && l < high_;
        }

        /**
         *  Returns whether the intervals overlap
         */
        bool overlaps(InternalSpan const &other) const
        {
            return overlaps(other.low_, other.high_);
        }

        /**
         *  Returns whether the intervals overlap, excluding border.
         */
        bool overlaps_exclusive(InternalSpan const &other) const
        {
            return overlaps_exclusive(other.low_, other.high_);
        }

        /**
         *  Returns whether the given value is in this.
         */
        bool within(value_type value) const
        {
            return interval_kind::within(low_, high_, value);
        }

        /**
         *  Returns whether the given interval is in this.
         */
        bool within(InternalSpan const &other) const
        {
            return low_ <= other.low_ && high_ >= other.high_;
        }

        /**
         *  Calculates the distance between the two intervals.
         *  Overlapping intervals have 0 distance.
         */
        value_type operator-(InternalSpan const &other) const
        {
            if (overlaps(other))
                return 0;
            if (high_ < other.low_)
                return other.low_ - high_;
            else
                return low_ - other.high_;
        }

        /**
         *  Returns the size of the interval.
         */
        value_type size() const
        {
            return high_ - low_;
        }

        /**
         *  Creates a new interval from this and other, that contains both intervals
         * and whatever is between.
         */
        InternalSpan join(InternalSpan const &other) const
        {
            return {qMin(low_, other.low_), qMax(high_, other.high_)};
        }

      private:
        value_type low_;
        value_type high_;

      public:
        int diagIndex;
    };

    QStyleSyntaxHighlighter *m_highlighter;
    QSyntaxStyle *m_syntaxStyle;
    QLineNumberArea *m_lineNumberArea;
    QCompleter *m_completer;

    bool m_autoIndentation;
    bool m_replaceTab;
    bool m_extraBottomMargin;
    bool m_textChanged;
    QString m_tabReplace;

    QList<QTextEdit::ExtraSelection> m_parenAndCurLineHilits, m_wordOccurHilits;

    QVector<Diagnostic> m_diagnostics;
    lib_interval_tree::interval_tree<InternalSpan> m_diagSpans;

    QVector<Parenthesis> m_parentheses;

    QRegularExpression m_lineStartIndentRegex;
    QRegularExpression m_lineStartCommentRegex;
};
