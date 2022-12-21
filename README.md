# COTI Ledger App

[![GitHub Stars](https://img.shields.io/github/stars/coti-io/coti-ledger.svg)](https://github.com/coti-io/coti-ledger/stargazers)
[![GitHub Issues](https://img.shields.io/github/issues/coti-io/coti-ledger.svg)](https://github.com/coti-io/coti-ledger/issues)
[![Current Version](https://img.shields.io/badge/version-1.1.0-yellow.svg)](https://github.com/coti-io/coti-ledger/)
[![GitHub CI](https://github.com/coti-io/coti-ledger/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/coti-io/coti-ledger/actions/workflows/ci-workflow.yml)
[![GitHub Lint](https://github.com/coti-io/coti-ledger/actions/workflows/lint-workflow.yml/badge.svg)](https://github.com/coti-io/coti-ledger/actions/workflows/lint-workflow.yml)
[![GitHub Coverity Scan](https://github.com/coti-io/coti-ledger/actions/workflows/coverity-scan-workflow.yml/badge.svg)](https://github.com/coti-io/coti-ledger/actions/workflows/coverity-scan-workflow.yml)
[![Coverity](https://scan.coverity.com/projects/25249/badge.svg)](https://scan.coverity.com/projects/coti-io-coti-ledger)
[![Discord Server](https://img.shields.io/discord/386571547508473876.svg)](https://discord.me/coti)

## Overview

This is the Ledger Nano S/S+/X COTI app.

## Building and Installing

Make sure that Docker is installed and build the `ledger-app-builder:2.1.0` container image:

```bash
$ docker build -t ledger-app-builder:2.1.0 .
```

Make sure to install `coreutils` to have the `realpath` command available:

```bash
$ brew install coreutils
```

Compile the app:

```bash
$ docker run --rm -ti -v "$(realpath .):/coti" ledger-app-builder:2.1.0
root@d83f688268b3:/coti# cd app
root@d83f688268b3:/coti/app# make
```

After you've built the application, you can generate an installation script `load.sh` via:

```bash
root@d83f688268b3:/coti/app# make load-script
root@d83f688268b3:/coti/app# exit
```

## Installing the Application

In order to install/uninstall the application, you need to install the `ledgerblue` python module and additional
dependencies:

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
