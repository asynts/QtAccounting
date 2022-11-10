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

-   After this line, `transaction` is an invalid pointer:

    ```c++
    // BillProxyModel.hpp:55
    for (auto *transaction : snapshot_transactions()) {
    ```

-   I tried moving the `snapshot_transactions` into another line to ensure that the iterators are still valid (they should be.)
    However, the same issue still occurs.

-   I tried printing out the model index that causes the crash:

    ```none
    [TransactionModel::TransactionModel] this: Accounting::Models::TransactionModel(0x55555610da10)
    [BillProxyModel::snapshot_transaction] index: QModelIndex(0,0,0x555555deea30,Accounting::Models::BillProxyModel(0x555555b98d50))
    ```
    
    Clearly, the `internalPointer` isn't what it should be.

### Ideas

-   I should research, how to correctly iterate through it.

-   I should verify that the ownership matches my expectations.

-   I should print out the `QModelIndex` that causes the crash.

### Theories

-   Maybe I forget to notify the view that something changed and it still uses the old `rowCount`?

-   Maybe I am looking at the model index of a proxy model that doesn't have the data set?
