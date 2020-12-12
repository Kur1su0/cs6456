

build:
make buildroot CFG_SECURE_DATA_PATH=y CFG_DYN_SHM_CAP=y CFG_TEE_RAM_VA_SIZE=0x00300000 -j `nproc`

copy shared_folder into build (directory)

run:

0. make run-only QEMU_VIRTFS_ENABLE=y QEMU_USERNET_ENABLE=y QEMU_VIRTFS_HOST_DIR=/home/zw9ga/git/optee-qemuv8/build/shared_folder/


1. mkdir shared && mount -t 9p -o trans=virtio host shared && cd shared

2. optee_example_aes dect test_enc.jpg plate.jpg 1234567890123456
