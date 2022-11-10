## 0.3.0

### Tweaks

-   Get Bucket name from settings.

-   Add `${BINARY_VERSION}` in uploaded filename.

-   `backup.sh` script.

### Features

-   Allow deletion of transactions and bills.

-   Keep track of pocket money.

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

## 0.Z.0

-   Use QML.

-   Switch to Visual Studio Code.

-   Package it for `pacman`.

    -   Separate settings for development and production.
