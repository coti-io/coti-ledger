# COTI Ledger App

## Overview

This is the Ledger Nano S COTI app.

## Building and Installing

Make sure that Docker is installed and build the `ledger-app-builder:1.6.1-2` container image:

```bash
$ docker build -t ledger-app-builder:1.6.1-2 .
```

Make sure to install `coreutils` to have the `realpath` command available:

```bash
$ brew install coreutils
```

Compile the app:

```bash
$ docker run --rm -ti -v "$(realpath .):/coti" ledger-app-builder:1.6.1-2
root@d83f688268b3:/coti# cd app
root@d83f688268b3:/coti/app# make
```

After you've built the application, you can generate an installation script `load.sh` via:

```bash
root@d83f688268b3:/coti/app# make load-script
root@d83f688268b3:/coti/app# exit
```

## Installing the Application

In order to install/uninstall the application, you need to install the `ledgerblue` python module and additional dependencies:

```bash
$ xcode-select --install

$ brew install python3 libusb
$ pip3 install ledgerblue hidapi
```

Afterwards, you can run the generated `load.sh` in order to install the application:

```bash
$ cd app
$ ./load.sh
```

You can uninstall the application via:

```bash
$ python3 -m ledgerblue.deleteApp --targetId 0x31100004 --appName "COTI"
```

## Documentation

This follows the specification available in the [`api.md`](app/doc/api.md).
