FROM alpine as build
RUN apk add build-base
COPY ./main.c ./Makefile /
RUN make
RUN ls -l ./server

FROM scratch
WORKDIR /app
COPY --from=build /server ./
EXPOSE 3000
ENTRYPOINT [ "./server" ]
