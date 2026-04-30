# syntax=docker/dockerfile:1
FROM alpine AS build
RUN apk add build-base
WORKDIR /app
COPY ./main.c ./Makefile ./
RUN make

# # download STATIC busybox binary 2MB leaner than curl
# ARG TARGETARCH
# ADD https://busybox.net/downloads/binaries/1.35.0-x86_64-linux-musl/busybox /usr/bin/busybox-amd64
# ADD https://busybox.net/downloads/binaries/1.31.0-defconfig-multiarch-musl/busybox-armv8l /usr/bin/busybox-arm64
# # Rename based on target
# RUN if [ "$TARGETARCH" = "amd64" ]; then mv /usr/bin/busybox-amd64 /busybox; \
#     else mv /usr/bin/busybox-arm64 /busybox; fi && \
#     chmod +x /busybox

FROM scratch
LABEL org.opencontainers.image.authors="Bartłomiej Deska"
LABEL org.opencontainers.image.title="SMOLL-HTTP"
LABEL org.opencontainers.image.description="Very small http server for a contest"
EXPOSE 3000

# Oszczędzam na miejscu więc nie dodaję curl
# COPY --from=build /busybox /busybox
# HEALTHCHECK --interval=10s --timeout=5s \
# 	CMD ["/busybox", "wget", "-qO-", "http://localhost:3000/"]

COPY --from=build /app/server ./
ENTRYPOINT [ "./server" ]
