## QtAccounting

This is an application I created to get familiar with the Qt framework.
It is designed for my own very specific usecase and not useful to anyone else.

Essentially, this is a GUI to edit the database defined in [Database.hpp](Persistance/Database.hpp).
The database file is fetched from S3 and re-uploaded when exiting the application.

### Installation

 1. Install the following packages:

    ```none
    pacman -S --needed aws-sdk-cpp qtcreator
    ```

 1. Install all the other stuff that I forgot.
