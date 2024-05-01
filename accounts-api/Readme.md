# Prepare project

```
cd build
cmake ..
```

# Run the app

```
ACCOUNTS_DB_URL=mongodb://localhost:27017 ACCOUNTS_JWT_KEY=superDuperKey ACCOUNTS_PORT=3000 ACCOUNTS_DB=accounts ACCOUNTS_GEOCODE_API_KEY=geoCodeApiKey ./AccountsApi
```

# Pack to the docker image

```
docker build --progress=plain -t accounts-api .
```

# Run mongo db

```
docker run --name accounts-db -p 27017:27017 -d mongo
```

# Run the app on the docker

```
docker stop accounts-api; \
docker rm accounts-api; \
docker run --name accounts-api \
 -e ACCOUNTS_DB_URL='mongodb://localhost:27017' \
 -e ACCOUNTS_JWT_KEY='superDuperKey' \
 -e ACCOUNTS_PORT=3000 \
 -e ACCOUNTS_DB=accounts \
 -e ACCOUNTS_GEOCODE_API_KEY='geocodeapi'
 accounts-api
```
