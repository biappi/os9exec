# Build Docker image

From the repo's root directory, run:

```
docker build -f docker/Dockerfile -t os9exec:latest .
```

# Run image

From the repo's root directory:

```
OS9_ROOT=<absolute-path-of-root-filesystem>
docker run --rm -it -v $OS9_ROOT:/app/dd -t os9exec:latest 
```
