# Build Docker image

From the repo's root directory, run:

```
docker build -f docker/Dockerfile -t os9exec:latest .
```

# Run image & emulator

From the repo's root directory:

```
OS9_ROOT=<absolute-path-of-root-filesystem>
docker run --rm -it -v $OS9_ROOT:/dd -t os9exec:latest <os9exec-options>
```
Running the command above without additional option will start the system shell.

