FROM ubuntu AS qemu-system-arm-builder
ARG QEMU_VERSION=4.2.0
ENV QEMU_TARBALL="qemu-${QEMU_VERSION}.tar.xz"
WORKDIR /qemu

RUN apt-get update
RUN apt-get -y install wget
RUN apt-get -y install gpg
RUN apt-get -y install pkg-config
RUN apt-get -y install python3
RUN apt-get -y install libglib2.0-dev
RUN apt-get -y install libpixman-1-dev
RUN apt-get -y install libfdt-dev
RUN apt-get -y install zlib1g-dev
RUN apt-get -y install flex
RUN apt-get -y install bison
RUN apt-get -y install libssl-dev
RUN apt-get -y install make
RUN apt-get -y install gcc
RUN apt-get -y install systemctl


RUN echo "HRNGDEVICE=/dev/urandom" >> /etc/default/rng-tools





RUN wget "https://download.qemu.org/${QEMU_TARBALL}"
RUN # Verify signatures...
RUN wget "https://download.qemu.org/${QEMU_TARBALL}.sig"
RUN gpg --keyserver keyserver.ubuntu.com --recv-keys CEACC9E15534EBABB82D3FA03353C9CEF108B584
RUN gpg --verify "${QEMU_TARBALL}.sig" "${QEMU_TARBALL}"


RUN tar xvf "${QEMU_TARBALL}"

RUN "qemu-${QEMU_VERSION}/configure" --static --target-list=arm-softmmu
RUN make -j$(nproc)
RUN strip "arm-softmmu/qemu-system-arm"

FROM ubuntu as pidoc-vm
ARG RPI_KERNEL_URL="https://github.com/dhruvvyas90/qemu-rpi-kernel/archive/afe411f2c9b04730bcc6b2168cdc9adca224227c.zip"
ARG RPI_KERNEL_CHECKSUM="295a22f1cd49ab51b9e7192103ee7c917624b063cc5ca2e11434164638aad5f4"

COPY --from=qemu-system-arm-builder /qemu/arm-softmmu/qemu-system-arm /usr/local/bin/qemu-system-arm

ADD $RPI_KERNEL_URL /tmp/qemu-rpi-kernel.zip

RUN apt-get update && \
    apt-get -y install \
                        unzip \
                        expect
RUN cd /tmp && \
    echo "$RPI_KERNEL_CHECKSUM  qemu-rpi-kernel.zip" | sha256sum -c && \
    unzip qemu-rpi-kernel.zip && \
    mkdir -p /root/qemu-rpi-kernel && \
    cp qemu-rpi-kernel-*/kernel-qemu-4.19.50-buster /root/qemu-rpi-kernel/ && \
    cp qemu-rpi-kernel-*/versatile-pb.dtb /root/qemu-rpi-kernel/ && \
    rm -rf /tmp/*

VOLUME /sdcard

ADD ./entrypoint.sh /entrypoint.sh

RUN chmod +x /entrypoint.sh

RUN ln -sf /lib/systemd/system/multi-user.target /etc/systemd/system/default.target

ENTRYPOINT ["./entrypoint.sh"]

FROM pidoc-vm as pidoc
ARG FILESYSTEM_IMAGE_URL="http://downloads.raspberrypi.org/raspbian_lite/images/raspbian_lite-2019-09-30/2019-09-26-raspbian-buster-lite.zip"
ARG FILESYSTEM_IMAGE_CHECKSUM="a50237c2f718bd8d806b96df5b9d2174ce8b789eda1f03434ed2213bbca6c6ff"

ADD $FILESYSTEM_IMAGE_URL /filesystem.zip
ADD pi_ssh_enable.exp /pi_ssh_enable.exp

RUN echo "$FILESYSTEM_IMAGE_CHECKSUM  /filesystem.zip" | sha256sum -c

