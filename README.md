# UDF Logger

Sample UDF which can be used to post messages to an URL.

The extension is using curl to send messages.

## Requirements

You need to the development package for curl and mysql installed.

    sudo apt-get update && sudo apt-get install -y libmysqlclient-dev libcurl4-openssl-dev

## Installing

When the needed headers and libraries are installed, make sure you update the `makefile` with the correct paths before building your shared library:

    make SERVER=http://localhost:8080 INCLUDE="-I/usr/include/mysql -I/include/x86_64-linux-gnu/curl" LDFLAGS="-L/usr/lib/x86_64-linux-gnu"

After we have built the shared library we need to copy it into our mysql plugin directory:

    cp logger.so `mysql_config --plugindir`

Now we need to tell mysql about our extension:

    sudo mysql -uroot
    DROP FUNCTION IF EXISTS udfLog;
    CREATE FUNCTION udfLog RETURNS STRING SONAME 'logger.so';

After this you could start a server running on the same host as your mysql instance and on port 8080.
You could of course use anything for this, even just using the built in php webserver:

    php -S localhost:8080

and then say greet it:

    SELECT udfLog("Greetings dark night!");
