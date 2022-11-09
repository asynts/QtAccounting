### 0.3.0

### Tweaks

-   Add `${BINARY_VERSION}` in uploaded filename.

-   `backup.sh` script.

-   Use `std::filesystem::path` everywhere.

-   Use `QPointer` everywhere.

### 0.X.0

-   Bucket name from settings.

-   Allow deletion of transactions and bills.

-   Keep track of pocket money.

    -   Add another tab in the main window.
    
    -   Allow creating special transactions.

-   Connect to S3 asynchronously.

    -   Open a dialog that closes when completed.
    
    -   Deal with errors as well: try again, or quit.

-   Error handling everywhere.

### 0.Y.0

-   Implement tab-stops properly.

-   Scroll to bottom in `BillEditorDialog`, or reverse list.

-   Allow calculations in amount text field.

-   Automatic migration.

### 0.Z.0

-   Use QML.

-   Switch to Visual Studio Code.

-   Package it for `pacman`.
