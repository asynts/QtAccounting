Somehow, my `FutureProgressDialog` completes successfully, but it returns `Rejected` for some reason.

### Notes

-   I verified that the `done(QDialog::DialogCode::Accepted)` runs.

-   I verified that the return value of the dialog is `0`.
    This corresponds to `QDialog::DialogCode::Rejected`.

-   We are calling `done` on another thread at the moment.

### Ideas

-   Maybe this got something todo with the button roles?

### Theories

-   I suspect, that `0` is the default value.

-   I suspect, that we need to call `done` on the UI thread.
