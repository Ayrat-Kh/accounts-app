FROM gcc:13.2.0 AS DEV

RUN --mount=type=cache,target=/var/cache/apt \
    apt-get update && apt-get install -y \
    cmake \
    libssl-dev
    
WORKDIR /app

COPY ./CMakeLists.txt /app/CMakeLists.txt
COPY /external /app/external/
COPY /src /app/src/

WORKDIR /app/build

RUN --mount=type=cache,target=/app/build \
    cmake .. && \
    cmake --build . && \
    cp /app/build/AccountsApi /app/AccountsApi && \
    cd /app/build/mongo_content-prefix/src/mongo_content-build/install && \
    cp -a ./lib /app

RUN chmod +x /app/AccountsApi


# last build
FROM ubuntu:24.04 AS PROD

WORKDIR /app

COPY --from=DEV /app/AccountsApi /app/AccountsApi
COPY --from=DEV /app/lib /app/lib

ENV LD_LIBRARY_PATH=/app/lib:$LD_LIBRARY_PATH

ENV ACCOUNTS_DB_URL=""
ENV ACCOUNTS_JWT_KEY=""
ENV ACCOUNTS_PORT=3000
ENV ACCOUNTS_DB="accounts"

RUN useradd -ms /bin/bash accounts
USER accounts

CMD ["/app/AccountsApi"]