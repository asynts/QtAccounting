In the `BillEditorDialog`, the `QFormLayout` takes up much more space than it should.

### Notes

-   I would expect, that since the widget below has it's sizePolicy set to `Expanding`, I would expect it to grow and take up the space.
    But instead, both seem to use the same amount of space.

-   I have used color to verify, that the form widget is the culprit.

### Ideas

-   I should reproduce the issue in a separate project using code only.

### Theories

-   I suspect, that something is missing a layout.
