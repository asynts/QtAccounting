#!/bin/bash
set -e

~/dev/scripts/backup.rb \
    --name 'accounting' \
    --url 'git@github.com:asynts/QtAccounting' \
    --upload 's3://backup.asynts.com/git/accounting'
