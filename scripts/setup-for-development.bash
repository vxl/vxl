#!/usr/bin/env bash

cd "${BASH_SOURCE%/*}/.." &&
scripts/gitsetup/setup-user && echo &&
scripts/gitsetup/setup-hooks && echo &&
(scripts/gitsetup/setup-sourceforge ||
 echo 'Failed to setup SourceForge.  Run this again to retry.') && echo &&
scripts/gitsetup/tips

# Rebase master by default
git config rebase.stat true
git config branch.master.rebase true
