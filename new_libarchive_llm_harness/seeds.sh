
# Create a small tar archive
echo "Hello, World!" > file.txt
tar -cf seed1.tar file.txt

# Create a small zip archive
zip seed2.zip file.txt

# Create a small cpio archive
echo file.txt | cpio -o > seed3.cpio

# Ensure the seeds directory exists
mkdir -p seeds

# Move seed files into the seeds directory
mv seed1.tar seed2.zip seed3.cpio seeds/