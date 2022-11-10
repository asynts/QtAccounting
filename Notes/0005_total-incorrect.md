### Notes

-   We appear to be adding up the incorrect transactions.

### Theories

-   I suspect that this happens because we are iterating though the transactions by using `sourceModel().index()`.
    Instead, we should iterate through our own index and then use `mapToSource`.
