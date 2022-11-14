## 0.5.0

### Tweaks

### Features

## Tweaks

-   Use `-O2` for `Release` builds.

-   Right align "amount" column in export.
    There are a few other things that could be aligned too.

-   Update the dependencies in `PKGBUILD`.

-   Forward `internalPointer` in proxy model's `createIndex`.

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

-   Scroll to bottom in `BillEditorDialog`, or reverse list.

-   Allow calculations in amount text field.

-   Automatic migration.

-   Develop a new hash algorithm that can bijectively map 20-bit integers.
    That could be used to make the ids shorter.
    Obviously, I would have to use some sort of prefix to differentiate them.

## 0.Z.0

-   Polish the tab stops, I think there are other shortcuts that I am not taking advantage of.

-   Use QML.

-   Switch to Visual Studio Code.
