FROM gcc_image2
#FROM gcc:4.9

COPY . /usr/src/myapp

WORKDIR /usr/src/myapp



# RUN gcc -o myapp synfl1.c
# run app when container is started
# CMD ["./myapp"]
