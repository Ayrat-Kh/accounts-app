# prepare project

```
cd build
cmake ..
```

# pack to docker

```
docker build --progress=plain -t accounts-api .
```

# run on docker

docker stop accounts-api; \
docker rm accounts-api; \
docker run --name accounts-api \
 -e ACCOUNTS_DB_URL='mongodb://localhost:27017' \
 -e ACCOUNTS_JWT_KEY='superDuperKey' \
 -e ACCOUNTS_PORT=3000 \
 -e ACCOUNTS_DB=accounts \
 accounts-api

# run mongo db

docker run --name accounts-db -p 27017 -d mongo

# run the app

ACCOUNTS_DB_URL=mongodb://localhost:27017 ACCOUNTS_JWT_KEY=superDuperKey ACCOUNTS_PORT=3000 ACCOUNTS_DB=accounts ./AccountsApi
