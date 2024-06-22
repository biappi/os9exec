#!/bin/sh

# os9exec requires the emulated app's root directory to reside in the same
# filesystem as the emulator and with read-write access. Therefore, we need
# to copy the external root directory first:
cp -a /dd dd

# This will exec the CMD from the Dockerfile.
exec "$@"