# =============================================================================
# Stage 1: Сборка POCO статически
# =============================================================================
FROM ubuntu:24.04 AS poco-builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates \
    libssl-dev \
    libsasl2-dev \
    zlib1g-dev \
    libexpat1-dev \
    && rm -rf /var/lib/apt/lists/*

ARG POCO_VERSION=1.13.0
WORKDIR /build

RUN git clone --depth 1 --branch poco-${POCO_VERSION}-release https://github.com/pocoproject/poco.git

WORKDIR /build/poco
RUN mkdir cmake-build && cd cmake-build \
    && cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DPOCO_STATIC=ON \
        -DPOCO_UNBUNDLED=OFF \
        -DENABLE_MONGODB=OFF \
        -DENABLE_NETSSL=ON \
        -DENABLE_CRYPTO=ON \
        -DENABLE_JWT=ON \
        -DENABLE_DATA=OFF \
        -DENABLE_DATA_MYSQL=OFF \
        -DENABLE_DATA_POSTGRESQL=OFF \
        -DENABLE_DATA_ODBC=OFF \
        -DENABLE_REDIS=OFF \
    && cmake --build . --target install -j$(nproc) \
    && rm -rf /build/poco/cmake-build

# =============================================================================
# Stage 2: Сборка приложения
# =============================================================================
FROM ubuntu:24.04 AS app-builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY --from=poco-builder /usr/local /usr/local

WORKDIR /build/app
COPY CMakeLists.txt ./
COPY includes ./includes/
COPY src ./src/
COPY configs ./configs/

RUN mkdir build && cd build \
    && cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_PREFIX_PATH=/usr/local \
        -DBUILD_SHARED_LIBS=OFF \
    && cmake --build . -j$(nproc)

# =============================================================================
# Stage 3: Минимальный рантайм
# =============================================================================
FROM ubuntu:24.04 AS runner

ENV DEBIAN_FRONTEND=noninteractive

# Только минимальные системные библиотеки (POCO уже статически залинкован)
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    libssl3 \
    zlib1g \
    libexpat1 \
    && rm -rf /var/lib/apt/lists/* \
    && ldconfig

COPY --from=app-builder /build/app/build/medical_server /usr/local/bin/

ENV PORT=8080
ENV LOG_LEVEL=information
ENV JWT_SECRET=change-me-in-production
ENV MONGO_HOST=mongodb
ENV MONGO_PORT=27017
ENV MONGO_DATABASE=medical_db

EXPOSE 8080

CMD ["medical_server"]