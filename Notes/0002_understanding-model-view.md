### Notes

-   This forum post provides a complete example with `QStyledItemDelegate` and everything.

    https://forum.qt.io/topic/116372/qstyleditemdelegate-createeditor-not-getting-called

-   I am having an issue where the editor is closed immediately, if I try to resize the window.

-   It seems, that it's better to intercept the edit by overriding `QStyledItemDelegate::editorEvent`.

    https://lists.qt-project.org/pipermail/interest/2019-November/034125.html

### Ideas

-   The `BillEditorDialog` should have an "Accept" button and nothing is applied without it.

### Theories

-   I suspect, that I am updating the model which updates the view and thus deletes the parent.
