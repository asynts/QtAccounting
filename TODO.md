## 0.5.0

### Tweaks

-   Use `-O2` for `Release` builds.

### Features

-   Connect to S3 asynchronously.

    -   Opens dialog that closes when operation completed.
    
    -   Provides "Retry" and "Cancel" actions.

## Bugs

-   Closing the `FutureProgressDialog` when loading database (e.g. after failure) crashes application.

    -   This is because of an `Q_UNREACHABLE` I put ther.
        It can't easily be removed because `close()` doesn't seem to work in that location.

-   AWS does not obey the timeout provided in the client configuration.

## Tweaks

-   Using `std::async` everywhere might create more threads than needed.

    -   This could be resolved by using `<coroutines>`.
    
    -   Alternatively, I could read about the semantics of `std::async`.

-   Right align "amount" column in export.
    There are a few other things that could be aligned too.

-   Update the dependencies in `PKGBUILD`.

-   Forward `internalPointer` in proxy model's `createIndex`.

-   Consider using Conan for dependencies.

## Next 0.X.0

-   Make `BillProxyModel::exportTo` async and failable.

-   Switch to Visual Studio Code.

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
