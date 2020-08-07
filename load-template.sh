#!/bin/sh

pip3 install ledgerblue

python3 -m ledgerblue.loadApp $APP_LOAD_PARAMS_EVALUATED
