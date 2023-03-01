#!/usr/bin/env bash
# xmorav41

# 1
echo "Creating disk0..."
dd if=/dev/zero of=disk0 bs=200M count=1
for i in {1..3}; do echo "Creating disk$i..." ; cp disk0 ;"disk$i"; done
for p in {0..3}; do echo "Creating loop$i..." ; losetup "loop$p" "disk$p"; done;

# 2
echo "Creating RAID1..."
mdadm --create /dev/md0 --level=mirror --raid-devices=2 /dev/loop0 /dev/loop1 # confirms este
echo "Creating RAID0..."
mdadm --create /dev/md1 --level=0 --raid-devices=2  /dev/loop2 /dev/loop3

# 3
echo "Creating FIT_vg..."
vgcreate FIT_vg /dev/md{0..1}

# 4
echo "Creating FIT_lv1..."
lvcreate FIT_vg -n FIT_lv1 -L100M
echo "Creating FIT_lv2..."
lvcreate FIT_vg -n FIT_lv2 -L100M

# 5
echo "Setting ext4 filesystem..."
mkfs.ext4 /dev/FIT_vg/FIT_lv1

# 6
echo "Setting xfs filesystem..."
mkfs.xfs /dev/FIT_vg/FIT_lv2

# 7
mkdir /mnt/test1
mkdir /mnt/test2
echo "Mounting FIT_lv1 to /mnt/test1..."
mount /dev/FIT_vg/FIT_lv1 /mnt/test1
echo "Mounting FIT_lv2 to /mnt/test2..."
mount /dev/FIT_vg/FIT_lv2 /mnt/test2

# 8
umount /dev/FIT_vg/FIT_lv1 /mnt/test1
echo "Extending FIT_lv1 to MAX..."
lvextend -l +100%FREE /dev/FIT_vg/FIT_lv1
e2fsck -f /dev/FIT_vg/FIT_lv1
resize2fs /dev/FIT_vg/FIT_lv1
mount /dev/FIT_vg/FIT_lv1 /mnt/test1

# 9
echo "Creating big_file..."
dd if=/dev/urandom of=/mnt/test1/big_file bs=300M count=1
echo "Checksum..."
sha256sum /mnt/test1/big_file

# 10
echo "Emulating faulty disk replacement..."
mdadm --manage /dev/md0 --fail /dev/loop0
echo "Removing faulty disk..."
mdadm --manage /dev/md0 --remove /dev/loop0
echo "Creating new disk..."
dd if=/dev/zero of=disk_replace bs=200M count=1
losetup /dev/loop4 disk_replace
echo "Replacing faulty disk..."
mdadm --manage /dev/md0 --add /dev/loop4
echo "Verification..."
cat /proc/mdstat
