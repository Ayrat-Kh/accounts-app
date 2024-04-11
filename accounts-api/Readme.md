# prepare project

```
chmod +x ./preinit
./preinit
```

# pack to docker

```
docker build --progress=plain -t accounts-api .
```

# run on docker

docker run --name accounts-api accounts-api
