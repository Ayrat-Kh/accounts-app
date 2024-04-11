FROM gcc:13.2.0 AS DEV

RUN apt-get update && apt-get install -y \
    cmake \
    libssl-dev

WORKDIR /app

# pre cache dependencies
COPY ./CMakeLists.txt /app/CMakeLists.txt
COPY ./preinit.sh /app/preinit.sh
COPY /external /app/external/
COPY /build.sh /app/build.sh

RUN chmod +x /app/preinit.sh
RUN chmod +x /app/build.sh

RUN /app/preinit.sh

COPY /src /app/src/
RUN /app/build.sh

FROM alpine:latest AS PROD

WORKDIR /app

COPY --from=DEV /app/build/AccountsApi /app/AccountsApi

CMD ["./AccountsApi"]