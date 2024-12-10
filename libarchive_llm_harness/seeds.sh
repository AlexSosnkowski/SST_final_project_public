
# Create a small tar archive
echo "Hello, World!" > hello.txt
tar -cvf hello.tar hello.txt

# Create a small zip archive
zip hello.zip hello.txt

# Create a small cpio archive
echo "Hello, World!" > hello2.txt
echo "hello2.txt" | cpio -o --format=crc > hello.cpio