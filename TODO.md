## 0.4.0

### Tweaks

### Features

## Tweaks

-   Copy S3 policy into README file.

-   Forward `internalPointer` in proxy model's `createIndex`.

-   Open bill editor dialog after creating new bill.

## Next 0.X.0

-   Connect to S3 asynchronously.

    -   Open a dialog that closes when completed.
    
    -   Deal with errors as well: try again, or quit.

-   Error handling everywhere.

## 0.Y.0

-   Detect if `totalAmount` changed.

    -   Maybe this is possible be connecting to all the relevant signals?

    -   This seems promising too:
    
        https://stackoverflow.com/q/14858629/8746648

-   Implement tab-stops properly.

-   Scroll to bottom in `BillEditorDialog`, or reverse list.

-   Allow calculations in amount text field.

-   Automatic migration.

-   Develop a new hash algorithm that can bijectively map 20-bit integers.
    That could be used to make the ids shorter.
    Obviously, I would have to use some sort of prefix to differentiate them.

## 0.Z.0

-   Use QML.

-   Switch to Visual Studio Code.

-   Package it for `pacman`.

    -   Separate settings for development and production.
