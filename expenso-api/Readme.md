```
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBSONCXX_POLY_USE_IMPLS=ON
cmake --build . --target install
cd ../../..

```

docker run --name expenso-api -p 27017:27017 -d mongo
