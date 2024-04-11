FROM gcc:13.2.0 AS DEV

RUN apt-get update && apt-get install -y \
    cmake \
    libssl-dev

WORKDIR /app

# pre cache dependencies
COPY ./CMakeLists.txt /app/CMakeLists.txt
COPY /external /app/external/

COPY /src /app/src/

WORKDIR /app/build

RUN --mount=type=cache,target=/app/build \
    cmake .. && \
    cmake --build . && \
    cp /app/build/AccountsApi /app/AccountsApi && \
    cd /app/build/mongo_content-prefix/src/mongo_content-build/install/lib && ls -la  &&\
    cp /app/build/mongo_content-prefix/src/mongo_content-build/install/lib/libmongocxx.so._noabi /app/libmongocxx.so._noabi
    
RUN chmod +x /app/AccountsApi

FROM ubuntu:latest AS PROD

WORKDIR /app

COPY --from=DEV /app/AccountsApi /app/AccountsApi
COPY --from=DEV /app/libmongocxx.so._noabi /app/libmongocxx.so._noabi

run ls -la

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/app

RUN useradd -ms /bin/bash accounts
USER accounts

CMD ["/app/AccountsApi"]