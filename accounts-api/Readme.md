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

# run mongo db

docker run --name accounts-db -p 27017 -d mongo

# run the app

ACCOUNTS_DB_URL=mongodb://localhost:27017 ACCOUNTS_JWT_KEY=superDuperKey ACCOUNTS_PORT=3001 ./AccountsApi
