# Build Docker image

From the repo's root directory, run:

```
docker build -f docker/Dockerfile -t os9exec:latest .
```

# Run image

From the repo's root directory:

```
OS9_ROOT=<absolute-path-of-root-filesystem>
docker run --rm -it -v $OS9_ROOT:/dd -t os9exec:latest 
```

## Run the emulator

`os9exec` requires the emulated app's root directory to reside in the same filesystem as the emulator and with read-write access. Therefore, you need to copy the external root directory (i.e. the directory mounted via Docker with the `-v`) first:

```
cp -a /dd dd
./os9exec
```