### Notes

-   Steps to reproduce:

     1. Start with empty database (but loaded from file.)
     
     2. Create new bill.
     
     3. Open editor for that transaction.
     
     4. Create new transaction
     
     5. Close the bill editor
     
    The result is that I receive a SIGSEGV in the getter of either `status` or something else.

-   This started to happen shortly after adding `BillProxyModel::snapshot_transactions`.

-   When closing the editor, this will reveal the `QTableView` of the main window which computes `totalAmount`.
    Similarly, when I export this iterates through the transactions of that bill.

-   We crash in `QBindingStorate::registerDependency` with a corrupted value of `bindingStatus`.

    I have seen the following values: `bindingStatus=0x1d`

-   This is not 100% consistent, sometimes if I use the amount `0.00`, it doesn't crash.
    Maybe this is because the `signalChanged` doesn't fire since the `totalAmount` doesn't change?

-   I encountered another similar crash in `QString` where `this` was invalid.

-   I found this which describes the same problem, but is likely unrelated:

    https://bugreports.qt.io/browse/QTBUG-101177

### Ideas

-   I should research, how to correctly iterate through it.

### Theories

-   Maybe the `reinterpret_cast` of the data from the `QModelIndex` is non-sense.
    That could happen if the index is `QModelIndex()`.
